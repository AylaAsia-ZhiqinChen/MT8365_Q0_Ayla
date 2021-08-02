// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=TextLogger.txt

// Edit the xtensa_system, xtensa_core, and xtensa_params assignments as required for the first core
-xtensa_system=/proj/mtk05764/ALPS_Base/alps-dev-p0.mp1-mt8168.ep-adsp/vendor/mediatek/proprietary/tinysys/hifi_dsp/source/pub/project/mt8168/common/hwcfg/RG-2018.9-linux/config
-xtensa_core=hifi4_Aquila_E2_PROD
-xtensa_params=
-core_program=target/core0.out
-create_core=core0

// Edit the xtensa_system, xtensa_core, and xtensa_params assignments as required for the second core
-xtensa_system=/proj/mtk05764/ALPS_Base/alps-dev-p0.mp1-mt8168.ep-adsp/vendor/mediatek/proprietary/tinysys/hifi_dsp/source/pub/project/mt8168/common/hwcfg/RG-2018.9-linux/config
-xtensa_core=hifi4_Aquila_E2_PROD
-xtensa_params=
-core_program=target/core1.out
-create_core=core1

