package com.mesh.test.provisioner.adapter;

import java.util.ArrayList;
import java.util.List;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.RecyclerView.ViewHolder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import com.mesh.test.provisioner.sqlite.Node;
import com.mesh.test.provisioner.util.MeshUtils;
import java.util.LinkedList;
import android.widget.LinearLayout;
import android.bluetooth.mesh.MeshConstants;
import java.util.Arrays;
import com.mesh.test.provisioner.R;
import com.mesh.test.provisioner.listener.RecyclerViewItemClickListener;
import com.mesh.test.provisioner.listener.RecyclerViewItemLongClickListener;


public class NodeAdapter extends RecyclerView.Adapter<NodeRecyclerViewHolder>{

    private static final String TAG = "NodeAdapter";

    private static final boolean DEBUG = true;

    private ArrayList<Node> nodeList;
    private Context mContext;
    private LayoutInflater inflater;
    private RecyclerViewItemClickListener mListener;
    private RecyclerViewItemLongClickListener mLongClickListener;
    public final static int ALL_ENABLE = -1;
    private int mPosition = -1;
    private int id = -1;
    private int itemPositin = -1;

    private void log(String string) {
        if(DEBUG) {
            Log.i(TAG, string);
        }
    }

    public void setOnItemClickListener(RecyclerViewItemClickListener mListener){
        this.mListener = mListener;
    }

    public void setOnItemLongClickListener(RecyclerViewItemLongClickListener mLongClickListener){
        this.mLongClickListener = mLongClickListener;
    }

    public NodeAdapter(Context context, ArrayList<Node> nodeList) {
        this.mContext = context;
        this.nodeList = nodeList;
        inflater = LayoutInflater.from(mContext);
    }

    public void setPosition(int position) {
        this.mPosition = position;
        notifyDataSetChanged();
    }

    public void setPosition(int position,boolean isRefresh) {
        this.mPosition = position;
        if(isRefresh) {
            notifyDataSetChanged();
        }
    }

    public int getPosition() {
        return mPosition;
    }

    public void addNode(Node node) {
        log("addNode mAddr = " + node.getAddr() + " , UUID = " + MeshUtils.intArrayToString(node.getUUID(), true) + " , GATTAdrr = " + node.getGattAddr());
        for(int i = 0 ; i < nodeList.size() ; i++) {
            if(nodeList.get(i).getAddr() == node.getAddr()) {
                log("Duplicate device UUID = " + MeshUtils.intArrayToString(node.getUUID(), true) + " , GATTAdrr = " + node.getGattAddr());
                return;
            }
        }
        nodeList.add(node);
        notifyDataSetChanged();
    }

    public List<Node> getNodes() {
        return nodeList;
    }

    public void removeNode(Node node) {
        log("removeNode mAddr = " + node.getAddr() + " , UUID = " + MeshUtils.intArrayToString(node.getUUID(), true) + " , GATTAdrr = " + node.getGattAddr());
        int index = -1;
        for(int i = 0 ; i < nodeList.size() ; i++) {
            if(nodeList.get(i).getAddr() == node.getAddr()) {
                index = i;
                break;
            }
        }
        if(index != -1) {
            nodeList.remove(index);
            notifyDataSetChanged();
        }else {
            log("not found device need to remove");
        }
    }

    public void removeNodeByIndex(int index) {
        log("removeNodeByIndex  mAddr = " + nodeList.get(index).getAddr() +
            " , UUID = " + MeshUtils.intArrayToString(nodeList.get(index).getUUID(), true) +
            " , GATTAdrr = " + nodeList.get(index).getGattAddr());
        nodeList.remove(index);
        notifyDataSetChanged();
    }

    public synchronized void nodeStateChange(Node node, int active){
        for(int i = 0;i < nodeList.size();i++) {
            Node mNode = nodeList.get(i);
            if(mNode.getAddr()== node.getAddr()){
                mNode.setActiveStatus(active);
                nodeList.set(i,mNode);
                break;
            }
        }
        notifyDataSetChanged();
    }

    public void nodeTimeChange(Node node, int position){
        this.mPosition = position;
        for(int i = 0;i < nodeList.size();i++) {
            Node mNode = nodeList.get(i);
            if(mNode.getAddr()== node.getAddr()){
                nodeList.set(i,mNode);
                break;
            }
        }
        notifyDataSetChanged();
    }

    public synchronized void heartBeatChange() {
        notifyDataSetChanged();
    }

    public void setEnabled(View view , boolean enabled) {
        if(null == view) {
            return;
        }
        if(view instanceof ViewGroup) {
            ViewGroup viewGroup = (ViewGroup)view;
            LinkedList<ViewGroup> queue = new LinkedList<ViewGroup>();
            queue.add(viewGroup);
            while(!queue.isEmpty()) {
                ViewGroup current = queue.removeFirst();
                current.setEnabled(enabled);
                for(int i = 0;i<current.getChildCount();i++) {
                    if(current.getChildAt(i) instanceof ViewGroup ) {
                        queue.addLast((ViewGroup)current.getChildAt(i));
                    } else {
                        current.getChildAt(i).setEnabled(enabled);
                    }
                }
            }
        }else {
            view.setEnabled(enabled);
        }
    }

