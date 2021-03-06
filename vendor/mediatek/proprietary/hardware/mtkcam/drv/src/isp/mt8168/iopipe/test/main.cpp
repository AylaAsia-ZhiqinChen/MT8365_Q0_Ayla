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
 * MediaTek Inc. (C) 2010. All rights reserved.
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

///////////////////////////////////////////////////////////////////////////////
// No Warranty
// Except as may be otherwise agreed to in writing, no warranties of any
// kind, whether express or implied, are given by MTK with respect to any MTK
// Deliverables or any use thereof, and MTK Deliverables are provided on an
// "AS IS" basis.  MTK hereby expressly disclaims all such warranties,
// including any implied warranties of merchantability, non-infringement and
// fitness for a particular purpose and any warranties arising out of course
// of performance, course of dealing or usage of trade.  Parties further
// acknowledge that Company may, either presently and/or in the future,
// instruct MTK to assist it in the development and the implementation, in
// accordance with Company's designs, of certain softwares relating to
// Company's product(s) (the "Services").  Except as may be otherwise agreed
// to in writing, no warranties of any kind, whether express or implied, are
// given by MTK with respect to the Services provided, and the Services are
// provided on an "AS IS" basis.  Company further acknowledges that the
// Services may contain errors, that testing is important and Company is
// solely responsible for fully testing the Services and/or derivatives
// thereof before they are used, sublicensed or distributed.  Should there be
// any third party action brought against MTK, arising out of or relating to
// the Services, Company agree to fully indemnify and hold MTK harmless.
// If the parties mutually agree to enter into or continue a business
// relationship or other arrangement, the terms and conditions set forth
// hereunder shall remain effective and, unless explicitly stated otherwise,
// shall prevail in the event of a conflict in the terms in any agreements
// entered into between the parties.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, MediaTek Inc.
// All rights reserved.
//
// Unauthorized use, practice, perform, copy, distribution, reproduction,
// or disclosure of this information in whole or in part is prohibited.
////////////////////////////////////////////////////////////////////////////////
// AcdkCLITest.cpp  $Revision$
////////////////////////////////////////////////////////////////////////////////

//! \file  AcdkCLITest.cpp
//! \brief

#define LOG_TAG "camiopipetest"

#include <stdio.h>
#include <stdlib.h>
//
#include <errno.h>
#include <fcntl.h>
#include <gtest/gtest.h>

#include <mtkcam/drv/IHalSensor.h>
using namespace NSCam;

extern int ef_test();
extern int main_testNormalStream(int testcaseType,int testcaseNum, int testtype);
extern int test_iopipe(int argc, char** argv);
extern int test_camio(int argc, char** argv);
extern int test_camioSecure(MUINT32 _sensorID, MUINT32 _enablePort);
extern int p1p2_BasicIT(int argc, char** argv);
extern int test_capibility(int argc, char** argv);
extern int basicSecureVss(int type,int loopNum);
extern int case0_Secure(int type, int sec_fmt);
extern int case0(int type);
extern int case0_yuvIn(int type, int loopNum);

/**
 * Test that ISP secure driver
 */
 TEST(SCP_CameraDriver, Pass2_NormalTest_0) {
	ASSERT_EQ(1, case0(0)) << "[ERROR] Test Pass2_NormalTest_0 Fail!";
}

TEST(SCP_CameraDriver, Pass2_SecureTest_0) {
	ASSERT_EQ(1, case0_Secure(0, 0)) << "[ERROR] Test Pass2_SecureTest_0 Fail!";
	ASSERT_EQ(1, case0_Secure(0, 1)) << "[ERROR] Test Pass2_SecureTest_1 Fail!";
	ASSERT_EQ(1, case0_Secure(0, 2)) << "[ERROR] Test Pass2_SecureTest_2 Fail!";
}

// iopipeTest32 --gtest_filter=SCP_CameraDriver.Pass2_SecureTest_1
TEST(SCP_CameraDriver, Pass2_SecureTest_1) {

	ASSERT_EQ(1, basicSecureVss(0, 1)) << "[ERROR] Test Pass2_SecureTest_1 Fail!";
}

// iopipeTest32 --gtest_filter=SCP_CameraDriver.Pass1_SecureTest_0
TEST(SCP_CameraDriver, Pass1_SecureTest_0) {
    ASSERT_EQ(1, test_camioSecure(1, 3)) << "[ERROR] Test Pass1_SecureTest_0 Fail!";
    ASSERT_EQ(1, test_camioSecure(2, 3)) << "[ERROR] Test Pass1_SecureTest_1 Fail!";
}

static void usage()
{
    printf("Usage: iopipetest <0: not spported, 1: camio, 3:pass2, 4:IT, 5:capibility>\n");
}
/*******************************************************************************
 *  Main Function
 ********************************************************************************/
int main(int argc, char** argv)
{
#if defined(CONFIG_MTK_CAM_SECURE)	/* Enable for Secure Camera Google Testsuites */
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    printf("End of test: ret(%d)\n", ret);
    return ret;
#else
	int ret = 0;

    if( argc < 2 )
    {
        usage();
        return -1;
    }

#if 0
    printf("getchar()");
    getchar();
#endif
    printf("argc num:%d\n",argc);

    int testcase = atoi(argv[1]);
    int testnum = 0;
    int testtype=0;
    int testcaseType = atoi(argv[2]);
    int testcaseNum = atoi(argv[3]);
    int testtype2= atoi(argv[4]);


    if(argc>2)
    {
        testnum = atoi(argv[2]);
    }
    if(argc>3)
    {
        testtype = atoi(argv[3]);
    }
    int new_argc;
    char** new_argv;
    printf("start of test (%d)\n", testcase);
    switch( testcase )
    {
    case 0:
        ef_test();
        break;
    case 1:
        new_argc = argc - 1;
        new_argv = argv + 1;
        ret = test_camio(new_argc, new_argv);
        break;

    case 2:
        new_argc = argc - 1;
        new_argv = argv + 1;
        ret = test_iopipe(new_argc, new_argv);
        break;
#if 0		
    case 3:
        printf("RYAN: test NormalStream\n");
        main_testNormalStream(testcaseType,testcaseNum,testtype2);
        break;
    case 4:
        new_argc = argc - 1;
        new_argv = argv + 1;
        ret = p1p2_BasicIT(new_argc,new_argv);
        break;
#endif
    case 5:
        new_argc = argc - 1;
        new_argv = argv + 1;
        ret = test_capibility(new_argc, new_argv);
        break;
    case 6:
        if(argc > 3)
            ret = test_camioSecure(atoi(argv[2]), atoi(argv[3]));
        else
            ret = test_camioSecure(1, 2);
        break;
    default:
        printf("wrong selection (%i)\n", testcase);
        break;
    }

    printf("end of test\n");
    return ret;
#endif
}
