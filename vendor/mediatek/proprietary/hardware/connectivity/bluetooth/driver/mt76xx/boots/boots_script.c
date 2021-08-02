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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>

#include "boots_pkt.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_script"

#define LINESIZE    8192
#define LINEVALUESIZE   4096

#define SCRIPTLINE(name) {#name, SCRIPT_##name, sizeof(#name)-1, FUN_##name}

//---------------------------------------------------------------------------
typedef struct per_cmd_info {
    char *name;
    uint8_t scriptType;
    int nameLen;
    void* (*func_cmd)(int, char*, uint8_t*, void **, int *); // reserved.now, only print log,  if needed to preprocess cmd, please register for each one
} S_per_cmd_info;

//---------------------------------------------------------------------------
static void* FUN_TITLE(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_PROC(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_TX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_RX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_WAITRX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_LOOP(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_LOOPEND(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_TIMEOUT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_WAIT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_USBALT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_CMD(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);
static void* FUN_END(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen);

//---------------------------------------------------------------------------
static S_per_cmd_info script_line_s[] = {
  SCRIPTLINE(TITLE),
  SCRIPTLINE(PROC),
  SCRIPTLINE(TX),
  SCRIPTLINE(RX),
  SCRIPTLINE(WAITRX),
  SCRIPTLINE(LOOPEND),    /* should be ahead of LOOP */
  SCRIPTLINE(LOOP),
  SCRIPTLINE(WAIT),
  SCRIPTLINE(TIMEOUT),
  SCRIPTLINE(USBALT),
  SCRIPTLINE(TIMEOUT),
  SCRIPTLINE(CMD),
  SCRIPTLINE(END)
};

static int script_line_num = sizeof(script_line_s)/sizeof(S_per_cmd_info);

static uint8_t packet_value_array[LINEVALUESIZE];
static int paramValue = 0;

static char *line = NULL;

//---------------------------------------------------------------------------
#if 0
static char hextoascii(int i)
{
    i &= 0x0F;

    if (i >= 0 && i <= 9)
        return i + '0';
    else
        return i + ('A' - 0x0A);
}

static int print_hexes(char *hex, int len)
{
    int i;
    printf("\tFormat in HEX   -- ");
    for (i = 0; i < len; i++, hex++) {
        putchar(hextoascii(*hex >> 4));
        putchar(hextoascii(*hex & 0x0f));
        putchar(' ');
    }
    putchar('\r');
    putchar('\n');
    return 0;
}

static char* xitoa(int val, int base)
{
    static char sbuf[32] = {0};
    int i = 30;
    for(; val && i; --i, val /= base)
    {
        sbuf[i]="0123456789abcdef"[val % base];
    }
    return &sbuf[i+1];
}
#endif

static uint8_t xatoi(char *p, int len)
{
    int n= 0;
    uint8_t v = 0;

    for(n=0; n<len; n++){
        if((*p>='0')&&(*p<='9'))  v = v*16 + (*p-'0');
        else if((*p>='A')&&(*p<='F'))  v = v*16 + (10+*p-'A');
        else if((*p>='a')&&(*p<='f'))  v = v*16 + (10+*p-'a');
        else return(-1);
        p++;
    }
    return v;
}

static void filter_space_c(char** ppStr, int head_filter, int tail_filter)
{
    char* str;
    int start, end, strLen;

    if (ppStr != NULL && *ppStr != NULL){
        str = *ppStr;
        //1. head fliter
        if ((head_filter == 1) && (*str != '\0')){
            for (start=0; *str==' '; start++){
               str++;
            }
        }
        //2. tail filter
        if ((tail_filter == 1) && (*str != '\0')){
            strLen = strlen(str);
            for (end = strLen - 1; end >= 0 && str[end]==' '; end--){
                str[end] = '\0';
            }
        }
        *ppStr = str;
    }
}

static int packet_safe_atoi(char valueStr[], uint8_t value[], int size)
{
    char *token = NULL, *savepStr=NULL;
    uint8_t *valueIs = value;
    int i = 0, repeat = 0, split_num = 0, len = 0;
    int num = 0;
    int ret = 0;

    if (size > LINEVALUESIZE) {
        BPRINT_W("Incorrect size %d", size);
        size = LINEVALUESIZE;
    }

    memset(valueIs, 0, size);

    token=strtok_r(valueStr, " ", &savepStr);
    while(token != NULL){
        len = strlen(token);
        repeat = len/2 + len%2;
        do{
            split_num = ((len >= 2)?(2):(1));
            ret = xatoi((token + i * 2), split_num);
            if (ret < 0) {
                BPRINT_E("Illegal string(%s), abort <\n", token + i*2);
                num = 0;
                break;
            } else {
                valueIs[num] = ret;
            }
            ++num;
            ++i;
            len -= i*2;
        }while((--repeat) > 0);

        if(num > 0){
            token=strtok_r(NULL, " ", &savepStr);
            i = 0;
        }
        else{
            break;
        }
    }
/*
    if(num != 0){
        int i = 0, tmpN = num;
        printf("packet[num==%d]>", num);
        while(tmpN){
            printf("%02x", valueIs[i]);
            printf(" ");
            --tmpN;
            ++i;
        }
        printf("\n");
    }
*/
    return num;
}

static void* FUN_TITLE(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = ((Invalue == NULL)?0:(strlen(Invalue) + 1));
    *oValue = (void*)((*oLen == 0)?NULL:Invalue);

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
        script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}


static void* FUN_PROC(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = ((Invalue == NULL)?0:(strlen(Invalue) + 1));
    *oValue = (void*)((*oLen == 0)?NULL:Invalue);

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}

static void* FUN_TX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = ((Invalue != NULL)?(xatoi(Invalue, 2)):(0));
    *oLen = 0;
    *oValue = NULL;

    if((Invalue != NULL)){
        *oLen = packet_safe_atoi(Invalue, packet_value_array, sizeof(packet_value_array));
        if (*oLen > 0){
            *oValue = packet_value_array;
        }
        else{
            *oLen = 0;
            *oValue = NULL;
        }
    }

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}

static void* FUN_RX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = ((Invalue != NULL)?(xatoi(Invalue, 2)):(0));
    *oLen = 0;
    *oValue = NULL;

    if((Invalue != NULL)){
        *oLen = packet_safe_atoi(Invalue, packet_value_array, sizeof(packet_value_array));
        if (*oLen > 0){
            *oValue = packet_value_array;
        }
        else{
            *oLen = 0;
            *oValue = NULL;
        }
    }

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}

static void* FUN_WAITRX(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = ((Invalue != NULL)?(xatoi(Invalue, 2)):(0));
    *oLen = 0;
    *oValue = NULL;

    if((Invalue != NULL)){
        *oLen = packet_safe_atoi(Invalue, packet_value_array, sizeof(packet_value_array));
        if (*oLen > 0){
            *oValue = packet_value_array;
        }
        else{
            *oLen = 0;
            *oValue = NULL;
        }
    }

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}


static void* FUN_LOOP(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = 1;
    paramValue = ((*oLen == 0)?(0):(atoi(Invalue)));
    *oValue = &paramValue;

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<LOOP Count>%d",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType, *((int*)(*oValue)));

    return (void*)NULL;
}


static void* FUN_LOOPEND(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = 0;
    *oValue = NULL;
    UNUSED(Invalue);

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType);

    return (void*)NULL;
}


static void* FUN_TIMEOUT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = 1;
    paramValue = ((*oLen == 0)?(0):(atoi(Invalue)));
    *oValue = &paramValue;

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<TIMEOUT>%d",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType, *((int*)(*oValue)));

    return (void*)NULL;
}


