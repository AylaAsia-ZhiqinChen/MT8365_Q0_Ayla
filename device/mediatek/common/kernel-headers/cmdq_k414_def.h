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
#include "cmdq_k414_event_common.h"
#include "cmdq_k414_subsys_common.h"
#define CMDQ_DRIVER_DEVICE_NAME "mtk_cmdq"
#define CMDQ_DVENT_FROM_DTS
#include "cmdq_engine.h"
#define CMDQ_SPECIAL_SUBSYS_ADDR (99)
#define CMDQ_MAX_PROFILE_MARKER_IN_TASK (5)
#define CMDQ_MAX_SRAM_OWNER_NAME (32)
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
  CMDQ_SCENARIO_DISP_VFP_CHANGE = 34,
  CMDQ_SCENARIO_RDMA2_DISP = 35,
  CMDQ_SCENARIO_HIGHP_TRIGGER_LOOP = 36,
  CMDQ_SCENARIO_LOWP_TRIGGER_LOOP = 37,
  CMDQ_SCENARIO_KERNEL_CONFIG_GENERAL = 38,
  CMDQ_SCENARIO_TIMER_LOOP = 39,
  CMDQ_SCENARIO_MOVE = 40,
  CMDQ_SCENARIO_SRAM_LOOP = 41,
  CMDQ_SCENARIO_DEBUG_MDP = 42,
  CMDQ_MAX_SCENARIO_COUNT
};
enum cmdq_gpr_reg {
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
enum sec_extension {
  SEC_MDP_AAL = 0,
  SEC_MDP_TDSHP
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
  CMDQ_SAM_PH_2_MVA = 3,
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
struct cmdqMetaBuf {
  uint64_t va;
  uint64_t size;
};
#define CMDQ_ISP_META_CNT 8
struct cmdqSecIspMeta {
  struct cmdqMetaBuf ispBufs[CMDQ_ISP_META_CNT];
  uint64_t CqSecHandle;
  uint32_t CqSecSize;
  uint32_t CqDesOft;
  uint32_t CqVirtOft;
  uint64_t TpipeSecHandle;
  uint32_t TpipeSecSize;
  uint32_t TpipeOft;
  uint64_t BpciHandle;
  uint64_t LsciHandle;
  uint64_t LceiHandle;
  uint64_t DepiHandle;
  uint64_t DmgiHandle;
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
  struct cmdqSecIspMeta ispMeta;
  uint64_t extension;
};
struct cmdq_v3_replace_struct {
  uint32_t number;
  cmdqU32Ptr_t position;
};
struct cmdqProfileMarkerStruct {
  uint32_t count;
  long long hSlot;
  cmdqU32Ptr_t tag[CMDQ_MAX_PROFILE_MARKER_IN_TASK];
};
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
  struct cmdq_v3_replace_struct replace_instr;
  bool use_sram_buffer;
  char sram_owner_name[CMDQ_MAX_SRAM_OWNER_NAME];
  uint32_t debugRegDump;
  cmdqU32Ptr_t privateData;
  uint32_t prop_size;
  cmdqU32Ptr_t prop_addr;
  struct cmdqProfileMarkerStruct profileMarker;
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
