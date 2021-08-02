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
#ifndef SENSORS_IO_H
#define SENSORS_IO_H
#include <linux/ioctl.h>
typedef struct {
  unsigned short x;
  unsigned short y;
  unsigned short z;
} GSENSOR_VECTOR3D;
typedef struct {
  int x;
  int y;
  int z;
} SENSOR_DATA;
struct biometric_cali {
  unsigned int pga6;
  unsigned int ambdac5_5;
};
struct biometric_test_data {
  int ppg_ir;
  int ppg_r;
  int ekg;
};
struct biometric_threshold {
  int ppg_ir_threshold;
  int ppg_r_threshold;
  int ekg_threshold;
};
#define GSENSOR 0x85
#define GSENSOR_IOCTL_INIT _IO(GSENSOR, 0x01)
#define GSENSOR_IOCTL_READ_CHIPINFO _IOR(GSENSOR, 0x02, int)
#define GSENSOR_IOCTL_READ_SENSORDATA _IOR(GSENSOR, 0x03, int)
#define GSENSOR_IOCTL_READ_RAW_DATA _IOR(GSENSOR, 0x06, int)
#define GSENSOR_IOCTL_SET_CALI _IOW(GSENSOR, 0x06, SENSOR_DATA)
#define GSENSOR_IOCTL_GET_CALI _IOW(GSENSOR, 0x07, SENSOR_DATA)
#define GSENSOR_IOCTL_CLR_CALI _IO(GSENSOR, 0x08)
#define GSENSOR_IOCTL_ENABLE_CALI _IO(GSENSOR, 0x09)
#define GSENSOR_IOCTL_SELF_TEST _IO(GSENSOR, 0x0A)
#define MSENSOR 0x83
#define MSENSOR_IOCTL_READ_SENSORDATA _IOR(MSENSOR, 0x03, int)
#define MSENSOR_IOCTL_SELF_TEST _IOW(MSENSOR, 0x0B, int)
#define MSENSOR_IOCTL_SENSOR_ENABLE _IOW(MSENSOR, 0x51, int)
#define ALSPS 0X84
#define ALSPS_SET_PS_MODE _IOW(ALSPS, 0x01, int)
#define ALSPS_GET_PS_RAW_DATA _IOR(ALSPS, 0x04, int)
#define ALSPS_SET_ALS_MODE _IOW(ALSPS, 0x05, int)
#define ALSPS_GET_ALS_RAW_DATA _IOR(ALSPS, 0x08, int)
#define ALSPS_GET_PS_TEST_RESULT _IOR(ALSPS, 0x09, int)
#define ALSPS_GET_PS_THRESHOLD_HIGH _IOR(ALSPS, 0x0B, int)
#define ALSPS_GET_PS_THRESHOLD_LOW _IOR(ALSPS, 0x0C, int)
#define ALSPS_IOCTL_CLR_CALI _IOW(ALSPS, 0x0F, int)
#define ALSPS_IOCTL_GET_CALI _IOR(ALSPS, 0x10, int)
#define ALSPS_IOCTL_SET_CALI _IOW(ALSPS, 0x11, int)
#define ALSPS_SET_PS_THRESHOLD _IOW(ALSPS, 0x12, int)
#define AAL_SET_ALS_MODE _IOW(ALSPS, 0x14, int)
#define AAL_GET_ALS_MODE _IOR(ALSPS, 0x15, int)
#define AAL_GET_ALS_DATA _IOR(ALSPS, 0x16, int)
#define ALSPS_ALS_ENABLE_CALI _IO(ALSPS, 0x17)
#define ALSPS_PS_ENABLE_CALI _IO(ALSPS, 0x18)
#define ALSPS_IOCTL_ALS_GET_CALI _IOW(ALSPS, 0x19, int)
#define ALSPS_ALS_SET_CALI _IOW(ALSPS, 0x20, int)
#define GYROSCOPE 0X86
#define GYROSCOPE_IOCTL_INIT _IO(GYROSCOPE, 0x01)
#define GYROSCOPE_IOCTL_SMT_DATA _IOR(GYROSCOPE, 0x02, int)
#define GYROSCOPE_IOCTL_READ_SENSORDATA _IOR(GYROSCOPE, 0x03, int)
#define GYROSCOPE_IOCTL_SET_CALI _IOW(GYROSCOPE, 0x04, SENSOR_DATA)
#define GYROSCOPE_IOCTL_GET_CALI _IOW(GYROSCOPE, 0x05, SENSOR_DATA)
#define GYROSCOPE_IOCTL_CLR_CALI _IO(GYROSCOPE, 0x06)
#define GYROSCOPE_IOCTL_READ_SENSORDATA_RAW _IOR(GYROSCOPE, 0x07, int)
#define GYROSCOPE_IOCTL_ENABLE_CALI _IO(GYROSCOPE, 0x0A)
#define GYROSCOPE_IOCTL_SELF_TEST _IO(GYROSCOPE, 0x0B)
#define BROMETER 0X87
#define BAROMETER_IOCTL_INIT _IO(BROMETER, 0x01)
#define BAROMETER_GET_PRESS_DATA _IOR(BROMETER, 0x02, int)
#define BAROMETER_GET_TEMP_DATA _IOR(BROMETER, 0x03, int)
#define BAROMETER_IOCTL_ENABLE_CALI _IO(BROMETER, 0x05)
#define HEARTMONITOR 0x88
#define HRM_IOCTL_INIT _IO(HEARTMONITOR, 0x01)
#define HRM_READ_SENSOR_DATA _IOR(HEARTMONITOR, 0x02, int)
#define HUMIDITY 0X89
#define HUMIDITY_IOCTL_INIT _IO(HUMIDITY, 0x01)
#define HUMIDITY_GET_HMDY_DATA _IOR(HUMIDITY, 0x02, int)
#define HUMIDITY_GET_TEMP_DATA _IOR(HUMIDITY, 0x03, int)
#define HUMIDITY_IOCTL_READ_CHIPINFO _IOR(HUMIDITY, 0x04, int)
#define BIOMETRIC 0X90
#define BIOMETRIC_IOCTL_INIT _IO(BIOMETRIC, 0x01)
#define BIOMETRIC_IOCTL_DO_CALI _IOW(BIOMETRIC, 0x02, struct biometric_cali)
#define BIOMETRIC_IOCTL_SET_CALI _IOW(BIOMETRIC, 0x03, struct biometric_cali)
#define BIOMETRIC_IOCTL_GET_CALI _IOW(BIOMETRIC, 0x04, struct biometric_cali)
#define BIOMETRIC_IOCTL_CLR_CALI _IO(BIOMETRIC, 0x05)
#define BIOMETRIC_IOCTL_FTM_START _IO(BIOMETRIC, 0x06)
#define BIOMETRIC_IOCTL_FTM_END _IO(BIOMETRIC, 0x07)
#define BIOMETRIC_IOCTL_FTM_GET_DATA _IOW(BIOMETRIC, 0x08, struct biometric_test_data)
#define BIOMETRIC_IOCTL_FTM_GET_THRESHOLD _IOW(BIOMETRIC, 0x09, struct biometric_threshold)
#define SAR 0x91
#define SAR_IOCTL_INIT _IOW(SAR, 0x01, int)
#define SAR_IOCTL_READ_SENSORDATA _IOR(SAR, 0x02, struct SENSOR_DATA)
#define SAR_IOCTL_GET_CALI  _IOR(SAR, 0x03, struct SENSOR_DATA)
#define SAR_IOCTL_ENABLE_CALI _IO(SAR, 0x04)
#endif