static void* FUN_WAIT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = ((Invalue == NULL)?0:(strlen(Invalue) + 1));
    paramValue = ((*oLen == 0)?(0):(atoi(Invalue)));
    *oValue = &paramValue;

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<WAIT>%d",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType, *((int*)(*oValue)));

    return (void*)NULL;
}


static void* FUN_USBALT(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = 1;
    paramValue = ((*oLen == 0)?(0):(atoi(Invalue)));
    *oValue = &paramValue;

    BPRINT_D("lineName>%s<Type>0x%02x-0x%02x<USBALT>%d",
            script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType, *opktType, *((int*)(*oValue)));

    return (void*)NULL;
}


static void* FUN_CMD(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0x01;
    *oLen = 0;
    *oValue = NULL;

    if((Invalue != NULL)){
        *oLen = packet_safe_atoi(Invalue, &(packet_value_array[1]), sizeof(packet_value_array) - 1);
        if (*oLen > 0){
            packet_value_array[0] = 0x01;
            *oLen += 1;
            *oValue = packet_value_array;
        }
        else{
            *oLen = 0;
            *oValue = NULL;
        }
    }

    BPRINT_D("lineName>%s<packetType>0x%02x<", script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType);
    return (void*)NULL;
}

static void* FUN_END(int InfoundIndex, char* Invalue, uint8_t* opktType, void** oValue, int* oLen)
{
    *opktType = 0;
    *oLen = 0;
    *oValue = NULL;
    UNUSED(Invalue);

    BPRINT_D("lineName>%s<packetType>0x%02x<", script_line_s[InfoundIndex].name, script_line_s[InfoundIndex].scriptType);
    return (void*)NULL;
}


