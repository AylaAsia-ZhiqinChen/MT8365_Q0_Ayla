/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */


#if defined(MRV)
#include "encoding.h"
#endif

#if __riscv_xlen == 64
# define STORE    sd
# define LOAD     ld
# define REGBYTES 8
#else
# define STORE    sw
# define LOAD     lw
# define FSTORE   fsw
# define FLOAD    flw
# define REGBYTES 4
#endif

.global portSAVE_CONTEXT
.global portRESTORE_CONTEXT
.global TIMER_CMP_INT
.global Mach_SW_INT
.global xPortStartScheduler
.global vTaskIncrementTick
.global vPortEndScheduler
.global xExitStack
.global FFCS_SAVE
.global FFCS_RESTORE

.macro portMACH_ENTER
	addi	sp, sp, -REGBYTES*2
	STORE	t0, 0x0(sp)
	STORE	t1, 0x4(sp)
#ifdef __track_irq
	la      t0, IRQ
	lw	t1, 0x0(t0)
	addi    t1, t1, 0x1
	sw	t1, 0x0(t0)
#endif /* __track_irq */
        la      t0, __kstack_nested
        lw      t1, 0x0(t0)
        addi    t1, t1, 0x1
        sw      t1, 0x0(t0)
	LOAD	t0, 0x0(sp)
	LOAD	t1, 0x4(sp)
	addi	sp, sp, REGBYTES*2
  .endm

.macro portMACH_EXIT
	addi	sp, sp, -REGBYTES * 2
        STORE   t0, 0x0(sp)
        STORE   t1, 0x4(sp)
#ifdef __track_irq
	la      t0, IRQ
	lw	t1, 0x0(t0)
	addi    t1, t1, -0x1
	sw	t1, 0x0(t0)
#endif /* __track_irq */
        la      t0, __kstack_nested
        lw      t1, 0x0(t0)
        addi    t1, t1, -0x1
        sw      t1, 0x0(t0)
        LOAD    t0, 0x0(sp)
        LOAD    t1, 0x4(sp)
        addi    sp, sp, REGBYTES * 2
	mret
  .endm

#ifdef configExtension_ACC
.global ACC_mstart
.global ACC_StartScheduler
.global ACC_EndScheduler
.global ACC_SAVE
.global ACC_RESTORE

  .type ACC_mstart, @function
ACC_mstart:
	setaccl acc0, x0
	setaccl acc1, x0
	setaccl acc2, x0
	setaccl acc3, x0
	ret
ACC_mstart_end:
  .size ACC_mstart, ACC_mstart_end-ACC_mstart
	nop

  .type ACC_StartScheduler, @function
ACC_StartScheduler:
	amvl	t1, acc0
	amvh	t2, acc0
	STORE	t1, 0x0(t0)
	STORE	t2, 1 * REGBYTES(t0)
	amvg	t1, acc0
	amvl	t2, acc1
	STORE	t1, 2 * REGBYTES(t0)
	STORE	t2, 3 * REGBYTES(t0)
	amvh	t1, acc1
	amvg	t2, acc1
	STORE	t1, 4 * REGBYTES(t0)
	STORE	t2, 5 * REGBYTES(t0)
	amvl	t1, acc2
	amvh	t2, acc2
	STORE	t1, 6 * REGBYTES(t0)
	STORE	t2, 7 * REGBYTES(t0)
	amvg	t1, acc2
	amvl	t2, acc3
	STORE	t1, 8 * REGBYTES(t0)
	STORE	t2, 9 * REGBYTES(t0)
	amvh	t1, acc3
	amvg	t2, acc3
	STORE	t1, 10 * REGBYTES(t0)
	STORE	t2, 11 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * ACC_reg
	ret
ACC_StartScheduler_end:
  .size ACC_StartScheduler, ACC_StartScheduler_end-ACC_StartScheduler
	nop

  .type ACC_EndScheduler, @function
ACC_EndScheduler:
	LOAD	t1, 0x0(t0)
	LOAD	t2, 1 * REGBYTES(t0)
	amvl	acc0, t1
	amvh	acc0, t2
	LOAD	t1, 2 * REGBYTES(t0)
	LOAD	t2, 3 * REGBYTES(t0)
	amvg	acc0, t1
	amvl	acc1, t2
	LOAD	t1, 4 * REGBYTES(t0)
	LOAD	t2, 5 * REGBYTES(t0)
	amvh	acc1, t1
	amvg	acc1, t2
	LOAD	t1, 6 * REGBYTES(t0)
	LOAD	t2, 7 * REGBYTES(t0)
	amvl	acc2, t1
	amvh	acc2, t2
	LOAD	t1, 8 * REGBYTES(t0)
	LOAD	t2, 9 * REGBYTES(t0)
	amvg	acc2, t1
	amvl	acc3, t2
	LOAD	t1, 10 * REGBYTES(t0)
	LOAD	t2, 11 * REGBYTES(t0)
	amvh	acc3, t1
	amvg	acc3, t2
	addi	t0, t0, REGBYTES * ACC_reg
	ret
ACC_EndScheduler_end:
  .size ACC_EndScheduler, ACC_EndScheduler_end-ACC_EndScheduler
	nop

  .type ACC_SAVE, @function
ACC_SAVE:
	amvl t0, acc0
	amvh t1, acc0
	STORE t0, 0*REGBYTES(sp)
	STORE t1, 1*REGBYTES(sp)
	amvg t0, acc0
	amvl t1, acc1
	STORE t0, 2*REGBYTES(sp)
	STORE t1, 3*REGBYTES(sp)
	amvh t0, acc1
	amvg t1, acc1
	STORE t0, 4*REGBYTES(sp)
	STORE t1, 5*REGBYTES(sp)
	amvl t0, acc2
	amvh t1, acc2
	STORE t0, 6*REGBYTES(sp)
	STORE t1, 7*REGBYTES(sp)
	amvg t0, acc2
	amvl t1, acc3
	STORE t0, 8*REGBYTES(sp)
	STORE t1, 9*REGBYTES(sp)
	amvh t0, acc3
	amvg t1, acc3
	STORE t0, 10*REGBYTES(sp)
	STORE t1, 11*REGBYTES(sp)
	ret
