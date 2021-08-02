/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.content.Context;
import android.content.res.XmlResourceParser;
import android.database.Cursor;
import android.net.NetworkRequest;
import android.os.Handler;
import android.os.IDeviceIdleController;
import android.os.Looper;
import android.os.ServiceManager;
import android.system.ErrnoException;
import android.system.Os;
import android.system.OsConstants;
import android.system.StructStatVfs;
import android.telephony.AccessNetworkConstants.TransportType;
import android.telephony.Rlog;
import android.text.TextUtils;

import com.android.internal.telephony.cdma.CdmaInboundSmsHandler;
import com.android.internal.telephony.cdma.CdmaSMSDispatcher;
import com.android.internal.telephony.cdma.CdmaSubscriptionSourceManager;
import com.android.internal.telephony.cdma.EriManager;
import com.android.internal.telephony.dataconnection.DataEnabledSettings;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.dataconnection.TransportManager;
import com.android.internal.telephony.dataconnection.TelephonyNetworkFactory;
import com.android.internal.telephony.emergency.EmergencyNumberTracker;
import com.android.internal.telephony.imsphone.ImsExternalCallTracker;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.imsphone.ImsPhoneCallTracker;
import com.android.internal.telephony.uicc.IccCardStatus;
import com.android.internal.telephony.uicc.UiccCard;
// MTK-START: add-on
import com.android.internal.telephony.uicc.UiccController;
// MTK-END
import com.android.internal.telephony.uicc.UiccProfile;

/// M: import data-related classes @{
import com.android.internal.telephony.dataconnection.DataConnection;
import com.android.internal.telephony.dataconnection.DataServiceManager;
import com.android.internal.telephony.dataconnection.DcController;
import com.android.internal.telephony.dataconnection.DcRequest;
import com.android.internal.telephony.dataconnection.DcTesterFailBringUpAll;
import com.android.internal.telephony.RetryManager;
import com.android.internal.telephony.uicc.UiccController;
/// @}

import dalvik.system.PathClassLoader;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.function.Consumer;
import java.util.stream.Collectors;



// SMS PART START
import com.android.internal.telephony.cdma.CdmaInboundSmsHandler;
import com.android.internal.telephony.cdma.CdmaSMSDispatcher;
import com.android.internal.telephony.gsm.GsmSMSDispatcher;
import com.android.internal.telephony.gsm.GsmInboundSmsHandler;
import com.android.internal.telephony.ImsSmsDispatcher;
import com.android.internal.telephony.SmsBroadcastUndelivered;
import com.android.internal.telephony.WapPushOverSms;
import com.android.internal.telephony.SmsHeader;
import com.android.internal.telephony.gsm.GsmCellBroadcastHandler;
import com.android.internal.telephony.SmsDispatchersController;
// SMS PART END
// STK-START
import android.os.Looper;
import com.android.internal.telephony.cat.CatService;
import com.android.internal.telephony.cat.CommandParamsFactory;
import com.android.internal.telephony.cat.IconLoader;
import com.android.internal.telephony.cat.RilMessageDecoder;
import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.IccRecords;
import com.android.internal.telephony.uicc.UiccCardApplication;
// STK-END

/**
 * This class has one-line methods to instantiate objects only. The purpose is to make code
 * unit-test friendly and use this class as a way to do dependency injection. Instantiating objects
 * this way makes it easier to mock them in tests.
 */
public class TelephonyComponentFactory {

    private static final String TAG = TelephonyComponentFactory.class.getSimpleName();

    private static TelephonyComponentFactory sInstance;
    public static final String LOG_TAG = "TelephonyComponentFactory";

    private InjectedComponents mInjectedComponents;

    private static class InjectedComponents {
        private static final String ATTRIBUTE_JAR = "jar";
        private static final String ATTRIBUTE_PACKAGE = "package";
        private static final String TAG_INJECTION = "injection";
        private static final String TAG_COMPONENTS = "components";
        private static final String TAG_COMPONENT = "component";
        private static final String SYSTEM = "/system/";
        private static final String PRODUCT = "/product/";

        private final Set<String> mComponentNames = new HashSet<>();
        private TelephonyComponentFactory mInjectedInstance;
        private String mPackageName;
        private String mJarPath;

