#include <mmio.h>
#include <stddef.h>
#include <debug.h>
#include <string.h>
#include <platform_def.h>
#include <delay_timer.h>
#include <mtk_mcdi.h>
#include <mtk_mcupm_reg.h>
#include <mt_spm_reg.h>
#include <plat_private.h>
#include <mtk_plat_common.h>
#include <console.h>

#define USE_MCUPM_DMA		1

static int mcdi_enable = 1;
static uint64_t mcdi_fw_base_addr;

/* used to confirm that fw is loaded successfully */
static int dyna_load_fw_done;
/* assign bin name */
static char *dyna_load_mcupm_path[] = {
	[MCUPM_FW_MCDI] = "mcdi.bin",
};
/* parse raw bin into dyna_load_mcupm */
struct dyna_load_mcupm_t dyna_load_mcupm[MCUPM_FW_MAX];

#ifndef MCUPM_FW_USE_PARTITION
static void mcdi_get_fw_base(void)
{
	/* init mcupm_bin */
	mcupm_bin.header.magic = header.magic;
	memcpy(mcupm_bin.header.name, header.name, sizeof((header.name)));
	mcupm_bin.desc.version = mcdi_mcupm.version;
	mcupm_bin.desc.pmem_words = mcdi_mcupm.pmem_words;
	mcupm_bin.desc.total_words = mcdi_mcupm.total_words;
	mcupm_bin.desc.pmem_start = mcdi_mcupm.pmem_start;
	mcupm_bin.desc.dmem_start = mcdi_mcupm.dmem_start;

	/* get mcupm_bin addr */
	mcdi_fw_base_addr = (uint64_t)&mcupm_bin;
}
#endif

static char *mcdi_load_firmware(char *name)
{
	struct mcupm_fw_header *header;
	char *addr;

	header = (struct mcupm_fw_header *) mcdi_fw_base_addr;

	while (header->magic == MCUPM_FW_MAGIC) {
		if (!strcmp(header->name, name))
			break;

		header = (struct mcupm_fw_header *) (((char *) header) +
						sizeof(*header) + header->size);
	}

	if (header->magic == MCUPM_FW_MAGIC) {
		addr = (((char *) header) + sizeof(*header));
		return addr;
	}

	return NULL;
}

static int mcdi_parse_firmware(void)
{
	int err = 0;
	int i;
	int offset = 0;
	int mcupm_fw_count = 0;

	if (dyna_load_fw_done)
		return err;

	for (i = MCUPM_FW_MCDI; i < MCUPM_FW_MAX; i++) {
		int copy_size = 0;
		struct mcupm_desc *pdesc = &(dyna_load_mcupm[i].desc);
		char *ptr;
		uint16_t firmware_size = 0;

		ptr = mcdi_load_firmware(dyna_load_mcupm_path[i]);
		if (ptr == NULL) {
			ERROR("[mcdi] Failed to load %s, err = %d.\n",
						dyna_load_mcupm_path[i], err);
			err = -1;
			continue;
		}

#ifdef MCUPM_FW_USE_PARTITION
		/* get fw size */
		offset = 0;
		copy_size = 2;
		memcpy(&firmware_size, ptr + offset, copy_size);

		/* get the start addr of binary */
		offset += copy_size;
		copy_size = (int)firmware_size * 4; /* bytes */
		dyna_load_mcupm[i].buf = (uint32_t *)(ptr + offset);
		INFO("[mcdi] binary start at %p\n", ptr + offset);

		/* start of mcupm_desc without pointer */
		offset += copy_size;
		copy_size = sizeof(struct mcupm_desc) -
				offsetof(struct mcupm_desc, pmem_words);
		memcpy((void *)&(pdesc->pmem_words), ptr + offset, copy_size);
		INFO("[mcdi] desc start at %p\n", ptr + offset);
		INFO("[mcdi] desc.pmem_words : 0x%x\n", pdesc->pmem_words);
		INFO("[mcdi] desc.dmem_start : 0x%x\n", pdesc->dmem_start);

		/* start of version */
		offset += copy_size;
		snprintf(dyna_load_mcupm[i].version, MCUPM_FW_VERSION_SIZE - 1,
				"%s", ptr + offset);
		pdesc->version = dyna_load_mcupm[i].version;
#else
		/* start of binary */
		firmware_size = MCUPM_BINARY_SIZE * 4;
		offset = firmware_size;
		dyna_load_mcupm[i].buf = (uint32_t *)ptr;

		/* start of pcm_desc without pointer */
		copy_size = sizeof(struct mcupm_desc);
		memcpy(pdesc, ptr + offset, copy_size);

		/* fw version */
		snprintf(dyna_load_mcupm[i].version, MCUPM_FW_VERSION_SIZE - 1,
				"%s", pdesc->version);
#endif /* MCUPM_FW_USE_PARTITION */

		INFO("[mcdi] use mcupmfw partition for %s - %s\n",
				dyna_load_mcupm_path[i], pdesc->version);

		dyna_load_mcupm[i].ready = 1;
		mcupm_fw_count++;
	}

	if (mcupm_fw_count == MCUPM_FW_MAX)
		dyna_load_fw_done = 1;

	return err;
}

