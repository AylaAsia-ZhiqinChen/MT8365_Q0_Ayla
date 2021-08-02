/*
 * Copyright (C) 2018 MICROTRUST Incorporated
 * All rights reserved
 *
 * This file and software is confidential and proprietary to MICROTRUST Inc.
 * Unauthorized copying of this file and software is strictly prohibited.
 * You MUST NOT disclose this file and software unless you get a license
 * agreement from MICROTRUST Incorporated.
 */

/**
 * Example for programming a secure driver.
 * 1. Secure driver should implement 5 interface
 *    init
 *    open_session
 *    invoke_command
 *    ree_command
 *    close_session
 * 2. Declare the driver module, the bta library needs them.
 */
#include <drv_i2c_gpio_api_internal.h>

#include "leak_detect.h"

/**
 * [Require] declare the driver module
 */
static int init(const struct msee_drv_device* dev);
static int invoke_command(unsigned int sid, unsigned int cmd, unsigned long args);
static int ree_command(unsigned int cmd, unsigned long args);
static int open_session(unsigned int sid, unsigned long args);
static int close_session(unsigned int sid);
DECLARE_DRIVER_MODULE(DRV_I2C_GPIO_NAME, DRV_I2C_GPIO_ID, init, invoke_command, ree_command,
                      open_session, close_session);

/**
 * [Require] init the secure driver.
 * \return Result
 */
static int init(const struct msee_drv_device* dev)
{
    (void)dev;
    return drv_i2c_gpio_api_init();
}

/**
 * [Require] Create the session to secure driver.
 * \return Result
 */
int open_session(unsigned int sid, unsigned long args)
{
    (void)sid;
    (void)args;

    leak_detection_start();
    return DRV_SUCCESS;
}

/**
 * [Require] Close the session to secure driver.
 * \return Result
 */
int close_session(unsigned int sid)
{
    (void)sid;

    leak_detection_check();
    return DRV_SUCCESS;
}

/**
 * [Require] Invoke the command to secure driver.
 * \return Result
 */
int invoke_command(unsigned int sid, unsigned int cmd, unsigned long args)
{
    (void)sid;
    (void)args;

    switch (cmd) {
    case MTK_TEST_I2C_TRANSFER:
        return drv_i2c_api_transfer(args);
    case MTK_TEST_I2C_SECURE:
        return drv_i2c_api_secure(args);
    case MTK_TEST_GPIO_SET_DIR:
        return drv_gpio_api_set_dir(args);
    case MTK_TEST_GPIO_GET_DIR:
        return drv_gpio_api_get_dir(args);
    case MTK_TEST_GPIO_SET_OUT:
        return drv_gpio_api_set_out(args);
    case MTK_TEST_GPIO_GET_OUT:
        return drv_gpio_api_get_out(args);
    case MTK_TEST_GPIO_GET_IN:
        return drv_gpio_api_get_in(args);
    case MTK_TEST_GPIO_SET_MODE:
        return drv_gpio_api_set_mode(args);
    case MTK_TEST_GPIO_GET_MODE:
        return drv_gpio_api_get_mode(args);
    default:
        return DRV_ERROR_UNKNOWN_COMMAND;
    }
}

/**
 * [Require] Invoke the command to ree driver.
 * \return Result
 */
int ree_command(unsigned int cmd, unsigned long args)
{
    (void)cmd;
    (void)args;

    return DRV_SUCCESS;
}