ACC_SAVE_end:
  .size ACC_SAVE, ACC_SAVE_end-ACC_SAVE
	nop

  .type ACC_RESTORE, @function
ACC_RESTORE:
	LOAD t0, 0*REGBYTES(sp)
	LOAD t1, 1*REGBYTES(sp)
	amvl acc0, t0
	amvh acc0, t1
	LOAD t0, 2*REGBYTES(sp)
	LOAD t1, 3*REGBYTES(sp)
	amvg acc0, t0
	amvl acc1, t1
	LOAD t0, 4*REGBYTES(sp)
	LOAD t1, 5*REGBYTES(sp)
	amvh acc1, t0
	amvg acc1, t1
	LOAD t0, 6*REGBYTES(sp)
	LOAD t1, 7*REGBYTES(sp)
	amvl acc2, t0
	amvh acc2, t1
	LOAD t0, 8*REGBYTES(sp)
	LOAD t1, 9*REGBYTES(sp)
	amvg acc2, t0
	amvl acc3, t1
	LOAD t0, 10*REGBYTES(sp)
	LOAD t1, 11*REGBYTES(sp)
	amvh acc3, t0
	amvg acc3, t1
	ret
ACC_RESTORE_end:
  .size ACC_RESTORE, ACC_RESTORE_end-ACC_RESTORE
	nop
#endif /* configExtension_ACC */

#ifdef configExtension_F
  .type FFCS_SAVE, @function
FFCS_SAVE:
	csrr t0, mstatus
	li   t1, (MSTATUS32_SD | MSTATUS_FS_D) #0x80006000
	and  t0, t0, t1
	beq  t0, t1, 1f
	ret
1:
	addi  sp, sp, -REGBYTES
	STORE ra, 0*REGBYTES(sp)
	jal   pxTaskGetEndOfStack

	addi   a0, a0, -REGBYTES*(F_reg_ctx)
	FSTORE f0, 0*REGBYTES(a0)
	FSTORE f1, 1*REGBYTES(a0)
	FSTORE f2, 2*REGBYTES(a0)
	FSTORE f3, 3*REGBYTES(a0)
	FSTORE f4, 4*REGBYTES(a0)
	FSTORE f5, 5*REGBYTES(a0)
	FSTORE f6, 6*REGBYTES(a0)
	FSTORE f7, 7*REGBYTES(a0)
	FSTORE f8, 8*REGBYTES(a0)
	FSTORE f9, 9*REGBYTES(a0)
	FSTORE f10, 10*REGBYTES(a0)
	FSTORE f11, 11*REGBYTES(a0)
	FSTORE f12, 12*REGBYTES(a0)
	FSTORE f13, 13*REGBYTES(a0)
	FSTORE f14, 14*REGBYTES(a0)
	FSTORE f15, 15*REGBYTES(a0)
	FSTORE f16, 16*REGBYTES(a0)
	FSTORE f17, 17*REGBYTES(a0)
	FSTORE f18, 18*REGBYTES(a0)
	FSTORE f19, 19*REGBYTES(a0)
	FSTORE f20, 20*REGBYTES(a0)
	FSTORE f21, 21*REGBYTES(a0)
	FSTORE f22, 22*REGBYTES(a0)
	FSTORE f23, 23*REGBYTES(a0)
	FSTORE f24, 24*REGBYTES(a0)
	FSTORE f25, 25*REGBYTES(a0)
	FSTORE f26, 26*REGBYTES(a0)
	FSTORE f27, 27*REGBYTES(a0)
	FSTORE f28, 28*REGBYTES(a0)
	FSTORE f29, 29*REGBYTES(a0)
	FSTORE f30, 30*REGBYTES(a0)
	FSTORE f31, 31*REGBYTES(a0)
        csrr   t0, fcsr
        STORE  t0, 32*REGBYTES(a0)

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	li   t0, (MSTATUS_FS_D ^ MSTATUS_FS_C) #0x2000
	csrc mstatus, t0
	ret
FFCS_SAVE_end:
  .size FFCS_SAVE, FFCS_SAVE_end-FFCS_SAVE
	nop

  .type FFCS_RESTORE, @function
FFCS_RESTORE:
	addi  sp, sp, -REGBYTES
	STORE ra, 0*REGBYTES(sp)
	jal   pxTaskGetEndOfStack

	addi  a0, a0, -REGBYTES*(F_reg_ctx)
	FLOAD f0, 0*REGBYTES(a0)
	FLOAD f1, 1*REGBYTES(a0)
	FLOAD f2, 2*REGBYTES(a0)
	FLOAD f3, 3*REGBYTES(a0)
	FLOAD f4, 4*REGBYTES(a0)
	FLOAD f5, 5*REGBYTES(a0)
	FLOAD f6, 6*REGBYTES(a0)
	FLOAD f7, 7*REGBYTES(a0)
	FLOAD f8, 8*REGBYTES(a0)
	FLOAD f9, 9*REGBYTES(a0)
	FLOAD f10, 10*REGBYTES(a0)
	FLOAD f11, 11*REGBYTES(a0)
	FLOAD f12, 12*REGBYTES(a0)
	FLOAD f13, 13*REGBYTES(a0)
	FLOAD f14, 14*REGBYTES(a0)
	FLOAD f15, 15*REGBYTES(a0)
	FLOAD f16, 16*REGBYTES(a0)
	FLOAD f17, 17*REGBYTES(a0)
	FLOAD f18, 18*REGBYTES(a0)
	FLOAD f19, 19*REGBYTES(a0)
	FLOAD f20, 20*REGBYTES(a0)
	FLOAD f21, 21*REGBYTES(a0)
	FLOAD f22, 22*REGBYTES(a0)
	FLOAD f23, 23*REGBYTES(a0)
	FLOAD f24, 24*REGBYTES(a0)
	FLOAD f25, 25*REGBYTES(a0)
	FLOAD f26, 26*REGBYTES(a0)
	FLOAD f27, 27*REGBYTES(a0)
	FLOAD f28, 28*REGBYTES(a0)
	FLOAD f29, 29*REGBYTES(a0)
	FLOAD f30, 30*REGBYTES(a0)
	FLOAD f31, 31*REGBYTES(a0)
        LOAD  t0,  32*REGBYTES(a0)
        csrw  fcsr, t0

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	ret
FFCS_RESTORE_end:
  .size FFCS_RESTORE, FFCS_RESTORE_end-FFCS_RESTORE
	nop
