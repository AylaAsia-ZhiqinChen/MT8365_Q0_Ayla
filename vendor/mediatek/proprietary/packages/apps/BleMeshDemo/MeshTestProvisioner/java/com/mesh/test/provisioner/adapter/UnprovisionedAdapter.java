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
import com.mesh.test.provisioner.sqlite.UnProvisionedDevice;




public class UnprovisionedAdapter extends RecyclerView.Adapter<UnProvisionedRecyclerViewHolder>{

    private static final String TAG = "UnprovisionedAdapter";

    private static final boolean DEBUG = true;

    private Context mContext;
    private LayoutInflater inflater;
    private ArrayList<UnProvisionedDevice> mUnProvisionedDevices = new ArrayList<>();
    public final static int ALL_ENABLE = -1;
    private int mPosition = -1;
    private RecyclerViewItemClickListener mListener;
    private RecyclerViewItemLongClickListener mLongClickListener;
    private int id = -1;

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

    public UnprovisionedAdapter(Context mContext) {
        this.mContext = mContext;
        inflater = LayoutInflater.from(mContext);
    }

    public boolean addUnprovisionedDevice(UnProvisionedDevice mUnProvisionedDevice) {
        log("addUnprovisionedDevice  bearerType = " + mUnProvisionedDevice.getBearerType() +
                 "  , Gattaddress = " + mUnProvisionedDevice.getAddress() +
                 " , uuid = " + MeshUtils.intArrayToString(mUnProvisionedDevice.getUUID(), true));
        for(int i = 0 ; i < mUnProvisionedDevices.size() ; i++) {
            UnProvisionedDevice unProvisionedDevice = mUnProvisionedDevices.get(i);
            if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT) {
                if(unProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT
                    && mUnProvisionedDevice.getAddress() == unProvisionedDevice.getAddress()) {
                    return false;
                }

            }
            if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV) {
                if(unProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV
                    && Arrays.equals(mUnProvisionedDevice.getUUID(),unProvisionedDevice.getUUID())) {
                    return false;
                }

            }

        }
        mUnProvisionedDevices.add(mUnProvisionedDevice);
        log("UnProvisioned Device size = " + mUnProvisionedDevices.size());
        notifyDataSetChanged();
        return true;
    }

    public void removeUnprovisionedDevice(UnProvisionedDevice mUnProvisionedDevice) {
        log("removeUnprovisionedDevice  bearerType = " + mUnProvisionedDevice.getBearerType() +
                 "  , Gattaddress = " + mUnProvisionedDevice.getAddress() +
                 " , uuid = " + MeshUtils.intArrayToString(mUnProvisionedDevice.getUUID(), true));
        for(int i = 0 ; i < mUnProvisionedDevices.size() ; i++) {
            UnProvisionedDevice unProvisionedDevice = mUnProvisionedDevices.get(i);
            if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT) {
                if(unProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT
                    && mUnProvisionedDevice.getAddress() == unProvisionedDevice.getAddress()) {
                    log("remove Gatt device Gattaddress = " + unProvisionedDevice.getAddress());
                    mUnProvisionedDevices.remove(i);
                    log("UnProvisioned Device size = " + mUnProvisionedDevices.size());
                    notifyDataSetChanged();
                    return;
                }

            }
            if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV) {
                if(unProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV
                    && Arrays.equals(mUnProvisionedDevice.getUUID(),unProvisionedDevice.getUUID())) {
                    log("remove ADV device UUID = " + MeshUtils.intArrayToString(unProvisionedDevice.getUUID(), true));
                    mUnProvisionedDevices.remove(i);
                    log("UnProvisioned Device size = " + mUnProvisionedDevices.size());
                    notifyDataSetChanged();
                    return;
                }

            }
        }
        log("not found device need to remove");
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


    public void clear() {
        mUnProvisionedDevices.clear();
        notifyDataSetChanged();
    }

    public UnProvisionedDevice getUnprovisionedDevice(int position) {
        log("getUnprovisionedDevice , postision = " + position + " , UnProvisioned Device size = " + mUnProvisionedDevices.size());
        if(position < mUnProvisionedDevices.size()) {
            return mUnProvisionedDevices.get(position);
        }
        return null;
    }

    public UnProvisionedDevice getUnprovisionedDeviceByUUID(int[] UUID) {
        if(mUnProvisionedDevices.size() > 0) {
            for(int i = 0; i < mUnProvisionedDevices.size(); i++) {
                if(Arrays.equals(UUID, mUnProvisionedDevices.get(i).getUUID())) {
                    return mUnProvisionedDevices.get(i);
                }
            }
        }
        return null;
    }

    public UnProvisionedDevice getUnprovisionedDeviceByGATTAddr(String gattAddr) {
        if(mUnProvisionedDevices.size() > 0) {
            for(int i = 0; i < mUnProvisionedDevices.size(); i++) {
                if(gattAddr.equals(mUnProvisionedDevices.get(i).getAddress())) {
                    return mUnProvisionedDevices.get(i);
                }
            }
        }
        return null;
    }

    public int getPositionByUnProvisioned(UnProvisionedDevice mUnProvisionedDevice) {
        if(mUnProvisionedDevices.size() > 0) {
            for(int i = 0; i < mUnProvisionedDevices.size(); i++) {
                if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_GATT &&
                    mUnProvisionedDevice.getAddress() == mUnProvisionedDevices.get(i).getAddress()) {
                   return i;
                }
                if(mUnProvisionedDevice.getBearerType() == MeshConstants.MESH_BEARER_ADV &&
                    Arrays.equals(mUnProvisionedDevice.getUUID(),mUnProvisionedDevices.get(i).getUUID())) {
                    return i;
                }
            }
        }
        return -1;
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


    @Override
    public int getItemCount() {
        if(mUnProvisionedDevices!=null){
            return mUnProvisionedDevices.size();
        }
        return 0;
    }

    @Override
    public void onBindViewHolder(UnProvisionedRecyclerViewHolder viewHolder, final int position) {
        if(mPosition == position ) {
            setEnabled(viewHolder.linearLayout,false);
            viewHolder.tvStatus.setVisibility(View.VISIBLE);
            if(mUnProvisionedDevices.get(position).getBearerType() == MeshConstants.MESH_BEARER_ADV) {
                viewHolder.tvName.setText(MeshUtils.intArrayToString(mUnProvisionedDevices.get(position).getUUID(), true));
            }else if(mUnProvisionedDevices.get(position).getBearerType() == MeshConstants.MESH_BEARER_GATT) {
                if(null != mUnProvisionedDevices.get(position).getGattDevName()) {
                    viewHolder.tvName.setText(mUnProvisionedDevices.get(position).getAddress() + " (" + mUnProvisionedDevices.get(position).getGattDevName() + ")");
                }else {
                    viewHolder.tvName.setText(mUnProvisionedDevices.get(position).getAddress() + " ( null ) ");
                }
            }
            viewHolder.tvStatus.setText("provisioning...");
        }else {
            setEnabled(viewHolder.linearLayout,true);
            if(mUnProvisionedDevices.get(position).getBearerType() == MeshConstants.MESH_BEARER_ADV) {
                viewHolder.tvName.setText(MeshUtils.intArrayToString(mUnProvisionedDevices.get(position).getUUID(), true));
                viewHolder.tvStatus.setText("PB-ADV");
            }else if(mUnProvisionedDevices.get(position).getBearerType() == MeshConstants.MESH_BEARER_GATT) {
                if(null != mUnProvisionedDevices.get(position).getGattDevName()) {
                    viewHolder.tvName.setText(mUnProvisionedDevices.get(position).getAddress() + " (" + mUnProvisionedDevices.get(position).getGattDevName() + ")");
                }else {
                    viewHolder.tvName.setText(mUnProvisionedDevices.get(position).getAddress() + " ( null ) ");
                }
                viewHolder.tvStatus.setText("PB-GATT");
            }
        }

    }


    @Override
    public UnProvisionedRecyclerViewHolder onCreateViewHolder(ViewGroup viewGroup, int arg1) {
        View view = inflater.inflate(R.layout.unprovisioned_list,viewGroup,false);
        id =  view.getId();
        UnProvisionedRecyclerViewHolder viewHoler = new UnProvisionedRecyclerViewHolder(view,mListener,mLongClickListener);
        return viewHoler;
    }


}

class UnProvisionedRecyclerViewHolder extends ViewHolder implements OnClickListener, OnLongClickListener {

    public TextView tvName;
    public TextView tvStatus;
    public LinearLayout linearLayout;
    private RecyclerViewItemClickListener mListener;
    private RecyclerViewItemLongClickListener mLongClickListener;

    public UnProvisionedRecyclerViewHolder(View itemView, RecyclerViewItemClickListener listener, RecyclerViewItemLongClickListener longClickListener) {
        super(itemView);
        tvName = (TextView)itemView.findViewById(R.id.name);
        tvStatus = (TextView)itemView.findViewById(R.id.status);
        linearLayout = (LinearLayout)itemView.findViewById(R.id.layout);
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
        return true;
    }

    @Override
    public void onClick(View view) {
        if(mListener != null){
            mListener.onRecyclerViewItemClick(view, getPosition());
        }
    }

}

