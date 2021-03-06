# Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from autotest_lib.client.common_lib import error
from autotest_lib.client.bin import utils
from autotest_lib.client.common_lib.cros.network import iw_runner
from autotest_lib.client.common_lib.cros.network import xmlrpc_datatypes
from autotest_lib.server.cros.network import hostap_config
from autotest_lib.server.cros.network import wifi_cell_test_base

class network_WiFi_BSSTMReq(wifi_cell_test_base.WiFiCellTestBase):
    """Tests a BSS Transition Management Request sent from the AP

    This test seeks to associate the DUT with an AP with a set of
    association parameters, create a second AP with a second set of
    parameters but the same SSID, and send a BSS Transition Management Request
    to the client. After that, the client will send a BSS Transition Management
    Response back to the first AP. We seek to observe that the DUT successfully
    connects to the second AP in a reasonable amount of time.
    """

    version = 1
    TIMEOUT_SECONDS = 15

    def dut_sees_bss(self, bssid):
        """
        Check if a DUT can see a BSS in scan results.

        @param bssid: string bssid of AP we expect to see in scan results.
        @return True iff scan results from DUT include the specified BSS.

        """
        runner = iw_runner.IwRunner(remote_host=self.context.client.host)
        is_requested_bss = lambda iw_bss: iw_bss.bss == bssid
        scan_results = runner.scan(self.context.client.wifi_if)
        return scan_results and filter(is_requested_bss, scan_results)

    def run_once(self):
        """Test body."""
        self._router0_conf = hostap_config.HostapConfig(channel=1)
        self._router1_conf = hostap_config.HostapConfig(channel=48,
                             mode=hostap_config.HostapConfig.MODE_11A)
        self._client_conf = xmlrpc_datatypes.AssociationParameters()

        # Configure the initial AP.
        self.context.configure(self._router0_conf)
        router_ssid = self.context.router.get_ssid()

        # Connect to the initial AP.
        self._client_conf.ssid = router_ssid
        self.context.assert_connect_wifi(self._client_conf)

        # Setup a second AP with the same SSID.
        self._router1_conf.ssid = router_ssid
        self.context.configure(self._router1_conf, multi_interface=True)

        # Get BSSIDs of the two APs
        bssid0 = self.context.router.get_hostapd_mac(0)
        bssid1 = self.context.router.get_hostapd_mac(1)

        # Wait for DUT to see the second AP
        utils.poll_for_condition(
            condition=lambda: self.dut_sees_bss(bssid1),
            exception=error.TestFail('Timed out waiting for DUT'
                                     'to see second AP'),
            timeout=self.TIMEOUT_SECONDS,
            sleep_interval=1)

        # Check which AP we are currently connected.
        # This is to include the case that wpa_supplicant
        # automatically roam to AP2 during the scan.
        interface = self.context.client.wifi_if
        curr_bssid = self.context.client.iw_runner.get_current_bssid(interface)
        if curr_bssid == bssid0:
            roam_to_bssid = bssid1
        else:
            roam_to_bssid = bssid0

        # Send BSS Transition Management Request to client
        if not self.context.router.send_bss_tm_req(self.context.client.wifi_mac,
                                                   [roam_to_bssid]):
            raise error.TestNAError('AP does not support BSS Transition '
                                    'Management')

        # Expect that the DUT will re-connect to the new AP
        if not self.context.client.wait_for_roam(
                roam_to_bssid, timeout_seconds=self.TIMEOUT_SECONDS):
            raise error.TestFail('Failed to roam.')

    def cleanup(self):
        """Cleanup function."""
        super(network_WiFi_BSSTMReq, self).cleanup()