#endif /* configExtension_F */

#ifdef configExtension_ACC
  .type FACCCS_SAVE, @function
FACCCS_SAVE:
	csrr t0, mstatus
	li   t1, (MSTATUS32_SD | MSTATUS_ACS_D | MSTATUS_XS_D) #0x81818000
	and  t0, t0, t1
	beq  t0, t1, 1f
	ret
1:
	addi  sp, sp, -REGBYTES
	STORE ra, 0*REGBYTES(sp)
	jal   pxTaskGetEndOfStack

	addi  a0, a0, -REGBYTES*(F_reg_ctx + ACC_reg_ctx)
	amvl  t0, acc0
	amvh  t1, acc0
	STORE t0, 0*REGBYTES(a0)
	STORE t1, 1*REGBYTES(a0)
	amvg  t0, acc0
	amvl  t1, acc1
	STORE t0, 2*REGBYTES(a0)
	STORE t1, 3*REGBYTES(a0)
	amvh  t0, acc1
	amvg  t1, acc1
	STORE t0, 4*REGBYTES(a0)
	STORE t1, 5*REGBYTES(a0)
	amvl  t0, acc2
	amvh  t1, acc2
	STORE t0, 6*REGBYTES(a0)
	STORE t1, 7*REGBYTES(a0)
	amvg  t0, acc2
	amvl  t1, acc3
	STORE t0, 8*REGBYTES(a0)
	STORE t1, 9*REGBYTES(a0)
	amvh  t0, acc3
	amvg  t1, acc3
	STORE t0, 10*REGBYTES(a0)
	STORE t1, 11*REGBYTES(a0)

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	li   t0, (MSTATUS_ACS_D ^ MSTATUS_ACS_C) #0x00800000
	csrc mstatus, t0
	ret
FACCCS_SAVE_end:
  .size FACCCS_SAVE, FACCCS_SAVE_end-FACCCS_SAVE
	nop

  .type FACCCS_RESTORE, @function
FACCCS_RESTORE:
	addi  sp, sp, -REGBYTES
	STORE ra, 0*REGBYTES(sp)
	jal   pxTaskGetEndOfStack

	addi a0, a0, -REGBYTES*(F_reg_ctx + ACC_reg_ctx)
	LOAD t0, 0*REGBYTES(a0)
	LOAD t1, 1*REGBYTES(a0)
	amvl acc0, t0
	amvh acc0, t1
	LOAD t0, 2*REGBYTES(a0)
	LOAD t1, 3*REGBYTES(a0)
	amvg acc0, t0
	amvl acc1, t1
	LOAD t0, 4*REGBYTES(a0)
	LOAD t1, 5*REGBYTES(a0)
	amvh acc1, t0
	amvg acc1, t1
	LOAD t0, 6*REGBYTES(a0)
	LOAD t1, 7*REGBYTES(a0)
	amvl acc2, t0
	amvh acc2, t1
	LOAD t0, 8*REGBYTES(a0)
	LOAD t1, 9*REGBYTES(a0)
	amvg acc2, t0
	amvl acc3, t1
	LOAD t0, 10*REGBYTES(a0)
	LOAD t1, 11*REGBYTES(a0)
	amvh acc3, t0
	amvg acc3, t1

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	ret
FACCCS_RESTORE_end:
  .size FACCCS_RESTORE, FACCCS_RESTORE_end-FACCCS_RESTORE
	nop
#endif /* configExtension_ACC */

#ifdef configExtension_UAM
  .type FUAMCS_SAVE, @function
FUAMCS_SAVE:
	csrr t0, mstatus
	li   t1, (MSTATUS32_SD | MSTATUS_UAS_D | MSTATUS_XS_D) #0x86018000
	and  t0, t0, t1
	beq  t0, t1, 1f
	ret
1:
	addi  sp, sp, -REGBYTES
	STORE ra, 0*REGBYTES(sp)
	jal   pxTaskGetEndOfStack

	addi   a0, a0, -REGBYTES*(F_reg_ctx + ACC_reg_ctx + UAM_reg_ctx)
        csrr   t0, uab0
        csrr   t1, uas0
        STORE  t0, 0*REGBYTES(a0)
        STORE  t1, 1*REGBYTES(a0)
        csrr   t0, uam0
        csrr   t1, uab1
        STORE  t0, 2*REGBYTES(a0)
        STORE  t1, 3*REGBYTES(a0)
        csrr   t0, uas1
        csrr   t1, uam1
        STORE  t0, 4*REGBYTES(a0)
        STORE  t1, 5*REGBYTES(a0)
        csrr   t0, uab2
        csrr   t1, uas2
        STORE  t0, 6*REGBYTES(a0)
        STORE  t1, 7*REGBYTES(a0)
        csrr   t0, uam2
        csrr   t1, uab3
        STORE  t0, 8*REGBYTES(a0)
        STORE  t1, 9*REGBYTES(a0)
        csrr   t0, uas3
        csrr   t1, uam3
        STORE  t0, 10*REGBYTES(a0)
        STORE  t1, 11*REGBYTES(a0)

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	li   t0, (MSTATUS_UAS_D ^ MSTATUS_UAS_C) #0x02000000
	csrc mstatus, t0
	ret
