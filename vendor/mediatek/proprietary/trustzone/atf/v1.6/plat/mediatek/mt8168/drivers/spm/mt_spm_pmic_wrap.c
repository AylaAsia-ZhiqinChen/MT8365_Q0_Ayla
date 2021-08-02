#include <arch_helpers.h>
#include <debug.h>
#include <mmio.h>
#include <mt_spm.h>
#include <mt_spm_internal.h>
#include <mt_spm_reg.h>
#include <mt_spm_vcorefs.h>
#include <mt_spm_pmic_wrap.h>
#include <plat_pm.h>
#include <platform.h>
#include <platform_def.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* PMIC_WRAP - MT6357 */
#define VCORE_BASE_UV		51875 /* PMIC MT6357 */
#define VOLT_TO_PMIC_VAL(volt)	(((volt) - VCORE_BASE_UV + 625 - 1) / 625)
#define PMIC_VAL_TO_VOLT(pmic)	(((pmic) * 625) + VCORE_BASE_UV)

#define DEFAULT_VOLT_VSRAM      (100000)
#define DEFAULT_VOLT_VCORE      (100000)
#define NR_PMIC_WRAP_CMD	(NR_IDX_ALL)
#define SPM_DATA_SHIFT		(16)

struct pmic_wrap_cmd {
	unsigned long cmd_addr;
	unsigned long cmd_wdata;
};

struct pmic_wrap_setting {
	enum pmic_wrap_phase_id phase;
	struct pmic_wrap_cmd addr[NR_PMIC_WRAP_CMD];
	struct {
		struct {
			unsigned long cmd_addr;
			unsigned long cmd_wdata;
		} _[NR_PMIC_WRAP_CMD];
		const int nr_idx;
	} set[NR_PMIC_WRAP_PHASE];
};

static struct pmic_wrap_setting pw = {
	.phase = NR_PMIC_WRAP_PHASE,
	.addr = {{0, 0} },
	.set[PMIC_WRAP_PHASE_ALLINONE] = {
		._[CMD_0] = {RG_BUCK_VCORE_VOSEL, VOLT_TO_PMIC_VAL(65000),},
		._[CMD_1] = {RG_BUCK_VCORE_VOSEL, VOLT_TO_PMIC_VAL(70000),},
		._[CMD_2] = {RG_BUCK_VCORE_VOSEL, VOLT_TO_PMIC_VAL(80000),},
		._[CMD_3] = {RG_LDO_VSRAM_PROC_VOSEL, VOLT_TO_PMIC_VAL(80000),},
		._[CMD_4] = {RG_LDO_VSRAM_PROC_VOSEL, VOLT_TO_PMIC_VAL(90000),},
		._[CMD_5] = {0, 0,},
		._[CMD_6] = {TOP_SPI_CON0, 1,},
		._[CMD_7] = {TOP_SPI_CON0, 0,},
		._[CMD_8] = {RG_BUCK_VPROC_VOSEL, VOLT_TO_PMIC_VAL(60000),},
		/* 1V should be reset before dpidle/sodi */
		._[CMD_9] = {RG_BUCK_VPROC_VOSEL, VOLT_TO_PMIC_VAL(100000),},
		._[CMD_10] = {RG_LDO_VSRAM_OTHERS_VOSEL,
			      VOLT_TO_PMIC_VAL(60000),},
		/* 1V should be reset before dpidle/sodi */
		._[CMD_11] = {RG_LDO_VSRAM_OTHERS_VOSEL,
			      VOLT_TO_PMIC_VAL(100000),},
		._[CMD_12] = {RG_BUCK_VPROC_EN, 0,},
		._[CMD_13] = {RG_BUCK_VPROC_EN, 1,},
		._[CMD_14] = {RG_LDO_VSRAM_OTHERS_EN, 0,},
		._[CMD_15] = {RG_LDO_VSRAM_OTHERS_EN, 1,},
		.nr_idx = NR_IDX_ALL,
	},
};

