#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2014-2018 Oracle and/or its affiliates. All Rights Reserved.
# Copyright (c) 2018 Petr Vorel <pvorel@suse.cz>
#
# Author:       Alexey Kodanev alexey.kodanev@oracle.com

TST_SETUP="init"
TST_TESTFUNC="test01"
TST_CLEANUP="cleanup"
TST_NEEDS_TMPDIR=1
TST_NEEDS_ROOT=1
TST_NEEDS_CMDS="cat $dhcp_name awk ip pgrep pkill dhclient"

. tst_net.sh
. daemonlib.sh

iface0="ltp_veth0"
iface1="ltp_veth1"

stop_dhcp()
{
	[ "$(pgrep -x $dhcp_name)" ] || return 0

	tst_res TINFO "stopping $dhcp_name"
	local count=0
	while [ $count -le 10 ]; do
		pkill -x $dhcp_name
		[ "$(pgrep -x $dhcp_name)" ] || return 0
		tst_sleep 100ms
		count=$((count + 1))
	done

	pkill -9 -x $dhcp_name
	tst_sleep 100ms
	[ "$(pgrep -x $dhcp_name)" ] && return 1 || return 0
}

init()
{
	if [ $TST_IPV6 ]; then
		ip_addr="fd00:1:1:2::12/64"
		ip_addr_check="fd00:1:1:2::100/64"
	else
		ip_addr="10.1.1.12/24"
		ip_addr_check="10.1.1.100/24"
	fi

	lsmod | grep -q '^veth ' && veth_loaded=yes || veth_loaded=no

	tst_res TINFO "create veth interfaces"
	ip li add $iface0 type veth peer name $iface1 || \
		tst_brk TBROK "failed to add veth $iface0"

	veth_added=1
	ip li set up $iface0 || tst_brk TBROK "failed to bring $iface0 up"
	ip li set up $iface1 || tst_brk TBROK "failed to bring $iface1 up"

	stop_dhcp || tst_brk TBROK "Failed to stop dhcp server"

	dhclient_lease="/var/lib/dhclient/dhclient${TST_IPV6}.leases"
	if [ -f $dhclient_lease ]; then
		tst_res TINFO "backup dhclient${TST_IPV6}.leases"
		mv $dhclient_lease .
	fi

	tst_res TINFO "add $ip_addr to $iface0"
	ip addr add $ip_addr dev $iface0 || \
		tst_brk TBROK "failed to add ip address"
}

cleanup()
{
	[ -z "$veth_loaded" ] && return

	stop_dhcp

	pkill -f "dhclient -$TST_IPVER $iface1"

	cleanup_dhcp

	# restore dhclient leases
	[ $dhclient_lease ] && rm -f $dhclient_lease
	[ -f "dhclient${TST_IPV6}.leases" ] && \
		mv dhclient${TST_IPV6}.leases $dhclient_lease

	[ $veth_added ] && ip li del $iface0

	[ "$veth_loaded" = "no" ] && lsmod | grep -q '^veth ' && rmmod veth
}

test01()
{
	local wicked

	tst_res TINFO "testing DHCP server $dhcp_name: $(print_dhcp_version)"
	tst_res TINFO "using DHCP client: $(dhclient --version 2>&1)"

	tst_res TINFO "starting DHCPv$TST_IPVER server on $iface0"

	start_dhcp$TST_IPV6
	if [ $? -ne 0 ]; then
		print_dhcp_log
		tst_brk TBROK "Failed to start $dhcp_name"
	fi

	sleep 1

	if [ "$(pgrep '$dhcp_name')" ]; then
		print_dhcp_log
		tst_brk TBROK "Failed to start $dhcp_name"
	fi

	if [ $HAVE_SYSTEMCTL -eq 1 ] && \
		systemctl --no-pager -p Id show network.service | grep -q Id=wicked.service; then
		tst_res TINFO "temporarily disabling wicked"
		wicked=1
		systemctl disable wicked
	fi
	tst_res TINFO "starting dhclient -$TST_IPVER $iface1"
	dhclient -$TST_IPVER $iface1 || tst_brk TBROK "dhclient failed"

	# check that we get configured ip address
	ip addr show $iface1 | grep $ip_addr_check > /dev/null
	if [ $? -eq 0 ]; then
		tst_res TPASS "'$ip_addr_check' configured by DHCPv$TST_IPVER"
	else
		tst_res TFAIL "'$ip_addr_check' not configured by DHCPv$TST_IPVER"
		print_dhcp_log
	fi

	if [ "$wicked" ]; then
		tst_res TINFO "reenabling wicked"
		systemctl enable wicked
	fi

	stop_dhcp
}
