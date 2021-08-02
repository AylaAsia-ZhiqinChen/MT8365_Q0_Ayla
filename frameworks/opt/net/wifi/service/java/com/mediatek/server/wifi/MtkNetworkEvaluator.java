package com.mediatek.server.wifi;

import android.net.wifi.WifiConfiguration;

import com.android.server.wifi.ScanDetail;
import com.android.server.wifi.WifiNetworkSelector;

import java.util.List;

public class MtkNetworkEvaluator implements WifiNetworkSelector.NetworkEvaluator {
    private static final String NAME = "MtkNetworkEvaluator";

    public MtkNetworkEvaluator() {
    }

    /**
     * Get the evaluator type.
     */
    @Override
    public @EvaluatorId int getId() {
        // MtknetworkEvaluator doesn't invoke onConnectableListener in evaluateNetworks, so this
        // value will not be used, thus an arbitrary value is returned.
        return EVALUATOR_ID_SAVED;
    }

    /**
     * Get the evaluator name.
     */
    @Override
    public String getName() {
        return NAME;
    }

    @Override
    public void update(List<ScanDetail> scanDetails) {
        // Do nothing here
    }

    /**
     * Evaluate all the networks from the scan results.
     *
     * @return configuration of the chosen network;
     *         null if no network in this category is available.
     */
    @Override
    public WifiConfiguration evaluateNetworks(List<ScanDetail> scanDetails,
                    WifiConfiguration currentNetwork, String currentBssid,
                    boolean connected, boolean untrustedNetworkAllowed,
                    OnConnectableListener onConnectableListener) {

        MtkWifiServiceAdapter.triggerNetworkEvaluatorCallBack();

        return null;
    }
}
