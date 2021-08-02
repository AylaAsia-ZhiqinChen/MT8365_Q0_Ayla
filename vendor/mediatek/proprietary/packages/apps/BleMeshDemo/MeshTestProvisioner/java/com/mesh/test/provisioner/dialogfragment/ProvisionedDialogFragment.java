package com.mesh.test.provisioner.dialogfragment;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Toast;
import android.widget.AdapterView;
import android.content.DialogInterface;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.mesh.test.provisioner.R;
import com.mesh.test.provisioner.holder.ArrowExpandSelectableHeaderHolder;
import com.mesh.test.provisioner.model.Element;
import com.mesh.test.provisioner.sqlite.LouSQLite;
import com.mesh.test.provisioner.sqlite.MyCallBack;
import com.mesh.test.provisioner.sqlite.Node;
import com.unnamed.b.atv.model.TreeNode;
import com.unnamed.b.atv.view.AndroidTreeView;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import com.mesh.test.provisioner.adapter.MessageAdapter;
import com.mesh.test.provisioner.adapter.ConfigAdapter;
import com.mesh.test.provisioner.MeshMessage;
import com.mesh.test.provisioner.listener.BluetoothMeshListener;
import android.widget.ArrayAdapter;
import com.mesh.test.provisioner.MyApplication;
import android.bluetooth.mesh.MeshConstants;
import android.widget.EditText;
import com.mesh.test.provisioner.util.MeshUtils;
import android.graphics.Color;




public class ProvisionedDialogFragment extends DialogFragment implements TreeNode.TreeNodeClickListener, View.OnClickListener , AdapterView.OnItemClickListener{

    private AndroidTreeView androidTreeView;
    private List<TreeNode> treeNodes = new ArrayList<>();
    private BluetoothMeshListener mBluetoothMeshListener;
    private TreeNode root;
    private int position;
    private LinearLayout llShow;
    private LinearLayout llInfo;
    private LinearLayout llConfig;
    private LinearLayout llMessage;
    private TextView tvInfo;
    private TextView tvConfig;
    private TextView tvMessage;
    private TextView tvConnect;
    private TextView tvDisconnect;
    private ListView configListView;
    private ListView messageListView;
    private ConfigAdapter mConfigAdapter;
    private MessageAdapter mMessageAdapter;
    private ArrayList<MeshMessage> messages;
    private ArrayList<Integer> groupAddrs;
    private ArrayList<String> modelDatas = new ArrayList<>();;
    private List<String> configDatas;
    private String appkeyIndex = "0x0000";
    private String groupAddr = "0x0000";
    private String elementAddr = "0x0000";
    private String modelId = "0x0000";
    private String address;
    private int messageState = 1;
    private String netkey_add_index = "0x0000";
    private String appkey_add_index = "0x0000";
    private ArrayList<Element> elements;
    private Node node = null;


