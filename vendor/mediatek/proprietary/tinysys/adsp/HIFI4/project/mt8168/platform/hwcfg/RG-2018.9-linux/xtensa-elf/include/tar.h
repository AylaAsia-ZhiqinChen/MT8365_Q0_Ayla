/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef	_TAR_H
#define	_TAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define TMAGIC 	        "ustar"
#define TMAGLEN 	6
#define TVERSION 	"00"
#define TVERSLEN 	2

#define REGTYPE 	'0'
#define AREGTYPE 	'\0'
#define LNKTYPE 	'1'
#define SYMTYPE 	'2'
#define CHRTYPE 	'3'
#define BLKTYPE 	'4'
#define DIRTYPE 	'5'
#define FIFOTYPE 	'6'
#define CONTTYPE 	'7'

#define TSUID 	        04000
#define TSGID 	        02000
#define TSVTX 	        01000
#define TUREAD 	        00400
#define TUWRITE 	00200
#define TUEXEC 	        00100
#define TGREAD 	        00040
#define TGWRITE 	00020
#define TGEXEC 	        00010
#define TOREAD 	        00004
#define TOWRITE 	00002
#define TOEXEC 	        00001

#ifdef __cplusplus
}
#endif
#endif
