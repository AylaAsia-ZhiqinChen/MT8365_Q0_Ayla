/* Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <reg.h>
#include <debug.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <platform.h>
#include <platform/mt_typedefs.h>
#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <platform/sec_devinfo.h>
#include <platform/env.h>
#include <platform/sec_export.h>
//#include <dfo_boot_default.h>
extern int g_nr_bank;
extern BOOT_ARGUMENT *g_boot_arg;
extern BI_DRAM bi_dram[MAX_NR_BANK];
#ifndef MACH_FPGA
extern int get_meta_port_id(void);
#endif
extern unsigned int g_fb_base;
extern unsigned int g_fb_size;


struct tag_header {
	u32 size;
	u32 tag;
};

#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define SIZE_1M             (1024 * 1024)
#define SIZE_2M             (2 * SIZE_1M)
#define SIZE_256M           (256 * SIZE_1M)
#define SIZE_512M           (512 * SIZE_1M)

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE   0x54410001
struct tag_core {
	u32 flags;      /* bit 0 = read-only */
	u32 pagesize;
	u32 rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM  0x54410002
typedef struct {
	uint32_t size;
	uint32_t start_addr;
} mem_info;

#define ATAG_MEM64  0x54420002
typedef struct {
	uint64_t size;
	uint64_t start_addr;
} mem64_info;

#define ATAG_EXT_MEM64  0x54430002
typedef struct {
	uint64_t size;
	uint64_t start_addr;
} ext_mem64_info;

/* command line: \0 terminated string */
#define ATAG_CMDLINE    0x54410009
struct tag_cmdline {
	char    cmdline[1]; /* this is the minimum size */
};

/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD2    0x54420005
struct tag_initrd {
	u32 start;  /* physical start address */
	u32 size;   /* size of compressed ramdisk image in bytes */
};

#define ATAG_VIDEOLFB   0x54410008
struct tag_videolfb {
	u64 fb_base;
	u32 islcmfound;
	u32 fps;
	u32 vram;
	char lcmname[1]; /* this is the minimum size */
};


/* boot information */
#define ATAG_BOOT   0x41000802
struct tag_boot {
	u32 bootmode;
};

/*META com port information*/
#define ATAG_META_COM 0x41000803
struct tag_meta_com {
	u32 meta_com_type; /* identify meta via uart or usb */
	u32 meta_com_id;  /* multiple meta need to know com port id */
	u32 meta_uart_port; /* identify meta uart port */
};

/*device information*/
#define ATAG_DEVINFO_DATA_SIZE    200

#define ATAG_MDINFO_DATA 0x41000806
struct tag_mdinfo_data {
	u8 md_type[4];
};

#define ATAG_VCORE_DVFS_INFO 0x54410007
struct tag_vcore_dvfs_info {
	u32 pllgrpreg_size;
	u32 freqreg_size;
	u32* low_freq_pll_val;
	u32* low_freq_cha_val;
	u32* low_freq_chb_val;
	u32* high_freq_pll_val;
	u32* high_freq_cha_val;
	u32* high_freq_chb_val;
};

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE   0x00000000

unsigned *target_atag_nand_data(unsigned *ptr)
{
	return ptr;
}
unsigned *target_atag_partition_data(unsigned *ptr)
{
	return ptr;
}


unsigned *target_atag_boot(unsigned *ptr)
{
	*ptr++ = tag_size(tag_boot);
	*ptr++ = ATAG_BOOT;
	*ptr++ = g_boot_mode;

	return ptr;
}

extern int imix_r;
unsigned *target_atag_imix_r(unsigned *ptr)
{
	*ptr++ = imix_r;

	dprintf(CRITICAL, "target_atag_imix_r:%d\n",imix_r);
	return ptr;
}

unsigned *target_atag_devinfo_data(unsigned *ptr)
{
	int i = 0;
	*ptr++ = ATAG_DEVINFO_DATA_SIZE;
	for (i=0; i<ATAG_DEVINFO_DATA_SIZE; i++) {
		*ptr++ = get_devinfo_with_index(i);
	}

	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(0));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(1));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(2));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(3));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(4));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(20));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(21));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(30));

	return ptr;
}

