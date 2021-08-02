/*
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 02/02/2007
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <lk/init.h>
#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* debug flag */
#define DEV_KDF_DGB_EN 1

/* debug dump */
#if DEV_KDF_DGB_EN
static void vHexDumpBuf
(
unsigned char* puc_buf,
unsigned int  i_buf_sz
)
{
    unsigned int i;

    for (i = 0; i < i_buf_sz ; i++) {
    	 printf("%02x",  puc_buf[i]);
    }
    printf("\n");
}
#endif

/* SHA256 implementation */
#if 0
#define UNROLL_LOOPS /* Enable loops unrolling */
#endif

#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA256_BLOCK_SIZE  ( 512 / 8)

typedef unsigned char      uint8;
typedef unsigned int       uint32;
typedef unsigned long long uint64;
typedef int                int32;

typedef struct {
    uint32 tot_len;
    uint32 len;
    uint8  block[2 * SHA256_BLOCK_SIZE];
    uint32 h[8];
} sha256_ctx;
          
#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))


#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8) ((x)      );       \
    *((str) + 2) = (uint8) ((x) >>  8);       \
    *((str) + 1) = (uint8) ((x) >> 16);       \
    *((str) + 0) = (uint8) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32) *((str) + 3)      )    \
           | ((uint32) *((str) + 2) <<  8)    \
           | ((uint32) *((str) + 1) << 16)    \
           | ((uint32) *((str) + 0) << 24);   \
}

#define UNPACK64(x, str)                      \
{                                             \
    *((str) + 7) = (uint8) ((x)      );       \
    *((str) + 6) = (uint8) ((x) >>  8);       \
    *((str) + 5) = (uint8) ((x) >> 16);       \
    *((str) + 4) = (uint8) ((x) >> 24);       \
    *((str) + 3) = (uint8) ((x) >> 32);       \
    *((str) + 2) = (uint8) ((x) >> 40);       \
    *((str) + 1) = (uint8) ((x) >> 48);       \
    *((str) + 0) = (uint8) ((x) >> 56);       \
}

#define PACK64(str, x)                        \
{                                             \
    *(x) =   ((uint64) *((str) + 7)      )    \
           | ((uint64) *((str) + 6) <<  8)    \
           | ((uint64) *((str) + 5) << 16)    \
           | ((uint64) *((str) + 4) << 24)    \
           | ((uint64) *((str) + 3) << 32)    \
           | ((uint64) *((str) + 2) << 40)    \
           | ((uint64) *((str) + 1) << 48)    \
           | ((uint64) *((str) + 0) << 56);   \
}

/* Macros used for loops unrolling */

#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
          + SHA256_F3(w[i - 15]) + w[i - 16]; \
}

#define SHA256_EXP(a, b, c, d, e, f, g, h, j)               \
{                                                           \
    t1 = wv[h] + SHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
         + sha256_k[j] + w[j];                              \
    t2 = SHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[h] = t1 + t2;                                        \
}

