# Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""FAFT config setting overrides for Rambi."""


class Values(object):
    """FAFT config values for Rambi."""
    chrome_ec = True
    ec_boot_to_console = 0.3
    ec_capability = ['battery', 'charging', 'keyboard', 'lid', 'x86',
                     'usb', 'smart_usb_charge']
    firmware_screen = 7
    spi_voltage = 'pp1800'
    smm_store = False