#ifdef MTK_SECURITY_SW_SUPPORT
unsigned *target_atag_masp_data(unsigned *ptr)
{
	/*tag size*/
	*ptr++ = tag_size(tag_masp_data);
	/*tag name*/
	*ptr++ = ATAG_MASP_DATA;
	ptr = fill_atag_masp_data(ptr);

	return ptr;
}
#endif

unsigned *target_atag_mem(unsigned *ptr)
{
	int i;

	for (i = 0; i < g_nr_bank; i++) {
#ifndef MTK_LM_MODE
		*ptr++ = 4; //tag size
		*ptr++ = ATAG_MEM; //tag name
		*ptr++ = bi_dram[i].size;
		*ptr++ = bi_dram[i].start;
#else
		*ptr++ = 6; //tag size
		*ptr++ = ATAG_MEM64; //tag name
		//*((unsigned long long*)ptr)++ = bi_dram[i].size;
		//*((unsigned long long*)ptr)++ = bi_dram[i].start;
		unsigned long long *ptr64 = (unsigned long long *)ptr;
		*ptr64++ = bi_dram[i].size;
		*ptr64++ = bi_dram[i].start;
		ptr = (unsigned int *)ptr64;
#endif
	}
	return ptr;
}

unsigned *target_atag_meta(unsigned *ptr)
{
	*ptr++ = tag_size(tag_meta_com);
	*ptr++ = ATAG_META_COM;
	*ptr++ = g_boot_arg->meta_com_type;
	*ptr++ = g_boot_arg->meta_com_id;
#ifndef MACH_FPGA
	*ptr++ = get_meta_port_id();
#endif
	dprintf(CRITICAL, "meta com type = %d\n", g_boot_arg->meta_com_type);
	dprintf(CRITICAL, "meta com id = %d\n", g_boot_arg->meta_com_id);
#ifndef MACH_FPGA
	dprintf(CRITICAL, "meta uart port = %d\n", get_meta_port_id());
#endif
	return ptr;
}

/* todo: give lk strtoul and nuke this */
static unsigned hex2unsigned(const char *x)
{
	unsigned n = 0;

	while (*x) {
		switch (*x) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = (n << 4) | (*x - '0');
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				n = (n << 4) | (*x - 'a' + 10);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				n = (n << 4) | (*x - 'A' + 10);
				break;
			default:
				return n;
		}
		x++;
	}

	return n;
}
#if 0
#define ATAG_DFO_DATA 0x41000805
unsigned *target_atag_dfo(unsigned *ptr)
{
	int i, j;
	dfo_boot_info *dfo_p;
	char tmp[11];
	unsigned  char *buffer;

	*ptr++ = ((sizeof(struct tag_header) + DFO_BOOT_COUNT * sizeof(dfo_boot_info)) >> 2);
	*ptr++ = ATAG_DFO_DATA;

	memcpy((void *)ptr, (void *)dfo_boot_default, DFO_BOOT_COUNT * sizeof(dfo_boot_info));
	dfo_p = (dfo_boot_info *)ptr;

	ptr += DFO_BOOT_COUNT * sizeof(dfo_boot_info) >> 2;

	buffer = (unsigned char *)get_env("DFO");

	if (buffer != NULL) {

		for (i = 0; i < DFO_BOOT_COUNT; i++) {
			j = 0;
			do {
				dfo_p[i].name[j] = *buffer;
				j++;
			} while (*buffer++ != ',' && j < 31);

			dfo_p[i].name[j-1] = '\0';
			j = 0;

			do {
				tmp[j] = *buffer;
				j++;
			} while (*buffer++ != ',' && j < 10);

			tmp[j] = '\0';

			if ((strncmp("0x", tmp, 2) == 0) || (strncmp("0X", tmp, 2) == 0))
				dfo_p[i].value = hex2unsigned(&tmp[2]);
			else
				dfo_p[i].value = atoi(tmp);
		}

		for (i = 0; i < DFO_BOOT_COUNT; i++)
			dprintf(INFO, "[DFO-%d] NAME:%s, Value:%lu\n",i , dfo_p[i].name, dfo_p[i].value);

	} else
		dprintf(INFO, "No DFO. Use default values.\n");

	return ptr;
}
#endif

