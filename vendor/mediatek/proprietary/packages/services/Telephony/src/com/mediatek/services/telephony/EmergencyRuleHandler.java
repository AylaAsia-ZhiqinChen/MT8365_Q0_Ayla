/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2017. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

package com.mediatek.services.telephony;

import android.os.SystemProperties;
import android.telecom.PhoneAccountHandle;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.ims.ImsManager;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import android.telephony.RadioAccessFamily;
import com.android.internal.telephony.TelephonyDevController;
import com.android.phone.PhoneUtils;
import com.android.services.telephony.Log;

import com.mediatek.internal.telephony.MtkGsmCdmaPhone;
import com.mediatek.internal.telephony.MtkHardwareConfig;
import com.mediatek.internal.telephony.MtkServiceStateTracker;
import com.mediatek.internal.telephony.RadioCapabilitySwitchUtil;
import com.mediatek.internal.telephony.ratconfiguration.RatConfiguration;

import static com.android.internal.telephony.PhoneConstants.PHONE_TYPE_CDMA;
import static com.android.internal.telephony.PhoneConstants.PHONE_TYPE_GSM;

import java.util.ArrayList;
import java.util.List;

/**
 * The emergency call handler.
 * Selected the proper Phone for setting up the ecc call.
 */
public class EmergencyRuleHandler {
    private static final String TAG = "ECCRuleHandler";
    private static final boolean DBG = true;

    private Phone mGsmPhone = null;
    private Phone mCdmaPhone = null;
    private Phone mEccRetryPhone = null;
    private Phone mDefaultEccPhone = null;
    private Phone mMainPhone = null;
    private int mMainPhoneId = 0;

    private boolean mIsEccRetry;
    private List<GCRuleHandler> mGCRuleList;
    private EmergencyNumberUtils mEccNumberUtils;
    private TelephonyManager mTm;
    private String mNumber;

    private static final int PROJECT_SIM_NUM = TelephonyManager.getDefault().getPhoneCount();

    private static final boolean MTK_CT_VOLTE_SUPPORT
            = !"0".equals(SystemProperties.get("persist.vendor.mtk_ct_volte_support", "0"));

    private static final String PROP_C_CAPABILITY_SLOT = "persist.vendor.radio.c_capability_slot";

    /// M: CC: Fusion RIL specific @{
    TelephonyDevController mTelDevController = TelephonyDevController.getInstance();
    private boolean hasC2kOverImsModem() {
        if (mTelDevController != null &&
                mTelDevController.getModem(0) != null &&
                ((MtkHardwareConfig) mTelDevController.getModem(0)).hasC2kOverImsModem() == true) {
                    return true;
        }
        return false;
    }

    private static final int RAT_GSM_ONLY = 1;
    private static final int RAT_CDMA_ONLY = 2;
    private static final int RAT_GSM_PREF = 3;
    private static final int RAT_CDMA_PREF = 4;
    int mPrefRat = 0;
    /// @}

    /**
     * The common interface for ECC rule.
     */
    public interface GCRuleHandler {
        /**
         * Handle the ecc reqeust.
         * @return Phone The Phone object used for ecc.
         */
        public Phone handleRequest();
    }

    /**
     * Init the EmergencyRuleHandler.
     * @param accountHandle The target PhoneAccountHandle.
     * @param number The Ecc number.
     * @param isEccRetry whether this is ECC retry.
     */
    public EmergencyRuleHandler(
            PhoneAccountHandle accountHandle,
            String number,
            boolean isEccRetry,
            Phone defaultEccPhone) {
        for (Phone p : PhoneFactory.getPhones()) {
            Log.d(TAG, "Phone" + p.getPhoneId() + ":" + (p.getPhoneType() == PHONE_TYPE_CDMA ?
                    "CDMA" : (p.getPhoneType() == PHONE_TYPE_GSM ? "GSM" : "NONE"))
                    + ", service state:" + serviceStateToString(p.getServiceState().getState()));
        }
        mTm = TelephonyManager.getDefault();
        mEccNumberUtils = new EmergencyNumberUtils(number);
        mIsEccRetry = isEccRetry;
        mNumber = number;

        /* default phone is
        1. target phone if SIM inserted, or
        2. TeleService::getFirstPhoneForEmergencyCall()
        */
        mDefaultEccPhone = defaultEccPhone;
        mMainPhoneId = RadioCapabilitySwitchUtil.getMainCapabilityPhoneId();
        mMainPhone = PhoneFactory.getPhone(mMainPhoneId);
        initPhones(accountHandle);
    }

