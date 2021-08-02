#!/bin/bash
# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../../../../../.."

DOWNLOADS_DIR=tensorflow/lite/experimental/micro/tools/make/downloads
BZL_FILE_PATH=tensorflow/workspace.bzl

# Ensure it is being run from repo root
if [ ! -f $BZL_FILE_PATH ]; then
  echo "Could not find ${BZL_FILE_PATH}":
  echo "Likely you are not running this from the root directory of the repository.";
  exit 1;
fi

GEMMLOWP_URL="https://github.com/google/gemmlowp/archive/719139ce755a0f31cbf1c37f7f98adcc7fc9f425.zip"
FLATBUFFERS_URL="https://github.com/google/flatbuffers/archive/1f5eae5d6a135ff6811724f6c57f911d1f46bb15.tar.gz"
CMSIS_URL="https://github.com/ARM-software/CMSIS_5/archive/5.4.0.zip"
STM32_BARE_LIB_URL="https://github.com/google/stm32_bare_lib/archive/c07d611fb0af58450c5a3e0ab4d52b47f99bc82d.zip"
SIFIVE_FE310_LIB_URL="https://github.com/sifive/freedom-e-sdk/archive/baeeb8fd497a99b3c141d7494309ec2e64f19bdf.zip"
AM_SDK_URL="http://s3.asia.ambiqmicro.com/downloads/AmbiqSuite-Rel2.0.0.zip"
AP3_URL="https://github.com/AmbiqMicro/TFLiteMicro_Apollo3/archive/dfbcef9a57276c087d95aab7cb234f1d4c9eaaba.zip"
CUST_CMSIS_URL="https://github.com/AmbiqMicro/TFLiteMicro_CustCMSIS/archive/8f63966c5692e6a3a83956efd2e4aed77c4c9949.zip"
KISSFFT_URL="https://github.com/mborgerding/kissfft/archive/v130.zip"
SPARKFUN_EDGE_BSP_URL="https://github.com/sparkfun/SparkFun_Edge_BSP/archive/620f5f7a69fc69e38cda8132b69302d9c28ba0dd.zip"

if [[ "$OSTYPE" == "darwin"* ]]; then
  RISCV_TOOLCHAIN_URL="https://static.dev.sifive.com/dev-tools/riscv64-unknown-elf-gcc-8.1.0-2019.01.0-x86_64-apple-darwin.tar.gz"
  GCC_EMBEDDED_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-mac.tar.bz2"
else
  RISCV_TOOLCHAIN_URL="https://static.dev.sifive.com/dev-tools/riscv64-unknown-elf-gcc-20181030-x86_64-linux-ubuntu14.tar.gz"
  GCC_EMBEDDED_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2"
fi

