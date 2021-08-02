#ifndef __PLAT_MT_GIC_H
#define __PLAT_MT_GIC_H

#include <mmio.h>

#define GIC_INT_MASK (MCUCFG_BASE + 0xA6F0)
#define GIC500_ACTIVE_SEL_SHIFT 3
#define GIC500_ACTIVE_SEL_MASK (0x7 << GIC500_ACTIVE_SEL_SHIFT)
#define GIC500_ACTIVE_CPU_SHIFT 16
#define GIC500_ACTIVE_CPU_MASK (0xff << GIC500_ACTIVE_CPU_SHIFT)

#define MAX_IRQ_NR	(512)
uint32_t mt_irq_get_pending(uint32_t irq);
void mt_irq_set_pending(uint32_t irq);

#endif /* end of __PLAT_MT_GIC_H */
