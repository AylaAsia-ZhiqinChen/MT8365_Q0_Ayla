package com.mesh.test.provisioner.dialogfragment;

import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.mesh.test.provisioner.MainActivity;
import com.mesh.test.provisioner.MyApplication;
import com.mesh.test.provisioner.R;
import com.mesh.test.provisioner.listener.BluetoothMeshListener;

import java.util.ArrayList;
import java.util.List;
import com.mesh.test.provisioner.util.MeshUtils;
import android.bluetooth.mesh.MeshConstants;
import com.mesh.test.provisioner.sqlite.UnProvisionedDevice;
import android.widget.LinearLayout;


public class UnprovisionedDialogFragment extends DialogFragment implements View.OnClickListener, AdapterView.OnItemSelectedListener {

    private Button btCancel;
    private Button btConfirm;
    private UnProvisionedDevice mUnProvisionedDevice;
    private LinearLayout lluuid;
    private LinearLayout lladdress;
    private TextView tvUUID;
    private TextView tvAddress;
    private Spinner netkeyIndexSpinner;
    private ArrayAdapter<String> netkeyAdapter;
    private ArrayAdapter<String> appkeyAdapter;
    private Spinner appkeyIndexSpinner;
    private List<String> netkeyIndexDatas = new ArrayList<String>();
    private List<String> appkeyIndexDatas = new ArrayList<String>();
    private BluetoothMeshListener mBluetoothMeshListener;
    private int position;
    private String netkeyIndex = "0x0000";
    private String appkeyIndex = "0x0000";
    private int netkeyIndexSize;
    private int appkeyIndexSize;

    public static UnprovisionedDialogFragment newInstance(int position,UnProvisionedDevice mUnProvisionedDevice,int netkeyIndexSize,int appkeyIndexSize) {
        UnprovisionedDialogFragment unprovisionedDialogFragment = new UnprovisionedDialogFragment();
        Bundle args = new Bundle();
        args.putInt("position",position);
        args.putSerializable("unprovisioneddevice",mUnProvisionedDevice);
        args.putInt("netkeyIndexSize",netkeyIndexSize);
        args.putInt("appkeyIndexSize",appkeyIndexSize);
        unprovisionedDialogFragment.setArguments(args);
        return unprovisionedDialogFragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mUnProvisionedDevice = (UnProvisionedDevice)getArguments().getSerializable("unprovisioneddevice");
        position = getArguments().getInt("position");
        netkeyIndexSize = getArguments().getInt("netkeyIndexSize");
        appkeyIndexSize = getArguments().getInt("appkeyIndexSize");
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog dialog = new Dialog(getContext());
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE); //cancel DialogFragment tile
        dialog.setContentView(R.layout.unprovisioned_dialog);
        tvUUID = (TextView) dialog.findViewById(R.id.uuid);
        tvAddress = (TextView) dialog.findViewById(R.id.address);
        lluuid = (LinearLayout) dialog.findViewById(R.id.lluuid);
        lladdress = (LinearLayout) dialog.findViewById(R.id.lladdress);
        if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV) {
            tvUUID.setText(MeshUtils.intArrayToString(mUnProvisionedDevice.getUUID(), true));
            lladdress.setVisibility(View.GONE);
        }else if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT) {
            if(null != mUnProvisionedDevice.getGattDevName()) {
                tvAddress.setText(mUnProvisionedDevice.getAddress() + " (" + mUnProvisionedDevice.getGattDevName() + ")");
            }else {
                tvAddress.setText(mUnProvisionedDevice.getAddress() + " ( null ) ");
            }
            lluuid.setVisibility(View.GONE);
        }
        netkeyIndexSpinner = (Spinner) dialog.findViewById(R.id.netkeyIndex);
        appkeyIndexSpinner = (Spinner) dialog.findViewById(R.id.appkeyIndex);
        for(int i = 0;i < netkeyIndexSize ; i++){
            netkeyIndexDatas.add(MeshUtils.decimalToHexString("%04X",i));

        }
        for(int j = 0;j < appkeyIndexSize ; j++){
            appkeyIndexDatas.add(MeshUtils.decimalToHexString("%04X",j));

        }
        netkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
        appkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,appkeyIndexDatas);
        netkeyIndexSpinner.setAdapter(netkeyAdapter);
        appkeyIndexSpinner.setAdapter(appkeyAdapter);
        netkeyIndexSpinner.setOnItemSelectedListener(this);
        appkeyIndexSpinner.setOnItemSelectedListener(this);
        btCancel = (Button)dialog.findViewById(R.id.cancel);
        btConfirm = (Button)dialog.findViewById(R.id.confirm);
        btCancel.setOnClickListener(this);
        btConfirm.setOnClickListener(this);
        // Set centered
        dialog.getWindow().getAttributes().gravity = Gravity.CENTER;
        return dialog;
    }

    @Override
    public void onStart() {
        super.onStart();
        Window window = getDialog().getWindow();
        //window.setBackgroundDrawableResource(android.R.color.transparent);
        WindowManager.LayoutParams attributes = window.getAttributes();
        //attributes.gravity = Gravity.BOTTOM;
        attributes.width = WindowManager.LayoutParams.MATCH_PARENT;
        attributes.height = WindowManager.LayoutParams.WRAP_CONTENT;
        window.setAttributes(attributes);
        //getDialog().getWindow().setLayout(300,200);
    }

    public void setBluetoothMeshListener(BluetoothMeshListener mBluetoothMeshListener) {
        this.mBluetoothMeshListener = mBluetoothMeshListener;
    }


    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.cancel:
                dismiss(); //Close the dialog and trigger the onDismiss() callback function
               break;
            case R.id.confirm:
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.inviteProvisioning(mUnProvisionedDevice.getUUID(),mUnProvisionedDevice.getBearerType(),
                        mUnProvisionedDevice.getAddress(),
                        mUnProvisionedDevice.getAddressType(),
                        mUnProvisionedDevice.getGattDevName(),
                        (int)MeshUtils.hexSrtingToDecimal(netkeyIndex),
                        (int)MeshUtils.hexSrtingToDecimal(appkeyIndex),
                        position);
                }
                dismiss();
               break;
            default:
                break;
        }

    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        switch (parent.getId()) {
            case R.id.netkeyIndex:
                netkeyIndex = netkeyIndexDatas.get(position);
                break;
            case R.id.appkeyIndex:
                appkeyIndex = appkeyIndexDatas.get(position);
                break;
            default:
                break;
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

}