        /**
         * @return paths correctly configured to inject.
         * 1) PackageName and JarPath mustn't be empty.
         * 2) JarPath is restricted under /system or /product only.
         * 3) JarPath is on a READ-ONLY partition.
         */
        private @Nullable String getValidatedPaths() {
            Rlog.e(TAG, "getValidatedPaths: " + mPackageName + " ," + mJarPath);
            if (TextUtils.isEmpty(mPackageName) || TextUtils.isEmpty(mJarPath)) {
                return null;
            }
            // filter out invalid paths
            return Arrays.stream(mJarPath.split(File.pathSeparator))
                    .filter(s -> (s.startsWith(SYSTEM) || s.startsWith(PRODUCT)))
                    .filter(s -> {
                        try {
                            // This will also throw an error if the target doesn't exist.
                            StructStatVfs vfs = Os.statvfs(s);
                            Rlog.e(TAG, "StructStatVfs: " + vfs.f_flag + " ,"
                                    + OsConstants.ST_RDONLY);
                            return (vfs.f_flag & OsConstants.ST_RDONLY) != 0;
                        } catch (ErrnoException e) {
                            Rlog.e(TAG, "Injection jar is not protected , path: " + s
                                    + e.getMessage());
                            return false;
                        }
                    }).distinct()
                    .collect(Collectors.joining(File.pathSeparator));
        }

        private void makeInjectedInstance() {
            String validatedPaths = getValidatedPaths();
            Rlog.d(TAG, "validated paths: " + validatedPaths);
            if (!TextUtils.isEmpty(validatedPaths)) {
                try {
                    PathClassLoader classLoader = new PathClassLoader(validatedPaths,
                            ClassLoader.getSystemClassLoader());
                    Class<?> cls = classLoader.loadClass(mPackageName);
                    mInjectedInstance = (TelephonyComponentFactory) cls.newInstance();
                } catch (ClassNotFoundException e) {
                    Rlog.e(TAG, "failed: " + e.getMessage());
                } catch (IllegalAccessException | InstantiationException e) {
                    Rlog.e(TAG, "injection failed: " + e.getMessage());
                }
            }
        }

        private boolean isComponentInjected(String componentName) {
            if (mInjectedInstance == null) {
                return false;
            }
            return mComponentNames.contains(componentName);
        }

        /**
         * Find the injection tag, set attributes, and then parse the injection.
         */
        private void parseXml(@NonNull XmlPullParser parser) {
            parseXmlByTag(parser, false, p -> {
                setAttributes(p);
                parseInjection(p);
            }, TAG_INJECTION);
        }

        /**
         * Only parse the first injection tag. Find the components tag, then try parse it next.
         */
        private void parseInjection(@NonNull XmlPullParser parser) {
            parseXmlByTag(parser, false, p -> parseComponents(p), TAG_COMPONENTS);
        }

        /**
         * Only parse the first components tag. Find the component tags, then try parse them next.
         */
        private void parseComponents(@NonNull XmlPullParser parser) {
            parseXmlByTag(parser, true, p -> parseComponent(p), TAG_COMPONENT);
        }

        /**
         * Extract text values from component tags.
         */
        private void parseComponent(@NonNull XmlPullParser parser) {
            try {
                int outerDepth = parser.getDepth();
                int type;
                while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                        && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
                    Rlog.i(TAG, "parseComponent: type " + type);
                    if (type == XmlPullParser.TEXT) {
                        mComponentNames.add(parser.getText());
                        Rlog.i(TAG, "parseComponent: text " + parser.getText());
                    }
                }
            } catch (XmlPullParserException | IOException e) {
                Rlog.e(TAG, "Failed to parse the component." , e);
            }
        }

