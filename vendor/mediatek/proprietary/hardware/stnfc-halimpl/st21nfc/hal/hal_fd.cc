/******************************************************************************
 *
 *  Copyright (C) 2018 ST Microelectronics S.A.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 ******************************************************************************/
#define LOG_TAG "NfcHalFd"
#include "hal_fd.h"
#include <cutils/properties.h>
#include <errno.h>
#include <hardware/nfc.h>
#include <string.h>
#include "android_logmsg.h"
#include "halcore.h"
/* Initialize fw info structure pointer used to access fw info structure */
FWInfo *mFWInfo = NULL;
FILE *mFwFileBin;
FILE *mCustomFileBin;
FILE *mCustomFileTxt;
fpos_t mPos;
fpos_t mPosInit;
uint8_t mBinData[260];
bool mRetry = true;
bool mCustomParamFailed = false;
bool mCustomParamDone = false;
uint8_t *pCmd;
int mFWRecovCount = 0;
const char *FwType = "generic";
char mApduAuthent[24];
static const uint8_t propNfcModeSetCmdOn[] = {0x2f, 0x02, 0x02, 0x02, 0x01};
static const uint8_t coreInitCmd[] = {0x20, 0x01, 0x02, 0x00, 0x00};
static const uint8_t NciPropNfcFwUpdate[] = {0x2F, 0x02, 0x05, 0x06,
                                             0x00, 0x01, 0x02, 0x03};
static const uint8_t ApduActivateLoader[] = {0x2F, 0x04, 0x07, 0x80, 0xA6,
                                             0x00, 0x00, 0x02, 0xA1, 0xA0};

// static const uint8_t ApduEraseFlashLoaderRecovery[] =
//  { 0x2F, 0x04, 0x06, 0x80, 0x0C, 0x00, 0x00, 0x01, 0x04 };
static const uint8_t ApduEraseFlashLoaderPart1[] = {
    0x2F, 0x04, 0x06, 0x80, 0x0C, 0x00, 0x00, 0x01, 0x01};
static const uint8_t ApduEraseFlashLoaderPart2[] = {
    0x2F, 0x04, 0x06, 0x80, 0x0C, 0x00, 0x00, 0x01, 0x02};
static const uint8_t ApduEraseNfcFull[] = {0x2F, 0x04, 0x06, 0x80, 0x0C,
                                           0x00, 0x00, 0x01, 0x03};
static uint8_t ApduEraseNfcKeepAppliAndNdef_54j[] = {
    0x2F, 0x04, 0x16, 0x80, 0x0C, 0x00, 0x00, 0x11, 0x05,
    0x00, 0x23, 0xDF, 0x00, 0x00, 0x23, 0xDF, 0xFF, 0x00,
    0x23, 0xE0, 0x00, 0x00, 0x23, 0xFF, 0xFF};
static uint8_t ApduEraseNfcKeepAppliAndNdef_nfcd[] = {
    0x2F, 0x04, 0x16, 0x80, 0x0C, 0x00, 0x00, 0x11, 0x05,
    0x00, 0x13, 0xDF, 0x00, 0x00, 0x13, 0xDF, 0xFF, 0x00,
    0x13, 0xE0, 0x00, 0x00, 0x13, 0xFF, 0xFF};

static const uint8_t ApduExitLoadMode[] = {0x2F, 0x04, 0x06, 0x80, 0xA0,
                                           0x00, 0x00, 0x01, 0x01};

hal_fd_state_e mHalFDState = HAL_FD_STATE_AUTHENTICATE;

int loader_patch_version = -1;
int loader_patch_cmd_nb;
char loader_patch_AuthKeyId;
const char **loader_patch;
const char *loader_patch_size_tab;
uint8_t *pCmdLd;
int ld_count = 0;
extern const int loader_RA7_patch_version;
extern const int loader_RA7_patch_cmd_nb;
extern const char loader_RA7_patch_AuthKeyId;
extern const char *loader_RA7_patch[];
extern const char loader_RA7_patch_size_tab[];
extern const int loader_RA9_patch_version;
extern const int loader_RA9_patch_cmd_nb;
extern const char loader_RA9_patch_AuthKeyId;
extern const char *loader_RA9_patch[];
extern const char loader_RA9_patch_size_tab[];

void SendExitLoadMode(HALHANDLE mmHalHandle);
extern void hal_wrapper_update_complete();

static int ascii2hex(char c) {
  int res = -1;

  if ((c >= '0') && (c <= '9')) {
    res = c - '0';
  } else if ((c >= 'A') && (c <= 'F')) {
    res = c - 'A' + 10;
  } else if ((c >= 'a') && (c <= 'f')) {
    res = c - 'a' + 10;
  }

  return res;
}
/**
 * Send a HW reset and decode NCI_CORE_RESET_NTF information
 * @param pHwVersion is used to return HW version, part of NCI_CORE_RESET_NTF
 * @param pFwVersion is used to return FW version, part of NCI_CORE_RESET_NTF
 * @param pLoaderVersion is used to return Loader version, part of
 * NCI_CORE_RESET_NTF
 * @param pCustVersion si used to return Customer field value, part of
 * NCI_CORE_RESET_NTF when in router mode
 *
 * @return mode: FT_CLF_MODE_ROUTER if router mode
 *               FT_CLF_MODE_LOADER if loader mode
 *               FT_CLF_MODE_ERROR if Error
 */