FUAMCS_SAVE_end:
  .size FUAMCS_SAVE, FUAMCS_SAVE_end-FUAMCS_SAVE
	nop

  .type FUAMCS_RESTORE, @function
FUAMCS_RESTORE:
	addi  sp, sp, -REGBYTES
	STORE ra, 0x0(sp)
	jal   pxTaskGetEndOfStack

	addi a0, a0, -REGBYTES*(F_reg_ctx + ACC_reg_ctx + UAM_reg_ctx)
        LOAD t0, 0*REGBYTES(a0)
        LOAD t1, 1*REGBYTES(a0)
        csrw uab0, t0
        csrw uas0, t1
        LOAD t0, 2*REGBYTES(a0)
        LOAD t1, 3*REGBYTES(a0)
        csrw uam0, t0
        csrw uab1, t1
        LOAD t0, 4*REGBYTES(a0)
        LOAD t1, 5*REGBYTES(a0)
        csrw uas1, t0
        csrw uam1, t1
        LOAD t0, 6*REGBYTES(a0)
        LOAD t1, 7*REGBYTES(a0)
        csrw uab2, t0
        csrw uas2, t1
        LOAD t0, 8*REGBYTES(a0)
        LOAD t1, 9*REGBYTES(a0)
        csrw uam2, t0
        csrw uab3, t1
        LOAD t0, 10*REGBYTES(a0)
        LOAD t1, 11*REGBYTES(a0)
        csrw uas3, t0
        csrw uam3, t1

	LOAD ra, 0x0(sp)
	addi sp, sp, REGBYTES
	ret
FUAMCS_RESTORE_end:
  .size FUAMCS_RESTORE, FUAMCS_RESTORE_end-FUAMCS_RESTORE
	nop
#endif /* configExtension_UAM */

/* Macro for saving task context */
.macro portSAVE_CONTEXT
	.global	pxCurrentTCB
#if defined(configExtension_F) && !defined(FF)
	addi  sp, sp, -REGBYTES * (F_reg_ctx+1)
	STORE t0, 0x0(sp)
	addi  sp, sp, REGBYTES

	FSTORE	f0, 0x0(sp)
	FSTORE	f1, 1 * REGBYTES(sp)
	FSTORE	f2, 2 * REGBYTES(sp)
	FSTORE	f3, 3 * REGBYTES(sp)
	FSTORE	f4, 4 * REGBYTES(sp)
	FSTORE	f5, 5 * REGBYTES(sp)
	FSTORE	f6, 6 * REGBYTES(sp)
	FSTORE	f7, 7 * REGBYTES(sp)
	FSTORE	f8, 8 * REGBYTES(sp)
	FSTORE	f9, 9 * REGBYTES(sp)
	FSTORE	f10, 10 * REGBYTES(sp)
	FSTORE	f11, 11 * REGBYTES(sp)
	FSTORE	f12, 12 * REGBYTES(sp)
	FSTORE	f13, 13 * REGBYTES(sp)
	FSTORE	f14, 14 * REGBYTES(sp)
	FSTORE	f15, 15 * REGBYTES(sp)
	FSTORE	f16, 16 * REGBYTES(sp)
	FSTORE	f17, 17 * REGBYTES(sp)
	FSTORE	f18, 18 * REGBYTES(sp)
	FSTORE	f19, 19 * REGBYTES(sp)
	FSTORE	f20, 20 * REGBYTES(sp)
	FSTORE	f21, 21 * REGBYTES(sp)
	FSTORE	f22, 22 * REGBYTES(sp)
	FSTORE	f23, 23 * REGBYTES(sp)
	FSTORE	f24, 24 * REGBYTES(sp)
	FSTORE	f25, 25 * REGBYTES(sp)
	FSTORE	f26, 26 * REGBYTES(sp)
	FSTORE	f27, 27 * REGBYTES(sp)
	FSTORE	f28, 28 * REGBYTES(sp)
	FSTORE	f29, 29 * REGBYTES(sp)
	FSTORE	f30, 30 * REGBYTES(sp)
	FSTORE	f31, 31 * REGBYTES(sp)
	csrr    t0, fcsr
	STORE   t0, 32 * REGBYTES(sp)

	addi sp, sp, -REGBYTES
	LOAD t0, 0x0(sp)
	addi sp, sp, REGBYTES
#endif

#if defined(configExtension_ACC) && !defined(FACC)
	addi  sp, sp, -REGBYTES * (ACC_reg_ctx+3)
	STORE t0, 0x0(sp)
	STORE t1, 1*REGBYTES(sp)
	STORE ra, 2*REGBYTES(sp)
	addi  sp, sp, REGBYTES * 3

	jal ACC_SAVE

	addi sp, sp, -REGBYTES * 3
	LOAD t0, 0x0(sp)
	LOAD t1, 1*REGBYTES(sp)
	LOAD ra, 2*REGBYTES(sp)
	addi sp, sp, REGBYTES * 3
#endif /* configExtension_ACC */

