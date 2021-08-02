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
#ifndef __VOW_H__
#define __VOW_H__
#define VOW_DEVNAME "vow"
#define VOW_IOC_MAGIC 'V'
#define VOW_SET_CONTROL _IOW(VOW_IOC_MAGIC, 0x03, unsigned int)
#define VOW_SET_SPEAKER_MODEL _IOW(VOW_IOC_MAGIC, 0x04, unsigned int)
#define VOW_CLR_SPEAKER_MODEL _IOW(VOW_IOC_MAGIC, 0x05, unsigned int)
#define VOW_SET_APREG_INFO _IOW(VOW_IOC_MAGIC, 0x09, unsigned int)
#define VOW_BARGEIN_ON _IOW(VOW_IOC_MAGIC, 0x0A, unsigned int)
#define VOW_BARGEIN_OFF _IOW(VOW_IOC_MAGIC, 0x0B, unsigned int)
#define VOW_CHECK_STATUS _IOW(VOW_IOC_MAGIC, 0x0C, unsigned int)
#define VOW_RECOG_ENABLE _IOW(VOW_IOC_MAGIC, 0x0D, unsigned int)
#define VOW_RECOG_DISABLE _IOW(VOW_IOC_MAGIC, 0x0E, unsigned int)
#define VOW_MODEL_START _IOW(VOW_IOC_MAGIC, 0x0F, unsigned int)
#define VOW_MODEL_STOP _IOW(VOW_IOC_MAGIC, 0x10, unsigned int)
#define RESERVED_DATA 4
enum vow_control_cmd_t {
  VOWControlCmd_Init = 0,
  VOWControlCmd_ReadVoiceData,
  VOWControlCmd_EnableDebug,
  VOWControlCmd_DisableDebug,
  VOWControlCmd_EnableSeamlessRecord,
  VOWControlCmd_EnableDump,
  VOWControlCmd_DisableDump,
  VOWControlCmd_Reset,
};
enum vow_eint_status_t {
  VOW_EINT_DISABLE = -2,
  VOW_EINT_FAIL = -1,
  VOW_EINT_PASS = 0,
  VOW_EINT_RETRY = 1,
  NUM_OF_VOW_EINT_STATUS = 4
};
enum vow_mtkif_type_t {
  VOW_MTKIF_NONE = 0,
  VOW_MTKIF_AMIC = 1,
  VOW_MTKIF_DMIC = 2,
  VOW_MTKIF_DMIC_LP = 3
};
struct vow_eint_data_struct_t {
  int size;
  int eint_status;
  int id;
  char data[RESERVED_DATA];
};
struct vow_model_info_t {
  long id;
  long addr;
  long size;
  long return_size_addr;
  long uuid;
  void * data;
};
struct vow_model_start_t {
  long handle;
  long confidence_level;
};
#endif