static pkt_list_s *script_line_handle(char *lineName, char *lineValue)
{
    int i;
    uint8_t pktType = 0;
    void *value = NULL;
    int valueLen;
    UNUSED(boots_btif);

    for(i = 0 ;i < script_line_num; i++){
        if(!strncmp(lineName, script_line_s[i].name, script_line_s[i].nameLen)){
            script_line_s[i].func_cmd(i, lineValue, &pktType, &value, &valueLen);
#if 0
            int num;
            switch (script_line_s[i].scriptType) {
                case SCRIPT_TX:
                case SCRIPT_RX:
                case SCRIPT_WAITRX:
                case SCRIPT_CMD:
                    num = 0;
                    printf("[boots_script:D] TYPE = %02x < ", script_line_s[i].scriptType);
                    while(num < valueLen){
                        printf("%02x ", *((uint8_t*)value + num));
                        ++num;
                    }
                    printf("\n");
                    break;
                case SCRIPT_TITLE:
                case SCRIPT_PROC:
                    BPRINT_D("TYPE = %02x < %s", script_line_s[i].scriptType, (char*)value);
                    break;
                case SCRIPT_LOOP:
                    BPRINT_D("TYPE = %02x < %d", script_line_s[i].scriptType, *((int*)value));
                    break;
                case SCRIPT_TIMEOUT:
                    BPRINT_D("TYPE = %02x < %d", script_line_s[i].scriptType, *((int*)value));
                    break;
                case SCRIPT_WAIT:
                    BPRINT_D("TYPE = %02x < %d", script_line_s[i].scriptType, *((int*)value));
                    break;
                case SCRIPT_USBALT:
                    BPRINT_D("TYPE = %02x < %d", script_line_s[i].scriptType, *((int*)value));
                    break;
                default:
                    break;
            }
#endif
            return boots_pkt_node_push(script_line_s[i].scriptType, pktType, value, valueLen, NULL, NULL);
        }

    }
    return NULL;
}

static pkt_list_s *script_line_parse(char *line)
{
    char *tmpStr = line;
    char *delimStr = ":=";
    char *subdelimStr = "//";
    char *lineName, *lineValue;//, *lineAnno;
    char *saveptr;
    int slen;

    // 1. filter out space until found out a char
    filter_space_c(&tmpStr, 1, 0);

    // 2. filter out annotation line
    if ((*tmpStr == '#') || (strncmp(tmpStr, "//", 2) == 0)) {
        // do nothing, just ignore

    } else {
        slen = strlen(tmpStr);
        // 3. filter out new line('\n' 0A) & carriage ret ('\r' 0D)
        if (tmpStr[slen - 1] == 0x0A) {
            tmpStr[slen - 1] = '\0';
            if (tmpStr[slen - 2] == 0x0D) {
                tmpStr[slen - 2] = '\0';
            }
        }
        slen = strlen(tmpStr);

        if (slen == 0) {
            // do nothing--only ignore
        } else {
            //BPRINT_D("line <%s>", tmpStr);
            // 4. parse
            lineName = strtok_r(tmpStr, delimStr, &saveptr);
            if (lineName != NULL) {
                lineValue = strtok_r(saveptr, subdelimStr, &lineValue);
            } else {
                return NULL;
            }
            //BPRINT_D("orgName <%s>", lineName);
            //BPRINT_D("orgValue <%s>", lineValue);
            //BPRINT_D("filter out space ...");

            // 5. filter out space
            filter_space_c(&lineName, 1, 1);
            filter_space_c(&lineValue, 1, 1);

            //BPRINT_D("newName <%s>", lineName);
            //BPRINT_D("newValue <%s>", lineValue);

            // 6. map Type and create node
            return script_line_handle(lineName, lineValue);
        }
    }
    return NULL;
}

//---------------------------------------------------------------------------
FILE *boots_script_open(char *file)
{
    BPRINT_D("%s: %s", __func__, file);
    return fopen(file, "r");
}

//---------------------------------------------------------------------------
void boots_script_close(FILE *fd)
{
    if (line) {
        free(line);
        line = NULL;
    }
    if (fd) fclose(fd);
}

//---------------------------------------------------------------------------
pkt_list_s *boots_script_get(FILE *fd)
{
    pkt_list_s *n = NULL;
    char *buf = NULL;
    size_t buf_size = 0;
    ssize_t nread;

    if (!fd) return NULL;

    do {
        if ((nread = getline(&buf, &buf_size, fd)) < 0)
            break;
        n = script_line_parse(buf);

    } while (!n || nread == 0);

    free(buf);

    return n;
}

//---------------------------------------------------------------------------
void boots_script_loop(FILE * fd, long size)
{
    (void)fseek(fd, size, SEEK_SET);
}

//---------------------------------------------------------------------------
