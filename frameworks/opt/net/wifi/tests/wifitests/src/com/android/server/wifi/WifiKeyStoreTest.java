/*
 * Copyright (C) 2019 The Android Open Source Project
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

package com.android.server.wifi;

import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.AdditionalMatchers.aryEq;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.validateMockitoUsage;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.mockito.Mockito.when;

import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiEnterpriseConfig;
import android.os.Process;
import android.security.Credentials;
import android.security.KeyStore;

import androidx.test.filters.SmallTest;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.security.cert.X509Certificate;

/**
 * Unit tests for {@link com.android.server.wifi.WifiConfigManager}.
 */
@SmallTest
public class WifiKeyStoreTest {
    @Mock private WifiEnterpriseConfig mWifiEnterpriseConfig;
    @Mock private KeyStore mKeyStore;

    private WifiKeyStore mWifiKeyStore;
    private static final String TEST_KEY_ID = "blah";
    private static final String USER_CERT_ALIAS = "aabbccddee";
    private static final String [] USER_CA_CERT_ALIAS = {"aacccddd", "bbbqqqqmmm"};
    private static final String TEST_PACKAGE_NAME = "TestApp";

    /**
     * Setup the mocks and an instance of WifiConfigManager before each test.
     */
    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        mWifiKeyStore = new WifiKeyStore(mKeyStore);

