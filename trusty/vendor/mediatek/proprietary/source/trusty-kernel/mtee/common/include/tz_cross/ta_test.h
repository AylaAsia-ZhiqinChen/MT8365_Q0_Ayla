/*
* Copyright (c) 2015 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* An example test TA implementation.
 */

#ifndef __TRUSTZONE_TA_TEST__
#define __TRUSTZONE_TA_TEST__

#define TZ_TA_TEST_UUID   "0d5fe516-821d-11e2-bdb4-d485645c4310"

/* Data Structure for Test TA */
/* You should define data structure used both in REE/TEE here
   N/A for Test TA */

/* Command for Test TA */
#define TZCMD_TEST_ADD           0
#define TZCMD_TEST_MUL           1
#define TZCMD_TEST_ADD_MEM       2
#define TZCMD_TEST_DO_A          3
#define TZCMD_TEST_DO_B          4
#define TZCMD_TEST_SLEEP         5
#define TZCMD_TEST_DELAY         6
#define TZCMD_TEST_DO_C          7
#define TZCMD_TEST_DO_D          8
#define TZCMD_TEST_SECUREFUNC    9
#define TZCMD_TEST_CP_SBUF2NBUF 10
#define TZCMD_TEST_CP_NBUF2SBUF 11
#define TZCMD_TEST_THREAD       12
#define TZCMD_TEST_TOMCRYPT     13
#define TZCMD_TEST_SPINLOCK     14
#define TZCMD_TEST_NOP          15
#define TZCMD_TEST_RPMB         16
#define TZCMD_TEST_CRYPTO_RSA   17
#define TZCMD_TEST_RNG          18

#endif /* __TRUSTZONE_TA_TEST__ */
