/*
* Copyright (c) 2015 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <assert.h>
#include <bits.h>
#include <err.h>
#include <sys/types.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <reg.h>
#include <kernel/thread.h>
#include <kernel/debug.h>
#include <lk/init.h>
#include <platform/interrupts.h>
#include <arch/ops.h>
#include <platform/gic.h>
#include <trace.h>
#if WITH_LIB_SM
#include <lib/sm.h>
#include <lib/sm/sm_err.h>
#endif

#define LOCAL_TRACE 0

#define GICREG(gic, reg) (*REG32(GICBASE(gic) + (reg)))

/* main cpu regs */
#define GICC_OFFSET		(GICR_OFFSET)
#define GICC_CTLR               (GICC_OFFSET + 0x0000)
#define GICC_PMR                (GICC_OFFSET + 0x0004)
#define GICC_BPR                (GICC_OFFSET + 0x0008)
#define GICC_IAR                (GICC_OFFSET + 0x000c)
#define GICC_EOIR               (GICC_OFFSET + 0x0010)
#define GICC_RPR                (GICC_OFFSET + 0x0014)
#define GICC_HPPIR              (GICC_OFFSET + 0x0018)
#define GICC_APBR               (GICC_OFFSET + 0x001c)
#define GICC_AIAR               (GICC_OFFSET + 0x0020)
#define GICC_AEOIR              (GICC_OFFSET + 0x0024)
#define GICC_AHPPIR             (GICC_OFFSET + 0x0028)
#define GICC_APR(n)             (GICC_OFFSET + 0x00d0 + (n) * 4)
#define GICC_NSAPR(n)           (GICC_OFFSET + 0x00e0 + (n) * 4)
#define GICC_IIDR               (GICC_OFFSET + 0x00fc)
#define GICC_DIR                (GICC_OFFSET + 0x1000)

/* distribution regs */
#if 1
#define GICD_CTLR               (0x000)
#define GICD_TYPER              (0x004)
#define GICD_IIDR               (0x008)
#define GICD_IGROUPR(n)         (0x080 + (n) * 4)
#define GICD_ISENABLER(n)       (0x100 + (n) * 4)
#define GICD_ICENABLER(n)       (0x180 + (n) * 4)
#define GICD_ISPENDR(n)         (0x200 + (n) * 4)
#define GICD_ICPENDR(n)         (0x280 + (n) * 4)
#define GICD_ISACTIVER(n)       (0x300 + (n) * 4)
#define GICD_ICACTIVER(n)       (0x380 + (n) * 4)
#define GICD_IPRIORITYR(n)      (0x400 + (n) * 4)
#define GICD_ITARGETSR(n)       (0x800 + (n) * 4)
#define GICD_ICFGR(n)           (0xc00 + (n) * 4)
#define GICD_IGRPMODR(n)	(0xd00 + (n) * 4)
#define GICD_NSACR(n)           (0xe00 + (n) * 4)
#define GICD_SGIR               (0xf00)
#define GICD_CPENDSGIR(n)       (0xf10 + (n) * 4)
#define GICD_SPENDSGIR(n)       (0xf20 + (n) * 4)
#else
#define GICD_CTLR               (GICD_OFFSET + 0x000)
#define GICD_TYPER              (GICD_OFFSET + 0x004)
#define GICD_IIDR               (GICD_OFFSET + 0x008)
#define GICD_IGROUPR(n)         (GICD_OFFSET + 0x080 + (n) * 4)
#define GICD_ISENABLER(n)       (GICD_OFFSET + 0x100 + (n) * 4)
#define GICD_ICENABLER(n)       (GICD_OFFSET + 0x180 + (n) * 4)
#define GICD_ISPENDR(n)         (GICD_OFFSET + 0x200 + (n) * 4)
#define GICD_ICPENDR(n)         (GICD_OFFSET + 0x280 + (n) * 4)
#define GICD_ISACTIVER(n)       (GICD_OFFSET + 0x300 + (n) * 4)
#define GICD_ICACTIVER(n)       (GICD_OFFSET + 0x380 + (n) * 4)
#define GICD_IPRIORITYR(n)      (GICD_OFFSET + 0x400 + (n) * 4)
#define GICD_ITARGETSR(n)       (GICD_OFFSET + 0x800 + (n) * 4)
#define GICD_ICFGR(n)           (GICD_OFFSET + 0xc00 + (n) * 4)
#define GICD_NSACR(n)           (GICD_OFFSET + 0xe00 + (n) * 4)
#define GICD_SGIR               (GICD_OFFSET + 0xf00)
#define GICD_CPENDSGIR(n)       (GICD_OFFSET + 0xf10 + (n) * 4)
#define GICD_SPENDSGIR(n)       (GICD_OFFSET + 0xf20 + (n) * 4)
#endif

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define GIC_REG_COUNT(bit_per_reg) DIV_ROUND_UP(MAX_INT, (bit_per_reg))
#define DEFINE_GIC_SHADOW_REG(name, bit_per_reg, init_val, init_from) \
	uint32_t (name)[GIC_REG_COUNT(bit_per_reg)] = { \
		[(init_from / bit_per_reg) ... \
		 (GIC_REG_COUNT(bit_per_reg) - 1)] = (init_val) \
	}

