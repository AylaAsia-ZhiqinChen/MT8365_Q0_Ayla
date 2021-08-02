package com.mesh.test.provisioner.model;

import java.util.ArrayList;
import java.util.HashSet;
import java.io.Serializable;
import android.bluetooth.mesh.MeshConstants;

public class Model implements Serializable{

    private long mModelID;
    private int mElementAddr;  //the element that the model belongs to
    private int mModelHandle;   //modelHandle
    private boolean isBindAppkey = false;

    //private ArrayList<ModelOperate> modelOperates = new ArrayList<>();
    private HashSet<Integer> mBoundAppKeySet = new HashSet<>();

    public Model() {
    }

    public Model(int id) {
        this.mModelID = id;
        //modelOperates.add(new ModelOperate("Bind appkey"));
        //modelOperates.add(new ModelOperate("Public appkey"));
    }

    public Model(long id, int elementAddr, int handle) {
        this.mModelID = id;
        this.mElementAddr = elementAddr;
        this.mModelHandle = handle;
        //modelOperates.add(new ModelOperate("Bind appkey"));
        //modelOperates.add(new ModelOperate("Public appkey"));
    }

    public String getModelName() {
        switch ((int)this.mModelID) {
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER: return "Configuration Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT: return "Configuration Client";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_HEALTH_SERVER: return "Health Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_HEALTH_CLIENT: return "Health Client";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER: return "Generic OnOff Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT: return "Generic OnOff Client";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_SERVER: return "Generic Level Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT: return "Generic Level Client";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SERVER: return "Generic Power OnOff Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SERVER: return "Generic Power OnOff Setup Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_CLIENT: return "Generic Power OnOff Client";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SERVER: return "Generic Power Level Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SERVER: return "Generic Power Level Setup Server";
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_CLIENT: return "Generic Power Level Client";
            //TODO add more
            default:
                break;
        }
        return null;
    }

    public long getID(){
        return this.mModelID;
    }

    public void setID(long id){
        this.mModelID = id;
    }

    public boolean isBindAppkey(){
        return this.isBindAppkey;
    }

    public void setBindAppkey(boolean isBindAppkey){
        this.isBindAppkey = isBindAppkey;
    }

    public void setElementAddr(int addr) {
        this.mElementAddr = addr;
    }

    public int getElementAddr() {
        return this.mElementAddr;
    }

    public HashSet<Integer> getBoundAppKeySet() {
        return this.mBoundAppKeySet;
    }

    /**
    public void addModelOperate(ModelOperate modelOperate){
        modelOperates.add(modelOperate);
    }
    public ArrayList<ModelOperate> getModelOperates(){
        return modelOperates;
    }
    */

}

