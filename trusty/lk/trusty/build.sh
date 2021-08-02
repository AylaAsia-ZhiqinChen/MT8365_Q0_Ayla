#!/bin/bash
#
# Copyright (c) 2015 MediaTek Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

set -e
TRUSTY_PROJECT=$1
if [ -z ${TRUSTY_PROJECT} ]; then
  if [ -z ${TARGET_PRODUCT} ]; then
    echo "Please lunch Android product first or run \"$0 TRUSTY_PROJECT\" !"
    exit 1
  else
    TRUSTY_PROJECT=${TARGET_PRODUCT/full_/}
  fi
fi

build_sh_dir=`dirname $0`
TRUSTY_DIR=`readlink -f ${build_sh_dir}/../..`
if [ -z ${ANDROID_BUILD_TOP} ]; then
  if [ -d ${TRUSTY_DIR}/prebuilts/gcc/linux-x86 ]; then
    ANDROID_BUILD_TOP=${TRUSTY_DIR}
  fi
  if [ -d ${TRUSTY_DIR}/../prebuilts/gcc/linux-x86 ]; then
    ANDROID_BUILD_TOP=`readlink -f ${TRUSTY_DIR}/..`
  fi
fi
if [ -z ${ANDROID_BUILD_TOP} ]; then
  echo "Couldn't locate the top of the tree. Try setting ANDROID_BUILD_TOP."
  exit 2
fi
if [ -z ${ANDROID_PRODUCT_OUT} ]; then
  if [ -z ${OUT_DIR} ]; then
    OUT_DIR="out"
  fi
  TARGET_TRUSTY_OUT="${ANDROID_BUILD_TOP}/${OUT_DIR}/target/product/${TRUSTY_PROJECT}/obj/TRUSTY_OBJ"
else
  TARGET_TRUSTY_OUT="${ANDROID_PRODUCT_OUT}/obj/TRUSTY_OBJ"
fi
echo "TRUSTY_DIR=${TRUSTY_DIR}"
echo "ANDROID_BUILD_TOP=${ANDROID_BUILD_TOP}"
echo "TARGET_TRUSTY_OUT=${TARGET_TRUSTY_OUT}"

TRUSTY_MAKE_JOB="24"
TRUSTY_CMD_PATH="-C ${TRUSTY_DIR} -f external/lk/makefile -I vendor/mediatek/proprietary ARCH_arm_TOOLCHAIN_PREFIX=${ANDROID_BUILD_TOP}/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi- ARCH_arm64_TOOLCHAIN_PREFIX=${ANDROID_BUILD_TOP}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android- LKMAKEROOT=${TRUSTY_DIR} BUILDROOT=${TARGET_TRUSTY_OUT} ROOTDIR=${ANDROID_BUILD_TOP} ANDROIDMAKEROOT=${ANDROID_BUILD_TOP}"

MAKE_CMD="-j ${TRUSTY_MAKE_JOB} ${TRUSTY_CMD_PATH} ${TRUSTY_PROJECT}"
echo make ${MAKE_CMD}
make ${MAKE_CMD}

if [ $? -eq 0 ] ; then
    echo "#### make completed successfully "
else
    echo "#### make failed to build some targets "
fi
