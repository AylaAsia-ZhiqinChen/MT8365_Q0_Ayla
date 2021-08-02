# Copyright 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#  blob(s) necessary for Marlin hardware
PRODUCT_COPY_FILES := \
    vendor/qcom/marlin/proprietary/pktlogconf:system/bin/pktlogconf:qcom \
    vendor/qcom/marlin/proprietary/ATT_profiles.xml:system/etc/cne/Nexus/ATT/ATT_profiles.xml:qcom \
    vendor/qcom/marlin/proprietary/ROW_profiles.xml:system/etc/cne/Nexus/ROW/ROW_profiles.xml:qcom \
    vendor/qcom/marlin/proprietary/VZW_profiles.xml:system/etc/cne/Nexus/VZW/VZW_profiles.xml:qcom \
    vendor/qcom/marlin/proprietary/qcrilhook.jar:system/framework/qcrilhook.jar:qcom \
    vendor/qcom/marlin/proprietary/lib64/vendor.qti.atcmdfwd@1.0.so:system/lib64/vendor.qti.atcmdfwd@1.0.so:qcom \
    vendor/qcom/marlin/proprietary/lib64/vendor.qti.qcril.am@1.0.so:system/lib64/vendor.qti.qcril.am@1.0.so:qcom \

