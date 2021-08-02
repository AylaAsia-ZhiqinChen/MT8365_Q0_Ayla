/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016~2017. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

//- vim: set ts=4 sts=4 sw=4 et: --------------------------------------------
#include <errno.h>
#include "boots_country.h"

#define LOG_TAG "boots_country"

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_get_limit(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_set_limit(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_dump_limit(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_create_limit_table(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_get_srs(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_set_srs(char *cmd[], size_t len, int index);
static pkt_list_s *mtk_boots_country_dump_srs(char *cmd[], size_t len, int index);

//---------------------------------------------------------------------------
#define USAGE_COUNTRY_GET_LIMIT_DETAILS \
    "      [c] Country code: country code value\n" \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtCountryLimit.dat"

#define USAGE_COUNTRY_SET_LIMIT_DETAILS \
    "      [c] Country code: country code value\n" \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtCountryLimit.dat" \
    "      [b] BT TX power level\n" \
    "      [e] BLE TX power level\n" \
    "      [m] Max TX power level"

#define USAGE_COUNTRY_DUMP_LIMIT_DETAILS \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtCountryLimit.dat"

#define USAGE_COUNTRY_CREATE_LIMIT_DETAILS \
    "      [c] Country code: country code value\n" \
    "      [t] File path: full path and name of country code [t]able file, default:/vendor/firmware/BtCountryLimit.dat\n" \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtTxPwrLimit.bin"

#define USAGE_COUNTRY_GET_SRS_DETAILS \
    "      [c] Country code: country code value\n" \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtSRS.dat"

#define USAGE_COUNTRY_SET_SRS_DETAILS \
    "      [c] Country_code: country code value\n" \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtSRS.dat\n" \
    "      [s] SRS: SRS value"

#define USAGE_COUNTRY_DUMP_SRS_DETAILS \
    "      [f] File path: full path and name of table file, default:/vendor/firmware/BtSRS.dat"

boots_country_cmds_s country_commands[] = {
    { "get_limit", mtk_boots_country_get_limit,
        "Get power limit with country code. [-c <country_code>] [-f <file_path>]",
        USAGE_COUNTRY_GET_LIMIT_DETAILS, false },
    { "set_limit", mtk_boots_country_set_limit,
        "Set power limit with country code. [-c <country_code>] [-f <file_path>] [-b <0-9>] [-e <0-9>] [-m <0-9>]",
        USAGE_COUNTRY_SET_LIMIT_DETAILS, false },
    { "dump_limit", mtk_boots_country_dump_limit,
        "Dump power limit with all country code. [-f <file_path>]",
        USAGE_COUNTRY_DUMP_LIMIT_DETAILS, false },
    { "create_table", mtk_boots_country_create_limit_table,
        "Create power limit table. [-c <country_code>] [-t <file_path>] [-f <file_path>]",
        USAGE_COUNTRY_CREATE_LIMIT_DETAILS, false },
    { "get_srs", mtk_boots_country_get_srs,
        "Get SRS with country code. [-c <country_code>] [-f <file_path>]",
        USAGE_COUNTRY_GET_SRS_DETAILS, false },
    { "set_srs", mtk_boots_country_set_srs,
        "Set SRS with country code. [-c <country_code>] [-f <file_path>] [-s <SRS>]",
        USAGE_COUNTRY_SET_SRS_DETAILS, false },
    { "dump_srs", mtk_boots_country_dump_srs,
        "Dump SRS with all country code. [-f <file_path>]",
        USAGE_COUNTRY_DUMP_SRS_DETAILS, false },
    { NULL, NULL, NULL, NULL, NULL }
};

/****************************************************
 Table format:
 Country code|BT TX power level|BLE TX power level|Max TX power level|Diff mode
****************************************************/
static pkt_list_s *mtk_boots_country_get_limit(char *cmd[], size_t len, int index)
{
    FILE *fp;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    char path[64] = DEFAULT_COUNTRY_LIMIT;
    size_t i = 1;
    int country_code = 0;
    int temp_code = 0;
    int bt_pwr = 0;
    int ble_pwr = 0;
    int max_pwr = 0;
    uint8_t find = 0;
    char *delim = "|";
    char *pch = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            i += 2;
            break;
        case 'c':
            country_code = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
    } else {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0) {
                    temp_code = atoi(pch);
                    if (temp_code != country_code)
                        break;
                    else
                        find = 1;
                } else if (i == 1)
                    bt_pwr = atoi(pch);
                else if (i == 2)
                    ble_pwr = atoi(pch);
                else if (i == 3)
                    max_pwr = atoi(pch);
                else
                    BPRINT_D("Out of table");
                pch = strtok (NULL, delim);
                i++;
            }
            if (find == 1)
                break;
        }
        free(line);
        fclose(fp);
        if (find == 1) {
            BPRINT_I("Country_code:%d BT_Power:%d BLE_Power:%d Max_Power:%d", country_code, bt_pwr, ble_pwr, max_pwr);
        } else {
            BPRINT_I("Country_code:%d Not Find.", country_code);
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_set_limit(char *cmd[], size_t len, int index)
{
    FILE *fp;
    ssize_t read;
    size_t line_len = 0;
    char *wbuf = NULL;
    char *line = NULL;
    char *line_cp = NULL;
    int fp_len = 0;
    char path[64] = DEFAULT_COUNTRY_LIMIT;
    char path_cp[64] = DEFAULT_COUNTRY_LIMIT;
    size_t i = 1;
    ssize_t w_size = 0;
    int country_code = 0;
    int temp_code = 0;
    int bt_pwr = 0;
    int ble_pwr = 0;
    int max_pwr = 0;
    char *delim = "|";
    char *pch = NULL;
    char str_buf[13];

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            strncpy(path_cp, cmd[i + 1], sizeof(path_cp) - 1);
            i += 2;
            break;
        case 'c':
            country_code = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 'b':
            bt_pwr = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 'e':
            ble_pwr = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 'm':
            max_pwr = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r+");
    if (fp != NULL) {
        fseek(fp, 0L, SEEK_END);
        fp_len = ftell(fp);
        wbuf = malloc(fp_len + 1 + 12);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            fclose(fp);
            return NULL;
        }
        fseek(fp, 0L, SEEK_SET);

        while ((read = getline(&line, &line_len, fp)) != -1) {
            line_cp = malloc(read);
            if (!line_cp) {
                BPRINT_E("%s: Alloc line_cp failed", __func__);
                free(line);
                free(wbuf);
                fclose(fp);
                return NULL;
            }
            memcpy(line_cp, line, read);
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0) {
                    temp_code = atoi(pch);
                    if (temp_code != country_code) {
                        memcpy(&wbuf[w_size], line_cp, read);
                        w_size += read;
                    }
                }
                pch = strtok (NULL, delim);
                i++;
            }
            free(line_cp);
        }
        free(line);
        fclose(fp);
    } else {
        wbuf = malloc(13);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            return NULL;
        }
    }

    i = sprintf(str_buf, "%03d|%01d|%01d|%01d|0\n", country_code, bt_pwr, ble_pwr, max_pwr);
    str_buf[12] = '\0';
    memcpy(&wbuf[w_size], str_buf, i);
    w_size += i;
    fp = fopen(path_cp, "w");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path_cp, strerror(errno), errno);
        free(wbuf);
        return NULL;
    }
    fwrite(wbuf, 1, w_size, fp);
    fclose(fp);
    free(wbuf);

    BPRINT_I("Set Country_code:%d BT_Power:%d BLE_Power:%d Max_Power:%d", country_code, bt_pwr, ble_pwr, max_pwr);
    return NULL;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_dump_limit(char *cmd[], size_t len, int index)
{
    FILE *fp;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    char path[64] = DEFAULT_COUNTRY_LIMIT;
    size_t i = 1;
    int country_code = 0;
    int bt_pwr = 0;
    int ble_pwr = 0;
    int max_pwr = 0;
    char *delim = "|";
    char *pch = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
    } else {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0)
                    country_code = atoi(pch);
                else if (i == 1)
                    bt_pwr = atoi(pch);
                else if (i == 2)
                    ble_pwr = atoi(pch);
                else if (i == 3)
                    max_pwr = atoi(pch);
                else if (i == 4) {
                    BPRINT_I("Country_code:%d BT_Power:%d BLE_Power:%d Max_Power:%d", country_code, bt_pwr, ble_pwr, max_pwr);
                } else
                    BPRINT_D("Out of table");
                pch = strtok (NULL, delim);
                i++;
            }
        }
        free(line);
        fclose(fp);
    }
    return NULL;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_create_limit_table(char *cmd[], size_t len, int index)
{
    FILE *fp;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    char path[64] = DEFAULT_COUNTRY_LIMIT;
    char table_path[64] = DEFAULT_POWER_LIMIT;
    size_t i = 1;
    int country_code = 0;
    int temp_code = 0;
    uint8_t bt_pwr = 0;
    uint8_t ble_pwr = 0;
    uint8_t max_pwr = 0;
    uint8_t diff = 0;
    uint8_t find = 0;
    char *delim = "|";
    char *pch = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 't':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            i += 2;
            break;
        case 'f':
            memset(table_path, 0x00, sizeof(table_path));
            strncpy(table_path, cmd[i + 1], sizeof(table_path) - 1);
            i += 2;
            break;
        case 'c':
            country_code = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
        return NULL;
    } else {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0) {
                    temp_code = atoi(pch);
                    if (temp_code != country_code)
                        break;
                    else
                        find = 1;
                } else if (i == 1)
                    bt_pwr = atoi(pch);
                else if (i == 2)
                    ble_pwr = atoi(pch);
                else if (i == 3)
                    max_pwr = atoi(pch);
                else if (i == 4)
                    diff = atoi(pch);
                else
                    BPRINT_D("Out of table");
                pch = strtok (NULL, delim);
                i++;
            }
            if (find == 1)
                break;
        }
        free(line);
        fclose(fp);
        if (find == 1) {
            BPRINT_I("Country_code:%d BT_Power:%d BLE_Power:%d Max_Power:%d", country_code, bt_pwr, ble_pwr, max_pwr);
        } else {
            BPRINT_I("Country_code:%d Not Find.", country_code);
            return NULL;
        }
    }
    fp = fopen(table_path, "wb");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
        return NULL;
    }

    BPRINT_I("Write to table: %s", table_path);
    fwrite(&bt_pwr, 1, sizeof(uint8_t), fp);
    fwrite(&ble_pwr, 1, sizeof(uint8_t), fp);
    fwrite(&max_pwr, 1, sizeof(uint8_t), fp);
    fwrite(&diff, 1, sizeof(uint8_t), fp);
    fclose(fp);
    return NULL;
}