static void mcdi_mcupm_sram_init(void)
{
	int i = 0;

	/* init mcupm debug/mbox sram to 0 */
	for (i = 0; i < MCUPM_DEBUG_SRAM_NF_WORD; i++)
		mmio_write_32(MCUPM_DEBUG_SRAM_BASE + (i * 4), 0);

	/* set avail_cpu_mask to 0xf */
	mmio_write_32(MCUPM_MCDI_AVAIL_CPU_MASK, 0xf);
}

#include <mtk_plat_common.h>

static void mcdi_fw_init(void)
{
#if USE_MCUPM_DMA
	uint32_t offset = 0x40000000;
	struct dyna_load_mcupm_t *dl = &dyna_load_mcupm[MCUPM_FW_MCDI];

	mmio_write_32(MCUPM_DMA0_SRC, (uintptr_t)dl->buf + offset);
	mmio_write_32(MCUPM_DMA0_DST, dl->desc.pmem_start);
	mmio_write_32(MCUPM_DMA0_WPTO, dl->desc.dmem_start);
	mmio_write_32(MCUPM_DMA0_WPPT, dl->desc.pmem_words);
	mmio_write_32(MCUPM_DMA0_COUNT, dl->desc.total_words);
	mmio_write_32(MCUPM_DMA0_CON, 0x0000820E);
	mmio_write_32(MCUPM_DMA0_START, 0x00008000);
#else /* !USE_MCUPM_DMA */
	uint32_t *src = dyna_load_mcupm[MCUPM_FW_MCDI].buf;
	uint32_t *dst = (void *)MCUPM_SRAM_BASE;
	uint32_t words = dyna_load_mcupm[MCUPM_FW_MCDI].desc.total_words;

	while (words-- != 0)
		*dst++ = *src++;
#endif /* USE_MCUPM_DMA */

	/* release mcupm core reset */
	mmio_setbits_32(MCUPM_MCUSYS_REQ_CON, BIT(0));

	/* spm mcusys request init and wait for dma done */
	mmio_setbits_32(SPM_POWER_ON_VAL0, BIT(1));

	while ((mmio_read_32(MCUPM_CFGREG_SW_RSTN) & BIT(0)) == 0x0)
		udelay(1000);

	/* spm mcusys request */
	mmio_setbits_32(MCUPM_MCUSYS_REQ_CON, BIT(1));

	/* unmask SPM spm_mcusys_ack */
	mmio_clrbits_32(MCUPM_MCUSYS_REQ_CON, BIT(2));
}

#ifdef MCUPM_FW_USE_PARTITION
static void mcdi_config_mcupmfw_base(uint64_t addr, uint64_t size)
{
	mcdi_fw_base_addr = addr;

	INFO("[mcdi] from LK: mcdi_base_addr 0x%lx, mcdi_base_size 0x%lx\n",
			mcdi_fw_base_addr, size);

	mmap_add_region((addr & ~(PAGE_SIZE_MASK)),
			(addr & ~(PAGE_SIZE_MASK)),
			ALIGN(size, (PAGE_SIZE_MASK)),
			MT_MEMORY | MT_RW | MT_SECURE);

	/* set up translation tables */
	init_xlat_tables();
}