    /**
     * Check if both CDMA and GSM phone exist.
     * @return true if both CDMA and GSM phone exist, otherwise false.
     */
    public static boolean isDualPhoneCdmaExist() {
        if (RatConfiguration.isC2kSupported()) {
            if (PROJECT_SIM_NUM >= 2) {
                for (Phone p : PhoneFactory.getPhones()) {
                    if (p.getPhoneType() == PHONE_TYPE_CDMA) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private void initPhones(PhoneAccountHandle accountHandle) {
        mGsmPhone = getProperPhone(PHONE_TYPE_GSM);
        mCdmaPhone = getProperPhone(PHONE_TYPE_CDMA);

        if (mGsmPhone != null) {
            Log.d(TAG, "GSM Network State == " +
                    serviceStateToString(mGsmPhone.getServiceState().getState()));
        } else {
            Log.d(TAG, "No GSM Phone exist.");
        }
        if (mCdmaPhone != null) {
            Log.d(TAG, "CDMA Network State == " +
                    serviceStateToString(mCdmaPhone.getServiceState().getState()));
        } else {
            Log.d(TAG, "No CDMA Phone exist.");
        }

        int phoneId = SubscriptionManager.INVALID_PHONE_INDEX;
        if (mIsEccRetry) {
            phoneId = Integer.parseInt(accountHandle.getId());
            mEccRetryPhone = PhoneFactory.getPhone(phoneId);
            Log.d(TAG, "EccRetry phoneId:" + phoneId);
        }
    }

    private Phone getProperPhone(int phoneType) {
        Phone phone = null;
        if (phoneType == PHONE_TYPE_GSM) {
            // 1. In service 3/4G phone
            if (mMainPhone != null && mMainPhone.getPhoneType() == PHONE_TYPE_GSM
                    && ServiceState.STATE_IN_SERVICE == mMainPhone.getServiceState().getState()) {
                Log.d(TAG, "getProperPhone(G) : in service, main phone, phoneId:" + mMainPhoneId);
                return mMainPhone;
            }
            // 2. In service phone
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (i == mMainPhoneId) {
                    continue;
                }
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_GSM
                        && ServiceState.STATE_IN_SERVICE == phone.getServiceState().getState()) {
                    Log.d(TAG, "getProperPhone(G) : in service, non-main phone, slotid:" + i);
                    return phone;
                }
            }
            // 3. Radio on and SIM card inserted 3/4G phone
            if (mMainPhone != null && mMainPhone.getPhoneType() == PHONE_TYPE_GSM
                    && ServiceState.STATE_POWER_OFF != mMainPhone.getServiceState().getState()
                    && mTm.hasIccCard(mMainPhoneId)) {
                Log.d(TAG, "getProperPhone(G) : radio on, with SIM, main phone, phoneId:" +
                        mMainPhoneId);
                return mMainPhone;
            }
            // 4. Radio on and SIM card inserted phone
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (i == mMainPhoneId) {
                    continue;
                }
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_GSM
                        && ServiceState.STATE_POWER_OFF != phone.getServiceState().getState()
                        && mTm.hasIccCard(i)) {
                    Log.d(TAG, "getProperPhone(G) : radio on + with SIM + non-main slot:" + i);
                    return phone;
                }
            }
            // 5. Radio on 3/4G phone
            if (mMainPhone != null && mMainPhone.getPhoneType() == PHONE_TYPE_GSM
                    && ServiceState.STATE_POWER_OFF != mMainPhone.getServiceState().getState()) {
                Log.d(TAG, "getProperPhone(G) : radio on + noSIM + main slot:" + mMainPhoneId);
                return mMainPhone;
            }
            // 6. Radio on phone
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (i == mMainPhoneId) {
                    continue;
                }
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_GSM
                        && ServiceState.STATE_POWER_OFF != phone.getServiceState().getState()) {
                    Log.d(TAG, "getProperPhone(G) : radio on + noSIM + non-main slot:" + i);
                    return phone;
                }
            }
            // 7. SIM card inserted 3/4G phone
            if (mMainPhone != null && mMainPhone.getPhoneType() == PHONE_TYPE_GSM
                    && mTm.hasIccCard(mMainPhoneId)) {
                Log.d(TAG, "getProperPhone(G) : radio off + with SIM + main slot:" + mMainPhoneId);
                return mMainPhone;
            }
            // 8. SIM card inserted phone
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (i == mMainPhoneId) {
                    continue;
                }
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_GSM && mTm.hasIccCard(i)) {
                    Log.d(TAG, "getProperPhone(G) : radio off + with SIM + non-main slot:" + i);
                    return phone;
                }
            }
            // 9. 3/4G phone
            if (mMainPhone != null && mMainPhone.getPhoneType() == PHONE_TYPE_GSM) {
                Log.d(TAG, "getProperPhone(G) : radio off + noSIM + main slot:" + mMainPhoneId);
                return mMainPhone;
            }
            // 10. other phone
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (i == mMainPhoneId) {
                    continue;
                }
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_GSM) {
                    Log.d(TAG, "getProperPhone(G) : radio off + noSIM + non-main slot:" + i);
                    return phone;
                }
            }
        } else if (phoneType == PHONE_TYPE_CDMA) {
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == PHONE_TYPE_CDMA) {
                    Log.d(TAG, "getProperPhone(C) : slot:" + i);
                    return phone;
                }
            }
        }
        return null;
    }

    /**
     * Check if gsm has registered to network.
     * @return indicates the register status.
     */
    private boolean isGsmNetworkReady() {
        if (mGsmPhone != null) {
            return ServiceState.STATE_IN_SERVICE
                    == mGsmPhone.getServiceState().getState();
        }
        return false;
    }

    /**
     * Check if cdma has registered to network.
     * @return indicates the register status.
     */
    private boolean isCdmaNetworkReady() {
        if (mCdmaPhone != null) {
            return ServiceState.STATE_IN_SERVICE
                    == mCdmaPhone.getServiceState().getState();
        }
        return false;
    }

    String serviceStateToString(int state) {
        String s = null;
        switch (state) {
            case ServiceState.STATE_IN_SERVICE:
                s = "STATE_IN_SERVICE";
                break;
            case ServiceState.STATE_OUT_OF_SERVICE:
                s = "STATE_OUT_OF_SERVICE";
                break;
            case ServiceState.STATE_EMERGENCY_ONLY:
                s = "STATE_EMERGENCY_ONLY";
                break;
            case ServiceState.STATE_POWER_OFF:
                s = "STATE_POWER_OFF";
                break;
            default:
                Log.d(TAG, "serviceStateToString, invalid state:" + state);
                s = "UNKNOWN_STATE";
                break;
        }
        return s;
    }

    /**
     * Get the proper Phone for ecc dial.
     * @return A object for Phone that used for setup call.
     */
   /*
   isDualPhoneCdmaExist = true:
       1. G + C
       2. No(G) + C
       3. G + (No)C
       4. No(G) + No(C)
   allSimInserted = true:
       5. G + G
       6. G
       7. C
   allSimInserted = false:
       8. G + No(G)
       9. No(G) + No(G)
       10. No(G)
       11. No(C)
   */
    public Phone getPreferredPhone() {
        if (!RatConfiguration.isC2kSupported()) {
            if (mIsEccRetry) {
                Log.d(TAG, "for non-c2k project, return eccRetry phone:" + mEccRetryPhone);
                return mEccRetryPhone;
            } else {
                Log.d(TAG, "for non-c2k project, return default phone:" + mDefaultEccPhone);
                return mDefaultEccPhone;
            }
        } else {
            Phone prefPhone = null;

            boolean allSimInserted = true;
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (!mTm.hasIccCard(i)) {
                    allSimInserted = false;
                    break;
                }
            }

            Log.d(TAG, "getPreferredPhone, allSimInserted:" + allSimInserted);

            if (isDualPhoneCdmaExist()) {
                // case 1~4
                generateGCRuleList();
                prefPhone = getPhoneFromGCRuleList();
                if (prefPhone != null) {
                    Log.d(TAG, "for G+C project with G+C phone, return " + prefPhone
                            + " rat:" + mPrefRat);
                } else {
                    Log.d(TAG, "for G+C project with G+C phone, return default phone:"
                            + mDefaultEccPhone);
                    return mDefaultEccPhone;
                }
            } else {
                if (mIsEccRetry) {
                    Log.d(TAG, "for G+C project w/o G+C phone, return eccRetry phone:"
                            + mEccRetryPhone);
                    /** [ALPS03582877][ALPS03640844]:
                     *     Still set ECC preferred RAT for GSM only and GSM prefer
                     *     ECC number to avoid modem making ECC via CDMA network first. */
                    if (mEccRetryPhone != null
                            && mEccRetryPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM
                            && (mEccNumberUtils.isGsmOnlyNumber()
                                    || mEccNumberUtils.isGsmPreferredNumber())) {
                        prefPhone = mEccRetryPhone;
                        mPrefRat = RAT_GSM_PREF;
                    } else {
                        return mEccRetryPhone;
                    }
                } else {
                    // case 5~7
                    // Cannot switch C capability when SIM inserted, no matter in service or not
                    if (allSimInserted) {
                        /** [ALPS03582877] Still set ECC preferred RAT for GSM only and GSM prefer
                         *   ECC number to avoid modem making ECC via CDMA network first. */
                        if (mEccNumberUtils.isGsmOnlyNumber()) {
                            prefPhone = mDefaultEccPhone;
                            mPrefRat = RAT_GSM_ONLY;
                        } else if (mEccNumberUtils.isGsmPreferredNumber()) {
                            prefPhone = mDefaultEccPhone;
                            mPrefRat = RAT_GSM_PREF;
                        } else if (hasC2kOverImsModem() && isSprintSupport() &&
                                mEccNumberUtils.isCdmaPreferredNumber()) {
                            prefPhone = mDefaultEccPhone;
                            mPrefRat = RAT_CDMA_PREF;
                        } else {
                            // 93 modem support MD retry ECC from IMS -> CS(C2K/GSM)
                            // G+G phone, if default ecc phone locked & no c2k support
                            if (hasC2kOverImsModem() && mEccNumberUtils.isCdmaPreferredNumber()
                                    && mDefaultEccPhone != null && isSimLocked(mDefaultEccPhone)) {
                                // check if location at "460"
                                boolean isLocationPlmn460 =
                                        checkLocatedPlmnMcc(mDefaultEccPhone, "460");
                                if (isLocationPlmn460) {
                                    Phone firstInServicePhone = getFirstInServicePhone();
                                    if (firstInServicePhone != null) {
                                        prefPhone = firstInServicePhone;
                                        Log.d(TAG, "for G+C project w/o G+C phone,allSimInserted,"
                                                + "default phone locked,"
                                                + "return found in-service phone: "
                                                + firstInServicePhone);
                                    } else if (hasC2kRaf(mDefaultEccPhone)) {
                                        prefPhone = mDefaultEccPhone;
                                        mPrefRat = RAT_CDMA_PREF;
                                        Log.d(TAG, "for G+C project w/o G+C phone,allSimInserted,"
                                                + "default phone locked with C2k RAF,"
                                                + "return default phone: " + mDefaultEccPhone);
                                    } else {
                                        Phone cCapablePhone = getFirstCCapablePhone();
                                        if (cCapablePhone != null && isSimLocked(cCapablePhone)
                                                && (cCapablePhone.getServiceState().getState()
                                                        != ServiceState.STATE_POWER_OFF)) {
                                            prefPhone = cCapablePhone;
                                            mPrefRat = RAT_CDMA_PREF;
                                            Log.d(TAG, "for G+C project w/o G+C phone,"
                                                    + "allSimInserted,"
                                                    + "default phone locked w/o C2k RAF,"
                                                    + "c capable phone locked and not power off,"
                                                    + "return c capable phone:" + cCapablePhone);
                                        } else {
                                            Log.d(TAG, "default phone locked w/o C2k RAF,"
                                                    + "cPhone null or not locked or power off");
                                        }
                                    }
                                } else {
                                    Log.d(TAG, "default phone locked, loc plmn not 460");
                                }
                            }
                            if (prefPhone == null && mPrefRat == 0) {
                                Log.d(TAG, "for G+C project w/o G+C phone,"
                                        + "allSimInserted,return default"
                                        + " phone:" + mDefaultEccPhone);
                                return mDefaultEccPhone;
                            }
                        }
                    } else {
                        // old design: 90/91/92 no need to switch phone here
                        if (!hasC2kOverImsModem()) {
                            return mDefaultEccPhone;
                        }

                        // new design: 93
                        // For numbers(999) as CdmaAlways & GsmPreferred, Always has higher prority
                        if (mEccNumberUtils.isGsmOnlyNumber()) {
                            prefPhone = mGsmPhone;
                            mPrefRat = RAT_GSM_ONLY;
                        } else if (mEccNumberUtils.isCdmaPreferredNumber()) {
                            prefPhone = mCdmaPhone;
                            mPrefRat = RAT_CDMA_PREF;
                        } else if (mEccNumberUtils.isGsmPreferredNumber()) {
                            prefPhone = mGsmPhone;
                            mPrefRat = RAT_GSM_PREF;
                        } else {
                            Log.d(TAG, "for G+C project w/o G+C phone, in Service with SIM,"
                                + " return default phone:" + mDefaultEccPhone);
                            return mDefaultEccPhone;
                        }

                        // Error handling: if ECC number has no rule
                        if (prefPhone == null) {
                            prefPhone = getGsmPhoneAndSwitchToCdmaIfNecessary();
                        }
                    }/* End of allSimInserted */
                }/* End of mIsEccRetry */
            }/* End of isDualPhoneCdmaExist() */

            /// M: CC: Fusion RIL specific @{
            // 93 modem support MD retry ECC from IMS -> CS(C2K/GSM)
            if (hasC2kOverImsModem() && mPrefRat != 0 && prefPhone != null) {
                ((MtkGsmCdmaPhone)prefPhone).mMtkCi.setEccPreferredRat(mPrefRat, null);
            }
            /// @}
            return prefPhone;
        }
    }

    /// M: CC: Fusion RIL specific @{
    private static final int MODE_GSM = 1;
    private static final int MODE_C2K = 4;
    private static final int RAF_C2K = RadioAccessFamily.RAF_IS95A | RadioAccessFamily.RAF_IS95B |
        RadioAccessFamily.RAF_1xRTT | RadioAccessFamily.RAF_EVDO_0 | RadioAccessFamily.RAF_EVDO_A |
        RadioAccessFamily.RAF_EVDO_B | RadioAccessFamily.RAF_EHRPD;

    // If called when allSimInserted = false
    private Phone getGsmPhoneAndSwitchToCdmaIfNecessary() {
        if (mPrefRat == RAT_CDMA_PREF || mPrefRat == RAT_CDMA_ONLY) {
            if (!mTm.hasIccCard(mDefaultEccPhone.getPhoneId())) {
                if (!hasC2kRaf(mDefaultEccPhone)) {
                    Log.d(TAG, "defaulEccPhone is not c2k-enabled, trigger switch");
                    ((MtkGsmCdmaPhone)mDefaultEccPhone).triggerModeSwitchByEcc(MODE_C2K, null);
                }
            } else {
                if (isSimLocked(mDefaultEccPhone) && !hasC2kRaf(mDefaultEccPhone)) {
                    Log.d(TAG, "defaulEccPhone is unable to be switched,"
                            + "try to switch on empty slot");
                    for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                        if (!mTm.hasIccCard(i)) {
                            Log.d(TAG, "trigger switch on slot " + i);
                            MtkGsmCdmaPhone phone = (MtkGsmCdmaPhone)PhoneFactory.getPhone(i);
                            phone.triggerModeSwitchByEcc(MODE_C2K, null);
                            return phone;
                        }
                    }
                }
            }
        }
        return mDefaultEccPhone;
    }
    /// @}

    private boolean checkLocatedPlmnMcc(Phone phone, String mcc) {
        String locatedPlmn = null;
        if (phone != null && phone.getServiceStateTracker() != null
                && phone.getServiceStateTracker() instanceof MtkServiceStateTracker) {
            locatedPlmn = ((MtkServiceStateTracker) phone.getServiceStateTracker())
                    .getLocatedPlmn();
        }
        if (locatedPlmn != null && mcc != null && locatedPlmn.startsWith(mcc)) {
            return true;
        } else {
            return false;
        }
    }

    private Phone getFirstCCapablePhone() {
        if (RatConfiguration.isC2kSupported()) {
            if (PROJECT_SIM_NUM >= 2) {
                int cSlot = SystemProperties.getInt(PROP_C_CAPABILITY_SLOT, 0);
                if (cSlot > 0 && cSlot <= PROJECT_SIM_NUM) {
                    return PhoneFactory.getPhone(cSlot - 1);
                }
                Log.d(TAG, "getFirstCCapablePhone no C phone found by RAF");
            }
        }
        return null;
    }

    private boolean hasC2kRaf(Phone phone) {
        int cSlot = SystemProperties.getInt(PROP_C_CAPABILITY_SLOT, 0);
        return phone != null && phone.getPhoneId() == cSlot - 1;
    }

    private Phone getFirstInServicePhone() {
        for (Phone p : PhoneFactory.getPhones()) {
            if (p.getServiceState().getState() != ServiceState.STATE_IN_SERVICE) {
                continue;
            }
            return p;
        }
        Log.d(TAG, "getFirstInServicePhone no in-service phone found");
        return null;
    }

    private boolean isSimLocked(Phone phone) {
        boolean isLocked = false;
        if (phone == null) {
            return false;
        }
        int simState = mTm.getSimState(phone.getPhoneId());
        if (simState == TelephonyManager.SIM_STATE_PIN_REQUIRED ||
                simState == TelephonyManager.SIM_STATE_PUK_REQUIRED ||
                simState == TelephonyManager.SIM_STATE_NETWORK_LOCKED ||
                simState == TelephonyManager.SIM_STATE_PERM_DISABLED) {
            isLocked = true;
        }
        return isLocked;
    }

    private void generateGCRuleList() {
        if (mGCRuleList != null) {
            mGCRuleList.clear();
        }
        mGCRuleList = new ArrayList<GCRuleHandler>();

        // Select main phone for GSM w/o SIM
        mGCRuleList.add(new MainPhoneNoSimGsmRule());
        // Select default phone if not emergency number of both slots
        mGCRuleList.add(new OneSlotEmergencyNumberRule());
        // Select phone based on only number rule
        mGCRuleList.add(new OnlyNumberRule());
        // Select ECC retry phone
        mGCRuleList.add(new EccRetryRule());
        // Select phone based on GSM/CDMA service state
        mGCRuleList.add(new GCReadyRule());
        mGCRuleList.add(new GsmReadyOnlyRule());
        mGCRuleList.add(new CdmaReadyOnlyRule());
        mGCRuleList.add(new GCUnReadyRule());
    }

    private Phone getPhoneFromGCRuleList() {
        for (GCRuleHandler rule : mGCRuleList) {
            Phone phone = rule.handleRequest();
            if (phone != null) {
                //Log.d(TAG, "getPhoneFromGCRuleList, preferred phone" + phone.getPhoneId());
                return phone;
            }
        }
        return null;
    }

    /**
     * MainPhoneNoSimGsmRule
     */
    class MainPhoneNoSimGsmRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "MainPhoneNoSimGsmRule: handleRequest...");

            boolean noSimInserted = true;
            for (int i = 0; i < PROJECT_SIM_NUM; i++) {
                if (mTm.hasIccCard(i)) {
                    noSimInserted = false;
                    break;
                }
            }

            // Select main capability phone for CMCC lab test
            if (!mIsEccRetry && noSimInserted && mMainPhone != null) {
                if (mEccNumberUtils.isGsmOnlyNumber()) {
                    mPrefRat = RAT_GSM_ONLY;
                    return mMainPhone;
                } else if (mEccNumberUtils.isGsmPreferredNumber()) {
                    mPrefRat = RAT_GSM_PREF;
                    return mMainPhone;
                }
                if (hasC2kOverImsModem()) {
                    // Select main capability phone to avoid turning on radio of both slots
                    // Only apply for 93 MD, for legacy MD phone switching should be controlled
                    // by SwitchPhoneHelper
                    if (mEccNumberUtils.isCdmaPreferredNumber()) {
                        mPrefRat = RAT_CDMA_PREF;
                        if (!hasC2kRaf(mMainPhone)) {
                            Log.d(TAG, "mMainPhone is not c2k-enabled, trigger switch");
                            ((MtkGsmCdmaPhone) mMainPhone).triggerModeSwitchByEcc(MODE_C2K, null);
                        }
                        return mMainPhone;
                    }
                }
            }
            return null;
        }
    }

    /**
     * OneSlotEmergencyNumberRule
     */
    class OneSlotEmergencyNumberRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "OneSlotEmergencyNumberRule: handleRequest...");
            if (!mIsEccRetry) {
                List<Phone> potentialEmergencyPhones = new ArrayList<>();
                for (Phone phone : PhoneFactory.getPhones()) {
                    if (phone.getEmergencyNumberTracker() != null &&
                            phone.getEmergencyNumberTracker().isEmergencyNumber(mNumber, true)) {
                        potentialEmergencyPhones.add(phone);
                    }
                }
                if (potentialEmergencyPhones.size() == 1) {
                    if (mEccNumberUtils.isGsmOnlyNumber()) {
                        mPrefRat = RAT_GSM_ONLY;
                    } else if (mEccNumberUtils.isGsmPreferredNumber()) {
                        mPrefRat = RAT_GSM_PREF;
                    }
                    // No need to handle CDMA preferred case
                    // since 110/119/120/122 must be ECC for both slots when mcc=460
                    Log.d(TAG, "not emergency number for both slots, return phone "
                            + potentialEmergencyPhones.get(0).getPhoneId());
                    return potentialEmergencyPhones.get(0);
                }
            }
            return null;
        }
    }

    /**
     * OnlyNumberRule
     */
    class OnlyNumberRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "OnlyNumberRule: handleRequest...");
            if (mEccNumberUtils.isGsmOnlyNumber()) {
                mPrefRat = RAT_GSM_ONLY;
                return mGsmPhone;
            }
            return null;
        }
    }

    /**
     * ECC retry rule
     */
    class EccRetryRule implements GCRuleHandler {
        public Phone handleRequest() {
            if (mIsEccRetry) {
                Log.d(TAG, "EccRetryRule: handleRequest...");
                // Still specify mPrefRat if the the first attempt is not from preferred RAT
                if (mEccRetryPhone != null
                        && mEccRetryPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA
                        && mEccNumberUtils.isCdmaPreferredNumber()) {
                    mPrefRat = RAT_CDMA_PREF;
                } else if (mEccRetryPhone != null
                        && mEccRetryPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM
                        && mEccNumberUtils.isGsmPreferredNumber()) {
                    mPrefRat = RAT_GSM_PREF;
                }
                return mEccRetryPhone;
            }
            return null;
        }
    }

    /**
     * Only GSM register to network.
     */
    class GsmReadyOnlyRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "GsmReadyOnlyRule: handleRequest...");
            if (mEccNumberUtils.isGsmOnlyNumber()) {
                return null;
            }
            if (isGsmNetworkReady() && !isCdmaNetworkReady()) {
                // do not specific mPrefRat without number rule
                return mGsmPhone;
            }
            return null;
        }
    }

    /**
     * Only CDMA register to network.
     */
    class CdmaReadyOnlyRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "CdmaReadyOnlyRule: handleRequest...");
            if (mEccNumberUtils.isGsmOnlyNumber()) {
                return null;
            }
            if (isCdmaNetworkReady() && !isGsmNetworkReady()) {
                // do not specific mPrefRat without number rule

                // M: For corner cases that ECC is made when CT VoLTE is on but phone type is still
                // CDMA, set preferred RAT.
                if (MTK_CT_VOLTE_SUPPORT
                        && ImsManager.getInstance(mCdmaPhone.getContext(), mCdmaPhone.getPhoneId())
                                .isEnhanced4gLteModeSettingEnabledByUser()) {
                    Log.d(TAG, "CdmaReadyOnlyRule: handleRequest... CT VoLTE");
                    if (mEccNumberUtils.isCdmaPreferredNumber()) {
                        mPrefRat = RAT_CDMA_PREF;
                    } else if (mEccNumberUtils.isGsmPreferredNumber()) {
                        mPrefRat = RAT_GSM_PREF;
                    }
                }
                return mCdmaPhone;
            }
            return null;
        }
    }


    /**
     * CDMA and GSM register to network.
     */
    class GCReadyRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "GCReadyRule: handleRequest...");
            if (mEccNumberUtils.isGsmOnlyNumber()) {
                return null;
            }
            if (isCdmaNetworkReady() && isGsmNetworkReady()) {
                if (mEccNumberUtils.isGsmPreferredNumber()) {
                    mPrefRat = RAT_GSM_PREF;
                    return mGsmPhone;
                }
                if (mEccNumberUtils.isCdmaPreferredNumber()) {
                    mPrefRat = RAT_CDMA_PREF;
                    return mCdmaPhone;
                }
            }
            return null;
        }
    }

    /**
     * Both CDMA and GSM are not ready.
     */
    class GCUnReadyRule implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "GCUnReadyRule: handleRequest...");
            if (mEccNumberUtils.isGsmOnlyNumber()) {
                return null;
            }
            if (!isCdmaNetworkReady() && !isGsmNetworkReady()) {
                if (mEccNumberUtils.isGsmPreferredNumber()) {
                    mPrefRat = RAT_GSM_PREF;
                    return mGsmPhone;
                }
                if (mEccNumberUtils.isCdmaPreferredNumber()) {
                    mPrefRat = RAT_CDMA_PREF;
                    return mCdmaPhone;
                }
            }
            return null;
        }
    }

    /**
     * Handle ECC default case.
     */
    class DefaultHandler implements GCRuleHandler {
        public Phone handleRequest() {
            Log.d(TAG, "Can't got here! something is wrong!");
            return mGsmPhone;
        }
    }

    private boolean isSprintSupport() {
        if ("OP20".equals(SystemProperties.get("persist.vendor.operator.optr", ""))) {
            Log.d(TAG, "isSprintSupport: true");
            return true;
        } else {
            return false;
        }
    }
}