//--------------------------------------------------------------------------
/****************************************************
 Table format:
 Country code|SRS
****************************************************/

static pkt_list_s *mtk_boots_country_get_srs(char *cmd[], size_t len, int index)
{
    FILE *fp;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    char path[64] = DEFAULT_SRS;
    size_t i = 1;
    int country_code = 0;
    int temp_code = 0;
    int srs = 0;
    uint8_t find = 0;
    char *delim = "|";
    char *pch = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            i += 2;
            break;
        case 'c':
            country_code = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
    } else {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0) {
                    temp_code = atoi(pch);
                    if (temp_code != country_code)
                        break;
                    else
                        find = 1;
                } else if (i == 1)
                    srs = atoi(pch);
                else
                    BPRINT_D("Out of table");
                pch = strtok (NULL, delim);
                i++;
            }
            if (find == 1)
                break;
        }
        free(line);
        fclose(fp);
        if (find == 1) {
            BPRINT_I("Country_code:%d SRS:%d", country_code, srs);
        } else {
            BPRINT_I("Country_code:%d Not Find.", country_code);
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_set_srs(char *cmd[], size_t len, int index)
{
    FILE *fp;
    ssize_t read;
    size_t line_len = 0;
    char *wbuf = NULL;
    char *line = NULL;
    char *line_cp = NULL;
    int fp_len = 0;
    char path[64] = DEFAULT_SRS;
    char path_cp[64] = DEFAULT_SRS;
    size_t i = 1;
    ssize_t w_size = 0;
    int country_code = 0;
    int temp_code = 0;
    int srs = 0;
    char *delim = "|";
    char *pch = NULL;
    char str_buf[12];

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            strncpy(path_cp, cmd[i + 1], sizeof(path_cp) - 1);
            i += 2;
            break;
        case 'c':
            country_code = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 's':
            srs = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r+");
    if (fp != NULL) {
        fseek(fp, 0L, SEEK_END);
        fp_len = ftell(fp);
        wbuf = malloc(fp_len + 1 + 12);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            fclose(fp);
            return NULL;
        }
        fseek(fp, 0L, SEEK_SET);

        while ((read = getline(&line, &line_len, fp)) != -1) {
            line_cp = malloc(read);
            if (!line_cp) {
                BPRINT_E("%s: Alloc line_cp failed", __func__);
                free(line);
                free(wbuf);
                fclose(fp);
                return NULL;
            }
            memcpy(line_cp, line, read);
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0) {
                    temp_code = atoi(pch);
                    if (temp_code != country_code) {
                        memcpy(&wbuf[w_size], line_cp, read);
                        w_size += read;
                    }
                }
                pch = strtok (NULL, delim);
                i++;
            }
            free(line_cp);
        }
        free(line);
        fclose(fp);
    } else {
        wbuf = malloc(13);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            return NULL;
        }
    }
    i = sprintf(str_buf, "%03d|%d\n", country_code, srs);
    memcpy(&wbuf[w_size], str_buf, i);
    w_size += i;
    fp = fopen(path_cp, "w");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path_cp, strerror(errno), errno);
        free(wbuf);
        return NULL;
    }
    fwrite(wbuf, 1, w_size, fp);
    fclose(fp);
    free(wbuf);

    BPRINT_I("Set Country_code:%d SRS:%d", country_code, srs);
    return NULL;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_country_dump_srs(char *cmd[], size_t len, int index)
{
    FILE *fp;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    char path[64] = DEFAULT_SRS;
    size_t i = 1;
    int country_code = 0;
    int srs = 0;
    char *delim = "|";
    char *pch = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        BPRINT_I("%s\n%s", country_commands[index].comment, country_commands[index].details);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path, strerror(errno), errno);
    } else {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            i = 0;
            pch = strtok(line, delim);
            while (pch != NULL) {
                if (i == 0)
                    country_code = atoi(pch);
                else if (i == 1) {
                    srs = atoi(pch);
                    BPRINT_I("Country_code:%d SRS:%d", country_code, srs);
                }
                else
                    BPRINT_D("Out of table");
                pch = strtok (NULL, delim);
                i++;
            }
        }
        free(line);
        fclose(fp);
    }
    return NULL;
}

//---------------------------------------------------------------------------
pkt_list_s *boots_country_set_handler(char *cmd[], size_t len)
{
    uint8_t i = 0;
    pkt_list_s *list = NULL;
    UNUSED(boots_btif);

    if (!cmd || !len) return NULL;

    for (i = 0; i < ARRAY_SIZE(country_commands); i++) {
        if (country_commands[i].cmd && !memcmp(cmd[0], country_commands[i].cmd, strlen(cmd[0]))) {
            list = country_commands[i].func(cmd, len, i);
            return list;
        }
    }
    BPRINT_I("%s: Command Not Found(%s)", __func__, cmd[0]);

    return NULL;
}

//---------------------------------------------------------------------------