int hal_fd_init() {
  uint8_t result = 0;
  char FwPath[256];
  char ConfPath[256];
  char fwBinName[256];
  char fwConfName[256];
  STLOG_HAL_D("  %s - enter", __func__);

  if (!GetStrValue(NAME_STNFC_FW_PATH_STORAGE, (char *)FwPath,
                   sizeof(FwPath))) {
    STLOG_HAL_D(
        "%s - FW path not found in conf. use default location "
        "/vendor/firmware/ "
        "\n",
        __func__);
    strcpy(FwPath, "/vendor/firmware/");
  }

  if (!GetStrValue(NAME_STNFC_FW_BIN_NAME, (char *)fwBinName,
                   sizeof(fwBinName))) {
    STLOG_HAL_D(
        "%s - FW binary file name not found in conf. use default name "
        "st21nfc_fw.bin \n",
        __func__);
    strcpy(fwBinName, "st21nfc_fw.bin");
  }

  if (!GetStrValue(NAME_STNFC_FW_CONF_NAME, (char *)fwConfName,
                   sizeof(fwConfName))) {
    STLOG_HAL_D(
        "%s - FW config file name not found in conf. use default name "
        "/vendor/etc/st21nfc_conf.txt\n",
        __func__);
    strcpy(fwConfName, "/vendor/etc/st21nfc_conf.txt");
  }

  // Getting information about FW patch, if any
  strcpy(ConfPath, FwPath);
  strncat(FwPath, fwBinName, sizeof(FwPath) - 1);
  if (fwConfName[0] == '/') {
    // absolute path
    strcpy(ConfPath, fwConfName);
  } else {
    // relative to STNFC_FW_PATH_STORAGE
    strncat(ConfPath, fwConfName, sizeof(ConfPath) - 1);
  }
  STLOG_HAL_D("%s - FW update binary file = %s", __func__, FwPath);
  STLOG_HAL_D("%s - FW config binary file = %s", __func__, ConfPath);

  // Initializing structure holding FW patch details
  mFWInfo = (FWInfo *)malloc(sizeof(FWInfo));

  if (mFWInfo == NULL) {
    STLOG_HAL_E("%s: malloc failed", __func__);
    result = 0;
  }

  memset(mFWInfo, 0, sizeof(FWInfo));

  mFwFileBin = NULL;
  mCustomFileBin = NULL;
  mCustomFileTxt = NULL;

  // Check if FW patch binary file is present
  // If not, get recovery FW patch file
  if ((mFwFileBin = fopen((char *)FwPath, "r")) == NULL) {
    STLOG_HAL_D("%s - %s not detected", __func__, fwBinName);
  } else {
    STLOG_HAL_D("%s - %s file detected\n", __func__, fwBinName);

    result |= FW_PATCH_AVAILABLE;
    fread(mBinData, sizeof(uint8_t), 4, mFwFileBin);
    mFWInfo->patchVersion =
        mBinData[0] << 24 | mBinData[1] << 16 | mBinData[2] << 8 | mBinData[3];

    fread(mApduAuthent, sizeof(uint8_t), 24, mFwFileBin);
    if (mApduAuthent[23] == 0xC7) {
      FwType = "RA7";
    }

    fgetpos(mFwFileBin, &mPosInit);

    STLOG_HAL_D(
        "%s --> st21nfc_fw integrates patch NFC FW version 0x%08X (%s)\n",
        __func__, mFWInfo->patchVersion, FwType);
  }

  if ((mCustomFileBin = fopen((char *)ConfPath, "r")) != NULL) {
    char conf_line[600];
    uint16_t fwconf_crc = 0;
    if (fwConfName[strlen(fwConfName) - 1] == 't') {
      mCustomFileTxt = mCustomFileBin;
      mCustomFileBin = NULL;
      STLOG_HAL_D("st21nfc text configuration detected\n");
      fgets(conf_line, sizeof conf_line, mCustomFileTxt);
      if ((conf_line[0] == 'R') && (conf_line[11] == 'C') &&
          (conf_line[12] == 'R')) {
        fwconf_crc = ascii2hex(conf_line[21]) |
                     ((ascii2hex(conf_line[20]) << 4) & 0xF0) |
                     ((ascii2hex(conf_line[19]) << 8) & 0xF00) |
                     ((ascii2hex(conf_line[18]) << 12) & 0xF000);
        mFWInfo->confVersion = fwconf_crc;
        STLOG_HAL_D("-> st21nfc_custom txt configuration version 0x%04X \n",
                    mFWInfo->confVersion);
        result |= FW_CUSTOM_PARAM_AVAILABLE;
      }

    } else if (fwConfName[strlen(fwConfName) - 1] == 'n') {
      fread(mBinData, sizeof(uint8_t), 2, mCustomFileBin);
      mFWInfo->confVersion = mBinData[0] << 8 | mBinData[1];
      STLOG_HAL_D("-> st21nfc_custom bin configuration version 0x%04X \n",
                  mFWInfo->confVersion);
      result |= FW_CUSTOM_PARAM_AVAILABLE;
    }
  } else {
    STLOG_HAL_D("st21nfc custom configuration not detected\n");
  }

  return result;
}