static status_t arm_gic_set_secure_locked(u_int irq, bool secure);
static void gic_set_enable(uint vector, bool enable);

static spin_lock_t gicd_lock;
#if WITH_LIB_SM
#define GICD_LOCK_FLAGS SPIN_LOCK_FLAG_IRQ_FIQ
#else
#define GICD_LOCK_FLAGS SPIN_LOCK_FLAG_INTERRUPTS
#endif
#define GIC_MAX_PER_CPU_INT 32

static inline uint32_t read32(uint32_t x)
{
	mb();
	return *(volatile uint32_t *)(x);
}

static inline void write32(uint32_t val, uint32_t x)
{
	*(volatile uint32_t *)x = val;
	mb();
}

#define SZ_64K                          (0x00010000)

#if WITH_LIB_SM
static bool arm_gic_non_secure_interrupts_frozen;

static bool arm_gic_interrupt_change_allowed(int irq)
{
	if (!arm_gic_non_secure_interrupts_frozen)
		return true;

	TRACEF("change to interrupt %d ignored after booting ns\n", irq);
	return false;
}

static void suspend_resume_fiq(bool resume_gicc, bool resume_gicd);
#else
static bool arm_gic_interrupt_change_allowed(int irq)
{
	return true;
}

static void suspend_resume_fiq(bool resume_gicc, bool resume_gicd)
{
}
#endif


struct int_handler_struct {
	int_handler handler;
	void *arg;
};

static struct int_handler_struct int_handler_table_per_cpu[GIC_MAX_PER_CPU_INT][SMP_MAX_CPUS];
static struct int_handler_struct int_handler_table_shared[MAX_INT-GIC_MAX_PER_CPU_INT];

static struct int_handler_struct *get_int_handler(unsigned int vector, uint cpu)
{
	if (vector < GIC_MAX_PER_CPU_INT)
		return &int_handler_table_per_cpu[vector][cpu];
	else
		return &int_handler_table_shared[vector - GIC_MAX_PER_CPU_INT];
}

static inline int gic_populate_redist(uint32_t *rbase)
{
	int cpu = arch_curr_cpu_num();

	*rbase = GICR_BASE_VIRT + cpu*SZ_64K*2;

	return 0;
}

static status_t gic_set_priority(u_int irq, uint8_t priority)
{
	u_int reg = irq / 4;
	u_int shift = 8 * (irq % 4);
	u_int mask = 0xff << shift;
	uint32_t regval;
	uint32_t base = GICD_BASE_VIRT;

	if (irq < 32) {
		gic_populate_redist(&base);
		base += SZ_64K;
	}

	regval = read32(base+GICD_IPRIORITYR(reg));
	regval = (regval & ~mask) | ((uint32_t)priority << shift);
	write32(regval, base+GICD_IPRIORITYR(reg));

	return 0;
}

static void gic_set_g1s(uint vector)
{
	int reg = vector / 32;
	uint32_t mask = 1ULL << (vector % 32);
	uint32_t base = GICD_BASE_VIRT;

	LTRACEF("gic_set_g1s: %d\n", vector);

	if (vector < 32) {
		gic_populate_redist(&base);
		base += SZ_64K;
	}

	/* set as G1S */
	write32(read32(base+GICD_IGRPMODR(reg)) | (mask),
		base+GICD_IGRPMODR(reg));

	write32(read32(base+GICD_IGROUPR(reg)) & (~mask),
		base+GICD_IGROUPR(reg));
}

