package com.mediatek.apmonitor;

import java.util.ArrayList;

public interface IApmSessionCallback {
    void onQueryKpi(Short msgId);
    void onKpiSubscriptionStateChanged(ArrayList<Short> subscribedMessageList);
}