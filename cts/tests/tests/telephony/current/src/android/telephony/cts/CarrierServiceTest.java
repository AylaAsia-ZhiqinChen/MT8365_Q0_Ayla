/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.telephony.cts;

import static org.junit.Assert.fail;

import android.os.PersistableBundle;
import android.service.carrier.CarrierIdentifier;
import android.service.carrier.CarrierService;

import org.junit.Before;
import org.junit.Test;

public class CarrierServiceTest {
    private CarrierService mCarrierService;

    @Before
    public void setUp() {
        mCarrierService = new TestCarrierService();
    }

    @Test
    public void testNotifyCarrierNetworkChange_true() {
        notifyCarrierNetworkChange(true);
    }

    @Test
    public void testNotifyCarrierNetworkChange_false() {
        notifyCarrierNetworkChange(false);
    }

    private void notifyCarrierNetworkChange(boolean active) {
        try {
            mCarrierService.notifyCarrierNetworkChange(active);
            fail("Expected SecurityException for notifyCarrierNetworkChange(" + active + ")");
        } catch (SecurityException e) { /* Expected */ }
    }

    public static class TestCarrierService extends CarrierService {
        @Override
        public PersistableBundle onLoadConfig(CarrierIdentifier id) { return null; }
    }
}
