/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
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


/*******************************************************************************
 * This is the Secure Payload Dispatcher (SPD). The dispatcher is meant to be a
 * plug-in component to the Secure Monitor, registered as a runtime service. The
 * SPD is expected to be a functional extension of the Secure Payload (SP) that
 * executes in Secure EL1. The Secure Monitor will delegate all SMCs targeting
 * the Trusted OS/Applications range to the dispatcher. The SPD will either
 * handle the request locally or delegate it to the Secure Payload. It is also
 * responsible for initialising and maintaining communication with the SP.
 ******************************************************************************/
#include <arch_helpers.h>
#include <assert.h>
#include <bl_common.h>
#include <bl31.h>
#include <context_mgmt.h>
#include <debug.h>
#include <errno.h>
#include <platform.h>
#include <runtime_svc.h>
#include <stddef.h>
#include <tsp.h>
#include <uuid.h>
#include "fiqd_private.h"
#include <platform_def.h>
#include <fiq_smp_call.h>

/*******************************************************************************
 * Array to keep track of per-cpu Secure Payload state
 ******************************************************************************/
fiq_context_t fiqd_sp_context[FIQD_CORE_COUNT];


/* TSP UID */
DEFINE_SVC_UUID(fiq_uuid,
		0x5b3056a0, 0x3291, 0x427b, 0x98, 0x11,
		0x71, 0x68, 0xca, 0x50, 0xf3, 0xfa);

int32_t fiqd_init(unsigned int, unsigned int);


/*******************************************************************************
 * This function is the handler registered for S-EL1 interrupts by the TSPD. It
 * validates the interrupt and upon success arranges entry into the TSP at
 * 'fiq_fiq_entry()' for handling the interrupt.
 ******************************************************************************/
static uint64_t fiqd_sel1_interrupt_handler(uint32_t id,
					    uint32_t flags,
					    void *handle,
					    void *cookie)
{
	/* uint64_t mpidr; */	
	unsigned int iar;

	/*FIXME: just print for debug*/
	//printf("[fiqd_sel1_interrupt_handler] flags = %d, id = %d\n", flags, id);

	/* Check the security state when the exception was generated */
	assert(get_interrupt_src_ss(flags) == NON_SECURE);

#if IMF_READ_INTERRUPT_ID
	/* Check the security status of the interrupt */
	assert(plat_ic_get_interrupt_type(id) == INTR_TYPE_S_EL1);
#endif

	/* Sanity check the pointer to this cpu's context
	mpidr = read_mpidr();
	assert(handle == cm_get_context(mpidr, NON_SECURE)); */

	/* Save the non-secure context before entering the TSP */
	cm_el1_sysregs_context_save(NON_SECURE);	

	iar = get_ack_info();
	ack_sgi(iar);


	if(id == WDT_IRQ_BIT_ID)
	{
		/* FIX-ME : change 0xFE to the kernel online CPU mask */
		fiq_smp_call_function(0xFE, aee_wdt_dump, 0, 0);
		aee_wdt_dump();
	}

	if(id == FIQ_SMP_CALL_SGI)
	{
		fiq_icc_isr();
	}

	SMC_RET0(handle);

#if 0
	/* Get a reference to this cpu's TSP context */
	linear_id = platform_get_core_pos(mpidr);
	fiq_ctx = &fiqd_sp_context[linear_id];
	assert(&fiq_ctx->cpu_ctx == cm_get_context(mpidr, SECURE));

	/*
	 * Determine if the TSP was previously preempted. Its last known
	 * context has to be preserved in this case.
	 * The TSP should return control to the TSPD after handling this
	 * FIQ. Preserve essential EL3 context to allow entry into the
	 * TSP at the FIQ entry point using the 'cpu_context' structure.
	 * There is no need to save the secure system register context
	 * since the TSP is supposed to preserve it during S-EL1 interrupt
	 * handling.
	 */
	if (get_std_smc_active_flag(fiq_ctx->state)) {
		fiq_ctx->saved_spsr_el3 = SMC_GET_EL3(&fiq_ctx->cpu_ctx,
						      CTX_SPSR_EL3);
		fiq_ctx->saved_elr_el3 = SMC_GET_EL3(&fiq_ctx->cpu_ctx,
						     CTX_ELR_EL3);
	}

	SMC_SET_EL3(&fiq_ctx->cpu_ctx,
		    CTX_SPSR_EL3,
		    SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS));
	SMC_SET_EL3(&fiq_ctx->cpu_ctx,
		    CTX_ELR_EL3,
		    (uint64_t) &fiq_vectors->fiq_entry);
	cm_el1_sysregs_context_restore(SECURE);
	cm_set_next_eret_context(SECURE);

	/*
	 * Tell the TSP that it has to handle an FIQ synchronously. Also the
	 * instruction in normal world where the interrupt was generated is
	 * passed for debugging purposes. It is safe to retrieve this address
	 * from ELR_EL3 as the secure context will not take effect until
	 * el3_exit().
	 */
	SMC_RET2(&fiq_ctx->cpu_ctx, TSP_HANDLE_FIQ_AND_RETURN, read_elr_el3());
