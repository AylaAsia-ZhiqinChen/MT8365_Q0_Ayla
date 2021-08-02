/**
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */

package android.net.wifi;

/**
 * Interface for Sta state callback.
 *
 * @hide
 */
oneway interface IStaStateCallback
{
   /**
    * Callback invoked to inform clients about the current sta state.
    *
    * @hide
    */
   void onStaToBeOff();
}