void _mt_spm_pmic_table_init(void)
{
	struct pmic_wrap_cmd pwrap_cmd_default[NR_PMIC_WRAP_CMD] = {
		{(uint32_t)SPM_DVFS_CMD0, (uint32_t)SPM_DVFS_CMD0,},
		{(uint32_t)SPM_DVFS_CMD1, (uint32_t)SPM_DVFS_CMD1,},
		{(uint32_t)SPM_DVFS_CMD2, (uint32_t)SPM_DVFS_CMD2,},
		{(uint32_t)SPM_DVFS_CMD3, (uint32_t)SPM_DVFS_CMD3,},
		{(uint32_t)SPM_DVFS_CMD4, (uint32_t)SPM_DVFS_CMD4,},
		{(uint32_t)SPM_DVFS_CMD5, (uint32_t)SPM_DVFS_CMD5,},
		{(uint32_t)SPM_DVFS_CMD6, (uint32_t)SPM_DVFS_CMD6,},
		{(uint32_t)SPM_DVFS_CMD7, (uint32_t)SPM_DVFS_CMD7,},
		{(uint32_t)SPM_DVFS_CMD8, (uint32_t)SPM_DVFS_CMD8,},
		{(uint32_t)SPM_DVFS_CMD9, (uint32_t)SPM_DVFS_CMD9,},
		{(uint32_t)SPM_DVFS_CMD10, (uint32_t)SPM_DVFS_CMD10,},
		{(uint32_t)SPM_DVFS_CMD11, (uint32_t)SPM_DVFS_CMD11,},
		{(uint32_t)SPM_DVFS_CMD12, (uint32_t)SPM_DVFS_CMD12,},
		{(uint32_t)SPM_DVFS_CMD13, (uint32_t)SPM_DVFS_CMD13,},
		{(uint32_t)SPM_DVFS_CMD14, (uint32_t)SPM_DVFS_CMD14,},
		{(uint32_t)SPM_DVFS_CMD15, (uint32_t)SPM_DVFS_CMD15,},
	};

	memcpy(pw.addr, pwrap_cmd_default, sizeof(pwrap_cmd_default));
}

void mt_spm_pmic_wrap_set_phase(enum pmic_wrap_phase_id phase)
{
	int idx, addr, data;

	if (phase >= NR_PMIC_WRAP_PHASE) {
		ERROR("max phase = %d, target phase = %d?\n",
		      NR_PMIC_WRAP_PHASE, phase);
		return;
	}

	if (pw.phase == phase) {
		ERROR("same phase c:%d, t:%d, ignore this request.\n",
		      pw.phase, phase);
		return;
	}

	if (pw.addr[0].cmd_addr == 0)
		_mt_spm_pmic_table_init();

	pw.phase = phase;

	for (idx = 0; idx < pw.set[phase].nr_idx; idx++) {
		addr = pw.set[phase]._[idx].cmd_addr << SPM_DATA_SHIFT;
		data = pw.set[phase]._[idx].cmd_wdata;
		/* INFO("addr = 0x%x, data = 0x%x\n", addr, data); */
		mmio_write_32(pw.addr[idx].cmd_addr, addr | data);
	}
}

void mt_spm_pmic_wrap_set_cmd(enum pmic_wrap_phase_id phase,
			      uint32_t idx, uint32_t cmd_wdata)
{
	uint32_t addr;

	if (phase >= NR_PMIC_WRAP_PHASE)
		return;

	if (idx >= pw.set[phase].nr_idx)
		return;

	/* just set wdata value */
	pw.set[phase]._[idx].cmd_wdata = cmd_wdata;

	if (pw.phase == phase) {
		addr = pw.set[phase]._[idx].cmd_addr << SPM_DATA_SHIFT;
		mmio_write_32(pw.addr[idx].cmd_addr, addr | cmd_wdata);
	}

}

uint64_t mt_spm_pmic_wrap_get_cmd(enum pmic_wrap_phase_id phase, uint32_t idx)
{
	if (phase >= NR_PMIC_WRAP_PHASE)
		return 0;

	if (idx >= pw.set[phase].nr_idx)
		return 0;

	/* just get wdata value */
	return pw.set[phase]._[idx].cmd_wdata;
}