#if defined(configExtension_UAM) && !defined(FUAM)
	addi  sp, sp, -REGBYTES * (UAM_reg_ctx+2)
	STORE t0, 0x0(sp)
	STORE t1, 1*REGBYTES(sp)
	addi  sp, sp, REGBYTES * (2)

        csrr  t0, uab0
        csrr  t1, uas0
	STORE t0, 0x0(sp)
	STORE t1, 1 * REGBYTES(sp)
        csrr  t0, uam0
        csrr  t1, uab1
	STORE t0, 2 * REGBYTES(sp)
	STORE t1, 3 * REGBYTES(sp)
        csrr  t0, uas1
        csrr  t1, uam1
	STORE t0, 4 * REGBYTES(sp)
	STORE t1, 5 * REGBYTES(sp)
        csrr  t0, uab2
        csrr  t1, uas2
	STORE t0, 6 * REGBYTES(sp)
	STORE t1, 7 * REGBYTES(sp)
        csrr  t0, uam2
        csrr  t1, uab3
	STORE t0, 8 * REGBYTES(sp)
	STORE t1, 9 * REGBYTES(sp)
        csrr  t0, uas3
        csrr  t1, uam3
	STORE t0, 10 * REGBYTES(sp)
	STORE t1, 11 * REGBYTES(sp)

	addi sp, sp, -REGBYTES * (2)
	LOAD t0, 0x0(sp)
	LOAD t1, 1*REGBYTES(sp)
	addi sp, sp, REGBYTES * (2)
#endif /* configExtension_UAM */

	/* make room in stack */
	addi	sp, sp, -REGBYTES * (X_reg_ctx)
	/* Save Context */
	STORE	x1, 0x0(sp)
	STORE	x2, 1 * REGBYTES(sp)
	STORE	x3, 2 * REGBYTES(sp)
	STORE	x4, 3 * REGBYTES(sp)
	STORE	x5, 4 * REGBYTES(sp)
	STORE	x6, 5 * REGBYTES(sp)
	STORE	x7, 6 * REGBYTES(sp)
	STORE	x8, 7 * REGBYTES(sp)
	STORE	x9, 8 * REGBYTES(sp)
	STORE	x10, 9 * REGBYTES(sp)
	STORE	x11, 10 * REGBYTES(sp)
	STORE	x12, 11 * REGBYTES(sp)
	STORE	x13, 12 * REGBYTES(sp)
	STORE	x14, 13 * REGBYTES(sp)
	STORE	x15, 14 * REGBYTES(sp)
	STORE	x16, 15 * REGBYTES(sp)
	STORE	x17, 16 * REGBYTES(sp)
	STORE	x18, 17 * REGBYTES(sp)
	STORE	x19, 18 * REGBYTES(sp)
	STORE	x20, 19 * REGBYTES(sp)
	STORE	x21, 20 * REGBYTES(sp)
	STORE	x22, 21 * REGBYTES(sp)
	STORE	x23, 22 * REGBYTES(sp)
	STORE	x24, 23 * REGBYTES(sp)
	STORE	x25, 24 * REGBYTES(sp)
	STORE	x26, 25 * REGBYTES(sp)
	STORE	x27, 26 * REGBYTES(sp)
	STORE	x28, 27 * REGBYTES(sp)
	STORE	x29, 28 * REGBYTES(sp)
	STORE	x30, 29 * REGBYTES(sp)
	STORE	x31, 30 * REGBYTES(sp)

#if defined(configExtension_F) && defined(FF)
	jal FFCS_SAVE
#endif

#if defined(configExtension_ACC) && defined(FACC)
	jal FACCCS_SAVE
#endif /* configExtension_ACC */

#if defined(configExtension_UAM) && defined(FUAM)
	jal FUAMCS_SAVE
#endif /* configExtension_UAM */

	/* Store current stackpointer in task control block (TCB) */
	LOAD	t0, pxCurrentTCB	/* pointer */
	STORE	sp, 0x0(t0)
	.endm

/* Disable interrupt (disable timer interrupt now) */
.macro portDISABLE_INT
	addi	sp,  sp, -REGBYTES * 1
	STORE   t0,  0x0(sp)
	li      t0,  0xff030888
	csrc    mie, t0
	LOAD    t0,  0x0(sp)
	addi    sp,  sp, REGBYTES * 1
	.endm

/* Enable interrupt (Enable timer interrupt now) */
.macro portENABLE_INT
	addi	sp,  sp, -REGBYTES * 1
	STORE   t0,  0x0(sp)
	li      t0,  0xff030888
	csrs    mie, t0
	LOAD    t0,  0x0(sp)
	addi    sp,  sp, REGBYTES * 1
	.endm

/* Saves current exception program counter (EPC) as task program counter */
.macro portSAVE_EPC
	csrr	t0, mepc
	STORE	t0, 31 * REGBYTES(sp)
	.endm

/* Saves current mstatus */
.macro portSAVE_MSTATUS
	csrr	t0, mstatus
	STORE	t0, 32 * REGBYTES(sp)
	.endm

/* Macro for restoring task context */
.macro portRESTORE_CONTEXT

	.global	pxCurrentTCB
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)

	/* Load task program counter */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrw	mepc, t0

	/* Restore registers,
	   Skip global pointer because that does not change */

#if defined(configExtension_F) && defined(FF)
	jal FFCS_RESTORE
#endif /* configExtension_F */

#if defined(configExtension_ACC) && defined(FACC)
	jal FACCCS_RESTORE
#endif /* configExtension_ACC */

#if defined(configExtension_UAM) && defined(FUAM)
	jal FUAMCS_RESTORE
#endif /* configExtension_UAM */

	/* Load mstatus */
	LOAD	t0, 32 * REGBYTES(sp)
	csrw	mstatus, t0

	LOAD	x1, 0x0(sp)
	LOAD	x3, 2 * REGBYTES(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)
	addi	sp, sp, REGBYTES * (X_reg_ctx)

