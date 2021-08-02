/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 2018 MICROTRUST Incorporated
 * All rights reserved
 *
 * This file and software is confidential and proprietary to MICROTRUST Inc.
 * Unauthorized copying of this file and software is strictly prohibited.
 * You MUST NOT disclose this file and software unless you get a license
 * agreement from MICROTRUST Incorporated.
 */

#ifndef DRV_I2C_GPIO_API_PRIVATE_H
#define DRV_I2C_GPIO_API_PRIVATE_H

#include <drv_fwk.h>
#include <drv_fwk_macro.h>
#include <teei_ca_ta_drv_types.h>
#include <teei_internal_types.h>
#include <teei_ta_drv_types.h>

#define DRV_I2C_GPIO_ID 0x90888
#define DRV_I2C_GPIO_NAME "i2c_gpio_util_drv"

#define MTK_TEST_I2C_TRANSFER 0x1
#define MTK_TEST_I2C_GET_SENSOR_ID_IN_TA 0x2
#define MTK_TEST_I2C_MULTI_WRITE_IN_TA 0x3
#define MTK_TEST_I2C_SECURE 0x4
#define MTK_TEST_GPIO_OUTPUT 0x5
#define MTK_TEST_GPIO_INPUT 0x6
#define MTK_TEST_GPIO_SET_DIR 0x7
#define MTK_TEST_GPIO_GET_DIR 0x8
#define MTK_TEST_GPIO_SET_OUT 0x9
#define MTK_TEST_GPIO_GET_OUT 0xA
#define MTK_TEST_GPIO_GET_IN 0xB
#define MTK_TEST_GPIO_SET_MODE 0xC
#define MTK_TEST_GPIO_GET_MODE 0xD

int drv_i2c_gpio_api_init(void);
int drv_i2c_api_transfer(unsigned long args);
int drv_i2c_api_secure(unsigned long args);
int drv_gpio_api_set_dir(unsigned long args);
int drv_gpio_api_get_dir(unsigned long args);
int drv_gpio_api_set_out(unsigned long args);
int drv_gpio_api_get_out(unsigned long args);
int drv_gpio_api_get_in(unsigned long args);
int drv_gpio_api_set_mode(unsigned long args);
int drv_gpio_api_get_mode(unsigned long args);

#endif