void hal_fd_close() {
  STLOG_HAL_D("  %s -enter", __func__);
  mCustomParamFailed = false;
  if (mFWInfo != NULL) {
    free(mFWInfo);
    mFWInfo = NULL;
  }
  if (mFwFileBin != NULL) {
    fclose(mFwFileBin);
    mFwFileBin = NULL;
  }
  if (mCustomFileBin != NULL) {
    fclose(mCustomFileBin);
    mCustomFileBin = NULL;
  }
  if (mCustomFileTxt != NULL) {
    fclose(mCustomFileTxt);
    mCustomFileTxt = NULL;
  }
}

/**
 * Send a HW reset and decode NCI_CORE_RESET_NTF information
 * @param pHwVersion is used to return HW version, part of NCI_CORE_RESET_NTF
 * @param pFwVersion is used to return FW version, part of NCI_CORE_RESET_NTF
 * @param pLoaderVersion is used to return Loader version, part of
 * NCI_CORE_RESET_NTF
 * @param pCustVersion si used to return Customer field value, part of
 * NCI_CORE_RESET_NTF when in router mode
 *
 * @return mode: FT_CLF_MODE_ROUTER if router mode
 *               FT_CLF_MODE_LOADER if loader mode
 *               FT_CLF_MODE_ERROR if Error
 */

uint8_t ft_cmd_HwReset(uint8_t *pdata, uint8_t *clf_mode) {
  uint8_t result = 0;

  STLOG_HAL_D("  %s - execution", __func__);

  if ((pdata[1] == 0x0) && (pdata[3] == 0x1)) {
    STLOG_HAL_D("-> Router Mode NCI_CORE_RESET_NTF received after HW Reset");

    /* retrieve HW Version from NCI_CORE_RESET_NTF */
    mFWInfo->hwVersion = pdata[8];
    STLOG_HAL_D("   HwVersion = 0x%02X", mFWInfo->hwVersion);
    mFWInfo->hwRevision = pdata[9];
    STLOG_HAL_D("   hwRevision = 0x%02X", mFWInfo->hwRevision);

    /* retrieve Authentication Key ID from NCI_CORE_RESET_NTF */
    mFWInfo->authKeyId = pdata[28];
    STLOG_HAL_D("   pAuthKeyId = 0x%02X", mFWInfo->authKeyId);

    /* retrieve FW Version from NCI_CORE_RESET_NTF */
    mFWInfo->fwVersion =
        (pdata[10] << 24) | (pdata[11] << 16) | (pdata[12] << 8) | pdata[13];
    STLOG_HAL_D("   FwVersion = 0x%08X", mFWInfo->fwVersion);

    /* retrieve Loader Version from NCI_CORE_RESET_NTF */
    mFWInfo->loaderVersion = (pdata[14] << 16) | (pdata[15] << 8) | pdata[16];
    STLOG_HAL_D("   LoaderVersion = 0x%06X", mFWInfo->loaderVersion);

    /* retrieve Customer Version from NCI_CORE_RESET_NTF */
    mFWInfo->custVersion = (pdata[31] << 8) | pdata[32];
    STLOG_HAL_D("   CustomerVersion = 0x%04X", mFWInfo->custVersion);

    *clf_mode = FT_CLF_MODE_ROUTER;
  } else if ((pdata[2] == 0x39) && (pdata[3] == 0xA1)) {
    STLOG_HAL_D("-> Loader Mode NCI_CORE_RESET_NTF received after HW Reset");

    /* deduce HW Version from Factory Loader version 16.17.18 */
    // RA6 : 020000
    // RA7 : 020200
    // RA8 : 020100
    // RA9 : 020300
    // WA3 : 010300
    if (pdata[16] == 0x01) {
      mFWInfo->hwVersion = 0x05;  // ST54J
    } else if (pdata[16] == 0x02) {
      mFWInfo->hwVersion = 0x04;  // ST21NFCD
      if (pdata[17] == 0x00) {
        mFWInfo->hwRevision = 0x03;  // RA6
      } else if (pdata[17] == 0x02 || pdata[17] == 0x03) {
        mFWInfo->hwRevision = 0x04;  // RA7, RA9
      }
    }

    STLOG_HAL_D("   HwVersion = 0x%02X", mFWInfo->hwVersion);
    STLOG_HAL_D("   HwRevision = 0x%02X", mFWInfo->hwRevision);

    /* retrieve Authentication Key ID from NCI_CORE_RESET_NTF */
    mFWInfo->authKeyId = pdata[51];
    STLOG_HAL_D("   pAuthKeyId = 0x%02X", mFWInfo->authKeyId);

    /* Identify the Active loader. Normally only one should be detected*/
    if (pdata[11] == 0xA0) {
      mFWInfo->loaderVersion = (pdata[8] << 16) | (pdata[9] << 8) | pdata[10];
      STLOG_HAL_D("         - Most recent loader activated, revision 0x%06X",
                  mFWInfo->loaderVersion);
    }
    if (pdata[15] == 0xA0) {
      mFWInfo->loaderVersion = (pdata[12] << 16) | (pdata[13] << 8) | pdata[14];
      STLOG_HAL_D("         - Least recent loader activated, revision 0x%06X",
                  mFWInfo->loaderVersion);
    }
    if (pdata[19] == 0xA0) {
      mFWInfo->loaderVersion = (pdata[16] << 16) | (pdata[17] << 8) | pdata[18];
      STLOG_HAL_D("         - Factory loader activated, revision 0x%06X",
                  mFWInfo->loaderVersion);
    }

    *clf_mode = FT_CLF_MODE_LOADER;
  } else {
    STLOG_HAL_E(
        "%s --> ERROR: wrong NCI_CORE_RESET_NTF received after HW Reset",
        __func__);
    *clf_mode = FT_CLF_MODE_ERROR;
  }

  if (FwType[0] == 'g') {
    if (mFWInfo->hwVersion == 0x04) {
      // St21NFCD
      if (mFWInfo->hwRevision == 0x04) {
        // RA9 is supported
        loader_patch_version = loader_RA9_patch_version;
        loader_patch_cmd_nb = loader_RA9_patch_cmd_nb;
        loader_patch_AuthKeyId = loader_RA9_patch_AuthKeyId;
        loader_patch = loader_RA9_patch;
        loader_patch_size_tab = loader_RA9_patch_size_tab;
      }
    }
  } else if (FwType[0] == 'R') {
    if (mFWInfo->hwVersion == 0x04) {
      // St21NFCD
      if (mFWInfo->hwRevision == 0x04) {
        // RA7 is supported
        loader_patch_version = loader_RA7_patch_version;
        loader_patch_cmd_nb = loader_RA7_patch_cmd_nb;
        loader_patch_AuthKeyId = loader_RA7_patch_AuthKeyId;
        loader_patch = loader_RA7_patch;
        loader_patch_size_tab = loader_RA7_patch_size_tab;
      }
    }
  }

  if (loader_patch_version != -1) {
    // can we update the loader ?
    if (((loader_patch_version & 0xFFFF00) ==
         (mFWInfo->loaderVersion & 0xFFFF00)) &&
        ((uint32_t)loader_patch_version > mFWInfo->loaderVersion)) {
      STLOG_HAL_D("Loader update needed.\n");
      result |= LD_UPDATE_NEEDED | FW_UPDATE_NEEDED | CONF_UPDATE_NEEDED;
    }
  }
  // Allow update only for ST54J or ST21NFCD
  if ((mFWInfo->hwVersion == HW_NFCD) || (mFWInfo->hwVersion == HW_ST54J)) {
    if ((mFwFileBin != NULL) && (mFWInfo->patchVersion != mFWInfo->fwVersion)) {
      STLOG_HAL_D("---> Firmware update needed\n");
      result |= FW_UPDATE_NEEDED | CONF_UPDATE_NEEDED;
    } else {
      STLOG_HAL_D("---> No Firmware update needed\n");
    }

    if ((mFWInfo->confVersion != 0) &&
        (mFWInfo->custVersion != mFWInfo->confVersion) &&
        (mFWInfo->custVersion != 0xEFAC /* DTA */)) {
      STLOG_HAL_D(
          "%s - Need to apply new st21nfc custom configuration settings\n",
          __func__);
      if (!mCustomParamFailed) result |= CONF_UPDATE_NEEDED;
    } else {
      STLOG_HAL_D("%s - No need to apply custom configuration settings\n",
                  __func__);
    }
  }

  return result;
} /* ft_cmd_HwReset */