void register_int_handler(unsigned int vector, int_handler handler, void *arg)
{
	struct int_handler_struct *h;
	uint cpu = arch_curr_cpu_num();

	spin_lock_saved_state_t state;

	if (vector >= MAX_INT)
		panic("register_int_handler: vector out of range %d\n", vector);

	spin_lock_save(&gicd_lock, &state, GICD_LOCK_FLAGS);

	if (arm_gic_interrupt_change_allowed(vector)) {
		h = get_int_handler(vector, cpu);
		h->handler = handler;
		h->arg = arg;
	}
	gic_set_g1s(vector);
	gic_set_priority(vector, 0);

	if (vector < 16) {
		gic_set_enable(vector, true);
	}

	spin_unlock_restore(&gicd_lock, state, GICD_LOCK_FLAGS);
}

#if 0
#if WITH_LIB_SM
static DEFINE_GIC_SHADOW_REG(gicd_igroupr, 32, ~0U, 0);
#endif
static DEFINE_GIC_SHADOW_REG(gicd_itargetsr, 4, 0x01010101, 32);
#endif

static uint32_t gicc_read_hppir0(void)
{
	uint32_t val = -1;

	__asm__ volatile("mrc p15, 0, %0, c12, c8, 2" :: "r" (val));

	return val;
}

static uint32_t gicc_read_hppir1(void)
{
	uint32_t val = -1;

	__asm__ volatile("mrc p15, 0, %0, c12, c12, 2" :: "r" (val));

	return val;
}

static uint32_t gicc_read_iar1(void)
{
        uint32_t val = -1;

        __asm__ volatile("mrc p15, 0, %0, c12, c12, 0" : "=r" (val));

        return val;
}

static uint32_t gicc_read_iar0(void)
{
        uint32_t val = -1;

        __asm__ volatile("mrc p15, 0, %0, c12, c8, 0" : "=r" (val));

        return val;
}

static void gicc_write_eoi0(uint32_t val)
{
	__asm__ volatile("mcr p15, 0, %0, c12, c8, 1" :: "r" (val));
}

static void gicc_write_eoi1(uint32_t val)
{
	__asm__ volatile("mcr p15, 0, %0, c12, c12, 1" :: "r" (val));
}


static void gic_set_enable(uint vector, bool enable)
{
	int reg = vector / 32;
	uint32_t mask = 1ULL << (vector % 32);
	uint32_t base = GICD_BASE_VIRT;

	LTRACEF("gic_set_enable: %d\n", vector);

	if (vector < 32) {
		gic_populate_redist(&base);
		base += SZ_64K;
	}

	if (enable)
		write32(mask, base+GICD_ISENABLER(reg));
	else
		write32(mask, base+GICD_ICENABLER(reg));
}


// FIXME:
// use hardcoded 128 CPU cores to avoid
// array overflow using SMP_MAX_CPUS
//uint32_t cpu_logical_map[SMP_MAX_CPUS] = {
uint32_t cpu_logical_map[128] = {
	0x80000000,
	0x80000001,
	0x80000002,
	0x80000003,
	0x80000100,
	0x80000101,
	0x80000102,
	0x80000103,
	0x80000200,
	0x80000201,
};

static uint32_t get_mpidr(void)
{
	uint32_t val = -1;

	__asm__ volatile("mrc p15, 0, %0, c0, c0, 5" : "=r" (val));

        return val;
}

static void arm_gic_init_percpu(uint level)
{
	uint32_t cpu = arch_curr_cpu_num();

	if (cpu >= SMP_MAX_CPUS) {
		return;
	}

	cpu_logical_map[cpu] = get_mpidr();
}

#if 0
LK_INIT_HOOK_FLAGS(arm_gic_init_percpu,
       arm_gic_init_percpu,
       LK_INIT_LEVEL_PLATFORM_EARLY, LK_INIT_FLAG_SECONDARY_CPUS);
#endif

static void arm_gic_suspend_cpu(uint level)
{
	/* TODO */
}

#if WITH_LIB_SM_MONITOR        /* ATF already initialize GIC */
LK_INIT_HOOK_FLAGS(arm_gic_suspend_cpu, arm_gic_suspend_cpu,
		LK_INIT_LEVEL_PLATFORM, LK_INIT_FLAG_CPU_SUSPEND);
#endif

static void arm_gic_resume_cpu(uint level)
{
	/* TODO */
}

