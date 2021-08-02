package com.mesh.test.provisioner;

import com.mesh.test.provisioner.model.Model;
import android.os.Parcel;
import android.os.Parcelable;
import java.util.ArrayList;
import java.util.Iterator;
import java.io.Serializable;


public class MeshMessage implements Serializable{
    private int mOpCode;
    private int mElementAddr;
    private String mDescriptor;
    private Model mModel; //the model instance that the message belongs to
    //private int mNetKeyIndex;
    //private int mAppKeyIndex;

    public MeshMessage(int opCode, int elementAddr, String desc,Model mModel) {
        this.mOpCode = opCode;
        this.mElementAddr = elementAddr;
        this.mDescriptor = desc;
        this.mModel = mModel;
    }

    public int getOpCode() {
        return this.mOpCode;
    }
    public int getElementAddr() {
        return this.mElementAddr;
    }

    public String getDescriptor() {
        return this.mDescriptor;
    }

    public void setMsgModel(Model model) {
        this.mModel = model;
    }

    public Model getMsgModel() {
        return this.mModel;
    }

    public ArrayList<Integer> getBoundAppKeyList() {
        ArrayList<Integer> list = new ArrayList<>();
        for (Iterator it = mModel.getBoundAppKeySet().iterator(); it.hasNext();) {
            list.add((Integer)it.next());
        }
        return list;
    }

}