void ExitHibernateHandler(HALHANDLE mHalHandle, uint16_t data_len,
                          uint8_t *p_data) {
  STLOG_HAL_D("%s - Enter", __func__);
  if (data_len < 3) {
    STLOG_HAL_E("%s - Error, too short data (%d)", __func__, data_len);
    return;
  }
  switch (p_data[0]) {
    case 0x40:  //
      STLOG_HAL_D("%s - hibernate_exited = %d ", __func__,
                  mFWInfo->hibernate_exited);

      // CORE_INIT_RSP
      if ((p_data[1] == 0x1) && (p_data[3] == 0x0) &&
          (mFWInfo->hibernate_exited == 0)) {
        // Send PROP_NFC_MODE_SET_CMD(ON)
        if (!HalSendDownstream(mHalHandle, propNfcModeSetCmdOn,
                               sizeof(propNfcModeSetCmdOn))) {
          STLOG_HAL_E("%s - SendDownstream failed", __func__);
        }
      } else if ((p_data[1] == 0x1) && (p_data[3] == 0x0) &&
                 (mFWInfo->hibernate_exited == 1)) {
        STLOG_HAL_D(
            "%s - send NCI_PROP_NFC_FW_UPDATE_CMD and use 100 ms timer for "
            "each cmd from here",
            __func__);

        if (!HalSendDownstreamTimer(mHalHandle, NciPropNfcFwUpdate,
                                    sizeof(NciPropNfcFwUpdate),
                                    FW_TIMER_DURATION)) {
          STLOG_HAL_E("%s  SendDownstream failed", __func__);
        }
      } else if (p_data[3] != 0x00) {
        STLOG_HAL_D("%s - Wrong response. Retry HW reset", __func__);
        I2cResetPulse();
        hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
      }
      break;

    case 0x4f:  //
      if ((p_data[1] == 0x02) && (p_data[3] == 0x00) &&
          (mFWInfo->hibernate_exited == 1)) {
        STLOG_HAL_D("%s - NCI_PROP_NFC_FW_RSP : loader mode", __func__);
        I2cResetPulse();
        hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
      } else if (p_data[3] != 0x00) {
        STLOG_HAL_D("%s - Wrong response. Retry HW reset", __func__);
        I2cResetPulse();
        hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
      }
      break;
    case 0x60:  //
      if (p_data[3] == 0x2) {
        STLOG_HAL_D("%s - CORE_RESET_NTF : after core_reset_cmd", __func__);

        if (!HalSendDownstream(mHalHandle, coreInitCmd, sizeof(coreInitCmd))) {
          STLOG_HAL_E("%s - SendDownstream failed", __func__);
        }
      } else if (p_data[3] == 0xa0) {
        mFWInfo->hibernate_exited = 1;
        STLOG_HAL_D("%s - hibernate_exited = %d ", __func__,
                    mFWInfo->hibernate_exited);

        if (!HalSendDownstream(mHalHandle, coreInitCmd, sizeof(coreInitCmd))) {
          STLOG_HAL_E("%s - SendDownstream failed", __func__);
        }
      }
      break;
  }
}

