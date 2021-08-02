package com.mesh.test.provisioner.listener;

import com.mesh.test.provisioner.sqlite.Node;
import com.mesh.test.provisioner.sqlite.StorageData;
import com.mesh.test.provisioner.sqlite.NodeData;
import com.mesh.test.provisioner.MeshMessage;
import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import java.util.List;

public interface BluetoothMeshListener {

    void onMeshEnabled();
    void inviteProvisioning(int[] UUID,int bearerType,String address,int addrType,String devName,int netKeyIndex,int appKeyIndex,int position);
    void onProvDone(int address, int[] deviceKey, boolean success, boolean gatt_bearer, double provision_time);
    void onProvScanComplete();
    void onScanUnProvDevice(int[] uuid, int oobInfom, int[] uriHash);
    void onScanUnProvDevicePBGatt(String name, String addr, int addrType, int rssi);
    void onProvStateChanged(Node node, boolean success);
    void sendMessage(MeshMessage msg, int position, int appKeyIndex, int groupAddr, int[] payload);
    void onConfigMsgAck(BluetoothMeshAccessRxMessage msg);
    void onNodeMsgRecieved(BluetoothMeshAccessRxMessage msg);
    void onStorageDataChange(StorageData data, int type);
    void onNodeDataChange(NodeData nodeData, int type);
    void onNodeActiveStatusChange(Node node, int active);
    void onNodeRemoved(boolean result, Node node);
    void onConfigMsgModelSubAdd(Node node, int eleAddr, int subAddr, long modelId);
    void onConfigMsgNetKeyAdd(Node node, int targetNetkeyIdx);
    void onConfigMsgAppKeyAdd(Node node, int targetNetKeyIndex, int appKeyIdx);
    void onConfigMsgCompositionDataGet(Node node, int page);
    void onConfigMsgNodeReset(int position);
    void onKeyRefreshComplete(int netkeyIndex, boolean result);
    void onGroupSendMsg(int status, List<String> lists);
    void GattConnect(int position);
    void GattDisconnect(int position);
    void GattConnectStatusChange(Node node, boolean connect);
    void GattConnectTimeout();
    void dialogFragmentCancel();
    void gattProvisioningConnectFail();
    void gattProxyConnectFail();
    void updateHeartBeatReceive();
    void provisionTimeout(int time);

}
