/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
package com.mediatek.server;

import android.util.Slog;

import com.mediatek.server.anr.AnrManager;
import com.android.server.power.ShutdownThread;
import com.mediatek.server.am.AmsExt;
import com.mediatek.server.display.MtkDisplayManagerService;
import com.mediatek.server.powerhal.PowerHalManager;
/// M: Privacy protected lock support
import com.mediatek.server.ppl.MtkPplManager;
import com.mediatek.server.powerhal.PowerHalManager;
import com.mediatek.server.wm.WmsExt;
import com.mediatek.server.wm.WindowManagerDebugger;
import com.mediatek.server.pm.PmsExt;

import dalvik.system.PathClassLoader;
import java.lang.reflect.Constructor;

/*
* This class for extend aosp system service which mtk will add some feature or modification.
*/
public class MtkSystemServiceFactory {
    private static MtkSystemServiceFactory sInstance;
    private static Object lock = new Object();
    private WmsExt mWmsExt = new WmsExt();

    public static MtkSystemServiceFactory getInstance() {
        if (null == sInstance) {
            String className = "com.mediatek.server.MtkSystemServiceFactoryImpl";
            Class<?> clazz = null;
            try {
                PathClassLoader classLoader = MtkSystemServer.sClassLoader;
                clazz = Class.forName(className, false, classLoader);
                Constructor constructor = clazz.getConstructor();
                sInstance = (MtkSystemServiceFactory) constructor.newInstance();
            } catch (Exception e) {
                Slog.e("MtkSystemServiceFactory", "getInstance: " + e.toString());
                sInstance = new MtkSystemServiceFactory();
            }
        }
        return sInstance;
    }

    /// M: ANR Debug Mechanism
    public AnrManager makeAnrManager() {
        return new AnrManager();
    }

    public ShutdownThread makeMtkShutdownThread() {
        return new ShutdownThread();
    }

    public PmsExt makePmsExt() {
        return new PmsExt();
    }

    public PowerHalManager makePowerHalManager() {
        return new PowerHalManager();
    }

    /// M: Privacy protected lock support
    public MtkPplManager makeMtkPplManager() {
        return new MtkPplManager();
    }

    public AmsExt makeAmsExt() {
        return new AmsExt();
    }
    public MtkDisplayManagerService makeMtkDisplayManagerService() {
        return new MtkDisplayManagerService();
    }

    public WindowManagerDebugger makeWindowManagerDebugger() {
        return new WindowManagerDebugger();
    }

    public WmsExt makeWmsExt() {
        return mWmsExt;
    }
}
