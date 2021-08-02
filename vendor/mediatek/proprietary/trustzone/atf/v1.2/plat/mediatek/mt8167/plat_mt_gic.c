/*
 * Copyright (c) 2013-2015, ARM Limited and Contributors. All rights reserved.
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
#include <gic_common.h>
#include <gicv2.h>
#include <interrupt_mgmt.h>
#include <platform.h>
#include <bl_common.h>
#include <mt8167_def.h>
#include <platform_def.h>
#include <mmio.h>
#include <debug.h>

const unsigned int mt_irq_sec_array[] = {
	MT_IRQ_SEC_SGI_0,
	MT_IRQ_SEC_SGI_1,
	MT_IRQ_SEC_SGI_2,
	MT_IRQ_SEC_SGI_3,
	MT_IRQ_SEC_SGI_4,
	MT_IRQ_SEC_SGI_5,
	MT_IRQ_SEC_SGI_6,
	MT_IRQ_SEC_SGI_7,
	WDT_IRQ_BIT_ID,
};

gicv2_driver_data_t mt_gic_data = {
	.gicd_base = BASE_GICD_BASE,
	.gicc_base = BASE_GICC_BASE,
	.g0_interrupt_num = ARRAY_SIZE(mt_irq_sec_array),
	.g0_interrupt_array = mt_irq_sec_array,
};

/******************************************************************************
 * ARM common helper to initialize the GICv2 only driver.
 *****************************************************************************/
void plat_mt_gic_driver_init(void)
{
	gicv2_driver_init(&mt_gic_data);
}

void plat_mt_gic_init(void)
{
	int idx;

	gicv2_distif_init();
	gicv2_pcpu_distif_init();
	gicv2_cpuif_enable();

	/* set WDT interrupt as falling edge trigger */
        gicd_write_icfgr(BASE_GICD_BASE, WDT_IRQ_BIT_ID,
			0x2<<((WDT_IRQ_BIT_ID%16)*2));

	/* set pol control as non-secure */
	for (idx=0;idx<INT_POL_SECCTL_NUM;idx++)
		mmio_write_32(INT_POL_SECCTL0+idx*4, 0);
}

/* from gicv2_private.h */
static inline unsigned int gicc_read_ctlr(uintptr_t base)
{
	return mmio_read_32(base + GICC_CTLR);
}

static inline void gicc_write_ctlr(uintptr_t base, unsigned int val)
{
	mmio_write_32(base + GICC_CTLR, val);
}

static inline void gicd_write_sgir(uintptr_t base, unsigned int val)
{
	mmio_write_32(base + GICD_SGIR, val);
}

void irq_raise_softirq(unsigned int map, unsigned int irq)
{
    int satt;

    satt = 1 << 15;

    if(plat_ic_get_interrupt_type(irq) == INTR_TYPE_S_EL1)
    {
        satt = 0;
    }

    gicd_write_sgir(BASE_GICD_BASE, (map << 16) | satt | irq);

    dsb();
}

#ifndef MAX_GIC_NR
#define MAX_GIC_NR			(1)
#endif
#define MAX_RDIST_NR			(64)
#define DIV_ROUND_UP(n,d)		(((n) + (d) - 1) / (d))

/* For saving ATF size, we reduce 1020 -> 320 */
struct gic_chip_data {
	unsigned int saved_spi_enable[DIV_ROUND_UP(320, 32)];
	unsigned int saved_spi_conf[DIV_ROUND_UP(320, 16)];
	unsigned int saved_spi_target[DIV_ROUND_UP(320, 4)];
	unsigned int saved_spi_group[DIV_ROUND_UP(320, 32)];
};
static struct gic_chip_data gic_data[MAX_GIC_NR];

/* TODO: check all registers to save */
void mt_gic_dist_save(void)
{
	unsigned int gic_irqs;
	unsigned int dist_base;
	int i;

	/* TODO: pending bit MUST added */
	dist_base = BASE_GICD_BASE;

	gic_irqs = 32 * ((gicd_read_typer(dist_base) & TYPER_IT_LINES_NO_MASK) + 1);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 16); i++)
		gic_data[0].saved_spi_conf[i] =
			mmio_read_32(dist_base + GICD_ICFGR + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 4); i++)
		gic_data[0].saved_spi_target[i] =
			mmio_read_32(dist_base + GICD_ITARGETSR + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		gic_data[0].saved_spi_enable[i] =
			mmio_read_32(dist_base + GICD_ISENABLER + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		gic_data[0].saved_spi_group[i] =
			mmio_read_32(dist_base + GICD_IGROUPR + i * 4);
}

/* TODO: check all registers to restore */
void mt_gic_dist_restore(void)
{
	unsigned int gic_irqs;
	unsigned int dist_base;
	unsigned int ctlr;
	int i;

	dist_base = BASE_GICD_BASE;

	gic_irqs = 32 * ((gicd_read_typer(dist_base) & TYPER_IT_LINES_NO_MASK) + 1);

	/* Disable the distributor before going further */
	ctlr = gicd_read_ctlr(dist_base);
	ctlr &= ~(CTLR_ENABLE_G0_BIT | CTLR_ENABLE_G1_BIT);
	gicd_write_ctlr(dist_base, ctlr);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 16); i++)
		mmio_write_32(dist_base + GICD_ICFGR + i * 4, gic_data[0].saved_spi_conf[i]);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 4); i++)
		mmio_write_32(dist_base + GICD_ITARGETSR + i * 4, gic_data[0].saved_spi_target[i]);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		mmio_write_32(dist_base + GICD_ISENABLER + i * 4, gic_data[0].saved_spi_enable[i]);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		mmio_write_32(dist_base + GICD_IGROUPR + i * 4, gic_data[0].saved_spi_group[i]);

	gicd_write_ctlr(dist_base, ctlr | CTLR_ENABLE_G0_BIT | CTLR_ENABLE_G1_BIT);
}

void mt_gic_cpuif_setup(void)
{
	unsigned int val = gicc_read_ctlr(BASE_GICC_BASE);

	val |= CTLR_ENABLE_G1_BIT;
	gicc_write_ctlr(BASE_GICC_BASE, val);
}
