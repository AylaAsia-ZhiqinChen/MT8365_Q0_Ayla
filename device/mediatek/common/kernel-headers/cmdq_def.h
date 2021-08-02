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
#ifndef __CMDQ_DEF_H__
#define __CMDQ_DEF_H__
#include <linux/kernel.h>
#include "cmdq_event_common.h"
#include "cmdq_subsys_common.h"
#define CMDQ_DRIVER_DEVICE_NAME "mtk_cmdq"
#define CMDQ_DVENT_FROM_DTS
#ifdef CMDQ_COMMON_ENG_SUPPORT
#include "cmdq_engine_common.h"
#else
#include "cmdq_engine.h"
#endif
#define CMDQ_SPECIAL_SUBSYS_ADDR (99)
#define CMDQ_GPR_SUPPORT
#define CMDQ_PROFILE_MARKER_SUPPORT
#ifdef CMDQ_PROFILE_MARKER_SUPPORT
#define CMDQ_MAX_PROFILE_MARKER_IN_TASK (5)
#endif
#define CMDQ_INVALID_THREAD (- 1)
#define CMDQ_MAX_THREAD_COUNT (16)
#define CMDQ_MAX_TASK_IN_THREAD (16)
#define CMDQ_MAX_READ_SLOT_COUNT (4)
#define CMDQ_INIT_FREE_TASK_COUNT (8)
#define CMDQ_MAX_HIGH_PRIORITY_THREAD_COUNT (7)
#define CMDQ_MIN_SECURE_THREAD_ID (12)
#define CMDQ_MAX_SECURE_THREAD_COUNT (3)
enum CMDQ_SCENARIO_ENUM {
  CMDQ_SCENARIO_JPEG_DEC = 0,
  CMDQ_SCENARIO_PRIMARY_DISP = 1,
  CMDQ_SCENARIO_PRIMARY_MEMOUT = 2,
  CMDQ_SCENARIO_PRIMARY_ALL = 3,
  CMDQ_SCENARIO_SUB_DISP = 4,
  CMDQ_SCENARIO_SUB_MEMOUT = 5,
  CMDQ_SCENARIO_SUB_ALL = 6,
  CMDQ_SCENARIO_MHL_DISP = 7,
  CMDQ_SCENARIO_RDMA0_DISP = 8,
  CMDQ_SCENARIO_RDMA0_COLOR0_DISP = 9,
  CMDQ_SCENARIO_RDMA1_DISP = 10,
  CMDQ_SCENARIO_TRIGGER_LOOP = 11,
  CMDQ_SCENARIO_USER_MDP = 12,
  CMDQ_SCENARIO_DEBUG = 13,
  CMDQ_SCENARIO_DEBUG_PREFETCH = 14,
  CMDQ_SCENARIO_DISP_ESD_CHECK = 15,
  CMDQ_SCENARIO_DISP_SCREEN_CAPTURE = 16,
  CMDQ_SCENARIO_SECURE_NOTIFY_LOOP = 17,
  CMDQ_SCENARIO_DISP_PRIMARY_DISABLE_SECURE_PATH = 18,
  CMDQ_SCENARIO_DISP_SUB_DISABLE_SECURE_PATH = 19,
  CMDQ_SCENARIO_DISP_COLOR = 20,
  CMDQ_SCENARIO_USER_DISP_COLOR = 21,
  CMDQ_SCENARIO_USER_SPACE = 22,
  CMDQ_SCENARIO_DISP_MIRROR_MODE = 23,
  CMDQ_SCENARIO_DISP_CONFIG_AAL = 24,
  CMDQ_SCENARIO_DISP_CONFIG_PRIMARY_GAMMA = 25,
  CMDQ_SCENARIO_DISP_CONFIG_SUB_GAMMA = 26,
  CMDQ_SCENARIO_DISP_CONFIG_PRIMARY_DITHER = 27,
  CMDQ_SCENARIO_DISP_CONFIG_SUB_DITHER = 28,
  CMDQ_SCENARIO_DISP_CONFIG_PRIMARY_PWM = 29,
  CMDQ_SCENARIO_DISP_CONFIG_SUB_PWM = 30,
  CMDQ_SCENARIO_DISP_CONFIG_PRIMARY_PQ = 31,
  CMDQ_SCENARIO_DISP_CONFIG_SUB_PQ = 32,
  CMDQ_SCENARIO_DISP_CONFIG_OD = 33,
  CMDQ_SCENARIO_RDMA2_DISP = 34,
  CMDQ_SCENARIO_HIGHP_TRIGGER_LOOP = 35,
  CMDQ_SCENARIO_LOWP_TRIGGER_LOOP = 36,
  CMDQ_SCENARIO_KERNEL_CONFIG_GENERAL = 37,
  CMDQ_MAX_SCENARIO_COUNT
};
enum CMDQ_DATA_REGISTER_ENUM {
  CMDQ_DATA_REG_JPEG = 0x00,
  CMDQ_DATA_REG_JPEG_DST = 0x11,
  CMDQ_DATA_REG_PQ_COLOR = 0x04,
  CMDQ_DATA_REG_PQ_COLOR_DST = 0x13,
  CMDQ_DATA_REG_2D_SHARPNESS_0 = 0x05,
  CMDQ_DATA_REG_2D_SHARPNESS_0_DST = 0x14,
  CMDQ_DATA_REG_2D_SHARPNESS_1 = 0x0a,
  CMDQ_DATA_REG_2D_SHARPNESS_1_DST = 0x16,
  CMDQ_DATA_REG_DEBUG = 0x0b,
  CMDQ_DATA_REG_DEBUG_DST = 0x17,
  CMDQ_DATA_REG_INVALID = - 1,
};
enum CMDQ_MDP_PA_BASE_ENUM {
  CMDQ_MDP_PA_BASE_MM_MUTEX,
  CMDQ_MAX_MDP_PA_BASE_COUNT,
};
#define CMDQ_SUBSYS_GRPNAME_MAX (30)
struct SubsysStruct {
  uint32_t msb;
  int32_t subsysID;
  uint32_t mask;
  char grpName[CMDQ_SUBSYS_GRPNAME_MAX];
};
struct cmdqDTSDataStruct {
  int32_t eventTable[CMDQ_SYNC_TOKEN_MAX];
  struct SubsysStruct subsys[CMDQ_SUBSYS_MAX_COUNT];
  uint32_t MDPBaseAddress[CMDQ_MAX_MDP_PA_BASE_COUNT];
};
#define cmdqJobHandle_t unsigned long long
#define cmdqU32Ptr_t unsigned long long
#define CMDQ_U32_PTR(x) ((uint32_t *) (unsigned long) x)
struct cmdqReadRegStruct {
  uint32_t count;
  cmdqU32Ptr_t regAddresses;
};
struct cmdqRegValueStruct {
  uint32_t count;
  cmdqU32Ptr_t regValues;
};
struct cmdqReadAddressStruct {
  uint32_t count;
  cmdqU32Ptr_t dmaAddresses;
  cmdqU32Ptr_t values;
};
enum CMDQ_SEC_ADDR_METADATA_TYPE {
  CMDQ_SAM_H_2_PA = 0,
  CMDQ_SAM_H_2_MVA = 1,
  CMDQ_SAM_NMVA_2_MVA = 2,
  CMDQ_SAM_DDP_REG_HDCP = 3,
};
struct cmdqSecAddrMetadataStruct {
  uint32_t instrIndex;
  uint32_t type;
  uint64_t baseHandle;
  uint32_t blockOffset;
  uint32_t offset;
  uint32_t size;
  uint32_t port;
};
struct cmdqSecDataStruct {
  bool isSecure;
  uint32_t addrMetadataCount;
  cmdqU32Ptr_t addrMetadatas;
  uint32_t addrMetadataMaxCount;
  uint64_t enginesNeedDAPC;
  uint64_t enginesNeedPortSecurity;
  int32_t waitCookie;
  bool resetExecCnt;
};
#ifdef CMDQ_PROFILE_MARKER_SUPPORT
struct cmdqProfileMarkerStruct {
  uint32_t count;
  long long hSlot;
  cmdqU32Ptr_t tag[CMDQ_MAX_PROFILE_MARKER_IN_TASK];
};
#endif
struct cmdqCommandStruct {
  uint32_t scenario;
  uint32_t priority;
  uint64_t engineFlag;
  cmdqU32Ptr_t pVABase;
  uint32_t blockSize;
  struct cmdqReadRegStruct regRequest;
  struct cmdqRegValueStruct regValue;
  struct cmdqReadAddressStruct readAddress;
  struct cmdqSecDataStruct secData;
  uint32_t debugRegDump;
  cmdqU32Ptr_t privateData;
#ifdef CMDQ_PROFILE_MARKER_SUPPORT
  struct cmdqProfileMarkerStruct profileMarker;
#endif
  cmdqU32Ptr_t userDebugStr;
  uint32_t userDebugStrLen;
};
enum CMDQ_CAP_BITS {
  CMDQ_CAP_WFE = 0,
};
struct cmdqSecCancelTaskResultStruct {
  bool throwAEE;
  bool hasReset;
  int32_t irqFlag;
  uint32_t errInstr[2];
  uint32_t regValue;
  uint32_t pc;
};
#endif