        when(mWifiEnterpriseConfig.getClientCertificateAlias()).thenReturn(USER_CERT_ALIAS);
        when(mWifiEnterpriseConfig.getCaCertificateAliases())
                .thenReturn(USER_CA_CERT_ALIAS);
        when(mKeyStore.put(anyString(), any(), anyInt(), anyInt())).thenReturn(true);
        when(mKeyStore.importKey(anyString(), any(), anyInt(), anyInt())).thenReturn(true);
        when(mWifiEnterpriseConfig.getClientPrivateKey()).thenReturn(FakeKeys.RSA_KEY1);
        when(mWifiEnterpriseConfig.getClientCertificate()).thenReturn(FakeKeys.CLIENT_CERT);
        when(mWifiEnterpriseConfig.getCaCertificate()).thenReturn(FakeKeys.CA_CERT0);
        when(mWifiEnterpriseConfig.getClientCertificateChain())
                .thenReturn(new X509Certificate[] {FakeKeys.CLIENT_CERT});
        when(mWifiEnterpriseConfig.getCaCertificates())
                .thenReturn(new X509Certificate[] {FakeKeys.CA_CERT0});
        when(mWifiEnterpriseConfig.getKeyId(any())).thenReturn(TEST_KEY_ID);
    }

    /**
     * Called after each test
     */
    @After
    public void cleanup() {
        validateMockitoUsage();
    }

    /**
     * Verifies that keys and certs are removed when they were installed by an app.
     */
    @Test
    public void testRemoveKeysForAppInstalledCerts() {
        when(mWifiEnterpriseConfig.isAppInstalledDeviceKeyAndCert()).thenReturn(true);
        when(mWifiEnterpriseConfig.isAppInstalledCaCert()).thenReturn(true);
        mWifiKeyStore.removeKeys(mWifiEnterpriseConfig);

        // Method calls the KeyStore#delete method 4 times, user key, user cert, and 2 CA cert
        verify(mKeyStore).delete(Credentials.USER_PRIVATE_KEY + USER_CERT_ALIAS, Process.WIFI_UID);
        verify(mKeyStore).delete(Credentials.USER_CERTIFICATE + USER_CERT_ALIAS, Process.WIFI_UID);
        verify(mKeyStore).delete(Credentials.CA_CERTIFICATE + USER_CA_CERT_ALIAS[0],
                Process.WIFI_UID);
        verify(mKeyStore).delete(Credentials.CA_CERTIFICATE + USER_CA_CERT_ALIAS[1],
                Process.WIFI_UID);
    }

    /**
     * Verifies that keys and certs are removed when they were installed by an app and not removed
     * when CA certs are installed by the user.
     */
    @Test
    public void testRemoveKeysForMixedInstalledCerts1() {
        when(mWifiEnterpriseConfig.isAppInstalledDeviceKeyAndCert()).thenReturn(true);
        when(mWifiEnterpriseConfig.isAppInstalledCaCert()).thenReturn(false);
        mWifiKeyStore.removeKeys(mWifiEnterpriseConfig);

        // Method calls the KeyStore#delete method 2 times: user key and user cert
        verify(mKeyStore).delete(Credentials.USER_PRIVATE_KEY + USER_CERT_ALIAS, Process.WIFI_UID);
        verify(mKeyStore).delete(Credentials.USER_CERTIFICATE + USER_CERT_ALIAS, Process.WIFI_UID);
        verifyNoMoreInteractions(mKeyStore);
    }

    /**
     * Verifies that keys and certs are not removed when they were installed by the user and
     * removed when CA certs are installed by the app.
     */
    @Test
    public void testRemoveKeysForMixedInstalledCerts2() {
        when(mWifiEnterpriseConfig.isAppInstalledDeviceKeyAndCert()).thenReturn(false);
        when(mWifiEnterpriseConfig.isAppInstalledCaCert()).thenReturn(true);
        mWifiKeyStore.removeKeys(mWifiEnterpriseConfig);

        // Method calls the KeyStore#delete method 2 times: 2 CA certs
        verify(mKeyStore).delete(Credentials.CA_CERTIFICATE + USER_CA_CERT_ALIAS[0],
                Process.WIFI_UID);
        verify(mKeyStore).delete(Credentials.CA_CERTIFICATE + USER_CA_CERT_ALIAS[1],
                Process.WIFI_UID);
        verifyNoMoreInteractions(mKeyStore);
    }

    /**
     * Verifies that keys and certs are not removed when they were installed by the user.
     */
    @Test
    public void testRemoveKeysForUserInstalledCerts() {
        when(mWifiEnterpriseConfig.isAppInstalledDeviceKeyAndCert()).thenReturn(false);
        when(mWifiEnterpriseConfig.isAppInstalledCaCert()).thenReturn(false);
        mWifiKeyStore.removeKeys(mWifiEnterpriseConfig);
        verifyNoMoreInteractions(mKeyStore);
    }

    /**
     * Add two same network credential one is from user saved, the other is from suggestion.
     * Both oh them should be installed successfully and has different alias, and will not override
     * each other.
     */
    @Test
    public void testAddFromBothSavedAndSuggestionNetwork() throws Exception {
        WifiConfiguration savedNetwork = WifiConfigurationTestUtil.createEapNetwork();
        WifiConfiguration suggestionNetwork = new WifiConfiguration(savedNetwork);
        savedNetwork.enterpriseConfig = mWifiEnterpriseConfig;
        suggestionNetwork.enterpriseConfig = mWifiEnterpriseConfig;
        suggestionNetwork.fromWifiNetworkSuggestion = true;
        suggestionNetwork.creatorName = TEST_PACKAGE_NAME;

        assertTrue(mWifiKeyStore.updateNetworkKeys(savedNetwork, null));
        assertTrue(mWifiKeyStore.updateNetworkKeys(suggestionNetwork, null));

        String savedNetworkAlias = savedNetwork.getKeyIdForCredentials(null);
        String savedNetworkCaAlias = savedNetworkAlias;

        String suggestionNetworkAlias = suggestionNetwork.getKeyIdForCredentials(null);
        String suggestionNetworkCaAlias = suggestionNetworkAlias;

        assertNotEquals(savedNetworkAlias, suggestionNetworkAlias);

        verify(mWifiEnterpriseConfig).setClientCertificateAlias(eq(savedNetworkAlias));
        verify(mWifiEnterpriseConfig).setCaCertificateAliases(
                aryEq(new String[] {savedNetworkCaAlias}));

        verify(mWifiEnterpriseConfig).setClientCertificateAlias(eq(suggestionNetworkAlias));
        verify(mWifiEnterpriseConfig).setCaCertificateAliases(
                aryEq(new String[] {suggestionNetworkCaAlias}));
    }
}
