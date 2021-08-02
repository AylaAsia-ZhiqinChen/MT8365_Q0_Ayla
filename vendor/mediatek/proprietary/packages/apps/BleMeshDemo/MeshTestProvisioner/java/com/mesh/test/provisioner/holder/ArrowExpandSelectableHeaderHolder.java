package com.mesh.test.provisioner.holder;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import com.mesh.test.provisioner.util.MeshUtils;
import com.mesh.test.provisioner.R;
import com.mesh.test.provisioner.model.Element;
import com.mesh.test.provisioner.model.Model;
import com.mesh.test.provisioner.model.ModelOperate;
import com.unnamed.b.atv.model.TreeNode;
import com.unnamed.b.atv.printview.PrintView;

public class ArrowExpandSelectableHeaderHolder extends TreeNode.BaseNodeViewHolder<Object> {
    private TextView tvValue;
    private PrintView arrowView;

    public ArrowExpandSelectableHeaderHolder(Context context) {
        super(context);
    }

    @Override
    public View createNodeView(final TreeNode node, Object obj) {
        final LayoutInflater inflater = LayoutInflater.from(context);
        final View view = inflater.inflate(R.layout.layout_selectable_header, null, false);

        tvValue = (TextView) view.findViewById(R.id.node_value);
        if(obj instanceof Element) {
            tvValue.setText(new StringBuffer("Element Address: ")
                            .append(MeshUtils.decimalToHexString("%04X", ((Element)obj).getAddress())).toString());
        } else if (obj instanceof Model){
            tvValue.setText(new StringBuffer("Model ID: ")
                            .append(MeshUtils.decimalToHexString("%04X", ((Model)obj).getID())).toString());
        }

        arrowView = (PrintView) view.findViewById(R.id.arrow_icon);
        arrowView.setPadding(20,0,10,0);
        if (node.isLeaf()) {
            arrowView.setVisibility(View.GONE);
        }
        arrowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                tView.toggleNode(node);
            }
        });


        return view;
    }

    @Override
    public void toggle(boolean active) {
        arrowView.setIconText(context.getResources().getString(active ? R.string.ic_keyboard_arrow_down : R.string.ic_keyboard_arrow_right));
    }

}