void resetHandlerState() {
  STLOG_HAL_D("%s", __func__);
  mHalFDState = HAL_FD_STATE_AUTHENTICATE;
}

void LdUpdateHandler(HALHANDLE mHalHandle, uint16_t data_len, uint8_t *p_data) {
  STLOG_HAL_D("%s : Enter state = %d", __func__, mHalFDState);
  HalSendDownstreamStopTimer(mHalHandle);

  switch (mHalFDState) {
    case HAL_FD_STATE_AUTHENTICATE:  // we receive response to GET ATR
      STLOG_HAL_D("%s - mHalFDState = HAL_FD_STATE_AUTHENTICATE", __func__);
      if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
        STLOG_HAL_D("%s - send APDU_AUTHENTICATION_CMD", __func__);
        if (!HalSendDownstreamTimer(mHalHandle, (uint8_t *)mApduAuthent,
                                    sizeof(mApduAuthent), FW_TIMER_DURATION)) {
          STLOG_HAL_E("%s - SendDownstream failed", __func__);
        }
        mHalFDState = HAL_LD_STATE_ERASE_FLASH1;
      } else {
        STLOG_HAL_D("%s : LD flash not succeeded", __func__);
        SendExitLoadMode(mHalHandle);
      }
      break;
    case HAL_LD_STATE_ERASE_FLASH1:  // 1
      STLOG_HAL_D("%s - mHalFDState = HAL_LD_STATE_ERASE_FLASH1", __func__);

      if ((p_data[0] == 0x4f) && (p_data[1] == 0x04)) {
        if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
          STLOG_HAL_D("  %s : send APDU_ERASE_FLASH_LOADER (area 1)", __func__);
          if (!HalSendDownstreamTimer(mHalHandle, ApduEraseFlashLoaderPart1,
                                      sizeof(ApduEraseFlashLoaderPart1),
                                      FW_TIMER_DURATION)) {
            STLOG_HAL_E("NFC-NCI HAL: %s  SendDownstream failed", __func__);
          }
          mHalFDState = HAL_LD_STATE_ERASE_FLASH2;
        } else {
          STLOG_HAL_D("%s : FW flash not succeeded", __func__);
          SendExitLoadMode(mHalHandle);
        }
      }
      break;
    case HAL_LD_STATE_ERASE_FLASH2:  // 2
      STLOG_HAL_D("%s - mHalFDState = HAL_LD_STATE_ERASE_FLASH2", __func__);

      if ((p_data[0] == 0x4f) && (p_data[1] == 0x04)) {
        if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
          STLOG_HAL_D("  %s : send APDU_ERASE_FLASH_LOADER (area 2)", __func__);
          if (!HalSendDownstreamTimer(mHalHandle, ApduEraseFlashLoaderPart2,
                                      sizeof(ApduEraseFlashLoaderPart2),
                                      FW_TIMER_DURATION)) {
            STLOG_HAL_E("NFC-NCI HAL: %s  SendDownstream failed", __func__);
          }
          mHalFDState = HAL_FD_STATE_SEND_RAW_APDU;
          pCmdLd = (uint8_t *)loader_patch;
        } else {
          STLOG_HAL_D("%s : FW flash not succeeded", __func__);
          SendExitLoadMode(mHalHandle);
        }
      }
      break;

    case HAL_FD_STATE_SEND_RAW_APDU:  // 2
      if ((p_data[0] == 0x4f) && (p_data[1] == 0x04)) {
        if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
          ALOGD("%s : ld ld_count = %d", __func__, ld_count);
          if (ld_count < loader_patch_cmd_nb) {
            if (!HalSendDownstreamTimer(mHalHandle, pCmdLd,
                                        loader_patch_size_tab[ld_count],
                                        FW_TIMER_DURATION)) {
              ALOGE("NFC-NCI HAL: %s  SendDownstream failed!", __func__);
            }
            pCmdLd += loader_patch_size_tab[ld_count];
            ld_count++;
          } else if (ld_count == loader_patch_cmd_nb) {
            STLOG_HAL_D("  %s : send APDU_ACTIVATE_LOADER", __func__);
            if (!HalSendDownstreamTimer(mHalHandle, ApduActivateLoader,
                                        sizeof(ApduActivateLoader),
                                        FW_TIMER_DURATION)) {
              STLOG_HAL_E("NFC-NCI HAL: %s  SendDownstream failed", __func__);
            }
            ld_count++;
          } else {
            ALOGD("%s : EOF of loader", __func__);
            ld_count = 0;
            SendExitLoadMode(mHalHandle);
          }
        } else {
          STLOG_HAL_D("%s : LD flash not succeeded", __func__);
          SendExitLoadMode(mHalHandle);
        }
      }
      break;

    case HAL_FD_STATE_EXIT_APDU:  // 2
      if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
        I2cResetPulse();
        hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
        mHalFDState = HAL_FD_STATE_AUTHENTICATE;
      } else {
        I2cResetPulse();
        hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
        mHalFDState = HAL_FD_STATE_AUTHENTICATE;
      }
      break;

    default:
      STLOG_HAL_D("%s : FW flash not succeeded", __func__);
      SendExitLoadMode(mHalHandle);
      break;
  }
}