unsigned *target_atag_initrd(unsigned *ptr, ulong initrd_start, ulong initrd_size)
{
	*ptr++ = tag_size(tag_initrd);
	*ptr++ = ATAG_INITRD2;
//TMP for bring up testing
//  *ptr++ = CFG_RAMDISK_LOAD_ADDR;
	// *ptr++ = 0x1072F9;

	*ptr++ = initrd_start;
	*ptr++ = initrd_size;
	return ptr;
}

#include <platform/mt_disp_drv.h>
#include <platform/disp_drv.h>
unsigned *target_atag_videolfb(unsigned *ptr, size_t buf_size)
{
	extern unsigned long long fb_addr_pa_k;
	const char   *lcmname = mt_disp_get_lcm_id();
	unsigned int *p       = NULL;
	unsigned long long *phy_p   = (unsigned long long *)ptr;
	int lcd_time;

#ifdef MACH_FPGA
	*phy_p = fb_addr_pa_k = g_fb_base;
#else
	*phy_p = fb_addr_pa_k;
#endif
	p = (unsigned int*)(phy_p + 1);
	*p++ = DISP_IsLcmFound();
#ifdef MACH_FPGA
	*p++ = lcd_time = 6000;
#else
	*p++ = lcd_time = mt_disp_get_lcd_time();
#endif
	*p++ = g_fb_size;
	strcpy((char *)p,lcmname);
	p += (strlen(lcmname) + 1 + 4) >> 2;

	dprintf(CRITICAL, "videolfb - fb_base    = 0x%llx\n",fb_addr_pa_k);
	dprintf(CRITICAL, "videolfb - islcmfound = %d\n",DISP_IsLcmFound());
	dprintf(CRITICAL, "videolfb - fps        = %d\n",lcd_time);
	dprintf(CRITICAL, "videolfb - vram       = %d\n",g_fb_size);
	dprintf(CRITICAL, "videolfb - lcmname    = %s\n",lcmname);

	return (unsigned *)p;

}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
unsigned *target_atag_ext_videolfb(unsigned *ptr)
{
	extern unsigned long long ext_fb_addr_pa_k;
	const char   *ext_lcmname = mt_ext_disp_get_lcm_id();
	unsigned int *p 	  = NULL;
	unsigned long long *ext_phy_p = (unsigned long long *)ptr;
	unsigned ext_fps = 6000; //todo?

	*ext_phy_p = ext_fb_addr_pa_k;
	p = (unsigned int*)(ext_phy_p + 1);
	*p++ = EXT_DISP_IsLcmFound();
	*p++ = ext_fps;
	*p++ = EXT_DISP_GetVRamSize();
	strcpy((char *)p,ext_lcmname);
	p += (strlen(ext_lcmname) + 1 + 4) >> 2;

	dprintf(CRITICAL, "videolfb - ext_fb_base	 = 0x%llx\n",ext_fb_addr_pa_k);
	dprintf(CRITICAL, "videolfb - ext_islcmfound = %d\n",EXT_DISP_IsLcmFound());
	dprintf(CRITICAL, "videolfb - ext_fps		 = %d\n",ext_fps);
	dprintf(CRITICAL, "videolfb - ext_vram		 = %d\n",EXT_DISP_GetVRamSize());
	dprintf(CRITICAL, "videolfb - ext_lcmname	 = %s\n",ext_lcmname);

	return (unsigned *)p;

}
#endif

unsigned *target_atag_mdinfo(unsigned *ptr)
{
	unsigned char *p;
	*ptr++=tag_size(tag_mdinfo_data);
	*ptr++=ATAG_MDINFO_DATA;
	p=(unsigned char *)ptr;
	*p++=g_boot_arg->md_type[0];
	*p++=g_boot_arg->md_type[1];
	*p++=g_boot_arg->md_type[2];
	*p++=g_boot_arg->md_type[3];
	return (unsigned *)p;
}