    public static ProvisionedDialogFragment newInstance(int position,ArrayList<MeshMessage> messages,ArrayList<Integer> groupAddrDatas) {
        ProvisionedDialogFragment provisionedDialogFragment = new ProvisionedDialogFragment();
        Bundle args = new Bundle();
        args.putInt("position" , position);
        args.putSerializable("message",messages);
        args.putIntegerArrayList("groupAddr",groupAddrDatas);
        provisionedDialogFragment.setArguments(args);
        return provisionedDialogFragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        position = getArguments().getInt("position");
        messages = (ArrayList<MeshMessage>)getArguments().getSerializable("message");
        groupAddrs = getArguments().getIntegerArrayList("groupAddr");
        configDatas = Arrays.asList(getContext().getResources().getStringArray(R.array.config));
        node = MyApplication.nodeList.get(position);
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog dialog = new Dialog(getContext());
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE); //cancel DialogFragment tile
        dialog.setContentView(R.layout.provisioned_dialog);
        ViewGroup containerView = (ViewGroup) dialog.findViewById(R.id.container);
        llShow = (LinearLayout) dialog.findViewById(R.id.llShow);
        llInfo = (LinearLayout) dialog.findViewById(R.id.llInfo);
        llConfig = (LinearLayout) dialog.findViewById(R.id.llConfig);
        llMessage = (LinearLayout) dialog.findViewById(R.id.llMessage);
        tvInfo = (TextView) dialog.findViewById(R.id.info);
        tvConfig = (TextView) dialog.findViewById(R.id.config);
        tvMessage = (TextView) dialog.findViewById(R.id.message);
        tvConnect = (TextView) dialog.findViewById(R.id.connect);
        tvDisconnect = (TextView) dialog.findViewById(R.id.disconnect);
        configListView = (ListView) dialog.findViewById(R.id.listConfig);
        messageListView = (ListView) dialog.findViewById(R.id.listMessage);
        mMessageAdapter = new MessageAdapter(getContext(),messages);
        mConfigAdapter = new ConfigAdapter(getContext(),configDatas);
        configListView.setAdapter(mConfigAdapter);
        messageListView.setAdapter(mMessageAdapter);
        configListView.setOnItemClickListener(this);
        messageListView.setOnItemClickListener(this);
        tvInfo.setOnClickListener(this);
        tvConfig.setOnClickListener(this);
        tvMessage.setOnClickListener(this);
        tvConnect.setOnClickListener(this);
        tvDisconnect.setOnClickListener(this);
        if (node.isConfigSuccess()) {
            if(node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
                tvConnect.setVisibility(View.GONE);
                tvDisconnect.setVisibility(View.GONE);
            }else if(node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT){
                if(MyApplication.GattConnectStatus == MyApplication.PB_GATT_CONNECT) { //PB-GATT connect
                    tvConnect.setEnabled(false);
                    tvDisconnect.setEnabled(true);
                }else if(MyApplication.GattConnectStatus == MyApplication.PB_GATT_DISCONNECT) { //PB-GATT disconnect
                    tvConnect.setEnabled(true);
                    tvDisconnect.setEnabled(false);
                }else { //should not happen
                    tvConnect.setEnabled(true);
                    tvDisconnect.setEnabled(true);
                }
            }
        } else {
            tvConnect.setEnabled(false);
            tvDisconnect.setEnabled(false);
        }

