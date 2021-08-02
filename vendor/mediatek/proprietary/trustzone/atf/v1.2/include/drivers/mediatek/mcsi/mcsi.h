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

#ifndef __MCSI_H__
#define __MCSI_H__

/* Slave interface offsets from PERIPHBASE */
#define SLAVE_IFACE7_OFFSET		0x1700
#define SLAVE_IFACE6_OFFSET		0x1600
#define SLAVE_IFACE5_OFFSET		0x1500
#define SLAVE_IFACE4_OFFSET		0x1400
#define SLAVE_IFACE3_OFFSET		0x1300
#define SLAVE_IFACE2_OFFSET		0x1200
#define SLAVE_IFACE1_OFFSET		0x1100
#define SLAVE_IFACE0_OFFSET		0x1000
#define SLAVE_IFACE_OFFSET(index)	SLAVE_IFACE0_OFFSET +	\
					(0x100 * (index))
/* Control and ID register offsets */
#define CENTRAL_CTRL_REG		0x0
#define ERR_FLAG_REG			0x4
#define INT_STATUS_REG			0x8	/* no use */
#define INT_EN_REG			0xc	/* no use */
#define SF_INIT_REG			0x10
#define SF_CTRL_REG			0x14
#define DCM_CTRL_REG			0x18
#define CACTIVE_CTRL_REG		0x1c	/* not sure */
#define ERR_FLAG2_REG			0x20
#define SNP_PENDING_REG			0x28
#define ACP_PENDING_REG			0x2c
#define PMU_CTRL_REG			0x100	/* no use */
#define IP_CTRL_REG			0x200	/* no use */
#define SYS_CCE_CTRL			0x2000
#define MST1_CTRL			0x2100
#define MTS2_CTRL			0x2200
#define XBAR_ARAW_ARB			0x3000
#define XBAR_R_ARB			0x3004

/* Slave interface register offsets */
#define SNOOP_CTRL_REG			0x0
#define QOS_CTRL_REG			0x4
#define QOS_OVERRIDE_REG		0x8
#define QOS_TARGET_REG			0xc
#define BD_CTRL_REG			0x40

/* Snoop Control register bit definitions */
#define DVM_SUPPORT			(1 << 31)
#define SNP_SUPPORT			(1 << 30)
#define SHAREABLE_OVWRT			(1 << 2)
#define DVM_EN_BIT			(1 << 1)
#define SNOOP_EN_BIT			(1 << 0)

#define SF2_INIT_DONE			(1 << 17)
#define SF1_INIT_DONE			(1 << 16)
#define TRIG_SF2_INIT			(1 << 1)
#define TRIG_SF1_INIT			(1 << 0)

/* Status register bit definitions */
#define SNP_PENDING			31

/* Status bit */
#define NS_ACC				1
#define S_ACC				0

/* Central control register bit definitions */
#define PMU_SECURE_ACC_EN		(1 << 4)
#define INT_EN				(1 << 3)
#define SECURE_ACC_EN			(1 << 2)
#define DVM_DIS				(1 << 1)
#define SNOOP_DIS			(1 << 0)

#define MSCI_MEMORY_SZ			(0x10000)

#define MCSI_REG_ACCESS_READ		(0x0)
#define MCSI_REG_ACCESS_WRITE		(0x1)
#define MCSI_REG_ACCESS_SET_BITMASK	(0x2)
#define MCSI_REG_ACCESS_CLEAR_BITMASK	(0x3)

#define NR_MAX_SLV			(7)

/* ICCS */
#define IDLE_CACHE			(1 << 3)
#define USE_SHARED_CACHE		(1 << 4)
#define CACHE_SHARED_PRE_EN		(1 << 5)
#define CACHE_SHARED_POST_EN		(1 << 6)

#define ACP_PENDING_MASK		(0x7f)

#ifndef __ASSEMBLY__

#include <common_def.h>
#include <stdint.h>

/* Function declarations */

/*
 * The MCSI driver must be initialized with the base address of the
 * MCSI device in the platform memory map, and the cluster indices for
 * the MCSI slave interfaces 3 and 4 respectively. These are the fully
 * coherent ACE slave interfaces of MCSI.
 * The cluster indices must either be 0 or 1, corresponding to the level 1
 * affinity instance of the mpidr representing the cluster. A negative cluster
 * index indicates that no cluster is present on that slave interface.
 */
void mcsi_init(unsigned long cci_base,
		unsigned int num_cci_masters);

void cci_enable_cluster_coherency(unsigned long mpidr);
void cci_disable_cluster_coherency(unsigned long mpidr);

void cci_secure_switch(unsigned int ns);
void cci_init_sf(void);
unsigned long cci_reg_access(unsigned int op, unsigned long offeset, unsigned long val);

#ifdef MTK_ICCS_SUPPORT
void iccs_cache_shared_enable(unsigned char cluster_id);
void iccs_cache_shared_disable(unsigned char cluster_id);
void iccs_disable_dvm(unsigned char cluster_id);
unsigned char iccs_is_cluster_cache_shared(unsigned char cluster_id);
unsigned char iccs_get_curr_cache_shared_state(void);
unsigned char iccs_get_next_cache_shared_state(void);
void iccs_set_next_cache_shared_state(unsigned char state);
unsigned char iccs_state(unsigned int op, unsigned char state, unsigned char cluster_id);

#define ICCS_GET_CURR_STATE	(0x1)
#define ICCS_GET_NEXT_STATE	(0x2)
#define ICCS_SET_NEXT_STATE	(0x3)
#define ICCS_SET_CACHE_SHARED	(0x4)

#endif

#endif /* __ASSEMBLY__ */
#endif /* __MCSI_H__ */
