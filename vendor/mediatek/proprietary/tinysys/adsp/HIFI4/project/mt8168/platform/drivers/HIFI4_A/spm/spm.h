#ifndef _SPM_H_
#define _SPM_H_

enum {
	SPM_NONE     = 0,   /* no request or fake 26M use */
	SPM_26M      = 0x1, /* srclken enable */
	SPM_INFRA    = 0x3, /* infra enable */
	SPM_PLL      = 0x7, /* vrf18 pll enable */
	SPM_DRAM     = 0x1F,/* apsrc & ddren enable */
	SPM_RES_NUM  = 5,
};

int spm_res_set(int req);
int spm_res_get(void);

#endif
