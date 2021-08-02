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
#ifndef _KD_CAMERA_FEATURE_H_
#define _KD_CAMERA_FEATURE_H_

#include "kd_camera_feature_id.h"
#include "kd_camera_feature_enum.h"

#define NULL 0

enum IMGSENSOR_SENSOR_IDX {
  IMGSENSOR_SENSOR_IDX_MIN_NUM = 0,
  IMGSENSOR_SENSOR_IDX_MAIN = IMGSENSOR_SENSOR_IDX_MIN_NUM,
  IMGSENSOR_SENSOR_IDX_SUB,
  IMGSENSOR_SENSOR_IDX_MAIN2,
  IMGSENSOR_SENSOR_IDX_SUB2,
  IMGSENSOR_SENSOR_IDX_MAIN3,
  IMGSENSOR_SENSOR_IDX_MAX_NUM,
  IMGSENSOR_SENSOR_IDX_NONE,
};
typedef enum {
  DUAL_CAMERA_NONE_SENSOR = 0,
  DUAL_CAMERA_MAIN_SENSOR = 1,
  DUAL_CAMERA_SUB_SENSOR = 2,
  DUAL_CAMERA_MAIN_2_SENSOR = 4,
  DUAL_CAMERA_SUB_2_SENSOR = 8,
  DUAL_CAMERA_MAIN_3_SENSOR = 16,
  DUAL_CAMERA_SENSOR_MAX,
  DUAL_CAMERA_MAIN_SECOND_SENSOR = DUAL_CAMERA_MAIN_2_SENSOR,
} CAMERA_DUAL_CAMERA_SENSOR_ENUM;
#define IMGSENSOR_SENSOR_DUAL2IDX(idx) ((ffs(idx) - 1))
#define IMGSENSOR_SENSOR_IDX2DUAL(idx) (1 << (idx))
#define IMGSENSOR_SENSOR_IDX_MAP(idx) ((idx) > DUAL_CAMERA_NONE_SENSOR && (idx) < DUAL_CAMERA_SENSOR_MAX) ? (enum IMGSENSOR_SENSOR_IDX) IMGSENSOR_SENSOR_DUAL2IDX(idx) : IMGSENSOR_SENSOR_IDX_NONE
#endif
