/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _MTK_LINUX_ION_H
#define _MTK_LINUX_ION_H
enum mtk_ion_heap_type {
 ION_HEAP_TYPE_MULTIMEDIA = 10,
 ION_HEAP_TYPE_FB = 11,
 ION_HEAP_TYPE_MULTIMEDIA_FOR_CAMERA = 12,
 ION_HEAP_TYPE_MULTIMEDIA_SEC = 13,
 ION_HEAP_TYPE_MULTIMEDIA_MAP_MVA = 14,
 ION_HEAP_TYPE_MULTIMEDIA_PROT = 16,
 ION_HEAP_TYPE_MULTIMEDIA_2D_FR = 17,
 ION_HEAP_TYPE_MULTIMEDIA_WFD = 18,
 ION_HEAP_TYPE_MULTIMEDIA_HAPP = 19,
 ION_HEAP_TYPE_MULTIMEDIA_HAPP_EXTRA = 20,
 ION_HEAP_TYPE_MULTIMEDIA_SDSP = 21,
 ION_HEAP_TYPE_MULTIMEDIA_SDSP_SHARED = 22,
};
#define ION_HEAP_MULTIMEDIA_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA)
#define ION_HEAP_FB_MASK (1 << ION_HEAP_TYPE_FB)
#define ION_HEAP_CAMERA_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_FOR_CAMERA)
#define ION_HEAP_MULTIMEDIA_SEC_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_SEC)
#define ION_HEAP_MULTIMEDIA_MAP_MVA_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_MAP_MVA)
#define ION_HEAP_MULTIMEDIA_TYPE_PROT_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_PROT)
#define ION_HEAP_MULTIMEDIA_TYPE_2D_FR_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_2D_FR)
#define ION_HEAP_MULTIMEDIA_WFD_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_WFD)
#define ION_HEAP_MULTIMEDIA_HAPP_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_HAPP)
#define ION_HEAP_MULTIMEDIA_HAPP_EXTRA_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_HAPP_EXTRA)
#define ION_HEAP_MULTIMEDIA_SDSP_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_SDSP)
#define ION_HEAP_MULTIMEDIA_SDSP_SHARED_MASK (1 << ION_HEAP_TYPE_MULTIMEDIA_SDSP_SHARED)

#define ION_NUM_HEAP_IDS sizeof(unsigned int) * 8
#endif