#if WITH_LIB_SM_MONITOR        /* ATF already initialize GIC */
LK_INIT_HOOK_FLAGS(arm_gic_resume_cpu, arm_gic_resume_cpu,
		LK_INIT_LEVEL_PLATFORM, LK_INIT_FLAG_CPU_RESUME);
#endif

static int arm_gic_max_cpu(void)
{
#if 0
	return (GICREG(0, GICD_TYPER) >> 5) & 0x7;
#else
	return 10;
#endif
}

void arm_gic_init(void)
{
	/* TODO */
}

void arm_gic_secondary_init(void)
{
	/* TODO */
#if 0
	arm_gic_init_percpu(0);
#endif
}

static status_t arm_gic_set_secure_locked(u_int irq, bool secure)
{
#if 0
#if WITH_LIB_SM
	int reg = irq / 32;
	uint32_t mask = 1ULL << (irq % 32);
	uint32_t base = 0;

	if (irq < 32) {
		gic_populate_redist(&base);
	}

	if (irq >= MAX_INT)
		return ERR_INVALID_ARGS;

	if (secure)
		GICREG(0, GICD_IGROUPR(reg)) = (gicd_igroupr[reg] &= ~mask);
	else
		GICREG(0, GICD_IGROUPR(reg)) = (gicd_igroupr[reg] |= mask);
	LTRACEF("irq %d, secure %d, GICD_IGROUP%d = %x\n",
		irq, secure, reg, GICREG(0, GICD_IGROUPR(reg)));
#endif
#endif
	return NO_ERROR;
}

static status_t arm_gic_set_target_locked(u_int irq, u_int cpu_mask, u_int enable_mask)
{
#if 0
	u_int reg = irq / 4;
	u_int shift = 8 * (irq % 4);
	u_int old_val;
	u_int new_val;

	cpu_mask = (cpu_mask & 0xff) << shift;
	enable_mask = (enable_mask << shift) & cpu_mask;

	old_val = GICREG(0, GICD_ITARGETSR(reg));
	new_val = (gicd_itargetsr[reg] & ~cpu_mask) | enable_mask;
	GICREG(0, GICD_ITARGETSR(reg)) = gicd_itargetsr[reg] = new_val;
	LTRACEF("irq %i, GICD_ITARGETSR%d %x => %x (got %x)\n",
		irq, reg, old_val, new_val, GICREG(0, GICD_ITARGETSR(reg)));
#endif
	return NO_ERROR;
}

static status_t arm_gic_get_priority(u_int irq)
{
	u_int reg = irq / 4;
	u_int shift = 8 * (irq % 4);
	uint32_t base = GICD_BASE_VIRT;
	uint32_t regval = 0;

	if (irq < 32) {
		gic_populate_redist(&base);
		base += SZ_64K;
	}

	regval = read32(base+GICD_IPRIORITYR(reg));

	return (regval>>shift)&0xff;
}

static status_t arm_gic_set_priority_locked(u_int irq, uint8_t priority)
{
	u_int reg = irq / 4;
	u_int shift = 8 * (irq % 4);
	u_int mask = 0xff << shift;
	uint32_t regval = 0;
	uint32_t base = GICD_BASE_VIRT;

	if (irq < 32) {
		gic_populate_redist(&base);
		base += SZ_64K;
	}

	regval = read32(base+GICD_IPRIORITYR(reg));
	LTRACEF("irq %i, old GICD_IPRIORITYR%d = %x\n", irq, reg, regval);
	regval = (regval & ~mask) | ((uint32_t)priority << shift);
	write32(regval, base+GICD_IPRIORITYR(reg));
	LTRACEF("irq %i, new GICD_IPRIORITYR%d = %x, req %x\n",
		irq, reg, read32(base+GICD_IPRIORITYR(reg)), regval);

	return 0;
}

