/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
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
#include <arch_helpers.h>
#include <debug.h>
#include <mcucfg.h>
#include <mmio.h>
#include <mt_cpuxgpt.h>
#include <platform_def.h>

#define CPUXGPT_BASE	0x10200000
#define INDEX_BASE	(CPUXGPT_BASE + 0x0674)
#define CTL_BASE	(CPUXGPT_BASE + 0x0670)

uint64_t normal_time_base;
uint64_t atf_time_base;

static unsigned int __read_cpuxgpt(unsigned int reg_index)
{
	unsigned int value = 0;

	mmio_write_32(INDEX_BASE, reg_index);
	value = mmio_read_32(CTL_BASE);
	return value;
}

static void __write_cpuxgpt(unsigned int reg_index, unsigned int value)
{
	mmio_write_32(INDEX_BASE, reg_index);
	mmio_write_32(CTL_BASE, value);
}

static void set_cpuxgpt_clk(unsigned int div)
{
	unsigned int tmp = 0;

	tmp = __read_cpuxgpt(INDEX_CTL_REG);
	tmp &= CLK_DIV_MASK;
	tmp |= div;
	__write_cpuxgpt(INDEX_CTL_REG, tmp);
}

static void enable_cpuxgpt(void)
{
	unsigned int tmp = 0;

	tmp = __read_cpuxgpt(INDEX_CTL_REG);
	tmp |= EN_CPUXGPT;
	__write_cpuxgpt(INDEX_CTL_REG, tmp);

	INFO("CPUxGPT reg(%x)\n", __read_cpuxgpt(INDEX_CTL_REG));
}

void setup_syscnt(void)
{
	/* set cpuxgpt as free run mode */
	/* set cpuxgpt to 13Mhz clock */
	set_cpuxgpt_clk(CLK_DIV2);
	/* enable cpuxgpt */
	enable_cpuxgpt();
}

void sched_clock_init(uint64_t normal_base, uint64_t atf_base)
{
	normal_time_base += normal_base;
	atf_time_base = atf_base;
}

uint64_t sched_clock(void)
{
	uint64_t cval;

	cval = (((read_cntpct_el0() - atf_time_base) * 1000) / SYS_COUNTER_FREQ_IN_MHZ)
		- normal_time_base;
	return cval;
}

static void cpuxgpt_set_init_cnt(unsigned int countH, unsigned int countL)
{
#if 0
	__write_cpuxgpt(INDEX_CNT_H_INIT, countH);
	/* update count when countL programmed */
	__write_cpuxgpt(INDEX_CNT_L_INIT, countL);
#endif
}

void generic_timer_backup(void)
{
	uint64_t cval;

	cval = read_cntpct_el0();
	cpuxgpt_set_init_cnt((uint32_t)(cval >> 32),
			       (uint32_t)(cval & 0xffffffff));
}

/*
 * Return: 0 - Trying to disable the CPUXGPT control bit, and not allowed to disable it.
 * Return: 1 - reg_addr is not realted to disable the control bit.
 */
uint32_t check_cpuxgpt_write_permission(uint32_t reg_addr, uint32_t reg_value)
{
	uint32_t idx;
	uint32_t ctl_val;

	if (reg_addr == CTL_BASE) {
		idx = mmio_read_32(INDEX_BASE);

		/* idx 0: CPUXGPT system control */
		if (idx == 0) {
			ctl_val = mmio_read_32(CTL_BASE);
			if (ctl_val & 1) {
				/* if enable bit already set, then bit 0 is not allow to set as 0 */
				if (!(reg_value & 1))
					return 0;
			}
		}
	}
	return 1;
}