        root = TreeNode.root();
        setData();
        androidTreeView = new AndroidTreeView(getActivity(), root);
        androidTreeView.setDefaultAnimation(true);
        androidTreeView.setUse2dScroll(true);
        androidTreeView.setDefaultContainerStyle(R.style.TreeNodeStyleCustom);
        androidTreeView.setDefaultNodeClickListener(ProvisionedDialogFragment.this);
        androidTreeView.setDefaultViewHolder(ArrowExpandSelectableHeaderHolder.class);
        containerView.addView(androidTreeView.getView());
        androidTreeView.setUseAutoToggle(false);
        dialog.getWindow().getAttributes().gravity = Gravity.CENTER;
        return dialog;
    }

    @Override
    public void onStart() {
        super.onStart();
        Window window = getDialog().getWindow();

        WindowManager.LayoutParams attributes = window.getAttributes();

        attributes.width = WindowManager.LayoutParams.MATCH_PARENT;
        attributes.height = WindowManager.LayoutParams.WRAP_CONTENT;
        window.setAttributes(attributes);

    }

    private void setData() {
        elements = (MyApplication.nodeList).get(position).getElements();
        for(int i=0;i<elements.size();i++) {
            TreeNode s = new TreeNode(elements.get(i)).setViewHolder(
                new ArrowExpandSelectableHeaderHolder(getActivity()));
            fillFolder(s);
            treeNodes.add(s);
        }
        root.addChildren(treeNodes);

    }

    private void fillFolder(TreeNode folder) {
        TreeNode currentNode = folder;
        TreeNode file = null;
        Element element = (Element)(currentNode.getValue());
        for(int i = 0; i<element.getModels().size(); i++) {
            file = new TreeNode(element.getModels().get(i));
            currentNode.addChild(file);
        }
    }

    public void setBluetoothMeshListener(BluetoothMeshListener mBluetoothMeshListener) {
        this.mBluetoothMeshListener = mBluetoothMeshListener;
    }


    @Override
    public void onClick(TreeNode node, Object value) {
        int level = node.getLevel();
        switch (level) {
            case 3:
                //Toast toast = Toast.makeText(getActivity(), ((ModelOperate) value).getModuleOperateValue(), Toast.LENGTH_SHORT);
                //Toast toast = Toast.makeText(getActivity(), (((Model) (node.getParent()).getValue())).getModuleValue(), Toast.LENGTH_SHORT);
                //Toast toast = Toast.makeText(getActivity(), ((ModelOperate) node.getValue()).getModuleOperateValue(), Toast.LENGTH_SHORT);
                //Toast toast = Toast.makeText(getActivity(), ((Model) node.getParent().getValue()).getModuleValue()+"", Toast.LENGTH_SHORT);
                //int moduleId = node.getParent().getId() - 1;
                //int elementId = node.getParent().getParent().getId() - 1;
                //Toast toast = Toast.makeText(getActivity(), node.getPath(), Toast.LENGTH_SHORT);
                //toast.show();
                break;
            default:
                androidTreeView.toggleNode(node);

        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.info:
                llShow.setVisibility(View.GONE);
                llInfo.setVisibility(View.VISIBLE);
               break;
            case R.id.config:
                llShow.setVisibility(View.GONE);
                llConfig.setVisibility(View.VISIBLE);
               break;
            case R.id.message:
                llShow.setVisibility(View.GONE);
                llMessage.setVisibility(View.VISIBLE);
               break;
            case R.id.connect:
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.GattConnect(position);
                }
                tvConnect.setEnabled(false);
                tvDisconnect.setEnabled(false);
               break;
            case R.id.disconnect:
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.GattDisconnect(position);
                }
                tvConnect.setEnabled(false);
                tvDisconnect.setEnabled(false);
               break;
            default:
                break;
        }
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int pos, long id) {
        switch (parent.getId()) {
            case R.id.listConfig:
                switch (configDatas.get(pos)) {
                    case "Config Model Subscription Add":
                        elements = (MyApplication.nodeList).get(position).getElements();
                        if(elements.size() <= 0) {
                            Toast toast = Toast.makeText(getActivity(), "Element is null , please click Config Composition Data Get", Toast.LENGTH_SHORT);
                            break;
                        }
                        showModelSubscriptionAdd();
                        break;
                    case "Config NetKey Add":
                        showNetKeyAdd();
                        break;
                    case "Config AppKey Add":
                        showAppKeyAdd();
                        break;
                    case "Config Composition Data Get":
                        if(mBluetoothMeshListener != null) {
                            mBluetoothMeshListener.onConfigMsgCompositionDataGet(MyApplication.nodeList.get(position), 0);
                        }
                        break;
                    case "Config Node Reset":
                        if(mBluetoothMeshListener != null) {
                            mBluetoothMeshListener.onConfigMsgNodeReset(position);
                            dismiss();
                        }
                        break;
                    default:
                        break;
                }
               break;
            case R.id.listMessage:
                MeshMessage msg = messages.get(pos);
                switch (msg.getOpCode()) {
                    case MeshConstants.MESH_MSG_GENERIC_ONOFF_GET:
                        showGetMessageDialog(msg);
                        break;
                    case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET:
                    case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE:
                        showSetMessageDialog(msg);
                        break;
                        //Add more...
                    default:
                        break;
                }
            default:
                break;
        }

    }

    @Override
    public void onCancel(DialogInterface dialog) {
        super.onCancel(dialog);
        if(mBluetoothMeshListener != null) {
            mBluetoothMeshListener.dialogFragmentCancel();
        }
    }

    public void updateGattConnectStatus(boolean connect) {
        tvConnect.setEnabled(!connect);
        tvDisconnect.setEnabled(connect);
    }

    public void gattConnectTimeout() {
        tvConnect.setEnabled(true);
        tvDisconnect.setEnabled(false);
    }

    private void showModelSubscriptionAdd() {
            ArrayList<Element> mElementList = MyApplication.nodeList.get(position).getElements();
            ArrayList<String> elementAddress = new ArrayList<>();
            for(int i = 0;i < mElementList.size();i++) {
                elementAddress.add(MeshUtils.decimalToHexString("%04X", mElementList.get(i).getAddress()));
            }
            modelDatas.clear();
            for(int j = 0;j<mElementList.get(0).getModels().size();j++) {
                modelDatas.add(MeshUtils.decimalToHexString("%04X", mElementList.get(0).getModels().get(j).getID()));
            }
            elementAddr = MeshUtils.decimalToHexString("%04X", mElementList.get(0).getAddress());
            ArrayAdapter<String> elementAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,elementAddress);
            final ArrayAdapter<String> modelAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,modelDatas);
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View dialoglayout = inflater.inflate(R.layout.model_sub_add, null);
            builder.setView(dialoglayout);
            builder.setCancelable(false);
            final EditText etInput = (EditText) dialoglayout.findViewById(R.id.addressET);
            Spinner elementSpinner = (Spinner) dialoglayout.findViewById(R.id.elementAddrSpinner);
            elementSpinner.setAdapter(elementAdapter);
            elementSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    elementAddr = elementAddress.get(position);
                    modelDatas.clear();
                    for(int i = 0;i<mElementList.get(position).getModels().size();i++) {
                        modelDatas.add(MeshUtils.decimalToHexString("%04X", mElementList.get(position).getModels().get(i).getID()));
                    }
                    modelAdapter.notifyDataSetChanged();
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {

                }
            });
            Spinner modelIdSpinner = (Spinner) dialoglayout.findViewById(R.id.modelidSpinner);
            modelIdSpinner.setAdapter(modelAdapter);
            modelIdSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    modelId = modelDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {

                }
            });
            builder.setPositiveButton("OK", null);
            builder.setNegativeButton("Cancel", null);
            final AlertDialog dialog = builder.create();
            dialog.show();
            dialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    address = etInput.getText().toString();
                    if(!MeshUtils.isHexNumber(address)) {
                        etInput.setText("");
                        etInput.setHintTextColor(Color.RED);
                        Toast.makeText(getActivity(), "Please input 4 valid hex number", Toast.LENGTH_SHORT).show();
                        return;
                    }
                    int subAddr = (int)MeshUtils.hexSrtingToDecimal("0x" + address);
                    if(mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.onConfigMsgModelSubAdd(MyApplication.nodeList.get(position),
                            (int)MeshUtils.hexSrtingToDecimal(elementAddr),
                            subAddr,
                            MeshUtils.hexSrtingToDecimal(modelId));

                    }
                    dialog.dismiss();
                }
            });

    }


    private void showAppKeyAdd() {
            ArrayList<String> netkeyIndexDatas = new ArrayList<>();
            for(int i = 0;i < MyApplication.netkeyindex; i++ ) {
                netkeyIndexDatas.add(MeshUtils.decimalToHexString("%04x",i));
            }
            ArrayList<String> appkeyIndexDatas = new ArrayList<>();
            for(int j = 0;j < MyApplication.appkeyindex; j++ ) {
                appkeyIndexDatas.add(MeshUtils.decimalToHexString("%04x",j));
            }
            ArrayAdapter<String> netkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
            ArrayAdapter<String> appkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,appkeyIndexDatas);
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View dialoglayout = inflater.inflate(R.layout.config_appkey_index , null);
            builder.setView(dialoglayout);
            builder.setCancelable(false);
            Spinner netkeySpinner = (Spinner) dialoglayout.findViewById(R.id.netkeyindexSpinner);
            netkeySpinner.setAdapter(netkeyAdapter);
            netkeySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    netkey_add_index = netkeyIndexDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {
                    netkey_add_index = "0x0000";
                }
            });
            Spinner appkeySpinner = (Spinner) dialoglayout.findViewById(R.id.appkeyindexSpinner);
            appkeySpinner.setAdapter(appkeyAdapter);
            appkeySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    appkey_add_index = appkeyIndexDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {
                    appkey_add_index = "0x0000";
                }
            });

            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface arg0, int arg1) {
                    if(mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.onConfigMsgAppKeyAdd(MyApplication.nodeList.get(position),
                            (int)MeshUtils.hexSrtingToDecimal(netkey_add_index),
                            (int)MeshUtils.hexSrtingToDecimal(appkey_add_index));
                    }

                }
            });

            builder.setNegativeButton("Cancel", null);

            builder.create().show();

    }


    private void showNetKeyAdd() {
            ArrayList<String> netkeyIndexDatas = new ArrayList<>();
            for(int i = 0;i < MyApplication.netkeyindex; i++ ) {
                netkeyIndexDatas.add(MeshUtils.decimalToHexString("%04x",i));
            }
            ArrayAdapter<String> netkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View dialoglayout = inflater.inflate(R.layout.config_netkey_index, null);
            builder.setView(dialoglayout);
            builder.setCancelable(false);
            Spinner netkeySpinner = (Spinner) dialoglayout.findViewById(R.id.netkeyindexSpinner);
            netkeySpinner.setAdapter(netkeyAdapter);
            netkeySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    netkey_add_index= netkeyIndexDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {
                    netkey_add_index = "0x0000";
                }
            });

            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface arg0, int arg1) {
                    if(mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.onConfigMsgNetKeyAdd(MyApplication.nodeList.get(position),
                            (int)MeshUtils.hexSrtingToDecimal(netkey_add_index));
                    }

                }
            });

            builder.setNegativeButton("Cancel", null);

            builder.create().show();

    }

    private void showGetMessageDialog(MeshMessage msg) {
            ArrayList<String> appkeyIndexDatas = new ArrayList<String>();
            ArrayList<String> groupAddrDatas = new ArrayList<String>();
            for(int i = 0; i < msg.getBoundAppKeyList().size(); i++) {
                appkeyIndexDatas.add(MeshUtils.decimalToHexString("%04X",msg.getBoundAppKeyList().get(i)));
            }
            for(int j = 0; j < groupAddrs.size(); j++) {
                groupAddrDatas.add(MeshUtils.decimalToHexString("%04X",groupAddrs.get(j)));
            }
            ArrayAdapter<String> appkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,appkeyIndexDatas);
            ArrayAdapter<String> groupAddrAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,groupAddrDatas);
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View dialoglayout = inflater.inflate(R.layout.onoff_get_message, null);
            builder.setView(dialoglayout);
            builder.setCancelable(false);
            Spinner appkeySpinner = (Spinner) dialoglayout.findViewById(R.id.appkeyIndexSpinner);
            appkeySpinner.setAdapter(appkeyAdapter);
            appkeySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    appkeyIndex = appkeyIndexDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {
                    appkeyIndex = "0x0000";
                }
            });
            groupAddr = "0x0000";
            Spinner groupAddrSpinner = (Spinner) dialoglayout.findViewById(R.id.groupAddrSpinner);
            groupAddrSpinner.setAdapter(groupAddrAdapter);
            groupAddrSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                    groupAddr = groupAddrDatas.get(position);
                }

                @Override
                public void onNothingSelected(AdapterView<?> parent) {
                    groupAddr = "0x0000";
                }
            });
            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface arg0, int arg1) {
                    if(mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.sendMessage(msg, position,
                            (int)MeshUtils.hexSrtingToDecimal(appkeyIndex),
                            (int)MeshUtils.hexSrtingToDecimal(groupAddr),
                            null);
                    }

                }
            });

            builder.setNegativeButton("Cancel", null);

            builder.create().show();

    }


    private void showSetMessageDialog(MeshMessage msg) {
        ArrayList<String> appkeyIndexDatas = new ArrayList<String>();
        ArrayList<String> groupAddrDatas = new ArrayList<String>();
        for(int i = 0; i < msg.getBoundAppKeyList().size(); i++) {
            appkeyIndexDatas.add(MeshUtils.decimalToHexString("%04X",msg.getBoundAppKeyList().get(i)));
        }

        for(int j = 0; j < groupAddrs.size(); j++) {
            groupAddrDatas.add(MeshUtils.decimalToHexString("%04X",groupAddrs.get(j)));
        }
        ArrayAdapter<String> appkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,appkeyIndexDatas);
        ArrayAdapter<String> groupAddrAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,groupAddrDatas);
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View dialoglayout = inflater.inflate(R.layout.onoff_set_message, null);
        builder.setView(dialoglayout);
        builder.setCancelable(false);

        RadioGroup onoffRG = (RadioGroup) dialoglayout.findViewById(R.id.onoffRG);
        final RadioButton onRB = (RadioButton)dialoglayout.findViewById(R.id.onRB);
        final RadioButton offRB = (RadioButton)dialoglayout.findViewById(R.id.offRB);
        onoffRG.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                if(checkedId == onRB.getId()) {
                    messageState = 1;
                }else if(checkedId == offRB.getId()){
                    messageState = 0;
                }
            }
        });
        Spinner appkeySpinner = (Spinner) dialoglayout.findViewById(R.id.appkeyIndexSpinner);
        appkeySpinner.setAdapter(appkeyAdapter);
        appkeySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                appkeyIndex = appkeyIndexDatas.get(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                appkeyIndex = "0x0000";
            }
        });
        groupAddr = "0x0000";
        Spinner groupAddrSpinner = (Spinner) dialoglayout.findViewById(R.id.groupAddrSpinner);
        groupAddrSpinner.setAdapter(groupAddrAdapter);
        groupAddrSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                groupAddr = groupAddrDatas.get(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                groupAddr = "0x0000";
            }
        });


        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface arg0, int arg1) {
                if(mBluetoothMeshListener != null) {
                    int[] payload = new int[1];
                    payload[0] = messageState;
                    mBluetoothMeshListener.sendMessage(msg, position,
                        (int)MeshUtils.hexSrtingToDecimal(appkeyIndex),
                        (int)MeshUtils.hexSrtingToDecimal(groupAddr),
                        payload);
                    messageState = 1;
                }

            }
        });

        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface arg0, int arg1) {
                messageState = 1;
            }
        });

        builder.create().show();
    }
}