static uint32_t compute_target_list(uint32_t *cpu, uint32_t map, uint32_t cluster_id)
{
	uint16_t tlist = 0;
	uint32_t target_cpu = 0;

	LTRACEF_LEVEL(2, "compute_target_list: cpu=%d, map=0x%x\n", *cpu, map);

	for (target_cpu = *cpu; target_cpu < SMP_MAX_CPUS; ++target_cpu) {
		uint32_t mpidr = cpu_logical_map[target_cpu];
		uint32_t target_cluster_id = mpidr & ~0xffL;

		LTRACEF_LEVEL(2, "compute_target_list: target_cpu=%d, mpidr=0x%x, target_cluster_id=%x\n",
			target_cpu, mpidr, target_cluster_id);

		/* gic-500 only support 16 cores in a cluster at max */
		if ((mpidr & 0xff) > 16) {
			break;
		}

		LTRACEF_LEVEL(2, "CCC\n");

		/* we assume cluster_id only changes _WHEN_
		* all cores in the same cluster is counted */
		if (target_cluster_id != cluster_id) {
			break;
		}

		LTRACEF_LEVEL(2, "DDD\n");

		/* only check those cores in map */
		if (((1 << target_cpu) & map) == 0) {
			continue;
		}

		/* the core in the same cluster, add it. */
		tlist |= 1 << (mpidr & 0xff);
		LTRACEF_LEVEL(2, "tlist=0x%x\n", tlist);
        }

	/* update cpu for the next cluster */
	*cpu = target_cpu - 1;
	LTRACEF_LEVEL(2, "next cpu = %d, result tlist = %x\n", *cpu, tlist);

	return tlist;
}

static void gicc_write_sgi0(uint64_t val)
{
	uint32_t low = (val&0xffffffff);
	uint32_t high = (val&0xffffffff00000000)>>32;

	__asm__ volatile("mcrr p15, 2, %0, %1, c12"
		: : "r" (low), "r" (high));
}

static void gicc_write_sgi1(uint64_t val)
{
	uint32_t low = (val&0xffffffff);
	uint32_t high = (val&0xffffffff00000000)>>32;

	__asm__ volatile("mcrr p15, 0, %0, %1, c12"
		: : "r" (low), "r" (high));
}

static void gic_send_sgi(uint32_t cluster_id, uint32_t tlist, uint32_t irq)
{
	uint32_t val = 0;

	val =   (irq<<24) |
		(((cluster_id>>8)&0xff)<<16)|
		tlist;

	LTRACEF_LEVEL(2, "val = 0x%x\n", val);
        gicc_write_sgi0(val);
	/* ensure sgi issued first */
	ISB;
}

#define local_fiq_enable()      __asm__ volatile("cpsie f")
#define local_fiq_disable()     __asm__ volatile("cpsid f")

static uint32_t read_isr(void)
{
	uint32_t val = -1;

	__asm__ volatile ("mrc p15, 0, %0, c12, c1, 0" :: "r" (val));

	return val;
}

status_t arm_gic_ns_sgi(u_int irq, u_int cpu_mask)
{
	int reg = irq / 32;
	uint32_t mask = 1ULL << (irq % 32);
	uint32_t rbase = 0;
	int cpu = 0;

	LTRACEF("cpu%d enter, irq %d cpu_mask 0x%x\n", arch_curr_cpu_num(), irq, cpu_mask);

	for (cpu = 0; cpu < SMP_MAX_CPUS ; ++cpu) {
		if (((1 << cpu) & cpu_mask) == 0) {
			continue;
		}

		rbase = GICR_BASE_VIRT + cpu*SZ_64K*2;
		rbase += SZ_64K;

		write32(mask, rbase+GICD_ISPENDR(reg));
	}

	return NO_ERROR;
}

status_t arm_gic_sgi(u_int irq, u_int flags, u_int cpu_mask)
{
	uint32_t cpu = 0;
	uint32_t mpidr = 0;
	uint32_t cluster_id = 0;
	uint32_t tlist = 0;

	LTRACEF_LEVEL(2, "arm_gic_sgi: cpu%d enter, ipi = %d\n", arch_curr_cpu_num(), irq);

	/* Ensure all data are visible for other cores */
	mb();
	for (cpu = 0; cpu < SMP_MAX_CPUS; ++cpu) {
		/* only check cpu in the mask */
		if ((cpu_mask & (1<<cpu)) == 0) {
			continue;
		}

		/* gicv3 can only send sgi per cluster,
		 * gather all cores in map in the same cluster */
		mpidr = cpu_logical_map[cpu];
		cluster_id = (mpidr&~0xff);
		tlist = compute_target_list(&cpu, cpu_mask, cluster_id);

		LTRACEF_LEVEL(2, "arm_gic_sgi: mpidr = 0x%x, cpu = %d, mask = 0x%x, cluster_id = 0x%x\n",
			mpidr, cpu, cpu_mask, cluster_id);
		gic_send_sgi(cluster_id, tlist, irq);
	}
#if 0
	if (read_isr()& (1<<6)) {
		while (1);
	}
#endif

	return NO_ERROR;
}