unsigned *target_atag_isram(unsigned *ptr)
{
	unsigned char *p;
	p=(unsigned char *)ptr;
	memcpy(p, (unsigned char*)&g_boot_arg->non_secure_sram_addr, sizeof(u32));
	p=p+sizeof(u32);
	memcpy(p, (unsigned char*)&g_boot_arg->non_secure_sram_size, sizeof(u32));
	p=p+sizeof(u32);
	dprintf(CRITICAL,"[LK] non_secure_sram (0x%x, 0x%x)\n", g_boot_arg->non_secure_sram_addr, g_boot_arg->non_secure_sram_size);
	return (unsigned *)p;
}

void *target_get_scratch_address(void)
{
	return ((void *)SCRATCH_ADDR);
}

unsigned *target_atag_lastpc(unsigned *ptr)
{
	int i, j;

	*ptr++=tag_size(boot_tag_lastpc);
	*ptr++=BOOT_TAG_LASTPC;

	for (i=0; i<4; i++)
		for (j=0; j<8; j++)
		{
			*ptr++ = g_boot_arg->lastpc[i][j];
			dprintf(0, "LASTPC[%d][%d] = %x\n", i, j, g_boot_arg->lastpc[i][j]);
		}

	return (unsigned *)ptr;
}

#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#if CONFGI_EXT_PMIC_MT6311
extern int is_mt6311_exist(void);
#endif
#define NUM_CLUSTERS  1
#define NUM_CORES_CLUSTER0  4
#define NUM_CORES_CLUSTER1  0
#define MAX_CLK_FREQ  2000000000


/*
 * 0000: Qual core
 * 1000: Triple core
 * 1100: Dual core
 * 1110: Single core
 * 1111: All disable
 */
#define DEVINFO_CPU_QUAL_CORE   0x0
#define DEVINFO_CPU_TRIPLE_CORE 0x8
#define DEVINFO_CPU_DUAL_CORE   0xC
#define DEVINFO_CPU_SINGLE_CORE 0xE
#define DEVINFO_CPU_ZERO_CORE   0xF

struct cpu_dev_info {
	unsigned int speed    : 3;
	unsigned int cluster0 : 4;
	unsigned int cluster1 : 4;
	unsigned int reserve  : 21;
};

unsigned int dev_info_max_clk_freq(void)
{
	unsigned int devinfo = get_devinfo_with_index(3);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;
	unsigned int max_clk_freq = 1300 * 1000 * 1000;

	/*
	  Free (default,3'b000), set as FY 1.3GHz
	  1.5GHz (3'b001)
	  1.4GHz (3'b111)
	  1.3GHz (3'b010)
	  1.2GHz (3'b011)
	  1.1 GHz (3'b100)
	  1.0GHz (3'b101)
	*/

	switch ((unsigned int)info->speed) {
		case 1: /* 1.5G */
			max_clk_freq = 1481 * 1000 * 1000;
			break;

		case 7: /* 1.4G */
			max_clk_freq = 1404 * 1000 * 1000;
			break;

		case 0: /* 1.3G - default FY IC */
		case 2: /* 1.3G */
			max_clk_freq = 1300 * 1000 * 1000;
			break;

		case 4: /* 1.1G */
			max_clk_freq = 1105 * 1000 * 1000;
			break;

		case 3: /* 1.2G, NOT support */
		case 5: /* 1.0G, NOT support */
		case 6: /* NOT Defined */
		default:
			break;
	}

	dprintf(CRITICAL, "efuse set max_clk_freq=%d\n", max_clk_freq);

	return max_clk_freq;
}

