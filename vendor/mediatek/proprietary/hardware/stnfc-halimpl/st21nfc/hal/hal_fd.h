/** ----------------------------------------------------------------------
 *
 * Copyright (C) 2018 ST Microelectronics S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ----------------------------------------------------------------------*/

#ifndef HAL_FD_H_
#define HAL_FD_H_

#include "halcore.h"

/*
 *Structure containing fw version info
 */
typedef struct FWInfo {
  uint32_t patchVersion;
  uint32_t fwVersion;
  uint8_t hwVersion;
  uint8_t hwRevision;
  uint8_t authKeyId;
  uint32_t loaderVersion;
  uint16_t custVersion;
  uint16_t confVersion;
  bool hibernate_exited;
} FWInfo;

typedef enum {
  HAL_FD_STATE_AUTHENTICATE,
  HAL_FD_STATE_ERASE_FLASH,
  HAL_LD_STATE_ERASE_FLASH1,
  HAL_LD_STATE_ERASE_FLASH2,
  HAL_FD_STATE_SEND_RAW_APDU,
  HAL_FD_STATE_EXIT_APDU,
} hal_fd_state_e;

#define FT_CLF_MODE_ERROR 0
#define FT_CLF_MODE_LOADER 1
#define FT_CLF_MODE_ROUTER 2

#define FW_TIMER_DURATION 3000

#define FW_PATCH_AVAILABLE 1
#define FW_CUSTOM_PARAM_AVAILABLE 2

#define FW_UPDATE_NEEDED 1
#define CONF_UPDATE_NEEDED 2
#define LD_UPDATE_NEEDED 4

#define MAX_BUFFER_SIZE 300

#define HW_NFCD 0x04
#define HW_ST54J 0x05

extern const int nfc_patch_cmd_nb;
extern const char ApduAuthentRecov[24];
extern const char nfc_patch[];
extern const char nfc_patch_size_tab[];

/* Function declarations */
int hal_fd_init();
void hal_fd_close();
uint8_t ft_cmd_HwReset(uint8_t* pdata, uint8_t* clf_mode);
void ExitHibernateHandler(HALHANDLE mHalHandle, uint16_t data_len,
                          uint8_t* p_data);
void UpdateHandler(HALHANDLE mHalHandle, uint16_t data_len, uint8_t* p_data);
void LdUpdateHandler(HALHANDLE mHalHandle, uint16_t data_len, uint8_t* p_data);
void ApplyCustomParamHandler(HALHANDLE mHalHandle, uint16_t data_len,
                             uint8_t* p_data);
void resetHandlerState();
#endif /* HAL_FD_H_ */