        /**
         * Iterates the tags, finds the corresponding tag and then applies the consumer.
         */
        private void parseXmlByTag(@NonNull XmlPullParser parser, boolean allowDuplicate,
                @NonNull Consumer<XmlPullParser> consumer, @NonNull final String tag) {
            try {
                int outerDepth = parser.getDepth();
                int type;
                while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                        && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
                    if (type == XmlPullParser.START_TAG && tag.equals(parser.getName())) {
                        consumer.accept(parser);
                        if (!allowDuplicate) {
                            return;
                        }
                    }
                }
            } catch (XmlPullParserException | IOException e) {
                Rlog.e(TAG, "Failed to parse or find tag: " + tag, e);
            }
        }

        /**
         * Sets the mPackageName and mJarPath by <injection/> tag.
         * @param parser
         * @return
         */
        private void setAttributes(@NonNull XmlPullParser parser) {
            for (int i = 0; i < parser.getAttributeCount(); i++) {
                String name = parser.getAttributeName(i);
                String value = parser.getAttributeValue(i);
                if (InjectedComponents.ATTRIBUTE_PACKAGE.equals(name)) {
                    mPackageName = value;
                } else if (InjectedComponents.ATTRIBUTE_JAR.equals(name)) {
                    mJarPath = value;
                }
            }
        }
    }

    public static TelephonyComponentFactory getInstance() {
        if (sInstance == null) {
            sInstance = new TelephonyComponentFactory();
        }
        return sInstance;
    }

    /**
     * Inject TelephonyComponentFactory using a xml config file.
     * @param parser a nullable {@link XmlResourceParser} created with the injection config file.
     * The config xml should has below formats:
     * <injection package="package.InjectedTelephonyComponentFactory" jar="path to jar file">
     *     <components>
     *         <component>example.package.ComponentAbc</component>
     *         <component>example.package.ComponentXyz</component>
     *         <!-- e.g. com.android.internal.telephony.GsmCdmaPhone -->
     *     </components>
     * </injection>
     */
    public void injectTheComponentFactory(XmlResourceParser parser) {
        if (mInjectedComponents != null) {
            Rlog.d(TAG, "Already injected.");
            return;
        }

        if (parser != null) {
            mInjectedComponents = new InjectedComponents();
            mInjectedComponents.parseXml(parser);
            mInjectedComponents.makeInjectedInstance();
            boolean injectSuccessful = !TextUtils.isEmpty(mInjectedComponents.getValidatedPaths());
            Rlog.i(TAG, "Total components injected: " + (injectSuccessful
                    ? mInjectedComponents.mComponentNames.size() : 0));
        }
    }

    /**
     * Use the injected TelephonyComponentFactory if configured. Otherwise, use the default.
     * @param componentName Name of the component class uses the injected component factory,
     * e.g. GsmCdmaPhone.class.getName() for {@link GsmCdmaPhone}
     * @return injected component factory. If not configured or injected, return the default one.
     */
    public TelephonyComponentFactory inject(String componentName) {
        if (mInjectedComponents != null && mInjectedComponents.isComponentInjected(componentName)) {
            return mInjectedComponents.mInjectedInstance;
        }
        return sInstance;
    }

    public GsmCdmaCallTracker makeGsmCdmaCallTracker(GsmCdmaPhone phone) {
        return new GsmCdmaCallTracker(phone);
    }

    /**
     * Create a default CallManager instance
     */
    public CallManager makeCallManager() {
        return new CallManager();
    }

    public SmsStorageMonitor makeSmsStorageMonitor(Phone phone) {
        return new SmsStorageMonitor(phone);
    }

    public SmsUsageMonitor makeSmsUsageMonitor(Context context) {
        return new SmsUsageMonitor(context);
    }

    public ServiceStateTracker makeServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        return new ServiceStateTracker(phone, ci);
    }

    /**
     * Create a new EmergencyNumberTracker.
     */
    public EmergencyNumberTracker makeEmergencyNumberTracker(Phone phone, CommandsInterface ci) {
        return new EmergencyNumberTracker(phone, ci);
    }

    /**
     * Sets the NitzStateMachine implementation to use during implementation. This boolean
     * should be removed once the new implementation is stable.
     */
    static final boolean USE_NEW_NITZ_STATE_MACHINE = true;

    /**
     * Returns a new {@link NitzStateMachine} instance.
     */
    public NitzStateMachine makeNitzStateMachine(GsmCdmaPhone phone) {
        return USE_NEW_NITZ_STATE_MACHINE
                ? new NewNitzStateMachine(phone)
                : new OldNitzStateMachine(phone);
    }

    public SimActivationTracker makeSimActivationTracker(Phone phone) {
        return new SimActivationTracker(phone);
    }

    public DcTracker makeDcTracker(Phone phone, @TransportType int transportType) {
        return new DcTracker(phone, transportType);
    }

    public CarrierSignalAgent makeCarrierSignalAgent(Phone phone) {
        return new CarrierSignalAgent(phone);
    }

    public CarrierActionAgent makeCarrierActionAgent(Phone phone) {
        return new CarrierActionAgent(phone);
    }

    public CarrierResolver makeCarrierResolver(Phone phone) {
        return new CarrierResolver(phone);
    }

    public IccPhoneBookInterfaceManager makeIccPhoneBookInterfaceManager(Phone phone) {
        return new IccPhoneBookInterfaceManager(phone);
    }

    public IccSmsInterfaceManager makeIccSmsInterfaceManager(Phone phone) {
        return new IccSmsInterfaceManager(phone);
    }

    // MTK START: add-on
    public SubscriptionController makeSubscriptionController(Phone phone) {
        return new SubscriptionController(phone);
    }

    public SubscriptionController makeSubscriptionController(Context c, CommandsInterface[] ci) {
        return new SubscriptionController(c);
    }

    public SubscriptionInfoUpdater makeSubscriptionInfoUpdater(Looper looper, Context context,
            Phone[] phone, CommandsInterface[] ci) {
        return new SubscriptionInfoUpdater(looper, context, phone, ci);
    }

    public MultiSimSettingController makeMultiSimSettingController(Context context,
            SubscriptionController sc) {
        return new MultiSimSettingController(context, sc);
    }
    // MTK END

    /**
     * Create a new UiccProfile object.
     */
    // MTK-START: add-on
    public UiccController makeUiccController(Context c, CommandsInterface[] ci) {
        return new UiccController(c, ci);
    }
    // MTK-END

    public UiccProfile makeUiccProfile(Context context, CommandsInterface ci, IccCardStatus ics,
                                       int phoneId, UiccCard uiccCard, Object lock) {
        return new UiccProfile(context, ci, ics, phoneId, uiccCard, lock);
    }

    public EriManager makeEriManager(Phone phone, int eriFileSource) {
        return new EriManager(phone, eriFileSource);
    }

    public WspTypeDecoder makeWspTypeDecoder(byte[] pdu) {
        return new WspTypeDecoder(pdu);
    }

    /**
     * Create a tracker for a single-part SMS.
     */
    public InboundSmsTracker makeInboundSmsTracker(byte[] pdu, long timestamp, int destPort,
            boolean is3gpp2, boolean is3gpp2WapPdu, String address, String displayAddr,
            String messageBody, boolean isClass0) {
        return new InboundSmsTracker(pdu, timestamp, destPort, is3gpp2, is3gpp2WapPdu, address,
                displayAddr, messageBody, isClass0);
    }

    /**
     * Create a tracker for a multi-part SMS.
     */
    public InboundSmsTracker makeInboundSmsTracker(byte[] pdu, long timestamp, int destPort,
            boolean is3gpp2, String address, String displayAddr, int referenceNumber,
            int sequenceNumber, int messageCount, boolean is3gpp2WapPdu, String messageBody,
            boolean isClass0) {
        return new InboundSmsTracker(pdu, timestamp, destPort, is3gpp2, address, displayAddr,
                referenceNumber, sequenceNumber, messageCount, is3gpp2WapPdu, messageBody,
                isClass0);
    }

    /**
     * Create a tracker from a row of raw table
     */
    public InboundSmsTracker makeInboundSmsTracker(Cursor cursor, boolean isCurrentFormat3gpp2) {
        return new InboundSmsTracker(cursor, isCurrentFormat3gpp2);
    }

    /**
     * Create a SmsHeader
     */
    public SmsHeader makeSmsHeader() {
        return new SmsHeader();
    }

    /**
     * Create an ImsSmsDispatcher
     */
    public ImsSmsDispatcher makeImsSmsDispatcher(Phone phone,
            SmsDispatchersController smsDispatchersController) {
        return new ImsSmsDispatcher(phone, smsDispatchersController);
    }

    /**
     * Create a dispatcher for CDMA SMS.
     */
    public CdmaSMSDispatcher makeCdmaSMSDispatcher(Phone phone,
            SmsDispatchersController smsDispatchersController) {
        return new CdmaSMSDispatcher(phone, smsDispatchersController);
    }

    /**
     * Create a dispatcher for GSM SMS.
     */
    public GsmSMSDispatcher makeGsmSMSDispatcher(Phone phone,
            SmsDispatchersController smsDispatchersController,
            GsmInboundSmsHandler gsmInboundSmsHandler) {
        return new GsmSMSDispatcher(phone, smsDispatchersController, gsmInboundSmsHandler);
    }

    /**
     * Create an object of SmsBroadcastUndelivered.
     */
    public void makeSmsBroadcastUndelivered(Context context,
            GsmInboundSmsHandler gsmInboundSmsHandler,
            CdmaInboundSmsHandler cdmaInboundSmsHandler) {
        SmsBroadcastUndelivered.initialize(context, gsmInboundSmsHandler, cdmaInboundSmsHandler);
    }

    public WapPushOverSms makeWapPushOverSms(Context context) {
        return new WapPushOverSms(context);
    }

    /**
     * Create an object of GsmInboundSmsHandler.
     */
    public GsmInboundSmsHandler makeGsmInboundSmsHandler(Context context,
            SmsStorageMonitor storageMonitor, Phone phone) {
        return GsmInboundSmsHandler.makeInboundSmsHandler(context, storageMonitor, phone);
    }

    /**
     * Create an object of GsmCellBroadcastHandler.
     */
    public GsmCellBroadcastHandler makeGsmCellBroadcastHandler(Context context,
            Phone phone) {
        return GsmCellBroadcastHandler.makeGsmCellBroadcastHandler(context, phone);
    }

    /**
     * Create an object of SmsDispatchersController.
     */
    public SmsDispatchersController makeSmsDispatchersController(Phone phone,
            SmsStorageMonitor storageMonitor,
            SmsUsageMonitor usageMonitor) {
        return new SmsDispatchersController(
                phone, phone.mSmsStorageMonitor, phone.mSmsUsageMonitor);
    }

    public ImsPhoneCallTracker makeImsPhoneCallTracker(ImsPhone imsPhone) {
        return new ImsPhoneCallTracker(imsPhone);
    }

    public ImsExternalCallTracker makeImsExternalCallTracker(ImsPhone imsPhone) {

        return new ImsExternalCallTracker(imsPhone);
    }

    /**
     * Create an AppSmsManager for per-app SMS message.
     */
    public AppSmsManager makeAppSmsManager(Context context) {
        return new AppSmsManager(context);
    }

    public DeviceStateMonitor makeDeviceStateMonitor(Phone phone) {
        return new DeviceStateMonitor(phone);
    }

    public TransportManager makeTransportManager(Phone phone) {
        return new TransportManager(phone);
    }

    public CdmaSubscriptionSourceManager
    getCdmaSubscriptionSourceManagerInstance(Context context, CommandsInterface ci, Handler h,
                                             int what, Object obj) {
        return CdmaSubscriptionSourceManager.getInstance(context, ci, h, what, obj);
    }

    // MTK-START: add on
    public CdmaSubscriptionSourceManager
    makeCdmaSubscriptionSourceManager(Context context, CommandsInterface ci, Handler h,
            int what, Object obj) {
        return new CdmaSubscriptionSourceManager(context, ci);
    }

    /// M: eMBMS feature
    /**
     * Create EmbmsAdaptor
     */
    public void initEmbmsAdaptor(Context context, CommandsInterface[] sCommandsInterfaces) {
    }
    /// M: eMBMS end

    // MTK-END
    public IDeviceIdleController getIDeviceIdleController() {
        return IDeviceIdleController.Stub.asInterface(
                ServiceManager.getService(Context.DEVICE_IDLE_CONTROLLER));
    }

    /**
     * Create a supplementary service manager instance
     */
    public void makeSuppServManager(Context context, Phone[] phones) {
    }

    public LocaleTracker makeLocaleTracker(Phone phone, NitzStateMachine nitzStateMachine,
                                           Looper looper) {
        return new LocaleTracker(phone, nitzStateMachine, looper);
    }

    public DataEnabledSettings makeDataEnabledSettings(Phone phone) {
        return new DataEnabledSettings(phone);
    }

    /**
     * Create RadioManager
     */
    public void initRadioManager(Context context, int numPhones,
            CommandsInterface[] sCommandsInterfaces) {
    }

    /**
     * Create CdmaInboundSmsHandler.
     *
     *  @param context  the context of the phone process
     *  @param storageMonitor  the object of SmsStorageMonitor
     *  @param phone  the object of the Phone
     *  @param smsDispatcher the object of the CdmaSMSDispatcher
     *
     *  @return the object of CdmaInboundSmsHandler
     */
    public CdmaInboundSmsHandler makeCdmaInboundSmsHandler(Context context,
            SmsStorageMonitor storageMonitor, Phone phone, CdmaSMSDispatcher smsDispatcher) {
        return new CdmaInboundSmsHandler(context, storageMonitor, phone, smsDispatcher);
    }

    /**
     * Create a data sub selector instance
     */
    public void makeDataSubSelector(Context context, int numPhones) {
    }

    /**
     * Create telephony network factories
     */
    public TelephonyNetworkFactory makeTelephonyNetworkFactories(
            SubscriptionMonitor subscriptionMonitor, Looper looper, Phone phone) {
        return new TelephonyNetworkFactory(subscriptionMonitor, looper, phone);
    }

    /**
     * Create a default phone switcher
     */
    public PhoneSwitcher makePhoneSwitcher(int maxActivePhones, int numPhones,
            Context context, SubscriptionController subscriptionController, Looper looper,
            ITelephonyRegistry tr, CommandsInterface[] cis, Phone[] phones) {
        return new PhoneSwitcher(maxActivePhones, numPhones, context, subscriptionController,
                looper, tr, cis, phones);
    }

    /**
     * Create a smart data swtich assistant instance
     */
    public void makeSmartDataSwitchAssistant(Context context, Phone[] phones) {
    }

     /**
     * Create a dc request instance.
     * @param nr {@link NetworkRequest} describing this request.
     * @param context  the context of the phone process
     *
     * @return the object of DcRequest
     */
    public DcRequest makeDcRequest(NetworkRequest nr, Context context) {
        return new DcRequest(nr, context);
    }

     /**
     * Create world phone instance .
     */
    public void makeWorldPhoneManager() {
    }

    /**
     * Create a proxy controller for radio capability switch.
     */
    public ProxyController makeProxyController(Context context, Phone[] phone,
            UiccController uiccController, CommandsInterface[] ci, PhoneSwitcher ps) {
        return new ProxyController(context, phone, uiccController, ci, ps);
    }

    /**
     * Create a default phone.
     */
    public GsmCdmaPhone makePhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            int phoneId, int precisePhoneType,
            TelephonyComponentFactory telephonyComponentFactory) {
        return new GsmCdmaPhone(context, ci, notifier, phoneId, precisePhoneType,
                telephonyComponentFactory);
    }

    /**
     * Create a default RIL.
     */
    public RIL makeRil(Context context, int preferredNetworkType, int cdmaSubscription,
            Integer instanceId) {
        return new RIL(context, preferredNetworkType, cdmaSubscription, instanceId);
    }

    /**
     * Create a default PhoneNotifier.
     */
    public DefaultPhoneNotifier makeDefaultPhoneNotifier() {
        Rlog.d(LOG_TAG , "makeDefaultPhoneNotifier aosp");
        return new DefaultPhoneNotifier();
    }

    /**
     * initialize GwsdService instance.
     */
    public void initGwsdService(Context context) {
    }

    /**
     * Create a default CatService.
     */
    public CatService makeCatService(CommandsInterface ci, UiccCardApplication ca, IccRecords ir,
            Context context, IccFileHandler fh, UiccProfile uiccProfile, int slotId) {
        return new CatService(ci, ca, ir, context, fh, uiccProfile, slotId);
   }

    /**
     * Create a default RilMessageDecoder.
     */
    public RilMessageDecoder makeRilMessageDecoder(Handler caller, IccFileHandler fh,
            int slotId) {
        return new RilMessageDecoder(caller, fh);
    }

    /**
     * Create a default CommandParamsFactory.
     */
    public CommandParamsFactory makeCommandParamsFactory(RilMessageDecoder caller,
            IccFileHandler fh) {
        return new CommandParamsFactory(caller, fh);
    }

    /**
     * Create a default IconLoader
     */
    public IconLoader makeIconLoader(Looper looper , IccFileHandler fh) {
        return new IconLoader(looper, fh);
    }

    /// M: Add data-related anchor methods @{
    /**
     * Create a dc controller instance
     */
    public DcController makeDcController(String name, Phone phone, DcTracker dct,
            DataServiceManager dataServiceManager, Handler handler) {
        return new DcController(name, phone, dct, dataServiceManager, handler);
    }

    /**
     * Create a retry manager instance
     */
    public RetryManager makeRetryManager(Phone phone, String apnType) {
        return new RetryManager(phone, apnType);
    }

    /**
     * Create a data connection instance
     */
    public DataConnection makeDataConnection(Phone phone, String name, int id,
            DcTracker dct, DataServiceManager dataServiceManager,
            DcTesterFailBringUpAll failBringUpAll, DcController dcc) {
        return new DataConnection(phone, name, id, dct, dataServiceManager, failBringUpAll, dcc);
    }

    /**
     * Create a data connection helper instance.
     */
    public void makeDcHelper(Context context, Phone[] phones) {
    }

    /**
     * Create a ImsPhone.
     */
    public ImsPhone makeImsPhone(Context context, PhoneNotifier phoneNotifier, Phone defaultPhone) {
        return new ImsPhone(context, phoneNotifier, defaultPhone);
    }

    /**
     * initialize carrier express instance .
     */
    public void initCarrierExpress() {
    }

    /**
     * Create a network status updater instance
     */
    public void makeNetworkStatusUpdater(Phone[] phones, int numPhones) {
    }
}