int dev_info_nr_cpu(void)
{
	unsigned int devinfo = get_devinfo_with_index(3);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;
	int cluster[NUM_CLUSTERS];
	int cluster_idx;
	int nr_cpu = 0;

	memset(cluster, 0, sizeof(cluster));

	cluster[0] = info->cluster0;
	cluster[1] = info->cluster1;

	for (cluster_idx = 0; cluster_idx < NUM_CLUSTERS; cluster_idx++) {
		switch (cluster[cluster_idx]) {
			case DEVINFO_CPU_QUAL_CORE:
				nr_cpu += 4;
				break;
			case DEVINFO_CPU_TRIPLE_CORE:
				nr_cpu += 3;
				break;
			case DEVINFO_CPU_DUAL_CORE:
				nr_cpu += 2;
				break;
			case DEVINFO_CPU_SINGLE_CORE:
				nr_cpu += 1;
				break;
			case DEVINFO_CPU_ZERO_CORE:
				break;
			default:
				nr_cpu += 4;
				break;
		}
	}

	return nr_cpu;
}

int target_fdt_model(void *fdt)
{
	unsigned int proj_code = get_devinfo_with_index(30) & 0xF0;
	unsigned int segment = get_devinfo_with_index(30) & 0x0F;
	int code = 0;
	int len;
	int nodeoffset;
	const struct fdt_property *prop;
	char *prop_name = "model";
	const char *model_name[] = {
		"MT8168A",
		"MT8168B",
		"MT8168K",
		"MT8175B",
	};

	switch (segment) {
		case 0x3:
			code = 2;		/* MT8168K */
			break;
		case 0x2:
			code = 1;		/* MT8168B */
			break;
		case 0x1:
			code = 0;		/* MT8168A */
			if (0x20 == proj_code)
				code = 3;	/* MT8175B */
			break;
		default:
			code = 0;
			break;
	}

	/* Becuase the model is at the begin of device tree.
	 * use nodeoffset=0
	 */
	nodeoffset = 0;
	prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);

	if (prop) {
		int namestroff;
		const char *str = model_name[code];
		//printf("prop->data=0x%8x\n", (uint32_t *)prop->data);
		fdt_setprop_string(fdt, nodeoffset, prop_name, str);
		prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);
		namestroff = fdt32_to_cpu(prop->nameoff);
		dprintf(CRITICAL, "%s=%s\n", fdt_string(fdt, namestroff), (char *)prop->data);
	}

	return 0;
}

int target_fdt_firmware(void *fdt, char *serialno)
{
	int nodeoffset;
	char *name, *value;

	nodeoffset = fdt_path_offset(fdt, "/firmware/android");
	if (nodeoffset < 0) {
		nodeoffset = fdt_path_offset(fdt, "/firmware");
		if (nodeoffset < 0) {
			/* Add subnode "firmware" in root */
			nodeoffset = fdt_add_subnode(fdt, 0, "firmware");
			if (nodeoffset < 0) {
				dprintf(CRITICAL, "Warning: can't add firmware node in device tree\n");
				return -1;
			}
		}

		/* Add subnode "android" in "/firmware" */
		nodeoffset = fdt_add_subnode(fdt, nodeoffset, "android");
		if (nodeoffset < 0) {
			dprintf(CRITICAL, "Warning: can't add firmware/android node in device tree\n");
			return -1;
		}
	}

	/* Add properties in "/firmware/android" */
	name = "compatible";
	value = "android,firmware";
	fdt_setprop_string(fdt, nodeoffset, name, value);

	name = "hardware";
	value = PLATFORM;
#ifdef MTK_PRODUCT_LINE_PLATFORM
	value = MTK_PRODUCT_LINE_PLATFORM;
#endif
	{
		char *tmp_str = value;
		for ( ; *tmp_str; tmp_str++ ) {
			*tmp_str = tolower(*tmp_str);
		}
	}
	fdt_setprop_string(fdt, nodeoffset, name, value);

	name = "serialno";
	value = serialno;
	fdt_setprop_string(fdt, nodeoffset, name, value);

	name = "mode";
	switch (g_boot_mode) {
		case META_BOOT:
		case ADVMETA_BOOT:
			value = "meta";
			break;
#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
		case KERNEL_POWER_OFF_CHARGING_BOOT:
		case LOW_POWER_OFF_CHARGING_BOOT:
			value = "charger";
			break;
#endif
		case FACTORY_BOOT:
		case ATE_FACTORY_BOOT:
			value = "factory";
			break;
		case RECOVERY_BOOT:
			value = "recovery";
			break;
		default:
			value = "normal";
			break;
	}

	fdt_setprop_string(fdt, nodeoffset, name, value);

	return 0;
}

