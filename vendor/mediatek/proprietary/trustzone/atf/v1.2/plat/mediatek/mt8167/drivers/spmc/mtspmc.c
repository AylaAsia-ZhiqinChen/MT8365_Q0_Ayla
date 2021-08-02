/*
 * Copyright (c) 2015,  ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms,  with or without
 * modification,  are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,  this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
 * CONSEQUENTIAL DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA,  OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN
 * CONTRACT,  STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <console.h>
#include <debug.h>
#include <delay_timer.h>
#include <mcucfg.h>
#include <mmio.h>
#include <mtcmos.h>
#include <mtspmc.h>
#include <plat_private.h>
#include <platform.h>
#include <platform_def.h>
#include <psci.h>
#include <spm.h>

#define RETRY_TIME_USEC   (10)
#define RETRY_TIME_CURR   (1)
#define DELAY_TIME_MEASURE (1)//(20000)//(1)

#define DVTFlowCtrl				 (0x10200070)

#define mcu_spm_read(addr)		spm_read(addr)
#define mcu_spm_write(addr,  val)	spm_write(addr,  val)

unsigned int cpu_bitmask = 1;
char little_on = 0x1; /* [7:0] = core7~core0,  core 0 is power-on in defualt */

unsigned long read_cpuectlr(void);
void write_cpuectlr(unsigned long);

#if 0
static void little_spmc_info(void)
{
PRINTF_SPMC("SPMC_MP0_CPUTOP_PWR_CON_0x%x=0x%x\n", SPMC_MP0_CPUTOP_PWR_CON, mmio_read_32(SPMC_MP0_CPUTOP_PWR_CON));
PRINTF_SPMC("SPMC_MP0_CPU0_PWR_CON_0x%x=0x%x\n", SPMC_MP0_CPU0_PWR_CON, mmio_read_32(SPMC_MP0_CPU0_PWR_CON));
PRINTF_SPMC("SPMC_MP0_CPU1_PWR_CON_0x%x=0x%x\n", SPMC_MP0_CPU1_PWR_CON, mmio_read_32(SPMC_MP0_CPU1_PWR_CON));
PRINTF_SPMC("SPMC_MP0_CPU2_PWR_CON_0x%x=0x%x\n", SPMC_MP0_CPU2_PWR_CON, mmio_read_32(SPMC_MP0_CPU2_PWR_CON));
PRINTF_SPMC("SPMC_MP0_CPU3_PWR_CON_0x%x=0x%x\n", SPMC_MP0_CPU3_PWR_CON, mmio_read_32(SPMC_MP0_CPU3_PWR_CON));

PRINTF_SPMC("CPUSYS0_CPU0_SPMC_CTL_0x%x=0x%x\n", CPUSYS0_CPU0_SPMC_CTL, mmio_read_32(CPUSYS0_CPU0_SPMC_CTL));
PRINTF_SPMC("CPUSYS0_CPU1_SPMC_CTL_0x%x=0x%x\n", CPUSYS0_CPU1_SPMC_CTL, mmio_read_32(CPUSYS0_CPU1_SPMC_CTL));
PRINTF_SPMC("CPUSYS0_CPU2_SPMC_CTL_0x%x=0x%x\n", CPUSYS0_CPU2_SPMC_CTL, mmio_read_32(CPUSYS0_CPU2_SPMC_CTL));
PRINTF_SPMC("CPUSYS0_CPU3_SPMC_CTL_0x%x=0x%x\n", CPUSYS0_CPU3_SPMC_CTL, mmio_read_32(CPUSYS0_CPU3_SPMC_CTL));
 }
#endif
/*
static void little_wfi_wfe_status(void)
{
unsigned int tmp;

	tmp = mmio_read_32(MP0_CA7_MISC_CONFIG);
	PRINTF_SPMC("MP0_CA7_MISC_CONFIG%x=0x%x\n",  MP0_CA7_MISC_CONFIG,  tmp);
	PRINTF_SPMC("cor3~0_WFE=%d %d %d %d\n",  (tmp&MP0_CPU3_STANDBYWFE)>>23,  (tmp&MP0_CPU2_STANDBYWFE)>>22,
		(tmp&MP0_CPU1_STANDBYWFE)>>21,  (tmp&MP0_CPU0_STANDBYWFE)>>20);
	tmp = mmio_read_32(MP1_CA7_MISC_CONFIG);
	PRINTF_SPMC("MP1_CA7_MISC_CONFIG%x=0x%x\n",  MP1_CA7_MISC_CONFIG,  tmp);
	PRINTF_SPMC("cor7~4_WFE=%d %d %d %d\n",  (tmp&MP1_CPU3_STANDBYWFE)>>23,  (tmp&MP1_CPU2_STANDBYWFE)>>22,
		(tmp&MP1_CPU1_STANDBYWFE)>>21,  (tmp&MP1_CPU0_STANDBYWFE)>>20);
}
*/
void set_cpu_retention_control(int retention_value)
{
	uint64_t cpuectlr;

	cpuectlr = read_cpuectlr();
	cpuectlr = ((cpuectlr >> 3) << 3);
	cpuectlr |= retention_value;
	write_cpuectlr(cpuectlr);
}