#if defined(configExtension_UAM) && !defined(FUAM)
	addi  sp, sp, -REGBYTES * 2
	STORE t0, 0x0(sp)
	STORE t1, 1*REGBYTES(sp)
	addi  sp, sp, REGBYTES * 2

        LOAD t0, 0*REGBYTES(sp)
        LOAD t1, 1*REGBYTES(sp)
        csrw uab0, t0
        csrw uas0, t1
        LOAD t0, 2*REGBYTES(sp)
        LOAD t1, 3*REGBYTES(sp)
        csrw uam0, t0
        csrw uab1, t1
        LOAD t0, 4*REGBYTES(sp)
        LOAD t1, 5*REGBYTES(sp)
        csrw uas1, t0
        csrw uam1, t1
        LOAD t0, 6*REGBYTES(sp)
        LOAD t1, 7*REGBYTES(sp)
        csrw uab2, t0
        csrw uas2, t1
        LOAD t0, 8*REGBYTES(sp)
        LOAD t1, 9*REGBYTES(sp)
        csrw uam2, t0
        csrw uab3, t1
        LOAD t0, 10*REGBYTES(sp)
        LOAD t1, 11*REGBYTES(sp)
        csrw uas3, t0
        csrw uam3, t1

	addi sp, sp, -REGBYTES * 2
	LOAD t0, 0x0(sp)
	LOAD t1, 1*REGBYTES(sp)
	addi sp, sp, REGBYTES * (UAM_reg_ctx+2)
#endif /* configExtension_UAM */

#if defined(configExtension_ACC) && !defined(FACC)
	addi  sp, sp, -REGBYTES * 3
	STORE t0, 0x0(sp)
	STORE t1, 1*REGBYTES(sp)
	STORE ra, 2*REGBYTES(sp)
	addi  sp, sp, REGBYTES * 3

	jal ACC_RESTORE

	addi sp, sp, -REGBYTES * 3
	LOAD t0, 0x0(sp)
	LOAD t1, 1*REGBYTES(sp)
	LOAD ra, 2*REGBYTES(sp)
	addi sp, sp, REGBYTES * (ACC_reg_ctx+3)
#endif /* configExtension_ACC */

#if defined(configExtension_F) && !defined(FF)
	addi  sp, sp, -REGBYTES
	STORE t0, 0x0(sp)
	addi  sp, sp, REGBYTES

	FLOAD	f0, 0x0(sp)
	FLOAD	f1, 1 * REGBYTES(sp)
	FLOAD	f2, 2 * REGBYTES(sp)
	FLOAD	f3, 3 * REGBYTES(sp)
	FLOAD	f4, 4 * REGBYTES(sp)
	FLOAD	f5, 5 * REGBYTES(sp)
	FLOAD	f6, 6 * REGBYTES(sp)
	FLOAD	f7, 7 * REGBYTES(sp)
	FLOAD	f8, 8 * REGBYTES(sp)
	FLOAD	f9, 9 * REGBYTES(sp)
	FLOAD	f10, 10 * REGBYTES(sp)
	FLOAD	f11, 11 * REGBYTES(sp)
	FLOAD	f12, 12 * REGBYTES(sp)
	FLOAD	f13, 13 * REGBYTES(sp)
	FLOAD	f14, 14 * REGBYTES(sp)
	FLOAD	f15, 15 * REGBYTES(sp)
	FLOAD	f16, 16 * REGBYTES(sp)
	FLOAD	f17, 17 * REGBYTES(sp)
	FLOAD	f18, 18 * REGBYTES(sp)
	FLOAD	f19, 19 * REGBYTES(sp)
	FLOAD	f20, 20 * REGBYTES(sp)
	FLOAD	f21, 21 * REGBYTES(sp)
	FLOAD	f22, 22 * REGBYTES(sp)
	FLOAD	f23, 23 * REGBYTES(sp)
	FLOAD	f24, 24 * REGBYTES(sp)
	FLOAD	f25, 25 * REGBYTES(sp)
	FLOAD	f26, 26 * REGBYTES(sp)
	FLOAD	f27, 27 * REGBYTES(sp)
	FLOAD	f28, 28 * REGBYTES(sp)
	FLOAD	f29, 29 * REGBYTES(sp)
	FLOAD	f30, 30 * REGBYTES(sp)
	FLOAD	f31, 31 * REGBYTES(sp)
	LOAD    t0, 32 * REGBYTES(sp)
	csrw    fcsr, t0

	addi sp, sp, -REGBYTES
	LOAD t0, 0x0(sp)
	addi sp, sp, REGBYTES * (F_reg_ctx+1)
#endif
	.endm

/* Macro for restoring task context */
  .type TIMER_CMP_INT, @function
TIMER_CMP_INT:
        portMACH_ENTER
#ifdef CONFIG_ESL_BUILD
	/* clear timer reload status */
	csrrwi x0,MRV_MTIME_MTMRSTATUS,0
#endif
	portSAVE_CONTEXT
	portSAVE_EPC
	portSAVE_MSTATUS
        csrrw sp, mscratch, sp
	jal   vPortSysTickHandler
        csrrw sp, mscratch, sp
	portRESTORE_CONTEXT
        portMACH_EXIT
TIMER_CMP_INT_end:
  .size TIMER_CMP_INT, TIMER_CMP_INT_end-TIMER_CMP_INT
        nop

  .type Mach_SW_INT, @function
Mach_SW_INT:
	/* Interupt is software interrupt */
        portMACH_ENTER
	portSAVE_CONTEXT
	portSAVE_EPC
	portSAVE_MSTATUS
        csrrw sp, mscratch, sp
	jal   vTaskSwitchContext
        csrrw sp, mscratch, sp
	portRESTORE_CONTEXT
        portMACH_EXIT
Mach_SW_INT_end:
  .size Mach_SW_INT, Mach_SW_INT_end-Mach_SW_INT
	nop

  .type xPortStartScheduler, @function