void UpdateHandler(HALHANDLE mHalHandle, uint16_t data_len, uint8_t *p_data) {
  STLOG_HAL_D("%s : Enter state = %d", __func__, mHalFDState);
  HalSendDownstreamStopTimer(mHalHandle);

  switch (mHalFDState) {
    case HAL_FD_STATE_AUTHENTICATE:
      STLOG_HAL_D("%s - mHalFDState = HAL_FD_STATE_AUTHENTICATE", __func__);

      if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
        STLOG_HAL_D("%s - send APDU_AUTHENTICATION_CMD", __func__);
        if (!HalSendDownstreamTimer(mHalHandle, (uint8_t *)mApduAuthent,
                                    sizeof(mApduAuthent), FW_TIMER_DURATION)) {
          STLOG_HAL_E("%s - SendDownstream failed", __func__);
        }
        mHalFDState = HAL_FD_STATE_ERASE_FLASH;
      } else {
        STLOG_HAL_D("%s - FW flash not succeeded", __func__);
        SendExitLoadMode(mHalHandle);
      }
      break;

    case HAL_FD_STATE_ERASE_FLASH:  // 1
      STLOG_HAL_D("%s - mHalFDState = HAL_FD_STATE_ERASE_FLASH", __func__);

      if ((p_data[0] == 0x4f) && (p_data[1] == 0x04)) {
        if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
          if (mFWInfo->hwVersion == HW_NFCD) {
            if (mFWInfo->hwRevision == 0x04) {
              // RA7, RA9
              STLOG_HAL_D(
                  " %s - send APDU_ERASE_FLASH_CMD (keep appli and NDEF areas)",
                  __func__);
              if (!HalSendDownstreamTimer(
                      mHalHandle, ApduEraseNfcKeepAppliAndNdef_nfcd,
                      sizeof(ApduEraseNfcKeepAppliAndNdef_nfcd),
                      FW_TIMER_DURATION)) {
                STLOG_HAL_E("%s - SendDownstream failed", __func__);
              }
            } else if (mFWInfo->hwRevision == 0x03) {
              // RA6, loader does not support partial erase
              STLOG_HAL_D(
                  " %s - send APDU_ERASE_FLASH_CMD (erase all NFC memory)",
                  __func__);
              if (!HalSendDownstreamTimer(mHalHandle, ApduEraseNfcFull,
                                          sizeof(ApduEraseNfcFull),
                                          FW_TIMER_DURATION)) {
                STLOG_HAL_E("%s - SendDownstream failed", __func__);
              }
            }
          } else if (mFWInfo->hwVersion == HW_ST54J) {
            STLOG_HAL_D(
                " %s - send APDU_ERASE_FLASH_CMD (keep appli and NDEF areas)",
                __func__);
            if (!HalSendDownstreamTimer(
                    mHalHandle, ApduEraseNfcKeepAppliAndNdef_54j,
                    sizeof(ApduEraseNfcKeepAppliAndNdef_54j),
                    FW_TIMER_DURATION)) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
          }

          fsetpos(mFwFileBin, &mPosInit);  // reset pos in stream

          mHalFDState = HAL_FD_STATE_SEND_RAW_APDU;

        } else {
          STLOG_HAL_D("%s - FW flash not succeeded", __func__);
          SendExitLoadMode(mHalHandle);
        }
      }
      break;

    case HAL_FD_STATE_SEND_RAW_APDU:  // 3
      STLOG_HAL_D("%s - mHalFDState = HAL_FD_STATE_SEND_RAW_APDU", __func__);
      if ((p_data[0] == 0x4f) && (p_data[1] == 0x04)) {
        if ((p_data[data_len - 2] == 0x90) && (p_data[data_len - 1] == 0x00)) {
          mRetry = true;

          fgetpos(mFwFileBin, &mPos);  // save current position in stream
          if ((fread(mBinData, sizeof(uint8_t), 3, mFwFileBin) == 3) &&
              (fread(mBinData + 3, sizeof(uint8_t), mBinData[2], mFwFileBin) ==
               mBinData[2])) {
            if (!HalSendDownstreamTimer(mHalHandle, mBinData, mBinData[2] + 3,
                                        FW_TIMER_DURATION)) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
          } else {
            STLOG_HAL_D("%s - EOF of FW binary", __func__);
            SendExitLoadMode(mHalHandle);
          }
        } else if (mRetry == true) {
          STLOG_HAL_D("%s - Last Tx was NOK. Retry", __func__);
          mRetry = false;
          fsetpos(mFwFileBin, &mPos);
          if ((fread(mBinData, sizeof(uint8_t), 3, mFwFileBin) == 3) &&
              (fread(mBinData + 3, sizeof(uint8_t), mBinData[2], mFwFileBin) ==
               mBinData[2])) {
            if (!HalSendDownstreamTimer(mHalHandle, mBinData, mBinData[2] + 3,
                                        FW_TIMER_DURATION)) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
            fgetpos(mFwFileBin, &mPos);  // save current position in stream
          } else {
            STLOG_HAL_D("%s - EOF of FW binary", __func__);
            SendExitLoadMode(mHalHandle);
          }
        } else {
          STLOG_HAL_D("%s - FW flash not succeeded.", __func__);
          I2cResetPulse();
          SendExitLoadMode(mHalHandle);
        }
      }
      break;

    case HAL_FD_STATE_EXIT_APDU:  // 2
      STLOG_HAL_D("%s - mHalFDState = HAL_FD_STATE_EXIT_APDU", __func__);
      if ((p_data[data_len - 2] != 0x90) || (p_data[data_len - 1] != 0x00)) {
        STLOG_HAL_D(
            "%s - Error exiting loader mode, i.e. a problem occured during FW "
            "update",
            __func__);
      }

      I2cResetPulse();
      hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
      mHalFDState = HAL_FD_STATE_AUTHENTICATE;
      break;

    default:
      STLOG_HAL_D("%s - mHalFDState = unknown", __func__);
      STLOG_HAL_D("%s - FW flash not succeeded", __func__);
      SendExitLoadMode(mHalHandle);
      break;
  }
}

