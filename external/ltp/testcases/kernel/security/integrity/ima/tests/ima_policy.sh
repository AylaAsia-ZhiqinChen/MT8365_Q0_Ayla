#!/bin/sh
# Copyright (c) 2009 IBM Corporation
# Copyright (c) 2018 Petr Vorel <pvorel@suse.cz>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it would be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Author: Mimi Zohar, zohar@ibm.vnet.ibm.com
#
# Test replacing the default integrity measurement policy.

TST_SETUP="setup"
TST_CNT=2

. ima_setup.sh

check_policy_writable()
{
	local err="IMA policy already loaded and kernel not configured to enable multiple writes to it (need CONFIG_IMA_WRITE_POLICY=y)"

	[ -f /sys/kernel/security/ima/policy ] || tst_brk TCONF "$err"
	# CONFIG_IMA_READ_POLICY
	echo "" 2> log > $IMA_POLICY
	grep -q "Device or resource busy" log && tst_brk TCONF "$err"
}

setup()
{
	IMA_POLICY="$IMA_DIR/policy"
	check_policy_writable

	VALID_POLICY="$TST_DATAROOT/measure.policy"
	[ -f $VALID_POLICY ] || tst_brk TCONF "missing $VALID_POLICY"

	INVALID_POLICY="$TST_DATAROOT/measure.policy-invalid"
	[ -f $INVALID_POLICY ] || tst_brk TCONF "missing $INVALID_POLICY"
}

load_policy()
{
	local ret

	exec 2>/dev/null 4>$IMA_POLICY
	[ $? -eq 0 ] || exit 1

	cat $1 >&4 2> /dev/null
	ret=$?
	exec 4>&-

	[ $ret -eq 0 ] && \
		tst_res TINFO "IMA policy updated, please reboot after testing to restore settings"

	return $ret
}

test1()
{
	tst_res TINFO "verify that invalid policy isn't loaded"

	local p1

	check_policy_writable
	load_policy $INVALID_POLICY & p1=$!
	wait "$p1"
	if [ $? -ne 0 ]; then
		tst_res TPASS "didn't load invalid policy"
	else
		tst_res TFAIL "loaded invalid policy"
	fi
}

test2()
{
	tst_res TINFO "verify that policy file is not opened concurrently and able to loaded multiple times"

	local p1 p2 rc1 rc2

	check_policy_writable
	load_policy $VALID_POLICY & p1=$!
	load_policy $VALID_POLICY & p2=$!
	wait "$p1"; rc1=$?
	wait "$p2"; rc2=$?
	if [ $rc1 -eq 0 ] && [ $rc2 -eq 0 ]; then
		tst_res TFAIL "policy opened concurrently"
	elif [ $rc1 -eq 0 ] || [ $rc2 -eq 0 ]; then
		tst_res TPASS "policy was loaded just by one process and able to loaded multiple times"
	else
		tst_res TFAIL "problem loading or extending policy (may require policy to be signed)"
	fi
}

tst_run