    public int getId() {
        return id;
    }

    public int getItemPosition() {
        return itemPositin;
    }


    @Override
    public int getItemCount() {
        if(nodeList!=null){
            return nodeList.size();
        }
        return 0;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public void onBindViewHolder(NodeRecyclerViewHolder viewHolder, final int position) {
        Node node = nodeList.get(position);
        if(node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
            viewHolder.textView.setText(MeshUtils.intArrayToString(node.getUUID(), true));
        }else if(node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT){
            if(null != node.getGattDevName()) {
                viewHolder.textView.setText(node.getGattAddr() + " (" + node.getGattDevName() + ")");
            }else {
                viewHolder.textView.setText(node.getGattAddr() + " ( null ) ");
            }

        }
        if(mPosition == position ) {
            setEnabled(viewHolder.linearLayout,false);
            viewHolder.tvState.setText("config...");
        }else {
            setEnabled(viewHolder.linearLayout,true);
            StringBuffer strStatus = new StringBuffer("Status: ");
            if(node.getActiveStatus() == 1) {
                viewHolder.tvState.setText(strStatus.append("Online").toString());
            }else if (node.getActiveStatus() == 0){
                viewHolder.tvState.setText(strStatus.append("Offline").toString());
            } else {
                viewHolder.tvState.setText(strStatus.append("Unknown").toString());
            }
        }
        double mProvisioningTime = node.getProvisioningTime();
        double mConfigTime = node.getConfigTime();
        StringBuffer strTime = new StringBuffer();
        StringBuffer heartBeat = new StringBuffer("HB Period: 32s   ,   Time: ");
        if(!node.isConfigSuccess()) {
            //log("config fail , uuid = " + MeshUtils.intArrayToString(node.getUUID()) + " , address = " + node.getGattAddr());
            strTime.append("Prov: ");
            strTime.append(mProvisioningTime+"s");
            if(mPosition != position) {
                strTime.append("   ,   Config fail!");
            }
            viewHolder.tvTime.setText(strTime.toString());
            viewHolder.tvHeartBeat.setVisibility(View.GONE);
        }else {
            //log("config success , uuid = " + MeshUtils.intArrayToString(node.getUUID()) + " , address = " + node.getGattAddr());
            strTime.append("Prov: ");
            strTime.append(mProvisioningTime+"s");
            strTime.append("   ,   Config: ");
            strTime.append(mConfigTime+"s");
            viewHolder.tvTime.setText(strTime.toString());
            viewHolder.tvHeartBeat.setVisibility(View.VISIBLE);
            heartBeat.append(node.getHeartBeatTime()/1000 + "s   ,   HB Num: ");
            heartBeat.append(node.getCurrentHeartBeatNumber()+"  ,  Cont Lost: ");
            heartBeat.append(node.getContinueLost()+ "  ,  Max Lost: ");
            heartBeat.append(node.getMaxLost()+"");
            viewHolder.tvHeartBeat.setText(heartBeat.toString());
        }

    }


    @Override
    public NodeRecyclerViewHolder onCreateViewHolder(ViewGroup viewGroup, int arg1) {
        View view = inflater.inflate(R.layout.provisioned_list,viewGroup,false);
        id =  view.getId();
        NodeRecyclerViewHolder viewHoler = new NodeRecyclerViewHolder(view,mListener,mLongClickListener);
        return viewHoler;
    }


}

class NodeRecyclerViewHolder extends ViewHolder implements OnClickListener, OnLongClickListener {

    public TextView textView;
    public TextView tvState;
    public TextView tvTime;
    public TextView tvHeartBeat;
    public LinearLayout linearLayout;
    private RecyclerViewItemClickListener mListener;
    private RecyclerViewItemLongClickListener mLongClickListener;

    public NodeRecyclerViewHolder(View itemView, RecyclerViewItemClickListener listener, RecyclerViewItemLongClickListener longClickListener) {
        super(itemView);
        textView = (TextView)itemView.findViewById(R.id.text);
        tvState = (TextView)itemView.findViewById(R.id.status);
        tvTime = (TextView)itemView.findViewById(R.id.time);
        tvHeartBeat = (TextView)itemView.findViewById(R.id.heartbeatReceive);
        linearLayout = (LinearLayout)itemView.findViewById(R.id.llprovisioned);
        this.mListener = listener;
        this.mLongClickListener = longClickListener;
        itemView.setOnClickListener(this);
        itemView.setOnLongClickListener(this);
    }

    @Override
    public boolean onLongClick(View view) {
        if(mLongClickListener != null){
            mLongClickListener.onRecyclerViewItemLongClick(view, getPosition());
        }
        return false;
    }

    @Override
    public void onClick(View view) {
        if(mListener != null){
            mListener.onRecyclerViewItemClick(view, getPosition());
        }
    }

}

