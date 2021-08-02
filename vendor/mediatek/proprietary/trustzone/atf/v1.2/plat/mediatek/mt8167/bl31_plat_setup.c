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
#include <arm_gic.h>
#include <assert.h>
#include <bl_common.h>
#include <console.h>
#include <context_mgmt.h>
#include <debug.h>
#include <mcucfg.h>
#include <mmio.h>
#include <mtcmos.h>
#include <plat_private.h>
#include <platform.h>
#include <spm.h>
#include "mtk_sip_svc.h"
#include <mt_common.h>
#include <string.h>
#include <log.h>
#include <xlat_tables.h>
#include <mtspmc.h>
#include <l2c.h>

/*******************************************************************************
 * Declarations of linker defined symbols which will help us find the layout
 * of trusted SRAM
 ******************************************************************************/
unsigned long __RO_START__;
unsigned long __RO_END__;

unsigned long __COHERENT_RAM_START__;
unsigned long __COHERENT_RAM_END__;

/*
 * The next 2 constants identify the extents of the code & RO data region.
 * These addresses are used by the MMU setup code and therefore they must be
 * page-aligned.  It is the responsibility of the linker script to ensure that
 * __RO_START__ and __RO_END__ linker symbols refer to page-aligned addresses.
 */
#define BL31_RO_BASE (unsigned long)(&__RO_START__)
#define BL31_RO_LIMIT (unsigned long)(&__RO_END__)

/*
 * The next 2 constants identify the extents of the coherent memory region.
 * These addresses are used by the MMU setup code and therefore they must be
 * page-aligned.  It is the responsibility of the linker script to ensure that
 * __COHERENT_RAM_START__ and __COHERENT_RAM_END__ linker symbols
 * refer to page-aligned addresses.
 */
#define BL31_COHERENT_RAM_BASE (unsigned long)(&__COHERENT_RAM_START__)
#define BL31_COHERENT_RAM_LIMIT (unsigned long)(&__COHERENT_RAM_END__)

/*
 * Placeholder variables for copying the arguments that have been passed to
 * BL3-1 from BL2.
 */
static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;
static struct kernel_info k_info;
static unsigned int gboot_reason;
static unsigned int gis_abnormal_boot;

atf_arg_t gteearg;

static void platform_setup_cpu(void)
{
	/* set LITTLE cores arm64 boot mode */
	mmio_setbits_32((uintptr_t)&mt8167_mcucfg->mp0_rv_addr[0].rv_addr_hw,
		MP0_CPUCFG_64BIT);
#if CONFIG_SPMC_MODE == 1
	spmc_init();
#endif
}

/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info;

	next_image_info = (type == NON_SECURE) ? &bl33_image_ep_info : &bl32_image_ep_info;

	/* None of the images on this platform can have 0x0 as the entrypoint */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

/*******************************************************************************
 * Perform any BL3-1 early platform setup. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables.
 * BL2 has flushed this information to memory, so we are guaranteed to pick up
 * good data.
 ******************************************************************************/
void bl31_early_platform_setup(bl31_params_t *from_bl2,
			       void *plat_params_from_bl2)
{
	mtk_bl_param_t *pmtk_bl_param_t = (mtk_bl_param_t *)from_bl2;
	atf_arg_t_ptr teearg;
	struct boot_tag *pboot_tag;

	config_L2_size();

	/* preloader is in 32 bit state, high 32bits of 64 bit GP registers are UNKNOWN if CPU change to 64 bit state
	   So we need to clear high 32bit, which may be random value*/
	pmtk_bl_param_t = (mtk_bl_param_t *)((uint64_t)pmtk_bl_param_t & 0x00000000ffffffff);
	gis_abnormal_boot = (unsigned int)((uint64_t)plat_params_from_bl2 & 0x00000000ffffffff);
	pboot_tag = (struct boot_tag *)(pmtk_bl_param_t->bootarg_loc);
	teearg = (atf_arg_t_ptr)pmtk_bl_param_t->tee_info_addr;

	console_init(teearg->atf_log_port, MT8167_UART_CLOCK, MT8167_BAUDRATE);
	memcpy((void *)&gteearg, (void *)teearg, sizeof(atf_arg_t));

	/* get boot_tag addres for LK */
	assert(pboot_tag->hdr.tag == BOOT_TAG_BOOT_REASON);
	gboot_reason = pboot_tag->u.boot_reason.boot_reason;
	NOTICE("Boot reason %x\n", gboot_reason);

	/* init system counter in ATF */
/* FIXME: syscnt porting */
/*
	setup_syscnt();
*/

	VERBOSE("bl31_setup\n");

	/* Populate entry point information for BL3-2 and BL3-3 */
	SET_PARAM_HEAD(&bl32_image_ep_info,
				PARAM_EP,
				VERSION_1,
				0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	bl32_image_ep_info.pc = teearg->tee_entry;
	bl32_image_ep_info.spsr = plat_get_spsr_for_bl32_entry();

	SET_PARAM_HEAD(&bl33_image_ep_info,
				PARAM_EP,
				VERSION_1,
				0);
	/*
	 * Tell BL3-1 where the non-trusted software image
	 * is located and the entry state information
	 */
	bl33_image_ep_info.pc = pmtk_bl_param_t->bl33_start_addr;
	bl33_image_ep_info.spsr = plat_get_spsr_for_bl33_entry();
	bl33_image_ep_info.args.arg4= pmtk_bl_param_t->bootarg_loc;
	bl33_image_ep_info.args.arg5= pmtk_bl_param_t->bootarg_size;
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);
}