uint32 sha256_h0[8] =
            {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

uint32 sha256_k[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/* SHA-256 functions */
static void sha256_transf
(
sha256_ctx  *ctx,
uint8       *message,
uint32      block_nb
)
{
    uint32 w[64];
    uint32 wv[8];
    uint32 t1, t2;
    uint8 *sub_block;
    uint32 i;

#ifndef UNROLL_LOOPS
    uint32 j;
#endif

    for (i = 0; i < block_nb; i++) {
        sub_block = message + (i << 6);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
#else
        PACK32(&sub_block[ 0], &w[ 0]); PACK32(&sub_block[ 4], &w[ 1]);
        PACK32(&sub_block[ 8], &w[ 2]); PACK32(&sub_block[12], &w[ 3]);
        PACK32(&sub_block[16], &w[ 4]); PACK32(&sub_block[20], &w[ 5]);
        PACK32(&sub_block[24], &w[ 6]); PACK32(&sub_block[28], &w[ 7]);
        PACK32(&sub_block[32], &w[ 8]); PACK32(&sub_block[36], &w[ 9]);
        PACK32(&sub_block[40], &w[10]); PACK32(&sub_block[44], &w[11]);
        PACK32(&sub_block[48], &w[12]); PACK32(&sub_block[52], &w[13]);
        PACK32(&sub_block[56], &w[14]); PACK32(&sub_block[60], &w[15]);

        SHA256_SCR(16); SHA256_SCR(17); SHA256_SCR(18); SHA256_SCR(19);
        SHA256_SCR(20); SHA256_SCR(21); SHA256_SCR(22); SHA256_SCR(23);
        SHA256_SCR(24); SHA256_SCR(25); SHA256_SCR(26); SHA256_SCR(27);
        SHA256_SCR(28); SHA256_SCR(29); SHA256_SCR(30); SHA256_SCR(31);
        SHA256_SCR(32); SHA256_SCR(33); SHA256_SCR(34); SHA256_SCR(35);
        SHA256_SCR(36); SHA256_SCR(37); SHA256_SCR(38); SHA256_SCR(39);
        SHA256_SCR(40); SHA256_SCR(41); SHA256_SCR(42); SHA256_SCR(43);
        SHA256_SCR(44); SHA256_SCR(45); SHA256_SCR(46); SHA256_SCR(47);
        SHA256_SCR(48); SHA256_SCR(49); SHA256_SCR(50); SHA256_SCR(51);
        SHA256_SCR(52); SHA256_SCR(53); SHA256_SCR(54); SHA256_SCR(55);
        SHA256_SCR(56); SHA256_SCR(57); SHA256_SCR(58); SHA256_SCR(59);
        SHA256_SCR(60); SHA256_SCR(61); SHA256_SCR(62); SHA256_SCR(63);

        wv[0] = ctx->h[0]; wv[1] = ctx->h[1];
        wv[2] = ctx->h[2]; wv[3] = ctx->h[3];
        wv[4] = ctx->h[4]; wv[5] = ctx->h[5];
        wv[6] = ctx->h[6]; wv[7] = ctx->h[7];

        SHA256_EXP(0,1,2,3,4,5,6,7, 0); SHA256_EXP(7,0,1,2,3,4,5,6, 1);
        SHA256_EXP(6,7,0,1,2,3,4,5, 2); SHA256_EXP(5,6,7,0,1,2,3,4, 3);
        SHA256_EXP(4,5,6,7,0,1,2,3, 4); SHA256_EXP(3,4,5,6,7,0,1,2, 5);
        SHA256_EXP(2,3,4,5,6,7,0,1, 6); SHA256_EXP(1,2,3,4,5,6,7,0, 7);
        SHA256_EXP(0,1,2,3,4,5,6,7, 8); SHA256_EXP(7,0,1,2,3,4,5,6, 9);
        SHA256_EXP(6,7,0,1,2,3,4,5,10); SHA256_EXP(5,6,7,0,1,2,3,4,11);
        SHA256_EXP(4,5,6,7,0,1,2,3,12); SHA256_EXP(3,4,5,6,7,0,1,2,13);
        SHA256_EXP(2,3,4,5,6,7,0,1,14); SHA256_EXP(1,2,3,4,5,6,7,0,15);
        SHA256_EXP(0,1,2,3,4,5,6,7,16); SHA256_EXP(7,0,1,2,3,4,5,6,17);
        SHA256_EXP(6,7,0,1,2,3,4,5,18); SHA256_EXP(5,6,7,0,1,2,3,4,19);
        SHA256_EXP(4,5,6,7,0,1,2,3,20); SHA256_EXP(3,4,5,6,7,0,1,2,21);
        SHA256_EXP(2,3,4,5,6,7,0,1,22); SHA256_EXP(1,2,3,4,5,6,7,0,23);
        SHA256_EXP(0,1,2,3,4,5,6,7,24); SHA256_EXP(7,0,1,2,3,4,5,6,25);
        SHA256_EXP(6,7,0,1,2,3,4,5,26); SHA256_EXP(5,6,7,0,1,2,3,4,27);
        SHA256_EXP(4,5,6,7,0,1,2,3,28); SHA256_EXP(3,4,5,6,7,0,1,2,29);
        SHA256_EXP(2,3,4,5,6,7,0,1,30); SHA256_EXP(1,2,3,4,5,6,7,0,31);
        SHA256_EXP(0,1,2,3,4,5,6,7,32); SHA256_EXP(7,0,1,2,3,4,5,6,33);
        SHA256_EXP(6,7,0,1,2,3,4,5,34); SHA256_EXP(5,6,7,0,1,2,3,4,35);
        SHA256_EXP(4,5,6,7,0,1,2,3,36); SHA256_EXP(3,4,5,6,7,0,1,2,37);
        SHA256_EXP(2,3,4,5,6,7,0,1,38); SHA256_EXP(1,2,3,4,5,6,7,0,39);
        SHA256_EXP(0,1,2,3,4,5,6,7,40); SHA256_EXP(7,0,1,2,3,4,5,6,41);
        SHA256_EXP(6,7,0,1,2,3,4,5,42); SHA256_EXP(5,6,7,0,1,2,3,4,43);
        SHA256_EXP(4,5,6,7,0,1,2,3,44); SHA256_EXP(3,4,5,6,7,0,1,2,45);
        SHA256_EXP(2,3,4,5,6,7,0,1,46); SHA256_EXP(1,2,3,4,5,6,7,0,47);
        SHA256_EXP(0,1,2,3,4,5,6,7,48); SHA256_EXP(7,0,1,2,3,4,5,6,49);
        SHA256_EXP(6,7,0,1,2,3,4,5,50); SHA256_EXP(5,6,7,0,1,2,3,4,51);
        SHA256_EXP(4,5,6,7,0,1,2,3,52); SHA256_EXP(3,4,5,6,7,0,1,2,53);
        SHA256_EXP(2,3,4,5,6,7,0,1,54); SHA256_EXP(1,2,3,4,5,6,7,0,55);
        SHA256_EXP(0,1,2,3,4,5,6,7,56); SHA256_EXP(7,0,1,2,3,4,5,6,57);
        SHA256_EXP(6,7,0,1,2,3,4,5,58); SHA256_EXP(5,6,7,0,1,2,3,4,59);
        SHA256_EXP(4,5,6,7,0,1,2,3,60); SHA256_EXP(3,4,5,6,7,0,1,2,61);
        SHA256_EXP(2,3,4,5,6,7,0,1,62); SHA256_EXP(1,2,3,4,5,6,7,0,63);

        ctx->h[0] += wv[0]; ctx->h[1] += wv[1];
        ctx->h[2] += wv[2]; ctx->h[3] += wv[3];
        ctx->h[4] += wv[4]; ctx->h[5] += wv[5];
        ctx->h[6] += wv[6]; ctx->h[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}

static void sha256_init(sha256_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    uint32 i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha256_h0[i];
    }
#else
    ctx->h[0] = sha256_h0[0]; ctx->h[1] = sha256_h0[1];
    ctx->h[2] = sha256_h0[2]; ctx->h[3] = sha256_h0[3];
    ctx->h[4] = sha256_h0[4]; ctx->h[5] = sha256_h0[5];
    ctx->h[6] = sha256_h0[6]; ctx->h[7] = sha256_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

static void sha256_update
(
sha256_ctx  *ctx,
uint8       *message,
uint32      len
)
{
    uint32 block_nb;
    uint32 new_len, rem_len, tmp_len;
    uint8  *shifted_message;

    tmp_len = SHA256_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA256_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(ctx, ctx->block, 1);
    sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

static void sha256_final
(
sha256_ctx *ctx,
uint8      *digest
)
{
    uint32 block_nb;
    uint32 pm_len;
    uint32 len_b;

#ifndef UNROLL_LOOPS
    uint32 i;
#endif

    block_nb = (1 + ((SHA256_BLOCK_SIZE - 9)
                     < (ctx->len % SHA256_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha256_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
#else
   UNPACK32(ctx->h[0], &digest[ 0]);
   UNPACK32(ctx->h[1], &digest[ 4]);
   UNPACK32(ctx->h[2], &digest[ 8]);
   UNPACK32(ctx->h[3], &digest[12]);
   UNPACK32(ctx->h[4], &digest[16]);
   UNPACK32(ctx->h[5], &digest[20]);
   UNPACK32(ctx->h[6], &digest[24]);
   UNPACK32(ctx->h[7], &digest[28]);
#endif /* !UNROLL_LOOPS */
}

static void sha256
(
uint8  *message,
uint32      len,
uint8       *digest
)
{
    sha256_ctx* pctx = NULL;

    pctx = malloc(sizeof(sha256_ctx));
    if(NULL == pctx)
    {
        printf("Error: fail to allocate memory for sha256\n");	
        return;
    }

    sha256_init(pctx);
    sha256_update(pctx, message, len);
    sha256_final(pctx, digest);
    memset(pctx, 0, sizeof(sha256_ctx));
    free(pctx);
}

/*-
 * HMAC-SHA-224/256/384/512 implementation
 * Last update: 06/15/2005
 * Issue date:  06/15/2005
 *
 * Copyright (C) 2005 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* HMAC-SHA-256 functions */
typedef struct {
    sha256_ctx ctx_inside;
    sha256_ctx ctx_outside;

    /* for hmac_reinit */
    sha256_ctx ctx_inside_reinit;
    sha256_ctx ctx_outside_reinit;

    uint8 block_ipad[SHA256_BLOCK_SIZE];
    uint8 block_opad[SHA256_BLOCK_SIZE];
} hmac_sha256_ctx;

void hmac_sha256_init
(
hmac_sha256_ctx *ctx,
uint8           *key,
uint32 key_size
)
{
    uint32 fill=0;
    uint32 num;

    uint8 *key_used;
    uint8 key_temp[SHA256_DIGEST_SIZE];
    uint32 i;

    if (key_size == SHA256_BLOCK_SIZE) {
        key_used = key;
        num = SHA256_BLOCK_SIZE;
    } else {
        if (key_size > SHA256_BLOCK_SIZE)
        { /* key_size > SHA256_BLOCK_SIZE */
            key_used = key_temp;
            num = SHA256_DIGEST_SIZE;
            sha256(key, key_size, key_used);
        }
        else
        {
            key_used = key;
            num = key_size;
        }
        fill = SHA256_BLOCK_SIZE - num;

        /* because 64bits mtee libc implementation has a problem, not use code below for MTEE case */
        /* 
        memset(&(ctx->block_ipad[num]), 0x36, fill);
        memset(&(ctx->block_opad[num]), 0x5c, fill);
        */
        for(i=num;i<(num+fill);i++)
        {
            ctx->block_ipad[i] = 0x36;
            ctx->block_opad[i] = 0x5c;
        }
    }

    for (i = 0; i < num; i++) {
        ctx->block_ipad[i] = key_used[i] ^ 0x36;
        ctx->block_opad[i] = key_used[i] ^ 0x5c;
    }

    sha256_init(&ctx->ctx_inside);
    sha256_update(&ctx->ctx_inside, ctx->block_ipad, SHA256_BLOCK_SIZE);

    sha256_init(&ctx->ctx_outside);
    sha256_update(&ctx->ctx_outside, ctx->block_opad,
                  SHA256_BLOCK_SIZE);

    /* for hmac_reinit */
    memcpy(&ctx->ctx_inside_reinit, &ctx->ctx_inside,
           sizeof(sha256_ctx));
    memcpy(&ctx->ctx_outside_reinit, &ctx->ctx_outside,
           sizeof(sha256_ctx));
}

void hmac_sha256_reinit
(
hmac_sha256_ctx *ctx
)
{
    memcpy(&ctx->ctx_inside, &ctx->ctx_inside_reinit,
           sizeof(sha256_ctx));
    memcpy(&ctx->ctx_outside, &ctx->ctx_outside_reinit,
           sizeof(sha256_ctx));
}

static void hmac_sha256_update
(
hmac_sha256_ctx *ctx,
uint8           *message,
uint32          message_len
)
{
    sha256_update(&ctx->ctx_inside, message, message_len);
}

static void hmac_sha256_final
(
hmac_sha256_ctx *ctx,
uint8           *mac,
uint32          mac_size
)
{
    uint8 digest_inside[SHA256_DIGEST_SIZE];
    uint8 mac_temp[SHA256_DIGEST_SIZE];

    sha256_final(&ctx->ctx_inside, digest_inside);
    sha256_update(&ctx->ctx_outside, digest_inside, SHA256_DIGEST_SIZE);
    sha256_final(&ctx->ctx_outside, mac_temp);
    memcpy(mac, mac_temp, mac_size);
}

static void hmac_sha256
(
uint8  *key,
uint32 key_size,
uint8  *message,
uint32 message_len,
uint8  *mac,
uint32 mac_size
)
{
    hmac_sha256_ctx* pctx = NULL;

    pctx = malloc(sizeof(hmac_sha256_ctx));
    if(NULL == pctx)
    {
        printf("Error: fail to allocate memory for hmac_sha256\n");	
        return;
    }

    hmac_sha256_init(pctx, key, key_size);
    hmac_sha256_update(pctx, message, message_len);
    hmac_sha256_final(pctx, mac, mac_size);
    memset(pctx, 0, sizeof(hmac_sha256_ctx));
    free(pctx);
}

/*
   Implements the HKDF algorithm (HMAC-based Extract-and-Expand Key
   Derivation Function, RFC 5869).

   This implementation is adapted from IETF's HKDF implementation,
   see associated license below.
*/
/*
   Copyright (c) 2011 IETF Trust and the persons identified as
   authors of the code.  All rights reserved.
   Redistribution and use in source and binary forms, with or
   without modification, are permitted provided that the following
   conditions are met:
   - Redistributions of source code must retain the above
     copyright notice, this list of conditions and
     the following disclaimer.
   - Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.
   - Neither the name of Internet Society, IETF or IETF Trust, nor
     the names of specific contributors, may be used to endorse or
     promote products derived from this software without specific
     prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 *  hkdf_sha256_extract
 *
 *  Description:
 *      This function will perform HKDF extraction with SHA256.
 *
 *  Parameters:
 *      salt: [in]
 *          The optional salt value (a non-secret random value);
 *          if not provided (salt == NULL), it is set internally
 *          to a string of HashLen(hmac_algorithm) zeros.
 *      salt_len: [in]
 *          The length of the salt value.  (Ignored if salt == NULL.)
 *      ikm: [in]
 *          Input keying material.
 *      ikm_len: [in]
 *          The length of the input keying material.
 *      prk: [out]
 *          Array where the HKDF extraction is to be stored.
 *          Must be euqual to or larger than 32 bytes
 *
 *  Returns:
 *      0 on success, -1 on error.
 *
 */
static int hkdf_sha256_extract(
uint8  *salt,
uint32      salt_len,
uint8  *ikm,
uint32      ikm_len,
uint8       *prk
)
{
  uint8 null_salt[SHA256_DIGEST_SIZE];

  if (ikm == NULL)
  {
  	  printf("Error: incorrect input parameter for hkdf_sha256_extract\n");	
    return -1;
  }

  if (salt == NULL) {
    salt = null_salt;
    salt_len = SHA256_DIGEST_SIZE;
    memset(null_salt, '\0', salt_len);
  }

  hmac_sha256(salt, salt_len,
              ikm,  ikm_len,
              prk,  SHA256_DIGEST_SIZE);
  return 0;
}

/*
 *  hkdf_sha256_expand
 *
 *  Description:
 *      This function will perform HKDF with SHA256 expansion.
 *
 *  Parameters:
 *      prk: [in]
 *          The pseudo-random key to be expanded; either obtained
 *          directly from a cryptographically strong, uniformly
 *          distributed pseudo-random number generator, or as the
 *          output from hkdfExtract().
 *      prk_len: [in]
 *          The length of the pseudo-random key in prk;
 *          should at least be equal to HashSize(hmac_algorithm).
 *      info: [in]
 *          The optional context and application specific information.
 *          If info == NULL or a zero-length string, it is ignored.
 *      info_len: [in]
 *          The length of the optional context and application specific
 *          information. (Ignored if info == NULL.)
 *      okm: [out]
 *          Where the HKDF is to be stored.
 *      okm_len: [in]
 *          The length of the buffer to hold okm.
 *          okm_len must be <= 255 * SHA256_DIGEST_SIZE
 *
 *  Returns:
 *      0 on success, -1 on error.
 *
 */
static int hkdf_sha256_expand
(
uint8   prk[],
uint32      prk_len,
uint8   *info,
uint32      info_len,
uint8       okm[],
uint32      okm_len
)
{
  uint32 hash_len;
  uint32 N;
  uint8 T[SHA256_DIGEST_SIZE];
  uint32 Tlen;
  uint32 pos;
  uint32 i;
  hmac_sha256_ctx* pctx;
  uint8 c;

  if( ( prk_len == 0 ) || ( okm_len == 0 ) || ( okm == NULL ) )
  {
  	  printf("Error: incorrect input parameter for hkdf_sha256\n");	
    return -1;
  }

  pctx = malloc(sizeof(hmac_sha256_ctx));
  if(NULL == pctx)
  {
  	  printf("Error: fail to allocate memory for hmac_sha256\n");	
      return -2;
  }

  if (info == NULL) {
    info = (uint8 *)"";
    info_len = 0;
  }

  hash_len = SHA256_DIGEST_SIZE;
  if (prk_len < hash_len)
  {
  	  printf("Error: prk size (%d) is smaller than hash size (%d)\n", prk_len, hash_len);
  	  free(pctx);
      return -3;
  }
  N = okm_len / hash_len;
  if (okm_len % hash_len)
    N++;
  if (N > 255)
  {
  	  printf("Error: incorrect input size (%d) for hkdf_sha256\n", N);
  	  free(pctx);
      return -4;
  }

  Tlen = 0;
  pos = 0;
  for (i = 1; i <= N; i++) {
    c = i;
    memset( pctx, 0, sizeof(hmac_sha256_ctx) ); 

    hmac_sha256_init(pctx, prk, prk_len);
    hmac_sha256_update(pctx, T, Tlen);    
    hmac_sha256_update(pctx, info, info_len);
    hmac_sha256_update(pctx, &c, 1);
    hmac_sha256_final(pctx, T, SHA256_DIGEST_SIZE);  	
    
    memcpy(okm + pos, T, (i != N) ? hash_len : (okm_len - pos));
    pos += hash_len;
    Tlen = hash_len;
  }
  memset( pctx, 0, sizeof(hmac_sha256_ctx) );
  free(pctx);
  return 0;
}

/*
 *  hkdf_sha256
 *
 *  Description:
 *      This function will generate keying material using HKDF with SHA256.
 *
 *  Parameters:
 *      salt: [in]
 *          The optional salt value (a non-secret random value);
 *          if not provided (salt == NULL), it is set internally
 *          to a string of HashLen(hmac_algorithm) zeros.
 *      salt_len: [in]
 *          The length of the salt value. (Ignored if salt == NULL.)
 *      ikm: [in]
 *          Input keying material.
 *      ikm_len: [in]
 *          The length of the input keying material.
 *      info: [in]
 *          The optional context and application specific information.
 *          If info == NULL or a zero-length string, it is ignored.
 *      info_len: [in]
 *          The length of the optional context and application specific
 *          information. (Ignored if info == NULL.)
 *      okm: [out]
 *          Where the HKDF is to be stored.
 *      okm_len: [in]
 *          The length of the buffer to hold okm.
 *          okm_len must be <= 255 * 32
 *
 *  Notes:
 *      Calls hkdfExtract() and hkdfExpand().
 *
 *  Returns:
 *      0 on success, <0 on error.
 *
 */
int hkdf_sha256( uint8 *salt, uint32 salt_len,
		uint8 *ikm, uint32 ikm_len,
		uint8 *info, uint32 info_len,
		uint8 *okm, uint32 okm_len)
{
	int32 i_ret;
	uint8 prk[SHA256_DIGEST_SIZE];

	i_ret = hkdf_sha256_extract(salt, salt_len, ikm, ikm_len, prk);
	if(0 != i_ret)
	{
		printf("Error: fail to execute hkdf_sha256_extract()\n");
		return -1;
	}

	i_ret =  hkdf_sha256_expand(prk, SHA256_DIGEST_SIZE, info, info_len, okm, okm_len);
	if(0 != i_ret)
	{
		printf("Error: fail to execute hkdf_sha256_expand()\n");
		return -2;
	}

	return 0;
}

int plat_hkdf_sha256(uint8  *salt, uint32 salt_len,
		uint8  *ikm, uint32 ikm_len,
		uint8  *info, uint32 info_len,
		uint8* okm, uint32 okm_len)
{
	return hkdf_sha256(salt, salt_len, ikm, ikm_len, info, info_len, okm, okm_len);
}

/* debug dump */
#if DEV_KDF_DGB_EN

typedef struct {
	const uint8_t ikm[80];
	const size_t ikm_len;
	const uint8_t salt[80];
	const size_t salt_len;
	const uint8_t info[80];
	const size_t info_len;
	const size_t out_len;
	const uint8_t out[82];
} hkdf_test_vector_t;

/* These test vectors are from RFC 5869. */
static const hkdf_test_vector_t kTests[] = {
	{
		{
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
		}, 22,
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
			0x0c,
		}, 13,
		{
			0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
		}, 10,
		42, {
			0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f, 0x64,
			0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c,
			0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf, 0x34, 0x00, 0x72, 0x08,
			0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65
		}
	},
	{
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
			0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
			0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
			0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
			0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
			0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
		}, 80,
		{
			0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
			0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
			0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
			0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
			0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
			0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
			0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf
		}, 80,
		{
			0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb,
			0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
			0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
			0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
			0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
			0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
			0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
		}, 80,
		82, {
			0xb1, 0x1e, 0x39, 0x8d, 0xc8, 0x03, 0x27, 0xa1, 0xc8, 0xe7, 0xf7, 0x8c,
			0x59, 0x6a, 0x49, 0x34, 0x4f, 0x01, 0x2e, 0xda, 0x2d, 0x4e, 0xfa, 0xd8,
			0xa0, 0x50, 0xcc, 0x4c, 0x19, 0xaf, 0xa9, 0x7c, 0x59, 0x04, 0x5a, 0x99,
			0xca, 0xc7, 0x82, 0x72, 0x71, 0xcb, 0x41, 0xc6, 0x5e, 0x59, 0x0e, 0x09,
			0xda, 0x32, 0x75, 0x60, 0x0c, 0x2f, 0x09, 0xb8, 0x36, 0x77, 0x93, 0xa9,
			0xac, 0xa3, 0xdb, 0x71, 0xcc, 0x30, 0xc5, 0x81, 0x79, 0xec, 0x3e, 0x87,
			0xc1, 0x4c, 0x01, 0xd5, 0xc1, 0xf3, 0x43, 0x4f, 0x1d, 0x87
		}
	},
	{
		{
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
		}, 22,
		{
			0,
		}, 0,
		{
			0,
		}, 0,
		42, {
			0x8d, 0xa4, 0xe7, 0x75, 0xa5, 0x63, 0xc1, 0x8f, 0x71, 0x5f, 0x80, 0x2a,
			0x06, 0x3c, 0x5a, 0x31, 0xb8, 0xa1, 0x1f, 0x5c, 0x5e, 0xe1, 0x87, 0x9e,
			0xc3, 0x45, 0x4e, 0x5f, 0x3c, 0x73, 0x8d, 0x2d, 0x9d, 0x20, 0x13, 0x95,
			0xfa, 0xa4, 0xb6, 0x1a, 0x96, 0xc8
		}
	},
};

#endif

static void platform_after_vm_hkdf_unittest(uint level)
{
	uint8_t buf[82], salt[80], ikm[80], info[80];
	int ret;
	size_t i;

	for (i = 0; i < sizeof(kTests) / sizeof(kTests[0]); i++) {
		const hkdf_test_vector_t *test = &kTests[i];
		memcpy(salt, test->salt, test->salt_len);
		memcpy(ikm, test->ikm, test->ikm_len);
		memcpy(info, test->info, test->info_len);

		ret = hkdf_sha256(salt, test->salt_len,
				ikm, test->ikm_len,
				info, test->info_len,
				buf, test->out_len);

		if (ret) {
			dprintf(ALWAYS, "HKDF-SHA256 ERR: %d\n", ret);
			dprintf(ALWAYS, "HKDF-SHA256 Verification: FAIL\n");
			return;
		}
		if (memcmp(buf, test->out, test->out_len) != 0) {
			dprintf(ALWAYS, "%u: Resulting key material does not match test vector\n",
					(unsigned)i);
			dprintf(ALWAYS, "HKDF-SHA256 Verification: FAIL\n");
			return;
		}
	}

	dprintf(ALWAYS, "HKDF-SHA256 Verification: PASS\n");

}

LK_INIT_HOOK(platform_after_vm_hkdf_unittest, platform_after_vm_hkdf_unittest, LK_INIT_LEVEL_VM + 4);

