package com.st.android.nfc_extensions;

import android.util.Log;
import java.util.ArrayList;
import java.util.List;

/**
 * This class is used when the list of pipes for a given host is requested.
 *
 * <p>It contains a class of type {@link ItemInfo},
 *
 * <p>a List of {@link ItemInfo} containing data for all the pipes of an host,
 *
 * <p>and the number of pipes in that List.
 */
public class PipesInfo {
    String tag = "NfcPipesInfo";
    private static final boolean DBG = true;

    /**
     * This class contains information for a single pipe:
     *
     * <p>The identity of the pipe
     *
     * <p>The State of the pipe (opened, opened and created, opened/created and RF active)
     *
     * <p>The identity of the source host
     *
     * <p>The identity of the source gata
     *
     * <p>The identity of the destination host.
     */
    public class ItemInfo {
        public String pipeId;
        public String pipeState;
        public String sourceHost;
        public String sourceGate;
        public String destHost;
    }

    private List<ItemInfo> info = new ArrayList<ItemInfo>();
    // private int storedPipes;
    private int nbPipes;

    /**
     * Constructor for the {@link PipesInfo} object
     *
     * @param nb This parameter indicates how many pipes the targeted host has.
     * @return
     */
    public PipesInfo(int nb) {
        if (DBG) Log.i(tag, "Contructor - nbPipes = " + nb);
        nbPipes = nb;
    }

    /**
     * Returns formated information about the gate located at index {@link idx} in the info field.
     *
     * @param idx This parameter identifies an entry in the info field of the class {@link
     *     PipesInfo}.
     * @return A String containing all the information about the gate located at index {@link idx}.
     */
    public String getPipeData(int idx) {
        String data =
                "    Gate: 0x"
                        + info.get(idx).sourceGate
                        + "\n      pipe Id: 0x"
                        + info.get(idx).pipeId
                        + " - "
                        + info.get(idx).pipeState
                        + "\n      Source Host: "
                        + info.get(idx).sourceHost
                        + " - Dest Host: "
                        + info.get(idx).destHost;

        return data;
    }

    /**
     * This method is used to retrieve the number of pipes owned by the targeted host.
     *
     * @return A Int for the number of pipes owned by the host.
     */
    public int getNbPipes() {
        return nbPipes;
    }

    /**
     * This method is used to retrieve information from the field info of the class {@link
     * PipesInfo} for the pipe stored at index {@link pipe_idx}.
     *
     * @param idx This parameter identifies an entry in the info field of the class {@link
     *     PipesInfo}.
     * @return An object of type {@link ItemInfo}.
     */
    public ItemInfo getPipeInfo(int pipe_idx) {
        if (DBG) Log.i(tag, "getPipeInfo()");
        return info.get(pipe_idx);
    }

    /**
     * This method stores information contained in the {@link data} parameter inside the info field
     * of the {@link PipesInfo} class for the pipe identified by the {@link pipe_id} parameter.
     *
     * @param pipe_id This parameter identifies the pipe.
     * @param data This parameter is an array of bytes containing information (pipe state, source
     *     gate, host) about the pipe identified by the parameter {@link pipe_id}.
     */
    public void setPipeInfo(int pipe_id, byte[] data) {
        if (DBG) Log.i(tag, "setPipeInfo() - for pipe " + pipe_id);
        ItemInfo item = new ItemInfo();

        item.pipeId = String.format("%02X", pipe_id & 0xFF);

        switch (data[0]) // pipe state
        {
            case 0x2:
                item.pipeState = "Pipe Created";
                break;
            case 0x6:
                item.pipeState = "Pipe Created/Opened";
                break;
            case (byte) 0x86:
                item.pipeState = "Pipe Created/Opened/RF active";
                break;
            default:
                item.pipeState = "Unknwon pipe state";
                break;
        }

        if (DBG) Log.i(tag, "setPipeInfo() - pipe state is " + item.pipeState);

        switch (data[1]) // source host
        {
            case 0x0:
                item.destHost = "NFCC";
                break;
            case 0x01:
                item.sourceHost = "Device Host";
                break;
            case 0x02:
                item.sourceHost = "UICC";
                break;
            case (byte) 0xc0:
                item.sourceHost = "eSE";
                break;
            default:
                item.sourceHost = "Unknwon Host";
                break;
        }

        if (DBG) Log.i(tag, "setPipeInfo() - source host is " + item.sourceHost);

        item.sourceGate = String.format("%02X", data[2] & 0xFF); // source gate
        String gateName = "";

        switch (data[2]) {
            case 0x04:
                gateName = " - Loopback mgmt";
                break;
            case 0x05:
                gateName = " - Identity mgmt";
                break;
            case 0x11:
                gateName = " - Reader B";
                break;
            case 0x12:
                gateName = " - Reader 15693";
                break;
            case 0x13:
                gateName = " - Reader A";
                break;
            case 0x14:
                gateName = " - Reader F";
                break;
            case 0x15:
                gateName = " - Reader A'";
                break;
            case 0x21:
                gateName = " - Card B";
                break;
            case 0x22:
                gateName = " - Card B'";
                break;
            case 0x23:
                gateName = " - Card A";
                break;
            case 0x24:
                gateName = " - Card F";
                break;
            case 0x41:
                gateName = " - Connectivity";
                break;
            case (byte) 0xF0:
                gateName = " - APDU";
                break;
        }
        item.sourceGate += gateName;
        if (DBG) Log.i(tag, "setPipeInfo() - source gate is " + item.sourceGate);

        switch (data[3]) // dest host
        {
            case 0x0:
                item.destHost = "NFCC";
                break;
            case 0x01:
                item.destHost = "Device Host";
                break;
            case 0x02:
                item.destHost = "UICC";
                break;
            case (byte) 0xc0:
                item.destHost = "eSE";
                break;
            default:
                item.destHost = "Unknwon Host";
                break;
        }

        if (DBG) Log.i(tag, "setPipeInfo() - destination host is " + item.destHost);

        info.add(item);
    }
}