/*******************************************************************************
 * Platform code used to process boot reason
 ******************************************************************************/
void plat_boot_process(unsigned int boot_reason)
{
	return;
}

/*******************************************************************************
 * Perform any BL3-1 platform setup code
 ******************************************************************************/
void bl31_platform_setup(void)
{
	VERBOSE("%s()\n", __FUNCTION__);

	plat_boot_process(gboot_reason);
	platform_setup_cpu();
	plat_delay_timer_init();

	/* Initialize the gic cpu and distributor interfaces */
	plat_mt_gic_driver_init();
	plat_mt_gic_init();

	/* Topologies are best known to the platform. */
	mt_setup_topology();

#if ENABLE_SPM
	/* Initialize spm at boot time */
	spm_boot_init();
#endif
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
	atf_arg_t_ptr teearg = &gteearg;

	plat_cci_init();
	plat_cci_enable();

	if(teearg->atf_log_buf_size !=0 ) {
		INFO("mmap atf buffer : 0x%lx, 0x%x\n\r",
			teearg->atf_log_buf_start,
			teearg->atf_log_buf_size);
		mmap_add_region((teearg->atf_log_buf_start & ~(PAGE_SIZE_2MB_MASK)),
				(teearg->atf_log_buf_start & ~(PAGE_SIZE_2MB_MASK)),
				PAGE_SIZE_2MB,
				MT_DEVICE | MT_RW | MT_NS);
		INFO("mmap atf buffer (force 2MB aligned): 0x%lx, 0x%x\n\r",
			(teearg->atf_log_buf_start & ~(PAGE_SIZE_2MB_MASK)), PAGE_SIZE_2MB);
	}

	plat_configure_mmu_el3(BL31_RO_BASE,
			       (BL31_COHERENT_RAM_LIMIT - BL31_RO_BASE),
			       BL31_RO_BASE,
			       BL31_RO_LIMIT,
			       BL31_COHERENT_RAM_BASE,
			       BL31_COHERENT_RAM_LIMIT);

	/* Initialize for ATF log buffer */
	if(teearg->atf_log_buf_size != 0)
	{
		teearg->atf_aee_debug_buf_size = ATF_AEE_BUFFER_SIZE;
		teearg->atf_aee_debug_buf_start = teearg->atf_log_buf_start +
							teearg->atf_log_buf_size -
							ATF_AEE_BUFFER_SIZE;
		mt_log_setup(teearg->atf_log_buf_start, teearg->atf_log_buf_size,
				teearg->atf_aee_debug_buf_size, gis_abnormal_boot);
		INFO("ATF log service is registered (0x%lx, aee:0x%lx)\n",
			teearg->atf_log_buf_start,
			teearg->atf_aee_debug_buf_start);
	}
	else
	{
		teearg->atf_aee_debug_buf_size = 0;
		teearg->atf_aee_debug_buf_start = 0;
	}

	/* Platform code before bl31_main */
	/* Show to ATF log buffer & UART */
	NOTICE("BL3-1: %s\n", version_string);
	NOTICE("BL3-1: %s\n", build_message);
}

void save_kernel_info(uint64_t pc, uint64_t r0, uint64_t r1,
                                                       uint64_t k32_64)
{
    k_info.k32_64 = k32_64;
    k_info.pc=pc;

    if ( LINUX_KERNEL_32 == k32_64 ) {
        /* for 32 bits kernel */
        k_info.r0=0;
        k_info.r1=r0;   /* machtype */
        k_info.r2=r1;   /* tags */
    } else {
        /* for 64 bits kernel */
        k_info.r0=r0;
        k_info.r1=r1;
    }
}