/*
 * SPMC Mode
 */

/*
GG
	Set bypass_cpu_spmc_mode = 0
	Wait 200ns
	FOR ( each cluster n)
	FOR (each core m)
			Set mp<n>_spmc_resetpwron_config_cpu<m> = 0
			Set mp<n>_spmc_pwr_rst_cpu<m> = 0
		ENDFOR
		Set mp<n>_spmc_resetpwron_config_cputop = 0
		Set mp<n>_spmc_pwr_rst_cputop = 0
		Set mp<n>_spmc_pwr_clk_dis_cputop = 0
	ENDFOR
*/

int spmc_init(void)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	mmio_write_32(SPM_POWERON_CONFIG_SET,  (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	mmio_write_32(SPMC_BYPASS,  0x0);/* de-assert Bypass SPMC  0: SPMC mode  1: Legacy mode */
	/* udelay(200); */
	PRINTF_SPMC("[%s]change to SPMC mode !!!\n",  __func__);

/*		mmio_write_32(SPMC_MP0_CPU0_PWR_CON,  mmio_read_32(SPMC_MP0_CPU0_PWR_CON) & ~SPMC_PWR_ON_2ND);
		mmio_write_32(SPMC_MP0_CPU0_PWR_CON,  mmio_read_32(SPMC_MP0_CPU0_PWR_CON) & ~SPMC_PWR_RST_B);*/
		mmio_write_32(SPMC_MP0_CPU1_PWR_CON,  mmio_read_32(SPMC_MP0_CPU1_PWR_CON) & ~SPMC_PWR_ON_2ND);
		mmio_write_32(SPMC_MP0_CPU1_PWR_CON,  mmio_read_32(SPMC_MP0_CPU1_PWR_CON) & ~SPMC_PWR_RST_B);/*Polarity don't need to inverse*/
		mmio_write_32(SPMC_MP0_CPU2_PWR_CON,  mmio_read_32(SPMC_MP0_CPU2_PWR_CON) & ~SPMC_PWR_ON_2ND);
		mmio_write_32(SPMC_MP0_CPU2_PWR_CON,  mmio_read_32(SPMC_MP0_CPU2_PWR_CON) & ~SPMC_PWR_RST_B);/*Polarity don't need to inverse*/
		mmio_write_32(SPMC_MP0_CPU3_PWR_CON,  mmio_read_32(SPMC_MP0_CPU3_PWR_CON) & ~SPMC_PWR_ON_2ND);
		mmio_write_32(SPMC_MP0_CPU3_PWR_CON,  mmio_read_32(SPMC_MP0_CPU3_PWR_CON) & ~SPMC_PWR_RST_B);/*Polarity don't need to inverse*/

/*		mmio_write_32(SPMC_MP0_CPUTOP_PWR_CON,  mmio_read_32(SPMC_MP0_CPUTOP_PWR_CON) & ~SPMC_PWR_ON_2ND);
		mmio_write_32(SPMC_MP0_CPUTOP_PWR_CON,  mmio_read_32(SPMC_MP0_CPUTOP_PWR_CON) & ~SPMC_PWR_RST_B);
		mmio_write_32(SPMC_MP0_CPUTOP_CLK_DIS,  mmio_read_32(SPMC_MP0_CPUTOP_CLK_DIS) & ~SPMC_PWR_CLK_DIS);*/


	return err;
}

int spmc_cpu_corex_onoff(int linear_id, int state, int mode)
{
	int err = 0;
	unsigned int CPUSYSx_CPUx_SPMC_CTL, MPx_CPUx_PWR_CON, MPx_CPUx_STANDBYWFI, MPx_CPUx_PWR_STA_MASK;
#if SPMC_DVT
	unsigned int MPx_CA7_MISC_CONFIG, MPx_CPUx_STANDBYWFE;
#endif
	/* TINFO="enable SPM register control" */
	mmio_write_32(SPM_POWERON_CONFIG_SET,  (SPM_PROJECT_CODE << 16) | (0x1 << 0));
	PRINTF_SPMC(">>>>>>>> %s >>>>>>>>linear_id=%d state=%d mode=%d\n", __func__, linear_id, state, mode);
	MPx_CPUx_PWR_STA_MASK = (1U << 16);
	switch (linear_id) {
	case 0:
			CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU0_SPMC_CTL;
			MPx_CPUx_STANDBYWFI   = MP0_CPU0_STANDBYWFI;
			MPx_CPUx_PWR_CON	  = SPMC_MP0_CPU0_PWR_CON;
#if SPMC_DVT
			MPx_CA7_MISC_CONFIG = MP0_CA7_MISC_CONFIG;
			MPx_CPUx_STANDBYWFE   = MP0_CPU0_STANDBYWFE;
#endif
			break;
	case 1:
			CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU1_SPMC_CTL;
			MPx_CPUx_STANDBYWFI   = MP0_CPU1_STANDBYWFI;
			MPx_CPUx_PWR_CON	  = SPMC_MP0_CPU1_PWR_CON;
#if SPMC_DVT
			MPx_CA7_MISC_CONFIG   = MP0_CA7_MISC_CONFIG;
			MPx_CPUx_STANDBYWFE   = MP0_CPU1_STANDBYWFE;
#endif
			break;
	case 2:
			CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU2_SPMC_CTL;
			MPx_CPUx_STANDBYWFI   = MP0_CPU2_STANDBYWFI;
			MPx_CPUx_PWR_CON	  = SPMC_MP0_CPU2_PWR_CON;
#if SPMC_DVT
			MPx_CA7_MISC_CONFIG   = MP0_CA7_MISC_CONFIG;
			MPx_CPUx_STANDBYWFE   = MP0_CPU2_STANDBYWFE;
#endif
			break;
	case 3:
			CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU3_SPMC_CTL;
			MPx_CPUx_STANDBYWFI   = MP0_CPU3_STANDBYWFI;
			MPx_CPUx_PWR_CON	  = SPMC_MP0_CPU3_PWR_CON;
#if SPMC_DVT
			MPx_CA7_MISC_CONFIG   = MP0_CA7_MISC_CONFIG;
			MPx_CPUx_STANDBYWFE   = MP0_CPU3_STANDBYWFE;
#endif
			break;
	default:
			ERROR("%s() CPU%d not exists\n",  __func__,  (int)linear_id);
			assert(0);
	}

	PRINTF_SPMC("SPM_SLEEP_TIMER_STA_0x%x=0x%x\n", SPM_SLEEP_TIMER_STA, mmio_read_32(SPM_SLEEP_TIMER_STA));

	if (state  ==  STA_POWER_DOWN) {
		if (!(cpu_bitmask & (1 << linear_id))) {
			PRINTF_SPMC("core%d already turn off !!!\n", linear_id);
			return 0;
			}

		if (mode == MODE_AUTO_SHUT_OFF) {
			mmio_write_32(CPUSYSx_CPUx_SPMC_CTL, mmio_read_32(CPUSYSx_CPUx_SPMC_CTL) & ~cpu_sw_no_wait_for_q_channel);
			PRINTF_SPMC("auto_off_CPUSYSx_CPUx_SPMC_CTL_0x%x=0x%x\n", CPUSYSx_CPUx_SPMC_CTL, mmio_read_32(CPUSYSx_CPUx_SPMC_CTL));
			set_cpu_retention_control(1);
		} else {
			mmio_write_32(CPUSYSx_CPUx_SPMC_CTL, mmio_read_32(CPUSYSx_CPUx_SPMC_CTL) | cpu_sw_no_wait_for_q_channel);
			PRINTF_SPMC("HW_off_CPUSYSx_CPUx_SPMC_CTL_0x%x=0x%x\n", CPUSYSx_CPUx_SPMC_CTL, mmio_read_32(CPUSYSx_CPUx_SPMC_CTL));
			#if SPMC_DVT	/* wait WFE */
			while (!(mmio_read_32(MPx_CA7_MISC_CONFIG) & MPx_CPUx_STANDBYWFE))
				;
			PRINTF_SPMC("MPx_CA7_MISC_CONFIG%x=0x%x\n", MPx_CA7_MISC_CONFIG, mmio_read_32(MPx_CA7_MISC_CONFIG));
			PRINTF_SPMC("SPM_SLEEP_TIMER_STA=0x%x MPx_CPUx_STANDBYWFI=0x%x\n", SPM_SLEEP_TIMER_STA , MPx_CPUx_STANDBYWFI);
			#else  /* wait WFI */
			while (!(mmio_read_32(SPM_SLEEP_TIMER_STA) & MPx_CPUx_STANDBYWFI))
				;
			PRINTF_SPMC("SPM_SLEEP_TIMER_STA_0x%x=0x%x\n", SPM_SLEEP_TIMER_STA, mmio_read_32(SPM_SLEEP_TIMER_STA));
			#endif
		}
		mmio_write_32(MPx_CPUx_PWR_CON,  mmio_read_32(MPx_CPUx_PWR_CON) & ~SPMC_PWR_ON);
		PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n", MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		if (mode == MODE_SPMC_HW) {
			while (mmio_read_32(MPx_CPUx_PWR_CON) & MPx_CPUx_PWR_STA_MASK)
				;
			PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n", MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
			}
		cpu_bitmask &= ~(1 << linear_id);
		PRINTF_SPMC("cpu_bitmask=0x%x\n", cpu_bitmask);
	} else {
		mmio_write_32(MPx_CPUx_PWR_CON,  mmio_read_32(MPx_CPUx_PWR_CON) | SPMC_PWR_ON);
		PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n", MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		while ((mmio_read_32(MPx_CPUx_PWR_CON) & MPx_CPUx_PWR_STA_MASK) != MPx_CPUx_PWR_STA_MASK)
			;
		PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n", MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		cpu_bitmask |= (1 << linear_id);
		PRINTF_SPMC("cpu_bitmask=0x%x\n", cpu_bitmask);
	}
	return err;
}

