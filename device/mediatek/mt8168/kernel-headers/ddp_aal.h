#ifndef __DDP_AAL_H__
#define __DDP_AAL_H__

#define AAL_HIST_BIN            33	/* [0..32] */
#define AAL_DRE_POINT_NUM       29

#define AAL_DRE30_GAIN_REGISTER_NUM		(544)
#define AAL_DRE30_HIST_REGISTER_NUM		(768)

#define AAL_SERVICE_FORCE_UPDATE 0x1

/* Custom "wide" pointer type for 64-bit compatibility. */
/* Always cast from uint32_t*. */
/* typedef unsigned long long aal_u32_ptr_t; */
#define aal_u32_handle_t unsigned long long

enum AAL_ESS_UD_MODE {
	CONFIG_BY_CUSTOM_LIB = 0,
	CONFIG_TO_LCD = 1,
	CONFIG_TO_AMOLED = 2
};

enum AAL_DRE_MODE {
	DRE_EN_BY_CUSTOM_LIB = 0xFFFF,
	DRE_OFF = 0,
	DRE_ON = 1
};

enum AAL_ESS_MODE {
	ESS_EN_BY_CUSTOM_LIB = 0xFFFF,
	ESS_OFF = 0,
	ESS_ON = 1
};

enum AAL_ESS_LEVEL {
	ESS_LEVEL_BY_CUSTOM_LIB = 0xFFFF
};

typedef struct {
	/* DRE */
	int dre_map_bypass;
	/* ESS */
	int cabc_gainlmt[33];
	/* DRE 3.0 Reg. */
	int dre_s_lower;
	int dre_s_upper;
	int dre_y_lower;
	int dre_y_upper;
	int dre_h_lower;
	int dre_h_upper;
	int dre_x_alpha_base;
	int dre_x_alpha_shift_bit;
	int dre_y_alpha_base;
	int dre_y_alpha_shift_bit;
	int act_win_x_end;
	int dre_blk_x_num;
	int dre_blk_y_num;
	int dre_blk_height;
	int dre_blk_width;
	int dre_blk_area;
	int dre_blk_area_min;
	int hist_bin_type;
	int dre_flat_length_slope;
} DISP_AAL_INITREG;

typedef struct {
	/* DRE 3.0 SW */
	aal_u32_handle_t dre30_hist_addr;
} DISP_DRE30_INIT;

typedef struct {
	int width;
	int height;
} DISP_AAL_DISPLAY_SIZE;

typedef struct {
	unsigned int dre_hist[AAL_DRE30_HIST_REGISTER_NUM];
	int dre_blk_x_num;
	int dre_blk_y_num;
} DISP_DRE30_HIST;

typedef struct {
	unsigned int serviceFlags;
	int backlight;
	int colorHist;
	unsigned int maxHist[AAL_HIST_BIN];
	int requestPartial;
	aal_u32_handle_t dre30_hist;
	unsigned int panel_type;
	int essStrengthIndex;
	int ess_enable;
	int dre_enable;
	unsigned int yHist[AAL_HIST_BIN];
} DISP_AAL_HIST;

enum DISP_AAL_REFRESH_LATENCY {
	AAL_REFRESH_17MS = 17,
	AAL_REFRESH_33MS = 33
};

typedef struct {
	unsigned int dre30_gain[AAL_DRE30_GAIN_REGISTER_NUM];
} DISP_DRE30_PARAM;

typedef struct {
	int DREGainFltStatus[AAL_DRE_POINT_NUM];
	int cabc_fltgain_force;	/* 10-bit ; [0,1023] */
	int cabc_gainlmt[33];
	int FinalBacklight;	/* 10-bit ; [0,1023] */
	int allowPartial;
	int refreshLatency;	/* DISP_AAL_REFRESH_LATENCY */
	aal_u32_handle_t dre30_gain;
} DISP_AAL_PARAM;

#endif