void set_kernel_k32_64(uint64_t k32_64)
{
    k_info.k32_64 = k32_64;
}

uint64_t get_kernel_k32_64(void)
{
    return k_info.k32_64;
}

uint64_t get_kernel_info_pc(void)
{
    return k_info.pc;
}

uint64_t get_kernel_info_r0(void)
{
    return k_info.r0;
}

uint64_t get_kernel_info_r1(void)
{
    return k_info.r1;
}
uint64_t get_kernel_info_r2(void)
{
    return k_info.r2;
}

entry_point_info_t *bl31_plat_get_next_kernel64_ep_info(void)
{
    entry_point_info_t *next_image_info;
    unsigned long el_status;
    unsigned int mode;

    el_status = 0;
    mode = 0;

    /* Kernel image is always non-secured */
    next_image_info = &bl33_image_ep_info;

    /* Figure out what mode we enter the non-secure world in */
    el_status = read_id_aa64pfr0_el1() >> ID_AA64PFR0_EL2_SHIFT;
    el_status &= ID_AA64PFR0_ELX_MASK;
    
    if (el_status){
        INFO("Kernel_EL2\n");
    	mode = MODE_EL2;
    } else{
        INFO("Kernel_EL1\n");
    	mode = MODE_EL1;
    }

    INFO("Kernel is 64Bit\n");
    next_image_info->spsr = SPSR_64(mode, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
    next_image_info->pc = get_kernel_info_pc();
    next_image_info->args.arg0=get_kernel_info_r0();
    next_image_info->args.arg1=get_kernel_info_r1();

    INFO("pc=0x%lx, r0=0x%lx, r1=0x%lx\n",
           next_image_info->pc,
           next_image_info->args.arg0,
           next_image_info->args.arg1);


    SET_SECURITY_STATE(next_image_info->h.attr, NON_SECURE);

    /* None of the images on this platform can have 0x0 as the entrypoint */
    if (next_image_info->pc)
        return next_image_info;
    else
        return NULL;
}

entry_point_info_t *bl31_plat_get_next_kernel32_ep_info(void)
{
    entry_point_info_t *next_image_info;
    unsigned int mode;

    mode = 0;

    /* Kernel image is always non-secured */
    next_image_info = &bl33_image_ep_info;

    /* Figure out what mode we enter the non-secure world in */
    mode = MODE32_hyp;
    /*
     * TODO: Consider the possibility of specifying the SPSR in
     * the FIP ToC and allowing the platform to have a say as
     * well.
     */

    INFO("Kernel is 32Bit\n");
    next_image_info->spsr = SPSR_MODE32 (mode, SPSR_T_ARM, SPSR_E_LITTLE,
                            (DAIF_FIQ_BIT | DAIF_IRQ_BIT | DAIF_ABT_BIT));
    next_image_info->pc = get_kernel_info_pc();
    next_image_info->args.arg0=get_kernel_info_r0();
    next_image_info->args.arg1=get_kernel_info_r1();
    next_image_info->args.arg2=get_kernel_info_r2();

    INFO("pc=0x%lx, r0=0x%lx, r1=0x%lx, r2=0x%lx\n",
           next_image_info->pc,
           next_image_info->args.arg0,
           next_image_info->args.arg1,
           next_image_info->args.arg2);


    SET_SECURITY_STATE(next_image_info->h.attr, NON_SECURE);

    /* None of the images on this platform can have 0x0 as the entrypoint */
    if (next_image_info->pc)
        return next_image_info;
    else
        return NULL;
}

void bl31_prepare_kernel_entry(uint64_t k32_64)
{
    entry_point_info_t *next_image_info;
    uint32_t image_type;

    /* Determine which image to execute next */
    image_type = NON_SECURE; //bl31_get_next_image_type();

	mtk_lk_stage = 0;

    /* Program EL3 registers to enable entry into the next EL */

    if (0 == k32_64) {
        next_image_info = bl31_plat_get_next_kernel32_ep_info();
    } else {

        next_image_info = bl31_plat_get_next_kernel64_ep_info();
    }
    assert(next_image_info);
    assert(image_type == GET_SECURITY_STATE(next_image_info->h.attr));

    INFO("BL3-1: Preparing for EL3 exit to %s world, Kernel\n",
        (image_type == SECURE) ? "secure" : "normal");
    INFO("BL3-1: Next image address = 0x%llx\n",
        (unsigned long long) next_image_info->pc);
    INFO("BL3-1: Next image spsr = 0x%x\n", next_image_info->spsr);
    cm_init_my_context(next_image_info);
    cm_prepare_el3_exit(image_type);
}
