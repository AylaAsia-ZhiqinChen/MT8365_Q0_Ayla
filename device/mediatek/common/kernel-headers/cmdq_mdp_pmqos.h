/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __CMDQ_MDP_PMQOS_H__
#define __CMDQ_MDP_PMQOS_H__

#define PMQOS_ISP_PORT_NUM  16
#define PMQOS_MDP_PORT_NUM  4

struct mdp_pmqos {
	uint32_t isp_total_datasize;
	uint32_t isp_total_pixel;
	uint32_t mdp_total_datasize;
	uint32_t mdp_total_pixel;

	uint32_t qos2_isp_bandwidth[PMQOS_ISP_PORT_NUM];
	uint32_t qos2_isp_total_pixel[PMQOS_ISP_PORT_NUM];
	uint32_t qos2_isp_port[PMQOS_ISP_PORT_NUM];
	uint32_t qos2_isp_count;

	uint32_t qos2_mdp_bandwidth[PMQOS_MDP_PORT_NUM];
	uint32_t qos2_mdp_total_pixel[PMQOS_MDP_PORT_NUM];
	uint32_t qos2_mdp_port[PMQOS_MDP_PORT_NUM];
	uint32_t qos2_mdp_count;

	uint64_t tv_sec;
	uint64_t tv_usec;

	uint64_t ispMetString;
	uint32_t ispMetStringSize;
	uint64_t mdpMetString;
	uint32_t mdpMetStringSize;

	uint64_t mdpMMpathString;
	uint32_t mdpMMpathStringSize;
};
#endif