xPortStartScheduler:
#ifdef __gracefulExit
	/* Stores context when starting the scheduler in xStartContext.
	   This is used for when you want to gracefully exit the scheduler.
	   For example if you want to test multiple instances after each other in one test suite.
     */
	la	t0, xStartContext

#ifdef configExtension_F
	FSTORE	f0, 0x0(t0)
	FSTORE	f1, 1 * REGBYTES(t0)
	FSTORE	f2, 2 * REGBYTES(t0)
	FSTORE	f3, 3 * REGBYTES(t0)
	FSTORE	f4, 4 * REGBYTES(t0)
	FSTORE	f5, 5 * REGBYTES(t0)
	FSTORE	f6, 6 * REGBYTES(t0)
	FSTORE	f7, 7 * REGBYTES(t0)
	FSTORE	f8, 8 * REGBYTES(t0)
	FSTORE	f9, 9 * REGBYTES(t0)
	FSTORE	f10, 10 * REGBYTES(t0)
	FSTORE	f11, 11 * REGBYTES(t0)
	FSTORE	f12, 12 * REGBYTES(t0)
	FSTORE	f13, 13 * REGBYTES(t0)
	FSTORE	f14, 14 * REGBYTES(t0)
	FSTORE	f15, 15 * REGBYTES(t0)
	FSTORE	f16, 16 * REGBYTES(t0)
	FSTORE	f17, 17 * REGBYTES(t0)
	FSTORE	f18, 18 * REGBYTES(t0)
	FSTORE	f19, 19 * REGBYTES(t0)
	FSTORE	f20, 20 * REGBYTES(t0)
	FSTORE	f21, 21 * REGBYTES(t0)
	FSTORE	f22, 22 * REGBYTES(t0)
	FSTORE	f23, 23 * REGBYTES(t0)
	FSTORE	f24, 24 * REGBYTES(t0)
	FSTORE	f25, 25 * REGBYTES(t0)
	FSTORE	f26, 26 * REGBYTES(t0)
	FSTORE	f27, 27 * REGBYTES(t0)
	FSTORE	f28, 28 * REGBYTES(t0)
	FSTORE	f29, 29 * REGBYTES(t0)
	FSTORE	f30, 30 * REGBYTES(t0)
	FSTORE	f31, 31 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * F_reg
#endif /* configExtension_F */

#ifdef configExtension_ACC
	jal ACC_StartScheduler
#endif /* configExtension_ACC */

#ifdef configExtension_UAM
        csrr    t1, uab0
        csrr    t2, uas0
	STORE	t1, 0x0(t0)
	STORE	t2, 1 * REGBYTES(t0)
        csrr    t1, uam0
        csrr    t2, uab1
	STORE	t1, 2 * REGBYTES(t0)
	STORE	t2, 3 * REGBYTES(t0)
        csrr    t1, uas1
        csrr    t2, uam1
	STORE	t1, 4 * REGBYTES(t0)
	STORE	t2, 5 * REGBYTES(t0)
        csrr    t1, uab2
        csrr    t2, uas2
	STORE	t1, 6 * REGBYTES(t0)
	STORE	t2, 7 * REGBYTES(t0)
        csrr    t1, uam2
        csrr    t2, uab3
	STORE	t1, 8 * REGBYTES(t0)
	STORE	t2, 9 * REGBYTES(t0)
        csrr    t1, uas3
        csrr    t2, uam3
	STORE	t1, 10 * REGBYTES(t0)
	STORE	t2, 11 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * UAM_reg
#endif /* configExtension_UAM */

	STORE	x0, 0x0(t0)
	STORE	x1, 1 * REGBYTES(t0)
	STORE	x2, 2 * REGBYTES(t0)
	STORE	x3, 3 * REGBYTES(t0)
	STORE	x4, 4 * REGBYTES(t0)
	STORE	x5, 5 * REGBYTES(t0)
	STORE	x6, 6 * REGBYTES(t0)
	STORE	x7, 7 * REGBYTES(t0)
	STORE	x8, 8 * REGBYTES(t0)
	STORE	x9, 9 * REGBYTES(t0)
	STORE	x10, 10 * REGBYTES(t0)
	STORE	x11, 11 * REGBYTES(t0)
	STORE	x12, 12 * REGBYTES(t0)
	STORE	x13, 13 * REGBYTES(t0)
	STORE	x14, 14 * REGBYTES(t0)
	STORE	x15, 15 * REGBYTES(t0)
	STORE	x16, 16 * REGBYTES(t0)
	STORE	x17, 17 * REGBYTES(t0)
	STORE	x18, 18 * REGBYTES(t0)
	STORE	x19, 19 * REGBYTES(t0)
	STORE	x20, 20 * REGBYTES(t0)
	STORE	x21, 21 * REGBYTES(t0)
	STORE	x22, 22 * REGBYTES(t0)
	STORE	x23, 23 * REGBYTES(t0)
	STORE	x24, 24 * REGBYTES(t0)
	STORE	x25, 25 * REGBYTES(t0)
	STORE	x26, 26 * REGBYTES(t0)
	STORE	x27, 27 * REGBYTES(t0)
	STORE	x28, 28 * REGBYTES(t0)
	STORE	x29, 29 * REGBYTES(t0)
	STORE	x30, 30 * REGBYTES(t0)
	STORE	x31, 31 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * X_reg
#endif /* __gracefulExit */
#if defined(P_MODE_1) // task/RTOS in user mode
        li      a7, 45 # SYS_umode_2_mmode
        ecall
#endif /* P_MODE_1 */
#ifdef __track_irq
	la      t0, IRQ
	lw	t1, 0x0(t0)
	addi    t1, t1,0x1
	sw	t1, 0x0(t0)