#else
	SMC_RET0(handle);//FIXME: Just to fix build error
#endif
}

/*******************************************************************************
 * Secure Payload Dispatcher setup. The SPD finds out the SP entrypoint and type
 * (aarch32/aarch64) if not already known and initialises the context for entry
 * into the SP for its initialisation.
 ******************************************************************************/
int32_t fiqd_setup(void)
{
	entry_point_info_t *image_info;
	int32_t rc;
	uint64_t mpidr = read_mpidr();
	uint32_t linear_id;

	linear_id = platform_get_core_pos(mpidr);

	/*
	 * Get information about the Secure Payload (BL32) image. Its
	 * absence is a critical failure.  TODO: Add support to
	 * conditionally include the SPD service
	 */
	image_info = bl31_plat_get_next_image_ep_info(SECURE);
	assert(image_info);

	/*
	 * If there's no valid entry point for SP, we return a non-zero value
	 * signalling failure initializing the service. We bail out without
	 * registering any handlers
	 */
	if (!image_info->pc)
		return 1;

	/*
	 * We could inspect the SP image and determine it's execution
	 * state i.e whether AArch32 or AArch64. Assuming it's AArch64
	 * for the time being.
	 */
	rc = fiqd_init_secure_context(image_info->pc,
				     TSP_AARCH64,
				     mpidr,
				     &fiqd_sp_context[linear_id]);
	assert(rc == 0);

	/*
	 * All TSPD initialization done. Now register our init function with
	 * BL31 for deferred invocation
	 */
	bl31_register_bl32_init(&fiqd_init);

	return rc;
}

/*******************************************************************************
 * This function passes control to the Secure Payload image (BL32) for the first
 * time on the primary cpu after a cold boot. It assumes that a valid secure
 * context has already been created by fiqd_setup() which can be directly used.
 * It also assumes that a valid non-secure context has been initialised by PSCI
 * so it does not need to save and restore any non-secure state. This function
 * performs a synchronous entry into the Secure payload. The SP passes control
 * back to this routine through a SMC.
 ******************************************************************************/
int32_t fiqd_init(unsigned int boot_entry, unsigned int boot_args)
{
	uint32_t flags;
	uint64_t rc;

	/*
	 * Register an interrupt handler for S-EL1 interrupts when generated
	 * during code executing in the non-secure state.
	 */
	flags = 0;
	set_interrupt_rm_flag(flags, NON_SECURE);
	rc = register_interrupt_type_handler(INTR_TYPE_S_EL1,
					     fiqd_sel1_interrupt_handler,
					     flags);
	if (rc)
		panic();

	return rc;
}


/*******************************************************************************
 * This function is responsible for handling all SMCs in the Trusted OS/App
 * range from the non-secure state as defined in the SMC Calling Convention
 * Document. It is also responsible for communicating with the Secure payload
 * to delegate work and return results back to the non-secure state. Lastly it
 * will also return any information that the secure payload needs to do the
 * work assigned to it.
 ******************************************************************************/
uint64_t fiqd_smc_handler(uint32_t smc_fid,
			 uint64_t x1,
			 uint64_t x2,
			 uint64_t x3,
			 uint64_t x4,
			 void *cookie,
			 void *handle,
			 uint64_t flags)
{
	/* DON'T support any SMC call here */
	switch (smc_fid) {
	default:
		break;
	}

	SMC_RET1(handle, SMC_UNK);
}

/* Define a SPD runtime service descriptor for fast SMC calls */
DECLARE_RT_SVC(
	fiqd_fast,

	OEN_TOS_START,
	OEN_TOS_END,
	SMC_TYPE_FAST,
	fiqd_setup,
	fiqd_smc_handler
);

/* Define a SPD runtime service descriptor for standard SMC calls */
DECLARE_RT_SVC(
	fiqd_std,

	OEN_TOS_START,
	OEN_TOS_END,
	SMC_TYPE_STD,
	NULL,
	fiqd_smc_handler
);
