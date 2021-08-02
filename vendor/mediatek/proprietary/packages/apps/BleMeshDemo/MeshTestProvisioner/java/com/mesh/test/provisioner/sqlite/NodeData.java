package com.mesh.test.provisioner.sqlite;

public class NodeData {
    private String mAddress;
    private Node node;

    public NodeData() {
    }

    public NodeData(Node node) {
        this.node = node;
        this.mAddress = node.getAddr() + "";
    }


    public void setAddress(String mAddress){
        this.mAddress = mAddress;
    }

    public String getAddress() {
        return mAddress + "";
    }

    public void setNode(Node node){
        this.node = node;
    }

    public Node getNode() {
        return node;
    }

}

