#!/bin/sh
# Diamond Standard processor install script.

# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2007 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

trap 'exit 1' 1 2 3 15

# Function to convert relative paths to absolute.
rel2abs() {
    case "$1" in
    /*)
	echo $1
	;;
    *)
	curdir=`pwd`
	if [ "X$curdir" = "X/" ]; then
	    echo "/$1"
	else
	    echo "$curdir/$1"
	fi
    esac
}

# Usage: install_DC_hw.sh <diamond_hardware_root>
config=`rel2abs "$1"`

# decide whether to use "echo -n" or "\c" to suppress newlines
(echo "test\c" ; echo " ") >/tmp/.echotest
if grep c /tmp/.echotest >/dev/null 2>&1 ; then
    n='-n'
    c=''
else
    n=''
    c='\c'
fi
rm -f /tmp/.echotest

# make sure this configuration has not already been installed
if [ -r "$config/misc/install-status" ]; then
    cat <<EOF

Error: This configuration has already been installed.  If you want to
reinstall it in a different location, or if you want to relocate the
Xtensa Tools files, you will need to start over with the original files
that you downloaded.

EOF
    exit 1
fi

# split the following to keep it from getting tagged
copyright_msg="Copyright 2006-2007"
copyright_msg="$copyright_msg Tensilica Inc."
cat <<EOF

Diamond Standard Processor Installation Tool
$copyright_msg

Before you can use a new Diamond Standard processor, you must run this script
to complete the installation.  You must have already installed the Xtensa 
Xplorer on your system before you can continue.  You may use either the
Diamond Edition of Xplorer, or alternatively, you may reinstall the Diamond
Config software from a Diamond Edition download under the Xtensa Xplorer
Standard Edition.

EOF
echo $n "Are you ready to proceed? [y] $c"
read ans
case "$ans" in
y|Y|yes|Yes|YES|"")
    echo "Continuing..."
    ;;
*)
    echo "Exiting without making any changes..."
    exit 0
    ;;
esac

# prompt for <xplorer>
echo
xplorer=
while [ -z "$xplorer" ]; do
    echo "Enter the directory where the Xtensa Xplorer is installed"
    echo "(the directory one level above XtDevToolsDE or XtDevTools):"
    read xplorer
done
xplorer=`echo "$xplorer" | sed -e "s|^~|$HOME/|"`
while [ ! -d "$xplorer" ]; do
    cat <<EOF

Error: The Xtensa Xplorer directory you have specified:

    $xplorer

does not exist.

EOF
    xplorer=
    while [ -z "$xplorer" ]; do
	echo "Please enter the directory again (or exit with Ctrl-C):"
	read xplorer
    done
    xplorer=`echo "$xplorer" | sed -e "s|^~|$HOME/|"`
done
xplorer=`rel2abs "$xplorer"`

# make sure that Xplorer is present
if [ -d "$xplorer/XtDevToolsDE/install" ]; then
    xplinst="$xplorer/XtDevToolsDE/install"
elif [ -d "$xplorer/XtDevTools/install" ]; then
    xplinst="$xplorer/XtDevTools/install"
elif [ -d "$xplorer/install" ]; then
    xplinst="$xplorer/install"
    xplorer=`rel2abs "$xplorer/.."` 
else
    cat <<EOF
Error: The directory you have specified does not contain a recognized
version of the Xtensa Xplorer.  Please check the directory path and
rerun this script.

EOF
    exit 1
fi

# Check which releases are available.
releases=`cd $xplinst/tools && ls -d R?-*`
relname=
multrel=
for r in dummy $releases; do
    if [ -d "$xplinst/tools/$r" ]; then
	if [ ! -z "$relname" ]; then
	    multrel=yes
	fi
	relname="$r"
    fi
done

# If multiple releases are installed, prompt for which one to use.
while [ ! -z "$multrel" ]; do
    echo
    echo $n "Which Xtensa Tools release would you like to use? [$relname] $c"
    read newrelname
    multrel=
    if [ ! -z "$newrelname" ]; then
	if [ -d "$xplinst/tools/$newrelname" ]; then
	    relname="$newrelname"
	else
	    multrel=yes
	    cat <<EOF

Error: This Xtensa Tools release is not currently installed.

You may try again, specifying a different Xtensa Tools release, or you
may exit without completing the installation.

EOF
	    echo $n "Do you want to try again? [y] $c"
	    read ans
	    case "$ans" in
	    n|N|no|No|NO)
		cat <<EOF

Exiting without installing this processor....

EOF
		exit 1
		;;
	    esac
	fi
    fi
done

diamond=`head -1 $config/misc/diamond`
xttools="$xplinst/tools/$relname/XtensaTools"
swconfig="$xplinst/builds/$relname/$diamond"
tools="$xttools/Tools"

# make sure the swconfig directory exists
if [ ! -d "$swconfig" ]; then
    cat <<EOF

Error: The $diamond processor configuration is not installed.
If you are using the Standard Edition of the Xtensa Xplorer, you must
get the $diamond processor configuration from the Diamond Edition
of Xtensa Xplorer and install it before you can continue.

EOF
    exit 1
fi

# check for overly long #! lines (Linux limit = 127 chars)
lentools=`echo "#!$tools/bin/perl -w" | wc -c`
if [ $lentools -gt 127 ]; then
    lenxpl=`echo "$xplorer" | wc -c`
    lenmax=`expr 127 + $lenxpl - $lentools`
    cat <<EOF

Error: The installation path for the Xtensa Xplorer is too long.  The
files for this Diamond Standard processor cannot be installed because
some of the scripts may exceed an operating system limit.  The Xtensa
Xplorer must be reinstalled in a directory with a shorter path (less
than $lenmax characters).

EOF
    exit 1
fi

# substitute the correct installation directory names
cat <<EOF

The files for this Diamond Standard processor will now be set up
to work with the installation directories that you have chosen.  This
process will take a few minutes, and once it has begun the
installation directories cannot be changed.  If you abort this script
after this point, or if you need to change the installation
directories for some reason, you will need to start over with the
original files that you downloaded for this processor.

EOF
echo $n "Do you want to continue? [y] $c"
read ans
case "$ans" in
y|Y|yes|Yes|YES|"")
    ;;
*)
    echo "Exiting without making any changes..."
    exit 0
    ;;
esac

echo "Installed in $config using Xplorer from $xplorer" \
   > "$config/misc/install-status"

# Copy the parameter file(s) to the hardware tree.
mkdir -p "$config/config"
cp "$swconfig"/config/*-params "$config/config"

# Fix up the paths in the hardware files.
"$config/misc/fix_DC_hw_install_paths" "$tools" "$xttools" "$config" "$swconfig"

echo
echo "The installation process is now complete."
exit 0
