/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

// Converts a hexadecimal string to integer
//   0    - Conversion is successful
//   1    - String is empty
//   2    - String has more than 8 bytes
//   4    - Conversion is in process but abnormally terminated by 
//          illegal hexadecimal character


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int xtoi(const char* xs, unsigned int* result)
{
    size_t szlen = strlen(xs);
    int i, xv, fact;

    if(szlen >= 2) {
        /* filter out 0x prefix */
        if(xs[0] == '0' && (xs[1] == 'x' || xs[1] == 'X')) {
            xs += 2;
            szlen -= 2;
        }
    }

    if (szlen > 0)
    {
    
        //Converting more than 32bit hexadecimal value?
        if (szlen > 8) 
            return 2; 

        // Begin conversion here
        *result = 0;
        fact = 1;

        for(i=szlen-1; i>=0 ;i--)
        {
            if (isxdigit(*(xs+i)))
            {
                if (*(xs+i)>=97)
                {
                    xv = ( *(xs+i) - 97) + 10;
                }
                else if ( *(xs+i) >= 65)
                {
                    xv = (*(xs+i) - 65) + 10;
                }
                else
                {
                    xv = *(xs+i) - 48;
                }
                *result += (xv * fact);
                fact *= 16;
            }
            else
            {
                return 4;
            }
        }
    }
    return 1;
}


int xtoAddrptr(const char* xs, unsigned char* ptr)
{
    size_t szlen = strlen(xs);
    unsigned int i, xv, res;

    if (szlen != 12) 
	return 0;

    for (i=0 ;i<szlen; i+=2)
    {
	res = 0;
	if (isxdigit(*(xs+i)) && isxdigit(*(xs+i+1)))
	{
	    if (*(xs+i)>=97)
            {
                xv = ( *(xs+i) - 97) + 10;
            }
            else if ( *(xs+i) >= 65)
            {
                xv = (*(xs+i) - 65) + 10;
            }
            else
            {
                xv = *(xs+i) - 48;
            }
	    res += xv << 4;
			
	    if (*(xs+i+1)>=97)
            {
                xv = ( *(xs+i+1) - 97) + 10;
            }
            else if ( *(xs+i+1) >= 65)
            {
                xv = (*(xs+i+1) - 65) + 10;
            }
            else
            {
                xv = *(xs+i+1) - 48;
            }
	    res += xv;
	    *(ptr+(i>>1)) = res;			
        }
    }
    return 1;
}