/**
 * ASCII to Hexadecimal conversion (whole line)
 * @param input, a \0-terminated string with ASCII bytes representation (e.g. 01
 * 23 45). Spaces are allowed, but must be aligned on bytes boundaries.
 * @param pCmd, converted bytes are stored here.
 * @param maxLen, storage size of pCmd
 * @param pcmdlen, how many bytes have been written upon return.
 * @return 0 on success, -1 on failure */
static int convstr2hex(char *input, uint8_t *pCmd, int maxLen,
                       uint16_t *pcmdlen) {
  char *in = input;
  int c;
  *pcmdlen = 0;

  while ((in[0] != '\0') && (in[1] != '\0') &&
         (*pcmdlen < maxLen))  // we need at least 2 characters left
  {
    // Skip white spaces
    if (in[0] == ' ' || in[0] == '\t' || in[0] == '\r' || in[0] == '\n') {
      in++;
      continue;
    }

    // Is MSB char a valid HEX value ?
    c = ascii2hex(*in);
    if (c < 0) {
      STLOG_HAL_E("    Error: invalid character (%x,'%c')\n", *in, *in);
      return -1;
    }
    // Store it
    pCmd[*pcmdlen] = c << 4;
    in++;

    // Is LSB char a valid HEX value ?
    c = ascii2hex(*in);
    if (c < 0) {
      STLOG_HAL_E("    Error: invalid character (%x,'%c')\n", *in, *in);
      return -1;
    }
    // Store it
    pCmd[*pcmdlen] |= c;
    in++;
    (*pcmdlen)++;
  }

  if (*pcmdlen == maxLen) {
    STLOG_HAL_D("    Warning: input conversion may be truncated\n");
  }

  return 0;
}

