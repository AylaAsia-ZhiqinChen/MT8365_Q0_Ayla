package com.mediatek.apmonitor;

/**
 * APM interface, KPI providers send vendor proprietary broadcast ACTION_APM_KPI_EVENT
 * with permission KPI_RECEIVE_PERMISSION to APM service, including a extra key EXTRA_KPI_SOURCE
 * to identify where the KPI come.
 */
public interface IApmKpiMonitor {
    public static final String ACTION_APM_KPI_EVENT = "mediatek.intent.action.APM_KPI_EVENT";
    // In order to avoid DMC runs during boot-up, use this broadcast to notify APM
    // to create HIDL session to DMC.
    public static final String ACTION_APM_INIT_EVENT = "mediatek.intent.action.APM_INIT_EVENT";
    public static final String APM_SERVICE_PACKAGE = "com.mediatek.apmonitor";

    public static final String EXTRA_KPI_SOURCE = "kpi_source";

    public static final int KPI_SOURCE_UNKNOWN = -1;
    public static final int KPI_SOURCE_FOO_BAR = 0;
}