int target_fdt_cpus(void *fdt)
{
	int cpus_offset, cpu_node, last_node = -1;
	int len;
	const struct fdt_property *prop;
	unsigned int *data;
	unsigned int reg, clk_freq;

	unsigned int cluster_idx;
	unsigned int core_num;

	unsigned int activated_cores[NUM_CLUSTERS] = {0};
	unsigned int available_cores[NUM_CLUSTERS] = {NUM_CORES_CLUSTER0, NUM_CORES_CLUSTER1};
	unsigned int max_clk_freq = dev_info_max_clk_freq();
	unsigned int devinfo = get_devinfo_with_index(3);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;

	dprintf(INFO, "info->cluster0=0x%x\n", info->cluster0);
	dprintf(INFO, "info->cluster1=0x%x\n", info->cluster1);
	dprintf(INFO, "info->speed=0x%x\n", info->speed);

	switch ((unsigned int)info->cluster0) {
		case DEVINFO_CPU_QUAL_CORE:
			available_cores[0] = 4;
			break;
		case DEVINFO_CPU_TRIPLE_CORE:
			available_cores[0] = 3;
			break;
		case DEVINFO_CPU_DUAL_CORE:
			available_cores[0] = 2;
			break;
		case DEVINFO_CPU_SINGLE_CORE:
			available_cores[0] = 1;
			break;
		/* core 0 doesn't have zore core case*/
		default:
			available_cores[0] = 4;
			break;
	}

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0) {
		dprintf(CRITICAL, "couldn't find /cpus\n");
		return cpus_offset;
	}

	for (cpu_node = fdt_first_subnode(fdt, cpus_offset); cpu_node >= 0;
	        cpu_node = ((last_node >= 0) ? fdt_next_subnode(fdt, last_node) : fdt_first_subnode(fdt, cpus_offset))) {
		prop = fdt_get_property(fdt, cpu_node, "device_type", &len);
		if ((!prop) || (len < 4) || (strcmp(prop->data, "cpu"))) {
			last_node = cpu_node;
			continue;
		}

		prop = fdt_get_property(fdt, cpu_node, "reg", &len);

		data = (uint32_t *)prop->data;
		reg = fdt32_to_cpu(*data);

		dprintf(INFO, "reg = 0x%x\n", reg);
		core_num = reg & 0xFF;
		cluster_idx = (reg & 0x100) >> 8;
		dprintf(INFO, "cluster_idx=%d, core_num=%d\n", cluster_idx, core_num);

		if (core_num >= available_cores[cluster_idx]) {
			dprintf(INFO, "delete: cluster = %d, core = %d\n", cluster_idx, core_num);
			fdt_del_node(fdt, cpu_node);
		}
		/* ========== */
		else {
			activated_cores[cluster_idx]++;
			last_node = cpu_node;
		}

		if (cluster_idx == NUM_CLUSTERS) {
			dprintf(CRITICAL, "Warning: unknown cpu type in device tree\n");
			last_node = cpu_node;
		}
	}

	for (cluster_idx = 0; cluster_idx < NUM_CLUSTERS; cluster_idx++) {
		if (activated_cores[cluster_idx] > available_cores[cluster_idx])
			dprintf(CRITICAL, "Warning: unexpected reg value in device tree\n");

		dprintf(CRITICAL, "cluster-%d: %d core\n", cluster_idx, available_cores[cluster_idx]);
	}


	return 0;
}

int platform_atag_append(void *fdt)
{
	char *ptr;
	int ret = 0;
	int offset;
	char buf[136] = {0};

	offset = fdt_path_offset(fdt, "/chosen");

	ptr = (char *)target_atag_lastpc((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,lastpc", buf, ptr - buf);
	if (ret)
		return 1;

	return 0;
}

#endif //DEVICE_TREE_SUPPORT