int ft_FwConfConvertor(char *string_cmd, uint8_t pCmd[256], uint16_t *pcmdlen) {
  uint16_t converted;
  int res = convstr2hex(string_cmd, pCmd + 3, 256 - 3, &converted);
  if (res < 0) {
    *pcmdlen = 0;
    return 0;
  }
  // We should be able to propagate an error here, TODO: if (res < 0) ....
  pCmd[0] = 0x2F;
  pCmd[1] = 0x02;
  pCmd[2] = converted;
  *pcmdlen = converted + 3;
  return 1;
}
// parse st21nfc_conf.txt until next command to send.
// return 1 if a command was found, 0 if EOF
int getNextCommandInTxt(uint8_t *cmd, uint16_t *sz) {
  int ret = 0;
  // f_cust_txt is already opened and 1st line read
  char conf_line[600];

  while (fgets(conf_line, sizeof conf_line, mCustomFileTxt) != NULL) {
    if (!strncmp(conf_line, "NCI_SEND_PROP", sizeof("NCI_SEND_PROP") - 1)) {
      STLOG_HAL_V("%s : parse %s", __func__, conf_line);
      ret = ft_FwConfConvertor((char *)conf_line + 20, cmd, sz);
      break;
    } else if (!strncmp(conf_line, "NCI_DIRECT_CTRL",
                        sizeof("NCI_DIRECT_CTRL") - 1)) {
      STLOG_HAL_V("%s : parse %s", __func__, conf_line);
      ret = ft_FwConfConvertor((char *)conf_line + 22, cmd, sz);
      break;
    } else {
      // any other, we ignore
      STLOG_HAL_V("%s : ignore %s", __func__, conf_line);
    }
  }

  return ret;
}
void ApplyCustomParamHandler(HALHANDLE mHalHandle, uint16_t data_len,
                             uint8_t *p_data) {
  STLOG_HAL_D("%s - Enter ", __func__);
  if (data_len < 3) {
    STLOG_HAL_E("%s : Error, too short data (%d)", __func__, data_len);
    return;
  }

  if (mCustomFileTxt != NULL) {
    uint8_t txtCmd[MAX_BUFFER_SIZE];
    uint16_t txtCmdLen = 0;

    switch (p_data[0]) {
      case 0x40:  //
        // CORE_RESET_RSP
        if ((p_data[1] == 0x0) && (p_data[3] == 0x0)) {
          // do nothing
        } else if ((p_data[1] == 0x1) && (p_data[3] == 0x0)) {
          if (mFWInfo->hibernate_exited == 0) {
            // Send a NFC mode on .
            if (!HalSendDownstream(mHalHandle, propNfcModeSetCmdOn,
                                   sizeof(propNfcModeSetCmdOn))) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
            // CORE_INIT_RSP
          } else if (mFWInfo->hibernate_exited == 1) {
            if (getNextCommandInTxt(txtCmd, &txtCmdLen)) {
              if (!HalSendDownstream(mHalHandle, txtCmd, txtCmdLen)) {
                STLOG_HAL_E("NFC-NCI HAL: %s  SendDownstream failed", __func__);
              }
            }
          }

        } else {
          STLOG_HAL_D("%s - Error in custom param application", __func__);
          mCustomParamFailed = true;
          I2cResetPulse();
          hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
        }
        break;

      case 0x4f:
        if (mFWInfo->hibernate_exited == 1) {
          if (getNextCommandInTxt(txtCmd, &txtCmdLen)) {
            if (!HalSendDownstream(mHalHandle, txtCmd, txtCmdLen)) {
              STLOG_HAL_E("NFC-NCI HAL: %s  SendDownstream failed", __func__);
            }
          } else {
            STLOG_HAL_D("%s - EOF of custom file", __func__);
            mCustomParamDone = true;
            I2cResetPulse();
          }

          // Check if an error has occured for PROP_SET_CONFIG_CMD
          // Only log a warning, do not exit code
          if (p_data[3] != 0x00) {
            STLOG_HAL_D("%s - Error in custom file, continue anyway", __func__);
          }
        }
        break;

      case 0x60:  //
        if (p_data[1] == 0x0) {
          if (p_data[3] == 0xa0) {
            mFWInfo->hibernate_exited = 1;
          }
          if (!HalSendDownstream(mHalHandle, coreInitCmd,
                                 sizeof(coreInitCmd))) {
            STLOG_HAL_E("%s - SendDownstream failed", __func__);
          }

        } else if ((p_data[1] == 0x6) && mCustomParamDone) {
          mCustomParamDone = false;
          hal_wrapper_update_complete();
        }
        break;
    }

  } else if (mCustomFileBin != NULL) {
    switch (p_data[0]) {
      case 0x40:  //
        // CORE_RESET_RSP
        if ((p_data[1] == 0x0) && (p_data[3] == 0x0)) {
          // do nothing
        } else if ((p_data[1] == 0x1) && (p_data[3] == 0x0)) {
          if (mFWInfo->hibernate_exited == 0) {
            // Send a NFC mode on .
            if (!HalSendDownstream(mHalHandle, propNfcModeSetCmdOn,
                                   sizeof(propNfcModeSetCmdOn))) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
            // CORE_INIT_RSP
          } else if (mFWInfo->hibernate_exited == 1) {
            if ((fread(mBinData, sizeof(uint8_t), 3, mCustomFileBin)) &&
                (fread(mBinData + 3, sizeof(uint8_t), mBinData[2],
                       mCustomFileBin))) {
              if (!HalSendDownstream(mHalHandle, mBinData, mBinData[2] + 3)) {
                STLOG_HAL_E("%s - SendDownstream failed", __func__);
              }
            }
          }

        } else {
          STLOG_HAL_D("%s - Error in custom param application", __func__);
          mCustomParamFailed = true;
          I2cResetPulse();
          hal_wrapper_set_state(HAL_WRAPPER_STATE_OPEN);
        }
        break;

      case 0x4f:
        if (mFWInfo->hibernate_exited == 1) {
          if ((fread(mBinData, sizeof(uint8_t), 3, mCustomFileBin) == 3) &&
              (fread(mBinData + 3, sizeof(uint8_t), mBinData[2],
                     mCustomFileBin) == mBinData[2])) {
            if (!HalSendDownstream(mHalHandle, mBinData, mBinData[2] + 3)) {
              STLOG_HAL_E("%s - SendDownstream failed", __func__);
            }
          } else {
            STLOG_HAL_D("%s - EOF of custom file", __func__);
            mCustomParamDone = true;
            I2cResetPulse();
          }

          // Check if an error has occured for PROP_SET_CONFIG_CMD
          // Only log a warning, do not exit code
          if (p_data[3] != 0x00) {
            STLOG_HAL_D("%s - Error in custom file, continue anyway", __func__);
          }
        }
        break;

      case 0x60:  //
        if (p_data[1] == 0x0) {
          if (p_data[3] == 0xa0) {
            mFWInfo->hibernate_exited = 1;
          }
          if (!HalSendDownstream(mHalHandle, coreInitCmd,
                                 sizeof(coreInitCmd))) {
            STLOG_HAL_E("%s - SendDownstream failed", __func__);
          }

        } else if ((p_data[1] == 0x6) && mCustomParamDone) {
          mCustomParamDone = false;
          hal_wrapper_update_complete();
        }
        break;
    }
  }
}

void SendExitLoadMode(HALHANDLE mmHalHandle) {
  STLOG_HAL_D("%s - Send APDU_EXIT_LOAD_MODE_CMD", __func__);

  if (!HalSendDownstreamTimer(mmHalHandle, ApduExitLoadMode,
                              sizeof(ApduExitLoadMode), FW_TIMER_DURATION)) {
    STLOG_HAL_E("%s - SendDownstream failed", __func__);
  }
  mHalFDState = HAL_FD_STATE_EXIT_APDU;
}
