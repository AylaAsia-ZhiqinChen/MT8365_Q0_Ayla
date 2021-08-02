package com.mediatek.engineermode.mcfconfig;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.telephony.PhoneStateListener;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.android.internal.telephony.PhoneConstants;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.EmUtils;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.R;

import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class McfConfigActivity extends Activity implements OnClickListener {

    public static final int OTA_FILE_CODE = 0;
    public static final int OPOTA_FILE_CODE = 1;
    public static final int GEN_OPOTA_FILE_CODE = 2;
    public static final String[] OP_OTA_SUFFIX = new String[]{"mcfopota", "bin"};
    public static final String[] OTA_SUFFIX = new String[]{"mcfota", "bin"};
    private static final String TAG = "McfConfig";
    private static final String MCF_CONFIG_SHAREPRE = "mcf_config_settings";
    private static final int SIM_COUNT = 2;
    private static final int MSG_SET_OTA_FILEPATH = 0;
    private static final int MSG_CLEAR_OTA_FILEPATH = 1;
    private static final int MSG_SET_OPOTA_FILEPATH = 2;
    private static final int MSG_CLEAR_OPOTA_FILEPATH = 3;
    private static final int MSG_SET_GEN_OPOTA_FILEPATH = 4;
    private static final int MSG_CLEAR_GEN_OPOTA_FILEPATH = 5;
    private static final int MSG_DUMP_OTA_FILE = 6;
    private static final int MSG_INI_REFRESH = 7;
    private static final int MSG_OTA_QUERY = 8;
    private static final int MSG_OPOTA_QUERY = 9;
    private static final int MSG_GEN_OPOTA_QUERY = 10;
    protected static final int MSG_GET_MCF_STATUS_URC = 11;
    private static final int SHOW_OPOTA_VIEW = 0;
    private static final int SHOW_GENERAL_VIEW = 1;
    private static final String DEFAULT_FILE_PATH = "/mnt/vendor/nvcfg/mdota";
    private static final String SET_OTA_COMMAND = "AT+EMCFC=6,0,1";
    private static final String SET_OPOTA_COMMAND = "AT+EMCFC=6,1,1";
    private static final String CMD_OTA_RETURN = "+EMCFC:";
    private static final String SET_GEN_OPOTA_COMMAND = "AT+EMCFC=6,2,1";
    private static final String DUMP_OTA_COMMAND = "AT+EMCFC=5";
    private static final String INI_REFRESH_COMMAND = "AT+EMCFC=7";
    private static final String OTA_QUERY_COMMAND = "AT+EMCFC=4,0";
    private static final String OPOTA_QUERY_COMMAND = "AT+EMCFC=4,1";
    private static final String GEN_OPOTA_QUERY_COMMAND = "AT+EMCFC=4,2";
    private static final int OTA_WAIT_TIME = 60 * 3;
    private static final int INI_REFRESH_WAIT_TIME = 60 * 5;
    private static final int REBOOT_MODEM_WAIT_TIME = 60;
    private static final String OTA_FILEPATH_KEY = "ota_file_path";
    private static final String GENERAL_OPOTA_FILEPATH_KEY = "general_opota_path";
    private static final String SIM2_OPOTA_FILEPATH_KEY = "sim2_opota_file_path";
    private static final String SIM1_OPOTA_FILEPATH_KEY = "sim1_opota_file_path";
    private static final int STATE_REBOOT_MODEM = 0;
    private static final int STATE_DUMP_OTA = 1;
    private static final int STATE_INI_REFRESH = 3;
    private final int MSG_MODEM_REBOOT_COMPLETE = 100;
    private final int PhoneStateListenerID = PhoneStateListener.LISTEN_RADIO_POWER_STATE_CHANGED;
    private final HashMap<String, String> OtaFilePathType =
            new HashMap<String, String>() {{
                put("0", "Android OTA path");
                put("1", "Runtime path");
            }};
    private final HashMap<Integer, String> mcfOtaDumpResult =
            new HashMap<Integer, String>() {{
                put(0, "SUCCESS");
                put(1, "MCF_NOT_SUPPORT");
                put(2, "VERSION_NOT_MATCH");
                put(3, "WRONG_BUFFER_SIZE");
                put(4, "INVALID_PARAMETER");
                put(5, "READ_NVRAM_FAIL");
                put(6, "WRITE_NVRAM_FAIL");
                put(7, "READ_OTA_FILE_FAIL");
                put(8, "INVALID_SBP_TAG");
                put(9, "INVALID_FILE");
                put(10, "INVALID_ATTR");
                put(11, "TAKE_READ_LOCK_FAIL");
                put(12, "ALLOCATE_BUFFER_FAIL");
                put(13, "ENCRYPTION_FAIL");
                put(14, "DECRYPTION_FAIL");
                put(15, "CHECKSUM_ERROR");
                put(16, "WRITE_DISK_FAIL");
            }};
    long dumpBegain;
    long dumpEnd;
    CountDownTimer timer;
    private Set taskSet = new HashSet<Integer>();
    private SharedPreferences mcfConfigSh;
    private LinearLayout opotaView;
    private LinearLayout generalView;
    private TextView opOtaFile;
    private TextView otaFile;
    private TextView genOpOtaFile;
    private RadioGroup iniStatusGroup;
    private Button addOtaBtn;
    private Button addOpOtaBtn;
    private Button addGenOpOtaBtn;
    private Button applyOtaBtn;
    private Button clearOtaBtn;
    private Button applyOpOtaBtn;
    private Button clearOpOtaBtn;
    private Button applyGenOpOtaBtn;
    private Button clearGenOpOtaBtn;
    private Button dumpOta;
    private Button iniRefreshBtn;
    private Button otaQueryBtn;
    private Button opOtaQueryBtn;
    private int phoneid;
    private int readySim;
    private int viewId;
    private String targetOtaFile;
    private String targetOpOtaFile;
    private String targetGenOpOtaFile;
    private String curOtaFile;
    private String curOpOtaFile;
    private String curGenOpOtaFile;
    private String targetPath;
    private boolean isOtaPathValid = false;
    private boolean isOpOtaPathValid = false;
    private boolean isGenOpOtaPathValid = false;
    private boolean mIsModemEnabled = true;
    private UrcListener mUrcListener;
    private TelephonyManager mTelephonyManager;
    private int iniStatus;
    private ProgressDialog mProgressDialog;
    int mRadioStates;
    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onRadioPowerStateChanged(
                @TelephonyManager.RadioPowerState int state) {
            if (state == TelephonyManager.RADIO_POWER_ON) {
                mRadioStates |= state;
                Elog.v(TAG, "RADIO_POWER_ON: " + state);
                if(mRadioStates > TelephonyManager.RADIO_POWER_ON) {
                    mHandler.sendEmptyMessage(MSG_MODEM_REBOOT_COMPLETE);
                }
            } else if (state == TelephonyManager.RADIO_POWER_OFF) {
                mRadioStates |= state;
                Elog.v(TAG, "RADIO_POWER_OFF " + state);
            } else if (state == TelephonyManager.RADIO_POWER_UNAVAILABLE) {
                mRadioStates |= state;
                Elog.v(TAG, "RADIO_POWER_UNAVAILABLE " + state);
            }
        }
    };
    private final Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = null;
            String return_code = null;
            switch (msg.what) {
                case MSG_SET_OPOTA_FILEPATH:
                case MSG_SET_OTA_FILEPATH:
                case MSG_SET_GEN_OPOTA_FILEPATH:
                    String setStr = msg.what == MSG_SET_OTA_FILEPATH ? "OTA" :
                            (msg.what == MSG_SET_OPOTA_FILEPATH ? "OPOTA" : "General OPOTA");
                    ar = (AsyncResult) msg.obj;
                    boolean applyOTA = false;
                    if (ar.exception == null) {
                        Elog.d(TAG, "ATCmd -> Return " + ar.result.toString());
                        applyOTA = true;
                        if (ar.result != null && ar.result instanceof String[] &&
                                ((String[]) ar.result).length > 0) {
                            Elog.d(TAG, "Apply Result: " + Arrays.toString((String[]) ar.result));
                            String[] results = ((String[]) ar.result)[0].split(",");
                            if (results != null && results[1] != null) {
                                if (results[1].equals("0")) {
                                    applyOTA = true;
                                } else {
                                    applyOTA = false;
                                    return_code = results[1];
                                    break;
                                }
                            }
                        }
                    } else {
                        applyOTA = false;
                        Elog.e(TAG, "Apply " + setStr + " returned exception:" + ar.exception);
                    }
                    if (applyOTA) {
                        new saveOTAPathTask().execute(
                                msg.what == MSG_SET_OTA_FILEPATH ? OTA_FILE_CODE :
                                        (msg.what == MSG_SET_OPOTA_FILEPATH ?
                                                OPOTA_FILE_CODE : GEN_OPOTA_FILE_CODE));
                        EmUtils.showToast("Apply  " + setStr + " Succeed!");
                        rebootModem();
                    } else {
                        showDialog("Apply " + setStr + " ", "Apply " + setStr + " Failed!"
                                + (return_code == null ? "" : " Error Code = " + return_code));
                    }
                    break;
                case MSG_CLEAR_OPOTA_FILEPATH:
                case MSG_CLEAR_OTA_FILEPATH:
                case MSG_CLEAR_GEN_OPOTA_FILEPATH:
                    ar = (AsyncResult) msg.obj;
                    String clearStr = msg.what == MSG_CLEAR_OTA_FILEPATH ? "OTA" :
                            (msg.what == MSG_CLEAR_OPOTA_FILEPATH ? "OPOTA" : "General OPOTA");
                    boolean clearOTA = false;
                    if (ar.exception == null) {
                        clearOTA = true;
                        if (ar.result != null && ar.result instanceof String[] &&
                                ((String[]) ar.result).length > 0) {
                            Elog.d(TAG, "Clear Result: " + Arrays.toString((String[]) ar.result));
                            String[] results = ((String[]) ar.result)[0].split(",");
                            if (results != null && results[1] != null) {
                                if (results[1].equals("0")) {
                                    applyOTA = true;
                                } else {
                                    applyOTA = false;
                                    return_code = results[1];
                                    break;
                                }
                            }
                        }
                    } else {
                        clearOTA = false;
                        Elog.e(TAG, "Clear " + clearStr + " returned exception:" + ar.exception);
                    }
                    if (clearOTA) {
                        if (msg.what == MSG_CLEAR_OTA_FILEPATH) {
                            otaFile.setText("");
                            targetOtaFile = "";
                        } else if (msg.what == MSG_CLEAR_OPOTA_FILEPATH) {
                            opOtaFile.setText("");
                            targetOpOtaFile = "";
                        } else {
                            genOpOtaFile.setText("");
                            targetGenOpOtaFile = "";
                        }
                        new saveOTAPathTask().execute(
                                msg.what == MSG_CLEAR_OTA_FILEPATH ? OTA_FILE_CODE :
                                        (msg.what == MSG_CLEAR_OPOTA_FILEPATH ?
                                                OPOTA_FILE_CODE : GEN_OPOTA_FILE_CODE));
                        EmUtils.showToast("Clear " + clearStr + " Succeed!");
                        rebootModem();
                    } else {
                        showDialog("Clear " + clearStr + " ", "Clear " + clearStr + " Failed!"
                                + (return_code == null ? "" : " Error Code = " + return_code));
                    }
                    break;
                case MSG_MODEM_REBOOT_COMPLETE:
                    if (mIsModemEnabled == true) {
                        Elog.d(TAG, "SIM" + phoneid +
                                " received MODEM_REBOOT_COMPLETE, but skiped!");
                        return;
                    }
                    taskSet.remove(STATE_REBOOT_MODEM);
                    if (timer != null) {
                        timer.cancel();
                        timer = null;
                    }
                    Elog.e(TAG, "Reset Modem Success!");
                    showDialog("Reset Modem", "Reset Modem Completed!");
                    mIsModemEnabled = true;
                    break;
                case MSG_DUMP_OTA_FILE:
                    ar = (AsyncResult) msg.obj;
                    boolean dumpOTA = false;
                    if (ar.exception == null) {
                        dumpOTA = true;
                        if (ar.result != null && ar.result instanceof String[]) {
                            String[] data = (String[]) ar.result;
                            Elog.d(TAG, "ATCommand with Dump OTA returned: " + Arrays.toString
                                    (data));
                        }
                    } else {
                        dumpOTA = false;
                        Elog.e(TAG, "ATCommand with Dump OTA returned exception:" + ar.exception);
                    }
                    if (dumpOTA) {
                        EmUtils.showToast("ATCommand with Dump OTA success!");
                        startTimer(STATE_DUMP_OTA, OTA_WAIT_TIME);
                        showProgressDialog("Waitting", "Dump OTA is ongoing......");
                        taskSet.add(STATE_DUMP_OTA);
                    } else {
                        showDialog("Dump OTA", "Dump OTA failed!");
                        Elog.e(TAG, "ATCommand with Dump OTA failed!");
                    }
                    break;
                case MSG_INI_REFRESH:
                    ar = (AsyncResult) msg.obj;
                    boolean iniRefresh = false;
                    if (ar.exception == null) {
                        iniRefresh = true;
                        if (ar.result != null && ar.result instanceof String[]) {
                            String[] data = (String[]) ar.result;
                            Elog.d(TAG, "AT with ini refresh returned:" + Arrays.toString(data));
                        }
                    } else {
                        iniRefresh = false;
                        Elog.e(TAG, "AT with ini refresh returned exception:" + ar.exception);
                    }
                    if (iniRefresh) {
                        EmUtils.showToast("ATCommand with ini refresh success!");
                        startTimer(STATE_INI_REFRESH, INI_REFRESH_WAIT_TIME);
                        taskSet.add(STATE_INI_REFRESH);
                    } else {
                        showDialog("INI Refresh", "INI Refresh failed!");
                        Elog.e(TAG, "ATCommand with INI Refresh failed!");
                    }
                    break;
                case MSG_OTA_QUERY:
                case MSG_OPOTA_QUERY:
                case MSG_GEN_OPOTA_QUERY:
                    ar = (AsyncResult) msg.obj;
                    boolean queryOta = false;
                    if (ar.exception == null) {
                        if (ar.result != null) {
                            Elog.d(TAG, "Query OTA (" + msg.what + " ): " +
                                    Arrays.toString((String[]) ar.result));
                            String[] results = ((String[]) ar.result)[0]
                                    .substring(CMD_OTA_RETURN.length())
                                    .replaceAll(" ", "").split(",");
                            if (results != null) {
                                queryOta = true;
                                String type = results.length >= 2 ? results[1] : "";
                                String config = results.length >= 3 &&
                                        OtaFilePathType.containsKey(results[2]) ?
                                        OtaFilePathType.get(results[2]) : "";
                                String value = results.length >= 4 ? results[3] : "";
                                value = value.equals("") ? "No File" : config + ":" + value;
                                if (type.equals("0")) {
                                    curOtaFile = value;
                                } else if (type.equals("1")) {
                                    curOpOtaFile = value;
                                } else if (type.equals("2")) {
                                    curGenOpOtaFile = value;
                                }
                            }
                        }
                    } else {
                        queryOta = false;
                        Elog.e(TAG, "AT with ota query returned exception:" + ar.exception);
                    }
                    if (queryOta) {
                        if (msg.what == MSG_OTA_QUERY) {
                            sendATCommand(GEN_OPOTA_QUERY_COMMAND, MSG_GEN_OPOTA_QUERY);
                            break;
                        }
                        EmUtils.showToast("ATCommand with ota query success!");
                        Elog.d(TAG, "OTA: " + curOtaFile + ", OPOTA: " + curOpOtaFile +
                                ", General OPOTA: " + curGenOpOtaFile);
                        showDialog("OTA Query ", msg.what == MSG_GEN_OPOTA_QUERY ?
                                ("OTA: " + curOtaFile +
                                        "\nGeneral OTA by OP: " + curGenOpOtaFile) :
                                ("OTA by OP: " + curOpOtaFile));
                    } else {
                        showDialog("OTA Query", "Query " +
                                (msg.what == MSG_OTA_QUERY ? "OTA" :
                                        (msg.what == MSG_GEN_OPOTA_QUERY ? "General OTA by OP" :
                                                "OTA by OP")) + " failed!");
                        Elog.e(TAG, "ATCommand with " + msg.what + " query failed!");
                    }
                    break;
                case MSG_GET_MCF_STATUS_URC:
                    int[] data = (int[]) ar.result;
                    Elog.d(TAG, "Readback from urc = " + Arrays.toString(data));
                    if (timer != null) {
                        timer.cancel();
                        timer = null;
                    }
                    if (mProgressDialog != null) {
                        mProgressDialog.dismiss();
                        mProgressDialog = null;
                    }
                    int type = data[1];
                    int result = data[2];
                    switch (type) {
                        case STATE_DUMP_OTA:
                            dumpEnd = System.currentTimeMillis();
                            SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss");
                            Elog.d(TAG, "Dump OTA file end :" + df.format(new Date(dumpBegain)));
                            showDialog("Dump Result", ((result == 0) ?
                                    "Success!" : ("Failed with Error Code: " +
                                    (mcfOtaDumpResult.containsKey(result) ?
                                            mcfOtaDumpResult.get(result) : result))) + "\nDump " +
                                    "costs " +
                                    String.format("%.2f", (dumpEnd - dumpBegain) / 1000.0f) + " s");
                            taskSet.remove(STATE_DUMP_OTA);
                            break;
                        case STATE_INI_REFRESH:
                            Elog.d(TAG, "Ini Refresh end with result:" + result);
                            showDialog("Ini Refresh",
                                    (result == 0 ? "Success!" :
                                            (result == 1 ? "Reset Modem" :
                                                    (result == 2 ? "AP retry" :
                                                            "Failed with error_code: " + result))));
                            taskSet.remove(STATE_INI_REFRESH);
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.em_mcf_config);
        opotaView = (LinearLayout) findViewById(R.id.mcf_opota_view);
        generalView = (LinearLayout) findViewById(R.id.mcf_general_view);
        addOtaBtn = (Button) findViewById(R.id.add_ota_browser);
        addOpOtaBtn = (Button) findViewById(R.id.add_op_ota_browser);
        addGenOpOtaBtn = (Button) findViewById(R.id.add_general_opota_browser);
        otaQueryBtn = (Button) findViewById(R.id.ota_query);
        opOtaQueryBtn = (Button) findViewById(R.id.opota_query);
        addOtaBtn.setOnClickListener(this);
        addOpOtaBtn.setOnClickListener(this);
        addGenOpOtaBtn.setOnClickListener(this);
        applyOtaBtn = (Button) findViewById(R.id.ota_apply);
        clearOtaBtn = (Button) findViewById(R.id.ota_clear);
        applyOpOtaBtn = (Button) findViewById(R.id.opota_apply);
        clearOpOtaBtn = (Button) findViewById(R.id.opota_clear);
        applyGenOpOtaBtn = (Button) findViewById(R.id.gen_opota_apply);
        clearGenOpOtaBtn = (Button) findViewById(R.id.gen_opota_clear);
        dumpOta = (Button) findViewById(R.id.ota_dump);
        iniRefreshBtn = (Button) findViewById(R.id.ini_refresh);
        iniStatusGroup = (RadioGroup) findViewById(R.id.ini_refresh_group);
        applyOtaBtn.setOnClickListener(this);
        clearOtaBtn.setOnClickListener(this);
        applyOpOtaBtn.setOnClickListener(this);
        clearOpOtaBtn.setOnClickListener(this);
        applyGenOpOtaBtn.setOnClickListener(this);
        clearGenOpOtaBtn.setOnClickListener(this);
        dumpOta.setOnClickListener(this);
        iniRefreshBtn.setOnClickListener(this);
        otaQueryBtn.setOnClickListener(this);
        opOtaQueryBtn.setOnClickListener(this);
        iniStatusGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.ini_disable:
                        iniStatus = 0;
                        break;
                    case R.id.ini_enable:
                        iniStatus = 1;
                        break;
                    default:
                        break;
                }
                Elog.d(TAG, "iniStatus:" + iniStatus);
            }

        });
        opOtaFile = (TextView) findViewById(R.id.op_ota_file);
        otaFile = (TextView) findViewById(R.id.ota_file);
        genOpOtaFile = (TextView) findViewById(R.id.general_opota_file);
        Intent intent = getIntent();
        int simType = intent.getIntExtra("mSimType", PhoneConstants.SIM_ID_1);
        if (simType == -1) {
            opotaView.setVisibility(View.GONE);
            generalView.setVisibility(View.VISIBLE);
            phoneid = ModemCategory.getCapabilitySim();
            viewId = SHOW_GENERAL_VIEW;
        } else {
            opotaView.setVisibility(View.VISIBLE);
            generalView.setVisibility(View.GONE);
            phoneid = simType;
            viewId = SHOW_OPOTA_VIEW;
        }
        readySim = isSimReady(phoneid) ? phoneid :
                isSimReady((phoneid + 1) % 2) ?
                        (phoneid + 1) % 2 : ModemCategory.getCapabilitySim();
        Elog.d(TAG, "Selected: Sim" + phoneid + ", Ready: Sim" + readySim);
        mTelephonyManager = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        mTelephonyManager.listen(mPhoneStateListener, PhoneStateListenerID);

        EmUtils.registerForUrcInfo(readySim, mHandler, MSG_GET_MCF_STATUS_URC);
        mUrcListener = new UrcListener();
        mTelephonyManager = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        targetPath = DEFAULT_FILE_PATH;

        mcfConfigSh = getSharedPreferences(MCF_CONFIG_SHAREPRE,
                android.content.Context.MODE_PRIVATE);
        new loadOTAPathTask().execute();
    }

    public boolean isSimReady(int slotId) {
        TelephonyManager telephonyManager = (TelephonyManager) this
                .getSystemService(this.TELEPHONY_SERVICE);
        int status = TelephonyManager.SIM_STATE_UNKNOWN;
        if (slotId < 0) {
            status = telephonyManager.getSimState();
        } else {
            status = telephonyManager.getSimState(slotId);
        }
        Elog.i(TAG, "slotId = " + slotId + ",simStatus = " + status);
        if (status == TelephonyManager.SIM_STATE_ABSENT)
            return false;
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Elog.d(TAG, "onDestroy");
        taskSet.clear();
        EmUtils.unregisterForUrcInfo(readySim);

    }

    @Override
    public void onBackPressed() {
        if (taskSet.contains(STATE_DUMP_OTA)) {
            showDialog("Please Wait", "Dump OTA is ongoing......");
        } else if (taskSet.contains(STATE_REBOOT_MODEM)) {
            showDialog("Please Wait", "Reboot modem is ongoing......");
        } else {
            this.finish();
        }
    }

    private void selectFile(int requestCode) {
        Elog.d(TAG, "[selectFile] storagePath: " + targetPath);
        McfFileSelectActivity.actionStart(McfConfigActivity.this, targetPath, requestCode);
    }

    private void showDialog(String title, String msg) {
        if (!isFinishing()) {
            AlertDialog dialog = new AlertDialog.Builder(this).setCancelable(
                    true).setTitle(title).setMessage(msg).
                    setPositiveButton(android.R.string.ok, null).create();
            dialog.show();
        }
    }

    private void showProgressDialog(String title, String msg) {
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(this);
        }
        mProgressDialog.setTitle(title);
        mProgressDialog.setMessage(msg);
        mProgressDialog.setCancelable(false);
        mProgressDialog.setIndeterminate(true);
        mProgressDialog.show();
        Elog.d(TAG, "After timer.start");
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode != Activity.RESULT_OK) {
            Elog.e(TAG, "[onActivityResult] error, resultCode: " + resultCode);
            super.onActivityResult(requestCode, resultCode, data);
            return;
        }
        Uri uri = data.getData();
        Elog.d(TAG, "[getSelectedFilePath] uri: " +
                ((uri != null) ? (uri.toString()) : "NULL"));
        if (uri == null) {
            return;
        }
        String srcOtaPath = uri.getPath();
        Elog.i(TAG, "[onActivityResult] otaFile: " + srcOtaPath);
        int ret = -1;
        switch (requestCode) {
            case OTA_FILE_CODE:
                isOtaPathValid = checkPathValid(srcOtaPath, OTA_SUFFIX);
                if (isOtaPathValid) {
                    otaFile.setText(srcOtaPath);
                    targetOtaFile = srcOtaPath;
                    Elog.d(TAG, "isOtaPathValid: " + isOtaPathValid +
                            ",targetOtaPath :" + targetOtaFile);
                } else {
                    otaFile.setText("");
                    showDialog("Select OTA Path: ", "Invalid File! (ex:*.mcfota)");
                }
                break;
            case OPOTA_FILE_CODE:
                isOpOtaPathValid = checkPathValid(srcOtaPath, OP_OTA_SUFFIX);
                if (isOpOtaPathValid) {
                    opOtaFile.setText(srcOtaPath);
                    targetOpOtaFile = srcOtaPath;
                    Elog.d(TAG, "isOpOtaPathValid: " + isOpOtaPathValid +
                            ",targetOpOtaPath :" + targetOpOtaFile);
                } else {
                    opOtaFile.setText("");
                    showDialog("Select OP-OTA Path: ", "Invalid File! (ex:*.mcfopota)");
                }
                break;
            case GEN_OPOTA_FILE_CODE:
                isGenOpOtaPathValid = checkPathValid(srcOtaPath, OP_OTA_SUFFIX);
                if (isGenOpOtaPathValid) {
                    genOpOtaFile.setText(srcOtaPath);
                    targetGenOpOtaFile = srcOtaPath;
                    Elog.d(TAG, "isGenOpOtaPathValid: " + isGenOpOtaPathValid +
                            ",targetGenOpOtaFile :" + targetGenOpOtaFile);
                } else {
                    genOpOtaFile.setText("");
                    showDialog("Select OP-OTA Path: ", "Invalid File! (ex:*.mcfopota)");
                }
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    private boolean checkPathValid(String filePath, String[] suffix) {
        if (suffix == null) return true;
        String fileExtension = FileUtils.getFileExtension(filePath);
        if (fileExtension != null && Arrays.asList(suffix).contains(fileExtension)) {
            return true;
        }
        Elog.e(TAG, "[checkPathValid] file path is InValid " + filePath
                + " suffixList:" + Arrays.toString(suffix));
        return false;
    }

    private void sendATCommand(String cmd, int what) {
        String[] atCmd = new String[]{cmd, CMD_OTA_RETURN};
        Elog.e(TAG, "[sendATCommand] cmd: " + Arrays.toString(atCmd));
        EmUtils.invokeOemRilRequestStringsEm(phoneid, atCmd, mHandler.obtainMessage(what));
    }

    private void rebootModem() {
        Elog.d(TAG, "[rebootModem] begining ...");
        mIsModemEnabled = false;
        taskSet.add(STATE_REBOOT_MODEM);
        if (EmUtils.ifAirplaneModeEnabled()) {
            Elog.d(TAG, "turn off rf");
            EmUtils.setAirplaneModeEnabled(false);
        }
        EmUtils.rebootModem();
        startTimer(STATE_REBOOT_MODEM, REBOOT_MODEM_WAIT_TIME);
    }

    public void getSelectedFilePath(Uri contentUri) {
        Elog.d(TAG, "[getSelectedFilePath] uri: " +
                ((contentUri != null) ? (contentUri.toString()) : "NULL"));
        if (contentUri == null) {
            return;
        }
        Elog.d(TAG, "[getUriForFile] path :" + contentUri.getPath());
    }

    public void startTimer(final int type, final int waitTime) {
        timer = new CountDownTimer(waitTime * 1000, 1000) {
            @Override
            public void onTick(long millisUntilFinishied) {
                Elog.d(TAG, "millisUntilFinishied = " + millisUntilFinishied + "ms");
            }

            @Override
            public void onFinish() {
                Elog.d(TAG, "timer Timeout.......");
                mIsModemEnabled = true;
                if (timer != null) {
                    timer.cancel();
                }
                if (mProgressDialog != null) {
                    mProgressDialog.dismiss();
                    mProgressDialog = null;
                }
                if (type == STATE_DUMP_OTA) {
                    EmUtils.showToast("Dump OTA timeout!");
                } else if (type == STATE_INI_REFRESH) {
                    showDialog("Ini Refresh ", "Ini Refresh is ongoing......");
                }
                timer = null;
                taskSet.remove(type);

            }
        };
        timer.start();
        Elog.d(TAG, "Start timer for " + type + ", WAIT_TIME=" + waitTime + "s");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.add_ota_browser:
                selectFile(OTA_FILE_CODE);
                break;
            case R.id.add_op_ota_browser:
                selectFile(OPOTA_FILE_CODE);
                break;
            case R.id.add_general_opota_browser:
                selectFile(GEN_OPOTA_FILE_CODE);
                break;
            case R.id.ota_apply:
                targetOtaFile = (String) otaFile.getText();
                isOtaPathValid = checkPathValid(targetOtaFile, OTA_SUFFIX);
                String setOtaCmd = SET_OTA_COMMAND + ",";
                if (!isOtaPathValid) {
                    showDialog("Apply OTA Error", "OTA File is invalid!");
                    return;
                }
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                if (targetOtaFile == null || targetOtaFile.trim().equals("")) {
                    setOtaCmd += "\"\",0";
                } else {
                    int dirPos = targetOtaFile.lastIndexOf(DEFAULT_FILE_PATH) +
                            DEFAULT_FILE_PATH.length() + 1;
                    setOtaCmd += ("\"" +
                            (isOtaPathValid && dirPos >= 0 ?
                                    targetOtaFile.substring(dirPos) : "") + "\",0");
                }
                Elog.d(TAG, "ATCommand: " + setOtaCmd);
                sendATCommand(setOtaCmd, MSG_SET_OTA_FILEPATH);
                break;
            case R.id.ota_clear:
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                String clearOtaCmd = (SET_OTA_COMMAND + ",\"\",0");
                sendATCommand(clearOtaCmd, MSG_CLEAR_OTA_FILEPATH);
                break;
            case R.id.opota_apply:
                targetOpOtaFile = (String) opOtaFile.getText();
                isOpOtaPathValid = checkPathValid(targetOpOtaFile, OP_OTA_SUFFIX);
                String setOpOtaCmd = SET_OPOTA_COMMAND + ",";
                if (!isOpOtaPathValid) {
                    showDialog("Apply Op-OTA Error", "OP-OTA File is invalid!");
                    return;
                }
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                if (targetOpOtaFile == null || targetOpOtaFile.trim().equals("")) {
                    setOpOtaCmd += "\"\",0";
                } else {
                    int opDirPos = targetOpOtaFile.lastIndexOf(DEFAULT_FILE_PATH) +
                            DEFAULT_FILE_PATH.length() + 1;
                    setOpOtaCmd += ("\"" +
                            (isOpOtaPathValid && opDirPos >= 0 ?
                                    targetOpOtaFile.substring(opDirPos) : "") +
                            "\",0");
                }
                sendATCommand(setOpOtaCmd, MSG_SET_OPOTA_FILEPATH);
                break;
            case R.id.opota_clear:
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                String clearOpOtaCmd = (SET_OPOTA_COMMAND + ",\"\",0");
                sendATCommand(clearOpOtaCmd, MSG_CLEAR_OPOTA_FILEPATH);
                break;
            case R.id.gen_opota_apply:
                targetGenOpOtaFile = (String) genOpOtaFile.getText();
                isGenOpOtaPathValid = checkPathValid(targetGenOpOtaFile, OP_OTA_SUFFIX);
                String setGenOpOtaCmd = SET_GEN_OPOTA_COMMAND + ",";
                if (!isGenOpOtaPathValid) {
                    showDialog("Apply General Op-OTA Error", "OP-OTA File is invalid!");
                    return;
                }
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                if (targetGenOpOtaFile == null || targetGenOpOtaFile.trim().equals("")) {
                    setGenOpOtaCmd += "\"\",0";
                } else {
                    int opDirPos = targetGenOpOtaFile.lastIndexOf(DEFAULT_FILE_PATH) +
                            DEFAULT_FILE_PATH.length() + 1;
                    setGenOpOtaCmd += ("\"" +
                            (isGenOpOtaPathValid && opDirPos >= 0 ?
                                    targetGenOpOtaFile.substring(opDirPos) : "") +
                            "\",0");
                }
                sendATCommand(setGenOpOtaCmd, MSG_SET_GEN_OPOTA_FILEPATH);
                break;
            case R.id.gen_opota_clear:
                if (taskSet.contains(STATE_REBOOT_MODEM)) {
                    showDialog("Please Wait", "Reboot modem is ongoing......");
                    return;
                }
                String clearGenOpOtaCmd = (SET_GEN_OPOTA_COMMAND + ",\"\",0");
                sendATCommand(clearGenOpOtaCmd, MSG_CLEAR_GEN_OPOTA_FILEPATH);
                break;
            case R.id.ota_dump:
                String dumpOtaCmd = DUMP_OTA_COMMAND;
                dumpBegain = System.currentTimeMillis();
                SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss");
                Elog.d(TAG, "Dump OTA file begain :" + df.format(new Date(dumpBegain)));
                sendATCommand(dumpOtaCmd, MSG_DUMP_OTA_FILE);
                break;
            case R.id.ini_refresh:
                Elog.d(TAG, "Ini refresh begain, iniStatus:" + iniStatus);
                sendATCommand(INI_REFRESH_COMMAND + "," + iniStatus, MSG_INI_REFRESH);
                break;
            case R.id.ota_query:
                Elog.d(TAG, "Ota query begain");
                sendATCommand(OTA_QUERY_COMMAND, MSG_OTA_QUERY);
                break;
            case R.id.opota_query:
                Elog.d(TAG, "OpOta query begain");
                sendATCommand(OPOTA_QUERY_COMMAND, MSG_OPOTA_QUERY);
                break;
        }
    }

    private void getSharedPreference() {
        targetOtaFile = mcfConfigSh.getString(OTA_FILEPATH_KEY, "");
        targetGenOpOtaFile = mcfConfigSh.getString(GENERAL_OPOTA_FILEPATH_KEY, "");
        if (phoneid == PhoneConstants.SIM_ID_2) {
            targetOpOtaFile = mcfConfigSh.getString(SIM2_OPOTA_FILEPATH_KEY, "");
        } else {
            targetOpOtaFile = mcfConfigSh.getString(SIM1_OPOTA_FILEPATH_KEY, "");
        }
    }

    private void saveSharedPreference(int actionCode) {
        SharedPreferences.Editor editor = mcfConfigSh.edit();
        if (actionCode == OPOTA_FILE_CODE) {
            if (phoneid == PhoneConstants.SIM_ID_2) {
                editor.putString(SIM2_OPOTA_FILEPATH_KEY, targetOpOtaFile);
            } else {
                editor.putString(SIM1_OPOTA_FILEPATH_KEY, targetOpOtaFile);
            }
            Elog.d(TAG, "[saveSharedPreference] Save opOtaFile success !" +
                    " opota_file_path: SIM" + phoneid + ":[" + targetOpOtaFile + "]");
        } else if (actionCode == OTA_FILE_CODE) {
            editor.putString(OTA_FILEPATH_KEY, targetOtaFile);
            Elog.d(TAG, "[saveSharedPreference] Save otaFile success " +
                    OTA_FILEPATH_KEY + ":" + targetOtaFile);
        } else if (actionCode == GEN_OPOTA_FILE_CODE) {
            editor.putString(GENERAL_OPOTA_FILEPATH_KEY, targetGenOpOtaFile);
            Elog.d(TAG, "[saveSharedPreference] Save genOpOtaFile success ! " +
                    GENERAL_OPOTA_FILEPATH_KEY + ":" + targetGenOpOtaFile);
        }
        editor.commit();
    }

    public String byteArrayToStr(byte[] byteArray) {
        if (byteArray == null) {
            return null;
        }
        String str = new String(byteArray);
        return str;
    }

    class saveOTAPathTask extends AsyncTask<Integer, Void, Void> {

        @Override
        protected Void doInBackground(Integer... params) {
            Elog.d(TAG, "[saveOTAPathTask] " + params[0]);
            saveSharedPreference(params[0]);
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            Elog.d(TAG, "Save OTA file path success!");
        }
    }

    class loadOTAPathTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected Void doInBackground(Void... params) {
            getSharedPreference();
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            otaFile.setText(targetOtaFile);
            opOtaFile.setText(targetOpOtaFile);
            genOpOtaFile.setText(targetGenOpOtaFile);
            Elog.d(TAG, "Load OTA file path success! OtaFile: " + targetOtaFile
                    + ", OpOtaFile: " + targetOpOtaFile
                    + ",GenOpOtaFile: " + targetGenOpOtaFile);
        }
    }

    /**
     * Urc listener.
     */
    class UrcListener extends PhoneStateListener {

        @Override
        public void onOemHookRawEvent(byte[] msg) {
            String str = byteArrayToStr(msg);
            Elog.d(TAG, "Readback from urc = " + str);
            if (str.startsWith("+EMCFRPT")) {
                if (timer != null) {
                    timer.cancel();
                    timer = null;
                }
                if (mProgressDialog != null) {
                    mProgressDialog.dismiss();
                    mProgressDialog = null;
                }
                if (str.split(":") != null && str.split(":").length > 0) {
                    str = str.split(":")[1];
                    String[] data = str.split(",");
                    int type = Integer.valueOf(data[0].trim());
                    int result = Integer.valueOf(data[1].trim());
                    switch (type) {
                        case STATE_DUMP_OTA:
                            dumpEnd = System.currentTimeMillis();
                            SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss");
                            Elog.d(TAG, "Dump OTA file end :" + df.format(new Date(dumpBegain)));
                            showDialog("Dump Result", ((result == 0) ?
                                    "Success!" : ("Failed with Error Code: " +
                                    (mcfOtaDumpResult.containsKey(result) ?
                                            mcfOtaDumpResult.get(result) : result))) + "\nDump " +
                                    "costs " +
                                    String.format("%.2f", (dumpEnd - dumpBegain) / 1000.0f) + " s");
                            taskSet.remove(STATE_DUMP_OTA);
                            break;
                        case STATE_INI_REFRESH:
                            Elog.d(TAG, "Ini Refresh end with result:" + result);
                            showDialog("Ini Refresh",
                                    (result == 0 ? "Success!" :
                                            (result == 1 ? "Reset Modem" :
                                                    (result == 2 ? "AP retry" :
                                                            "Failed with error_code: " + result))));
                            taskSet.remove(STATE_INI_REFRESH);
                            break;
                    }
                }
            }
        }
    }
}