#endif /* __track_irq */
	jal	vPortSetupTimer
	portRESTORE_CONTEXT
	portENABLE_INT
        portMACH_EXIT
xPortStartScheduler_end:
  .size xPortStartScheduler, xPortStartScheduler_end-xPortStartScheduler
	nop

  .type vPortEndScheduler, @function
vPortEndScheduler:
#ifdef __gracefulExit
	/* Load current context from xStartContext */
	la	t0, xStartContext

#ifdef configExtension_F
	FLOAD	f0, 0x0(t0)
	FLOAD	f1, 1 * REGBYTES(t0)
	FLOAD	f2, 2 * REGBYTES(t0)
	FLOAD	f3, 3 * REGBYTES(t0)
	FLOAD	f4, 4 * REGBYTES(t0)
	FLOAD	f5, 5 * REGBYTES(t0)
	FLOAD	f6, 6 * REGBYTES(t0)
	FLOAD	f7, 7 * REGBYTES(t0)
	FLOAD	f8, 8 * REGBYTES(t0)
	FLOAD	f9, 9 * REGBYTES(t0)
	FLOAD	f10, 10 * REGBYTES(t0)
	FLOAD	f11, 11 * REGBYTES(t0)
	FLOAD	f12, 12 * REGBYTES(t0)
	FLOAD	f13, 13 * REGBYTES(t0)
	FLOAD	f14, 14 * REGBYTES(t0)
	FLOAD	f15, 15 * REGBYTES(t0)
	FLOAD	f16, 16 * REGBYTES(t0)
	FLOAD	f17, 17 * REGBYTES(t0)
	FLOAD	f18, 18 * REGBYTES(t0)
	FLOAD	f19, 19 * REGBYTES(t0)
	FLOAD	f20, 20 * REGBYTES(t0)
	FLOAD	f21, 21 * REGBYTES(t0)
	FLOAD	f22, 22 * REGBYTES(t0)
	FLOAD	f23, 23 * REGBYTES(t0)
	FLOAD	f24, 24 * REGBYTES(t0)
	FLOAD	f25, 25 * REGBYTES(t0)
	FLOAD	f26, 26 * REGBYTES(t0)
	FLOAD	f27, 27 * REGBYTES(t0)
	FLOAD	f28, 28 * REGBYTES(t0)
	FLOAD	f29, 29 * REGBYTES(t0)
	FLOAD	f30, 30 * REGBYTES(t0)
	FLOAD	f31, 31 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * F_reg
#endif /* configExtension_F */

#ifdef configExtension_ACC
	jal ACC_EndScheduler
#endif /* configExtension_ACC */

#ifdef configExtension_UAM
        LOAD t1, 0*REGBYTES(t0)
        LOAD t2, 1*REGBYTES(t0)
        csrw uab0, t1
        csrw uas0, t2
        LOAD t1, 2*REGBYTES(t0)
        LOAD t2, 3*REGBYTES(t0)
        csrw uam0, t1
        csrw uab1, t2
        LOAD t1, 4*REGBYTES(t0)
        LOAD t2, 5*REGBYTES(t0)
        csrw uas1, t1
        csrw uam1, t2
        LOAD t1, 6*REGBYTES(t0)
        LOAD t2, 7*REGBYTES(t0)
        csrw uab2, t1
        csrw uas2, t2
        LOAD t1, 8*REGBYTES(t0)
        LOAD t2, 9*REGBYTES(t0)
        csrw uam2, t1
        csrw uab3, t2
        LOAD t1, 10*REGBYTES(t0)
        LOAD t2, 11*REGBYTES(t0)
        csrw uas3, t1
        csrw uam3, t2
	addi t0, t0, REGBYTES * UAM_reg
#endif /* configExtension_UAM */

	LOAD	x1, 1 * REGBYTES(t0)
	LOAD	x2, 2 * REGBYTES(t0)
	LOAD	x3, 3 * REGBYTES(t0)
	LOAD	x4, 4 * REGBYTES(t0)
	LOAD	x5, 5 * REGBYTES(t0)
	LOAD	x6, 6 * REGBYTES(t0)
	LOAD	x7, 7 * REGBYTES(t0)
	LOAD	x8, 8 * REGBYTES(t0)
	LOAD	x9, 9 * REGBYTES(t0)
	LOAD	x10, 10 * REGBYTES(t0)
	LOAD	x11, 11 * REGBYTES(t0)
	LOAD	x12, 12 * REGBYTES(t0)
	LOAD	x13, 13 * REGBYTES(t0)
	LOAD	x14, 14 * REGBYTES(t0)
	LOAD	x15, 15 * REGBYTES(t0)
	LOAD	x16, 16 * REGBYTES(t0)
	LOAD	x17, 17 * REGBYTES(t0)
	LOAD	x18, 18 * REGBYTES(t0)
	LOAD	x19, 19 * REGBYTES(t0)
	LOAD	x20, 20 * REGBYTES(t0)
	LOAD	x21, 21 * REGBYTES(t0)
	LOAD	x22, 22 * REGBYTES(t0)
	LOAD	x23, 23 * REGBYTES(t0)
	LOAD	x24, 24 * REGBYTES(t0)
	LOAD	x25, 25 * REGBYTES(t0)
	LOAD	x26, 26 * REGBYTES(t0)
	LOAD	x27, 27 * REGBYTES(t0)
	LOAD	x28, 28 * REGBYTES(t0)
	LOAD	x29, 29 * REGBYTES(t0)
	LOAD	x30, 30 * REGBYTES(t0)
	LOAD	x31, 31 * REGBYTES(t0)
	addi	t0, t0, REGBYTES * (X_reg)
#endif /* __gracefulExit */
	ret
vPortEndScheduler_end:
  .size vPortEndScheduler, vPortEndScheduler_end-vPortEndScheduler
	nop
