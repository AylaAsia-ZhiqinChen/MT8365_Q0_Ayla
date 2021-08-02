/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.exchange.adapter;

import com.android.emailcommon.provider.Policy;
import com.android.exchange.adapter.ProvisionParser;

import android.content.Context;
import android.test.AndroidTestCase;
import android.test.suitebuilder.annotation.SmallTest;

import java.io.ByteArrayInputStream;
import java.io.IOException;

/**
 * You can run this entire test case with:
 *   runtest -c com.android.exchange.adapter.ProvisionParserTests exchange
 */
@SmallTest
public class ProvisionParserTests extends AndroidTestCase {
    // <Sync><Status>1</Status></Sync>
    private final byte[] wbxmlBytes = new byte[] {
        0x03, 0x01, 0x6A, 0x00, 0x45, 0x4E, 0x03, 0x31, 0x00, 0x01, 0x01};
    private final ByteArrayInputStream mTestInputStream =
        new ByteArrayInputStream(wbxmlBytes);

    // A good sample of an Exchange 2003 (WAP) provisioning document for end-to-end testing
    private String mWapProvisioningDoc1 =
        "<wap-provisioningdoc>" +
            "<characteristic type=\"SecurityPolicy\"><parm name=\"4131\" value=\"0\"/>" +
            "</characteristic>" +
            "<characteristic type=\"Registry\">" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\AE\\" +
                        "{50C13377-C66D-400C-889E-C316FC4AB374}\">" +
                    "<parm name=\"AEFrequencyType\" value=\"1\"/>" +
                    "<parm name=\"AEFrequencyValue\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"DeviceWipeThreshold\" value=\"20\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"CodewordFrequency\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"MinimumPasswordLength\" value=\"8\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"PasswordComplexity\" value=\"0\"/>" +
                "</characteristic>" +
            "</characteristic>" +
        "</wap-provisioningdoc>";

    // Provisioning document with passwords turned off
    private String mWapProvisioningDoc2 =
        "<wap-provisioningdoc>" +
            "<characteristic type=\"SecurityPolicy\"><parm name=\"4131\" value=\"1\"/>" +
            "</characteristic>" +
            "<characteristic type=\"Registry\">" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\AE\\" +
                        "{50C13377-C66D-400C-889E-C316FC4AB374}\">" +
                    "<parm name=\"AEFrequencyType\" value=\"0\"/>" +
                    "<parm name=\"AEFrequencyValue\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"DeviceWipeThreshold\" value=\"20\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"CodewordFrequency\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"MinimumPasswordLength\" value=\"8\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"PasswordComplexity\" value=\"0\"/>" +
                "</characteristic>" +
            "</characteristic>" +
        "</wap-provisioningdoc>";

    // Provisioning document with simple password, 4 chars, 5 failures
    private String mWapProvisioningDoc3 =
        "<wap-provisioningdoc>" +
            "<characteristic type=\"SecurityPolicy\"><parm name=\"4131\" value=\"0\"/>" +
            "</characteristic>" +
            "<characteristic type=\"Registry\">" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\AE\\" +
                        "{50C13377-C66D-400C-889E-C316FC4AB374}\">" +
                    "<parm name=\"AEFrequencyType\" value=\"1\"/>" +
                    "<parm name=\"AEFrequencyValue\" value=\"2\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"DeviceWipeThreshold\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\">" +
                    "<parm name=\"CodewordFrequency\" value=\"5\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"MinimumPasswordLength\" value=\"4\"/>" +
                "</characteristic>" +
                "<characteristic type=\"HKLM\\Comm\\Security\\Policy\\LASSD\\LAP\\lap_pw\">" +
                    "<parm name=\"PasswordComplexity\" value=\"1\"/>" +
                "</characteristic>" +
            "</characteristic>" +
        "</wap-provisioningdoc>";

    public void testWapProvisionParser1() throws IOException {
        ProvisionParser parser = new ProvisionParser(getContext(), mTestInputStream);
        parser.parseProvisionDocXml(mWapProvisioningDoc1);
        Policy policy = parser.getPolicy();
        assertNotNull(policy);
        // Check the settings to make sure they were parsed correctly
        assertEquals(5*60, policy.mMaxScreenLockTime);  // Screen lock time is in seconds
        assertEquals(8, policy.mPasswordMinLength);
        assertEquals(Policy.PASSWORD_MODE_STRONG, policy.mPasswordMode);
        assertEquals(20, policy.mPasswordMaxFails);
        assertTrue(policy.mRequireRemoteWipe);
    }

    public void testWapProvisionParser2() throws IOException {
        ProvisionParser parser = new ProvisionParser(getContext(), mTestInputStream);
        parser.parseProvisionDocXml(mWapProvisioningDoc2);
        Policy policy = parser.getPolicy();
        assertNotNull(policy);
        // Password should be set to none; others are ignored in this case.
        assertEquals(Policy.PASSWORD_MODE_NONE, policy.mPasswordMode);
    }

    public void testWapProvisionParser3() throws IOException {
        ProvisionParser parser = new ProvisionParser(getContext(), mTestInputStream);
        parser.parseProvisionDocXml(mWapProvisioningDoc3);
        Policy policy = parser.getPolicy();
        assertNotNull(policy);
        // Password should be set to simple
        assertEquals(2*60, policy.mMaxScreenLockTime);  // Screen lock time is in seconds
        assertEquals(4, policy.mPasswordMinLength);
        assertEquals(Policy.PASSWORD_MODE_SIMPLE, policy.mPasswordMode);
        assertEquals(5, policy.mPasswordMaxFails);
        assertTrue(policy.mRequireRemoteWipe);
    }
}
