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
import java.util.List;

public class ConfigAdapter extends BaseAdapter{

    private Context mContext;
    private List<String> configDatas;
    private LayoutInflater mInflater;

    public ConfigAdapter(Context mContext, List<String> configDatas) {
        this.mContext = mContext;
        this.configDatas = configDatas;
        mInflater = LayoutInflater.from(mContext);
    }

    @Override
    public int getCount() {
        return configDatas.size();
    }

    @Override
    public Object getItem(int position) {
        return configDatas.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder;
        if(convertView == null) {
            convertView = mInflater.inflate(R.layout.config_item,parent,false);
            viewHolder = new ViewHolder();
            viewHolder.config = (TextView)convertView.findViewById(R.id.config);
            convertView.setTag(viewHolder);
        }else {
            viewHolder = (ViewHolder) convertView.getTag();
        }
        viewHolder.config.setText(configDatas.get(position));
        return convertView;
    }

    public static class ViewHolder {
        TextView config;
    }
}
