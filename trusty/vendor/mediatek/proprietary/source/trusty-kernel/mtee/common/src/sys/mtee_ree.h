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

/*
 * REE Service call
 *
 * Internal used, TA should not use this.
 */

#ifndef __MTEE_REE_H__
#define __MTEE_REE_H__

/**
 * Calling REE service call
 * The parameter should be filled to REE service parameter buffer returned
 * by MTEE_GetReeParamAddress
 *
 * @param command Ree service command
 * @return Return value from Ree service command.
 */
TZ_RESULT MTEE_ReeServiceCall(uint32_t command);

/**
 * Get REE Service parameter buffer
 *
 * @return pointer to REE service parameter buffer.
 *         Size of the buffer is REE_SERVICE_BUFFER_SIZE.
 */
void *MTEE_GetReeParamAddress(void);

#endif /* __MTEE_REE_H__ */
