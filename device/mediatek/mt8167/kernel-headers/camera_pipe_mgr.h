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
#ifndef CAMERA_PIPE_MGR_H
#define CAMERA_PIPE_MGR_H
#define CAM_PIPE_MGR_DEV_NAME "camera-pipemgr"
#define CAM_PIPE_MGR_MAGIC_NO 'p'
#define CAM_PIPE_MGR_PIPE_MASK_CAM_IO ((unsigned long) 1 << 0)
#define CAM_PIPE_MGR_PIPE_MASK_POST_PROC ((unsigned long) 1 << 1)
#define CAM_PIPE_MGR_PIPE_MASK_XDP_CAM ((unsigned long) 1 << 2)
typedef enum {
  CAM_PIPE_MGR_SCEN_SW_NONE,
  CAM_PIPE_MGR_SCEN_SW_CAM_IDLE,
  CAM_PIPE_MGR_SCEN_SW_CAM_PRV,
  CAM_PIPE_MGR_SCEN_SW_CAM_CAP,
  CAM_PIPE_MGR_SCEN_SW_VIDEO_PRV,
  CAM_PIPE_MGR_SCEN_SW_VIDEO_REC,
  CAM_PIPE_MGR_SCEN_SW_VIDEO_VSS,
  CAM_PIPE_MGR_SCEN_SW_ZSD,
  CAM_PIPE_MGR_SCEN_SW_N3D,
} CAM_PIPE_MGR_SCEN_SW_ENUM;
typedef enum {
  CAM_PIPE_MGR_SCEN_HW_NONE,
  CAM_PIPE_MGR_SCEN_HW_IC,
  CAM_PIPE_MGR_SCEN_HW_VR,
  CAM_PIPE_MGR_SCEN_HW_ZSD,
  CAM_PIPE_MGR_SCEN_HW_IP,
  CAM_PIPE_MGR_SCEN_HW_N3D,
  CAM_PIPE_MGR_SCEN_HW_VSS
} CAM_PIPE_MGR_SCEN_HW_ENUM;
typedef enum {
  CAM_PIPE_MGR_DEV_CAM,
  CAM_PIPE_MGR_DEV_ATV,
  CAM_PIPE_MGR_DEV_VT
} CAM_PIPE_MGR_DEV_ENUM;
typedef struct {
  unsigned int PipeMask;
  unsigned int Timeout;
} CAM_PIPE_MGR_LOCK_STRUCT;
typedef struct {
  unsigned int PipeMask;
} CAM_PIPE_MGR_UNLOCK_STRUCT;
typedef struct {
  CAM_PIPE_MGR_SCEN_SW_ENUM ScenSw;
  CAM_PIPE_MGR_SCEN_HW_ENUM ScenHw;
  CAM_PIPE_MGR_DEV_ENUM Dev;
} CAM_PIPE_MGR_MODE_STRUCT;
typedef struct {
  unsigned int PipeMask;
} CAM_PIPE_MGR_ENABLE_STRUCT;
typedef struct {
  unsigned int PipeMask;
} CAM_PIPE_MGR_DISABLE_STRUCT;
typedef enum {
  CAM_PIPE_MGR_CMD_VECNPLL_CTRL_SET_HIGH,
  CAM_PIPE_MGR_CMD_VECNPLL_CTRL_SET_LOW
} CAM_PIPE_MGR_CMD_VECNPLL_CTRL_ENUM;
typedef enum {
  CAM_PIPE_MGR_CMD_LOCK,
  CAM_PIPE_MGR_CMD_UNLOCK,
  CAM_PIPE_MGR_CMD_DUMP,
  CAM_PIPE_MGR_CMD_SET_MODE,
  CAM_PIPE_MGR_CMD_GET_MODE,
  CAM_PIPE_MGR_CMD_ENABLE_PIPE,
  CAM_PIPE_MGR_CMD_DISABLE_PIPE,
  CAM_PIPE_MGR_CMD_VENC_PLL_CTRL
} CAM_PIPE_MGR_CMD_ENUM;
#define CAM_PIPE_MGR_LOCK _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_LOCK, CAM_PIPE_MGR_LOCK_STRUCT)
#define CAM_PIPE_MGR_UNLOCK _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_UNLOCK, CAM_PIPE_MGR_UNLOCK_STRUCT)
#define CAM_PIPE_MGR_DUMP _IO(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_DUMP)
#define CAM_PIPE_MGR_SET_MODE _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_SET_MODE, CAM_PIPE_MGR_MODE_STRUCT)
#define CAM_PIPE_MGR_GET_MODE _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_GET_MODE, CAM_PIPE_MGR_MODE_STRUCT)
#define CAM_PIPE_MGR_ENABLE_PIPE _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_ENABLE_PIPE, CAM_PIPE_MGR_ENABLE_STRUCT)
#define CAM_PIPE_MGR_DISABLE_PIPE _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_DISABLE_PIPE, CAM_PIPE_MGR_DISABLE_STRUCT)
#define CAM_PIPE_MGR_VENCPLL_CTRL _IOW(CAM_PIPE_MGR_MAGIC_NO, CAM_PIPE_MGR_CMD_VENC_PLL_CTRL, CAM_PIPE_MGR_CMD_VECNPLL_CTRL_ENUM)
#endif