status_t mask_interrupt(unsigned int vector)
{
	if (vector >= MAX_INT)
		return ERR_INVALID_ARGS;

	if (arm_gic_interrupt_change_allowed(vector))
		gic_set_enable(vector, false);

	return NO_ERROR;
}

status_t unmask_interrupt(unsigned int vector)
{
	if (vector >= MAX_INT)
		return ERR_INVALID_ARGS;

	if (arm_gic_interrupt_change_allowed(vector))
		gic_set_enable(vector, true);

	return NO_ERROR;
}

static
enum handler_return __platform_irq(struct arm_iframe *frame)
{
#if 0
	// get the current vector
	uint32_t iar = GICREG(0, GICC_IAR);
	unsigned int vector = iar & 0x3ff;

	if (vector >= 0x3fe) {
		// spurious
		return INT_NO_RESCHEDULE;
	}

	THREAD_STATS_INC(interrupts);
	KEVLOG_IRQ_ENTER(vector);

	uint cpu = arch_curr_cpu_num();

//	printf("platform_irq: iar 0x%x cpu %u spsr 0x%x, pc 0x%x, currthread %p, vector %d\n",
//			iar, cpu, frame->spsr, frame->pc, get_current_thread(), vector);

	// deliver the interrupt
	enum handler_return ret;

	ret = INT_NO_RESCHEDULE;
	struct int_handler_struct *handler = get_int_handler(vector, cpu);
	if (handler->handler)
		ret = handler->handler(handler->arg);

	GICREG(0, GICC_EOIR) = iar;

//	printf("platform_irq: cpu %u exit %d\n", cpu, ret);

	KEVLOG_IRQ_EXIT(vector);

	return ret;
#else
	return INT_NO_RESCHEDULE;
#endif
}

enum handler_return platform_irq(struct arm_iframe *frame)
{
#if 1
	while (1) {
		LTRACEF("ERROR: In %s, this is incorrect in FIQ only flow!\n", __func__);
	}

	return 0;
#else
	uint32_t hppir1 = gicc_read_hppir1();
	uint32_t pending_irq = hppir1 & 0x3ff;
	struct int_handler_struct *h;
	uint32_t cpu = arch_curr_cpu_num();
	uint32_t irq = gicc_read_iar0() & 0x3ff;
	enum handler_return ret = 0;

	LTRACEF("cpu%d: hppir0 = %d, hppir1 = %d, irq = %d\n",
			cpu, hppir0, gicc_read_hppir1(), irq);

	if (pending_irq < MAX_INT &&
		get_int_handler(pending_irq, cpu)->handler) {

		if (irq < MAX_INT &&
			(h = get_int_handler(pending_irq, cpu))->handler) {
			ret = h->handler(h->arg);
		} else {
			TRACEF("unexpected irq %d != %d may get lost\n",
				irq, pending_irq);
		}

		gicc_write_eoi0(irq);
		return ret;
	}

	/* go back to NS world */
	LTRACEF("!!go NWd\n");
	//sm_handle_fiq_my();
	return sm_handle_irq();
#endif
}

extern void sm_handle_fiq_my(void);

void platform_fiq(struct arm_iframe *frame)
{
}

enum handler_return platform_g1s_irq(struct arm_iframe *frame)
{
	enum handler_return ret = 0;

	struct int_handler_struct *h;

	uint32_t cpu = arch_curr_cpu_num();
	uint32_t irq = gicc_read_iar1() & 0x3ff;

	LTRACEF_LEVEL(2, "cpu%d: hppir0 = %d, hppir1 = %d, irq = %d\n",
			cpu, gicc_read_hppir0(), gicc_read_hppir1(), irq);

	if (irq < MAX_INT
	&& (h = get_int_handler(irq, cpu))
	&& h->handler) {
		ret = h->handler(h->arg);
		gicc_write_eoi1(irq);

		LTRACEF_LEVEL(2, "cpu%d: irq %d handle done\n", cpu, irq);
		return ret;
	}

	TRACEF("unexpected irq %d may get lost\n", irq);

	return ret;
}

enum handler_return platform_interrupt_route(struct arm_iframe *frame)
{
	uint32_t cpu = arch_curr_cpu_num();

