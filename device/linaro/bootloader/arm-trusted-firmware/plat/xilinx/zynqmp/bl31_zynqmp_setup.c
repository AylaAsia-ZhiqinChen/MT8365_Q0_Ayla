/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <bl31.h>
#include <bl_common.h>
#include <console.h>
#include <debug.h>
#include <errno.h>
#include <plat_arm.h>
#include <platform.h>
#include "zynqmp_private.h"

#define BL31_END (unsigned long)(&__BL31_END__)

static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;

/*
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 */
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	assert(sec_state_is_valid(type));

	if (type == NON_SECURE)
		return &bl33_image_ep_info;

	return &bl32_image_ep_info;
}

/*
 * Perform any BL31 specific platform actions. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables.
 */
void bl31_early_platform_setup(bl31_params_t *from_bl2,
			       void *plat_params_from_bl2)
{
	/* Initialize the console to provide early debug support */
	console_init(ZYNQMP_UART_BASE, zynqmp_get_uart_clk(),
		     ZYNQMP_UART_BAUDRATE);

	/* Initialize the platform config for future decision making */
	zynqmp_config_setup();

	/* There are no parameters from BL2 if BL31 is a reset vector */
	assert(from_bl2 == NULL);
	assert(plat_params_from_bl2 == NULL);

	/*
	 * Do initial security configuration to allow DRAM/device access. On
	 * Base ZYNQMP only DRAM security is programmable (via TrustZone), but
	 * other platforms might have more programmable security devices
	 * present.
	 */

	/* Populate common information for BL32 and BL33 */
	SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	SET_PARAM_HEAD(&bl33_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);

	if (zynqmp_get_bootmode() == ZYNQMP_BOOTMODE_JTAG) {
		/* use build time defaults in JTAG boot mode */
		bl32_image_ep_info.pc = BL32_BASE;
		bl32_image_ep_info.spsr = arm_get_spsr_for_bl32_entry();
		bl33_image_ep_info.pc = plat_get_ns_image_entrypoint();
		bl33_image_ep_info.spsr = SPSR_64(MODE_EL2, MODE_SP_ELX,
						  DISABLE_ALL_EXCEPTIONS);
	} else {
		/* use parameters from FSBL */
		fsbl_atf_handover(&bl32_image_ep_info, &bl33_image_ep_info);
	}

	NOTICE("BL31: Secure code at 0x%lx\n", bl32_image_ep_info.pc);
	NOTICE("BL31: Non secure code at 0x%lx\n", bl33_image_ep_info.pc);
}

/* Enable the test setup */
#ifndef ZYNQMP_TESTING
static void zynqmp_testing_setup(void) { }
#else
static void zynqmp_testing_setup(void)
{
	uint32_t actlr_el3, actlr_el2;

	/* Enable CPU ACTLR AND L2ACTLR RW access from non-secure world */
	actlr_el3 = read_actlr_el3();
	actlr_el2 = read_actlr_el2();

	actlr_el3 |= ACTLR_EL3_L2ACTLR_BIT | ACTLR_EL3_CPUACTLR_BIT;
	actlr_el2 |= ACTLR_EL3_L2ACTLR_BIT | ACTLR_EL3_CPUACTLR_BIT;
	write_actlr_el3(actlr_el3);
	write_actlr_el2(actlr_el2);
}
#endif

void bl31_platform_setup(void)
{
	/* Initialize the gic cpu and distributor interfaces */
	plat_arm_gic_driver_init();
	plat_arm_gic_init();
	zynqmp_testing_setup();
}

void bl31_plat_runtime_setup(void)
{
}

/*
 * Perform the very early platform specific architectural setup here.
 */
void bl31_plat_arch_setup(void)
{
	plat_arm_interconnect_init();
	plat_arm_interconnect_enter_coherency();

	arm_setup_page_tables(BL31_BASE,
			      BL31_END - BL31_BASE,
			      BL_CODE_BASE,
			      BL_CODE_END,
			      BL_RO_DATA_BASE,
			      BL_RO_DATA_END,
			      BL_COHERENT_RAM_BASE,
			      BL_COHERENT_RAM_END);
	enable_mmu_el3(0);
}