download_and_extract() {
  local usage="Usage: download_and_extract URL DIR"
  local url="${1:?${usage}}"
  local dir="${2:?${usage}}"
  echo "downloading ${url}" >&2
  mkdir -p "${dir}"
  if [[ "${url}" == *gz ]]; then
    curl -Ls "${url}" | tar -C "${dir}" --strip-components=1 -xz
  elif [[ "${url}" == *bz2 ]]; then
    curl -Ls "${url}" | tar -C "${dir}" --strip-components=1 -xj
  elif [[ "${url}" == *zip ]]; then
    tempdir=$(mktemp -d)
    tempdir2=$(mktemp -d)

    curl -L ${url} > ${tempdir}/zipped.zip
    unzip ${tempdir}/zipped.zip -d ${tempdir2}

    # If the zip file contains nested directories, extract the files from the
    # inner directory.
    if ls ${tempdir2}/*/* 1> /dev/null 2>&1; then
      # unzip has no strip components, so unzip to a temp dir, and move the
      # files we want from the tempdir to destination.
      cp -R ${tempdir2}/*/* ${dir}/
    else
      cp -R ${tempdir2}/* ${dir}/
    fi
    rm -rf ${tempdir2} ${tempdir}
  fi

  # Delete any potential BUILD files, which would interfere with Bazel builds.
  find "${dir}" -type f -name '*BUILD' -delete
}

patch_am_sdk() {
  local am_dir="${1}"
  if [ ! -f ${am_dir}/VERSION.txt ]; then
    echo "Could not find ${am_dir}, skipping AmbiqMicro SDK patch";
    return;
  fi

  local src_dir=${am_dir}/boards/apollo3_evb/examples/hello_world/gcc
  local dest_dir=${am_dir}/boards/apollo3_evb/examples/hello_world/gcc_patched

  rm -rf ${dest_dir}
  mkdir ${dest_dir}

  cp "${src_dir}/startup_gcc.c" "${dest_dir}/startup_gcc.c"
  cp "${src_dir}/hello_world.ld" "${dest_dir}/apollo3evb.ld"

  sed -i -e '114s/1024/1024\*20/g' "${dest_dir}/startup_gcc.c"
  #sed -i -e 's/main/_main/g' "${dest_dir}/startup_gcc.c"

  sed -i -e '3s/hello_world.ld/apollo3evb.ld/g' "${dest_dir}/apollo3evb.ld"
  sed -i -e '3s/startup_gnu/startup_gcc/g' "${dest_dir}/apollo3evb.ld"
  sed -i -e $'22s/\*(.text\*)/\*(.text\*)\\\n\\\n\\\t\/\* These are the C++ global constructors.  Stick them all here and\\\n\\\t \* then walk through the array in main() calling them all.\\\n\\\t \*\/\\\n\\\t_init_array_start = .;\\\n\\\tKEEP (\*(SORT(.init_array\*)))\\\n\\\t_init_array_end = .;\\\n\\\n\\\t\/\* XXX Currently not doing anything for global destructors. \*\/\\\n/g' "${dest_dir}/apollo3evb.ld"
  sed -i -e $'70s/} > SRAM/} > SRAM\\\n    \/\* Add this to satisfy reference to symbol "end" from libnosys.a(sbrk.o)\\\n     \* to denote the HEAP start.\\\n     \*\/\\\n   end = .;/g' "${dest_dir}/apollo3evb.ld"

  # Workaround for bug in 2.0.0 SDK, remove once that's fixed.
  sed -i -e $'s/#ifndef AM_HAL_GPIO_H/#ifdef __cplusplus\\\nextern "C" {\\\n#endif\\\n#ifndef AM_HAL_GPIO_H/g' ${am_dir}/mcu/apollo3/hal/am_hal_gpio.h

  echo "Finished preparing Apollo3 files"
}

patch_kissfft() {
  sed -i -E $'s@#ifdef FIXED_POINT@// Patched automatically by download_dependencies.sh so default is 16 bit.\\\n#ifndef FIXED_POINT\\\n#define FIXED_POINT (16)\\\n#endif\\\n// End patch.\\\n\\\n#ifdef FIXED_POINT@g' tensorflow/lite/experimental/micro/tools/make/downloads/kissfft/kiss_fft.h
  sed -i -E "s@#define KISS_FFT_MALLOC malloc@#define KISS_FFT_MALLOC(X) (void*)(0) /* Patched. */@g" tensorflow/lite/experimental/micro/tools/make/downloads/kissfft/kiss_fft.h
  sed -i -E "s@#define KISS_FFT_FREE free@#define KISS_FFT_FREE(X) /* Patched. */@g" tensorflow/lite/experimental/micro/tools/make/downloads/kissfft/kiss_fft.h
  sed -ir -E "s@(fprintf.*\);)@/* \1 */@g" tensorflow/lite/experimental/micro/tools/make/downloads/kissfft/tools/kiss_fftr.c
  sed -ir -E "s@(exit.*\);)@return; /* \1 */@g" tensorflow/lite/experimental/micro/tools/make/downloads/kissfft/tools/kiss_fftr.c
  echo "Finished patching kissfft"
}

download_and_extract "${GEMMLOWP_URL}" "${DOWNLOADS_DIR}/gemmlowp"
download_and_extract "${FLATBUFFERS_URL}" "${DOWNLOADS_DIR}/flatbuffers"
download_and_extract "${CMSIS_URL}" "${DOWNLOADS_DIR}/cmsis"
download_and_extract "${STM32_BARE_LIB_URL}" "${DOWNLOADS_DIR}/stm32_bare_lib"
download_and_extract "${SIFIVE_FE310_LIB_URL}" "${DOWNLOADS_DIR}/sifive_fe310_lib"
download_and_extract "${RISCV_TOOLCHAIN_URL}" "${DOWNLOADS_DIR}/riscv_toolchain"
download_and_extract "${AM_SDK_URL}" "${DOWNLOADS_DIR}/AmbiqSuite-Rel2.0.0"
patch_am_sdk "${DOWNLOADS_DIR}/AmbiqSuite-Rel2.0.0"
download_and_extract "${AP3_URL}" "${DOWNLOADS_DIR}/apollo3_ext"
download_and_extract "${CUST_CMSIS_URL}" "${DOWNLOADS_DIR}/CMSIS_ext"
download_and_extract "${GCC_EMBEDDED_URL}" "${DOWNLOADS_DIR}/gcc_embedded"
download_and_extract "${KISSFFT_URL}" "${DOWNLOADS_DIR}/kissfft"
patch_kissfft "${DOWNLOADS_DIR}/kissfft"
download_and_extract "${SPARKFUN_EDGE_BSP_URL}" "${DOWNLOADS_DIR}/AmbiqSuite-Rel2.0.0/boards/SparkFun_TensorFlow_Apollo3_BSP"

echo "download_dependencies.sh completed successfully." >&2