	/* go back to ATF/NS world */
	LTRACEF_LEVEL(2, "!!cpu%d go NWd\n", cpu);

	/* since we handles G1NS irq, neeed to use sm_handle_irq() to return to NS */
	return sm_handle_irq();
}

#if WITH_LIB_SM

#if TRUSTY_INTERRUPT_FIQ_ONLY
static status_t arm_gic_get_next_irq_locked(u_int min_irq, bool per_cpu)
{
	if (per_cpu && min_irq <= NON_SECURE_NOP_RETRY_IPI)
		return NON_SECURE_NOP_RETRY_IPI;

	return SM_ERR_END_OF_INPUT;
}
#else
static status_t arm_gic_get_next_irq_locked(u_int min_irq, bool per_cpu)
{
	u_int irq;
	u_int max_irq = per_cpu ? GIC_MAX_PER_CPU_INT : MAX_INT;
	uint cpu = arch_curr_cpu_num();

	if (!per_cpu && min_irq < GIC_MAX_PER_CPU_INT)
		min_irq = GIC_MAX_PER_CPU_INT;

	for (irq = min_irq; irq < max_irq; irq++)
		if (get_int_handler(irq, cpu)->handler)
			return irq;

	return SM_ERR_END_OF_INPUT;
}
#endif

long smc_intc_get_next_irq(smc32_args_t *args)
{
	status_t ret;
	spin_lock_saved_state_t state;

	spin_lock_save(&gicd_lock, &state, GICD_LOCK_FLAGS);

#if !TRUSTY_INTERRUPT_FIQ_ONLY
	arm_gic_non_secure_interrupts_frozen = true;
#endif
	ret = arm_gic_get_next_irq_locked(args->params[0], args->params[1]);
	LTRACEF("min_irq %d, per_cpu %d, ret %d\n",
		args->params[0], args->params[1], ret);

	spin_unlock_restore(&gicd_lock, state, GICD_LOCK_FLAGS);

	return ret;
}

#if 0
static u_long enabled_fiq_mask[BITMAP_NUM_WORDS(MAX_INT)];
#endif

static void bitmap_update_locked(u_long *bitmap, u_int bit, bool set)
{
	u_long mask = 1UL << BITMAP_BIT_IN_WORD(bit);

	bitmap += BITMAP_WORD(bit);
	if (set)
		*bitmap |= mask;
	else
		*bitmap &= ~mask;
}

long smc_intc_request_fiq(smc32_args_t *args)
{
#if 0
	/* No Need */
#endif
	return NO_ERROR;
}

#if 0
static u_int current_fiq[8] = { 0x3ff, 0x3ff, 0x3ff, 0x3ff, 0x3ff, 0x3ff, 0x3ff, 0x3ff };
#endif

static bool update_fiq_targets(u_int cpu, bool enable, u_int triggered_fiq, bool resume_gicd)
{
#if 0
	/* TODO */

	u_int i, j;
	u_long mask;
	u_int fiq;
	bool smp = arm_gic_max_cpu() > 0;
	bool ret = false;

	spin_lock(&gicd_lock); /* IRQs and FIQs are already masked */
	for (i = 0; i < BITMAP_NUM_WORDS(MAX_INT); i++) {
		mask = enabled_fiq_mask[i];
		while (mask) {
			j = _ffz(~mask);
			mask &= ~(1UL << j);
			fiq = i * BITMAP_BITS_PER_WORD + j;
			if (fiq == triggered_fiq)
				ret = true;
			LTRACEF("cpu %d, irq %i, enable %d\n", cpu, fiq, enable);
			if (smp)
				arm_gic_set_target_locked(fiq, 1U << cpu, enable ? ~0 : 0);
			if (!smp || resume_gicd)
				gic_set_enable(fiq, enable);
		}
	}
	spin_unlock(&gicd_lock);
	return ret;
#else
	return 0;
#endif
}

static void suspend_resume_fiq(bool resume_gicc, bool resume_gicd)
{
#if 0
	/* TODO */

	u_int cpu = arch_curr_cpu_num();

	ASSERT(cpu < 8);

	update_fiq_targets(cpu, resume_gicc, ~0, resume_gicd);
#endif
}

status_t sm_intc_fiq_enter(void)
{
	/* No Need */
	return 0;
}

void sm_intc_fiq_exit(void)
{
	/* No Need */
}
#endif

/* vim: set ts=4 sw=4 noexpandtab: */
