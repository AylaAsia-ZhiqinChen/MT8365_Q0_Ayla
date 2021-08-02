#include <arch_helpers.h>
#include <arch.h>
#include <l2c.h>
#include <mcucfg.h>
#include <mmio.h>
#include <plat_private.h>
#include <platform_def.h>
#include <spinlock.h>

void config_L2_size(void)
{
	unsigned int cache_cfg0;

	cache_cfg0 = mmio_read_32(L2C_CFG_MP0) & (0xF << L2C_SIZE_CFG_OFF);
	cache_cfg0 = (cache_cfg0 << 1) | (0x1 << L2C_SIZE_CFG_OFF);
	cache_cfg0 = cache_cfg0 & ~(0x3 << 10);
	cache_cfg0 = (mmio_read_32(L2C_CFG_MP0) & ~(0xF << L2C_SIZE_CFG_OFF)) | cache_cfg0;
	mmio_write_32(L2C_CFG_MP0, cache_cfg0);
	cache_cfg0 = mmio_read_32(L2C_CFG_MP0) & ~(0x1 << L2C_SHARE_ENABLE);
	mmio_write_32(L2C_CFG_MP0, cache_cfg0);
}