void mcdi_args(uint64_t x1, uint64_t x2, uint64_t x3)
{
	uint64_t cmd = x1;

	if (!mcdi_enable) {
		INFO("[mcdi] mcdi disabled\n");
		return;
	}

	switch (cmd) {
	case MCDI_ARGS_MCUPMFW_INIT:
		console_init(gteearg.atf_log_port, UART_CLOCK, UART_BAUDRATE);
		mcdi_config_mcupmfw_base(x2, x3);
		mcdi_mcupm_sram_init(); /* init mcupm internal sram */
		mcdi_parse_firmware();
		mcdi_fw_init();
		console_uninit();
		break;
	default:
		break;
	}
}
#endif /* MCUPM_FW_USE_PARTITION */

void mcdi_mcupm_boot_init(void)
{
	if (!mcdi_enable) {
		INFO("[mcdi] mcdi disabled\n");
		return;
	}

#ifndef MCUPM_FW_USE_PARTITION
	mcdi_get_fw_base();
	mcdi_mcupm_sram_init(); /* init mcupm internal sram */
	mcdi_parse_firmware();
	mcdi_fw_init();
#endif
}

void mcdi_mcupm_cluster_auto_off_enable(unsigned long cpu)
{
	if (!mcdi_enable)
		return;

	mmio_write_32(MCUPM_CFGREG_WFI_EN_SET, 0xf << 16);
}

void mcdi_mcupm_cluster_auto_off_disable(unsigned long cpu)
{
	if (!mcdi_enable)
		return;

	mmio_write_32(MCUPM_CFGREG_WFI_EN_CLR, BIT(cpu + 16));
}

void mcdi_mcupm_standbywfi_irq_enable(unsigned long cpu)
{
	if (!mcdi_enable)
		return;

	mmio_write_32(MCUPM_CFGREG_WFI_EN_SET, BIT(cpu));
}

void mcdi_mcupm_wakeup_irq_enable(unsigned long cpu)
{
}

#define MCUPM_CLUSTER_OFF_CNT	(SPM_BASE + 0x618)
#define MCUPM_HP_CMD_SET	(SPM_BASE + 0x4b4)
#define MCUPM_IDLE		(SPM_BASE + 0x4b0)
#define MCUPM_HP_OFF_OFS	0
#define MCUPM_HP_ON_OFS		4

int mcdi_mcupm_is_first_wakeup_cpu(unsigned long cpu)
{
	uint32_t cnt;
	int r = 0;

	if (!mcdi_enable)
		return 0;

	cnt = mmio_read_32(MCUPM_CLUSTER_OFF_CNT);
	r = (cnt & BIT(3)) && ((cnt & 0x7) == cpu);

	/* clear valid bit */
	if (r)
		mmio_write_32(MCUPM_CLUSTER_OFF_CNT, cnt & 0xfffffff7);

	return r;
}

void mcupm_hp_off(int cpu)
{
	mmio_write_32(MCUPM_HP_CMD_SET, BIT(MCUPM_HP_OFF_OFS + cpu));
}

void mcupm_hp_on(int cpu)
{
	mmio_write_32(MCUPM_HP_CMD_SET, BIT(MCUPM_HP_ON_OFS + cpu));
}

void mcupm_hp_idle(void)
{
	mmio_write_32(MCUPM_HP_CMD_SET, 0xffff);
	while (mmio_read_32(MCUPM_IDLE) != 0x0)
		;
}

void mcupm_hold_req(void)
{
	mmio_clrbits_32(MCUPM_MCUSYS_REQ_CON, BIT(0));
	mmio_clrbits_32(MCUPM_MCUSYS_REQ_CON, BIT(1));
}

void mcupm_release_req(void)
{
	mmio_setbits_32(MCUPM_MCUSYS_REQ_CON, BIT(0));
}
