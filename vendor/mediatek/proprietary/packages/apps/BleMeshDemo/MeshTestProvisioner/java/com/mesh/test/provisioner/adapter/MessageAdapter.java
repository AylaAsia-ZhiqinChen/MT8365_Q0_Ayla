package com.mesh.test.provisioner.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.mesh.test.provisioner.R;
import com.mesh.test.provisioner.MeshMessage;

import java.util.ArrayList;

public class MessageAdapter extends BaseAdapter{

    private Context mContext;
    private ArrayList<MeshMessage> messages;
    private LayoutInflater mInflater;

    public MessageAdapter(Context mContext, ArrayList<MeshMessage> messages) {
        this.mContext = mContext;
        this.messages = messages;
        mInflater = LayoutInflater.from(mContext);
    }

    @Override
    public int getCount() {
        return messages.size();
    }

    @Override
    public Object getItem(int position) {
        return messages.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder;
        if(convertView == null) {
            convertView = mInflater.inflate(R.layout.message_item,parent,false);
            viewHolder = new ViewHolder();
            viewHolder.msg = (TextView)convertView.findViewById(R.id.msg);
            convertView.setTag(viewHolder);
        }else {
            viewHolder = (ViewHolder) convertView.getTag();
        }
        viewHolder.msg.setText(messages.get(position).getDescriptor());
        return convertView;
    }

    public static class ViewHolder {
        TextView msg;

    }
}
