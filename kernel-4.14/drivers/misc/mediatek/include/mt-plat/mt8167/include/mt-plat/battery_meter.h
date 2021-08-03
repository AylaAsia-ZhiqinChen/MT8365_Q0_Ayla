/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _BATTERY_METER_H
#define _BATTERY_METER_H

#include <linux/platform_device.h>
#include <mach/mt_battery_meter.h>
#include <mt-plat/charging.h>
#if 0
#include <mt-plat/mt_typedefs.h>
#endif
/* ============================================================ */
/* define */
/* ============================================================ */
#define FG_CURRENT_AVERAGE_SIZE 30

/* ============================================================ */
/* ENUM */
/* ============================================================ */

/* ============================================================ */
/* structure */
/* ============================================================ */

#define FGD_NL_MSG_T_HDR_LEN 12
#define FGD_NL_MSG_MAX_LEN 9200

struct fgd_nl_msg_t {
	unsigned int fgd_cmd;
	unsigned int fgd_data_len;
	unsigned int fgd_ret_data_len;
	char fgd_data[FGD_NL_MSG_MAX_LEN];
};

enum {
	FG_MAIN = 1,
	FG_SUSPEND = 2,
	FG_RESUME = 4,
	FG_CHARGER = 8,
	FG_INIT = 16,
	FG_ZCV_INT = 32
};

enum { HW_FG, SW_FG, AUXADC };

/* ============================================================ */
/* typedef */
/* ============================================================ */
struct BATT_TEMPERATURE {
	signed int BatteryTemp;
	signed int TemperatureR;
};

#if !defined(CONFIG_MTK_HAFG_20)
struct battery_meter_custom_data {

	/* mt_battery_meter.h */

	/* ADC resister */
	int r_bat_sense;
	int r_i_sense;
	int r_charger_1;
	int r_charger_2;

	int temperature_t0;
	int temperature_t1;
	int temperature_t2;
	int temperature_t3;
	int temperature_t;

	int fg_meter_resistance;

	/* Qmax for battery  */
	int q_max_pos_50;
	int q_max_pos_25;
	int q_max_pos_0;
	int q_max_neg_10;
	int q_max_pos_50_h_current;
	int q_max_pos_25_h_current;
	int q_max_pos_0_h_current;
	int q_max_neg_10_h_current;

	int oam_d5; /* 1 : D5,   0: D2 */

	int change_tracking_point;
	int cust_tracking_point;
	int cust_r_sense;
	int cust_hw_cc;
	int aging_tuning_value;
	int cust_r_fg_offset;
	int ocv_board_compesate;
	int r_fg_board_base;
	int r_fg_board_slope;
	int car_tune_value;

	/* HW Fuel gague  */
	int current_detect_r_fg;
	int minerroroffset;
	int fg_vbat_average_size;
	int r_fg_value;
	int cust_poweron_delta_capacity_tolrance;
	int cust_poweron_low_capacity_tolrance;
	int cust_poweron_max_vbat_tolrance;
	int cust_poweron_delta_vbat_tolrance;
	int cust_poweron_delta_hw_sw_ocv_capacity_tolrance;

	int fixed_tbat_25;
	int vbat_remove_detection;

	/* Dynamic change wake up period of battery thread when suspend */
	int vbat_normal_wakeup;
	int vbat_low_power_wakeup;
	int normal_wakeup_period;
	int low_power_wakeup_period;
	int close_poweroff_wakeup_period;

	/* mt_battery_meter.h */
	int bat_ntc;
	int rbat_pull_up_r;
	int rbat_pull_up_volt;
};
#else

struct battery_meter_custom_data {

	int versionID1;
	int versionID2;

	int last_low_battery_temp;

	/* cust_battery_meter.h */
	int soc_flow;

	int hw_fg_force_use_sw_ocv;

	/* ADC resister */
	int r_bat_sense;
	int r_i_sense;
	int r_charger_1;
	int r_charger_2;

	int temperature_t0;
	int temperature_t1;
	int temperature_t2;
	int temperature_t3;
	int temperature_t;

	int fg_meter_resistance;

	/* Qmax for battery  */
	int q_max_pos_50;
	int q_max_pos_25;
	int q_max_pos_0;
	int q_max_neg_10;
	int q_max_pos_50_h_current;
	int q_max_pos_25_h_current;
	int q_max_pos_0_h_current;
	int q_max_neg_10_h_current;

	int oam_d5; /* 1 : D5,   0: D2 */

	int change_tracking_point;
	int cust_tracking_point;
	int cust_r_sense;
	int cust_hw_cc;
	int aging_tuning_value;
	int cust_r_fg_offset;
	int ocv_board_compesate;
	int r_fg_board_base;
	int r_fg_board_slope;
	int car_tune_value;

	/* HW Fuel gague  */
	int current_detect_r_fg;
	int minerroroffset;
	int fg_vbat_average_size;
	int r_fg_value;
	int difference_hwocv_rtc;
	int difference_hwocv_swocv;
	int difference_swocv_rtc;
	int difference_vbat_rtc;
	int difference_swocv_rtc_pos;
	int max_swocv;

	int max_hwocv;
	int max_vbat;
	int difference_hwocv_vbat;

	int suspend_current_threshold;
	int ocv_check_time;
	int shutdown_system_voltage;
	int recharge_tolerance;
	int fixed_tbat_25;

	int batterypseudo100;
	int batterypseudo1;

	/* Dynamic change wake up period of battery thread when suspend*/
	int vbat_normal_wakeup;
	int vbat_low_power_wakeup;
	int normal_wakeup_period;
	int low_power_wakeup_period;
	int close_poweroff_wakeup_period;

	int init_soc_by_sw_soc;
	int sync_ui_soc_imm;		/*3. ui soc sync to fg soc immediately*/
	int mtk_enable_aging_algorithm; /*6. q_max aging algorithm*/
	/*5. gauge adjust by ocv 9. md sleep current check*/
	int md_sleep_current_check;
	int q_max_by_current;  /*7. qmax variant by current loading.*/
	int q_max_sys_voltage; /*8. qmax variant by sys voltage.*/

	int shutdown_gauge0;
	int shutdown_gauge1_xmins;
	int shutdown_gauge1_mins;

	int trk_point_en;
	int trk_point_thr;

	int min_charging_smooth_time;

	int embedded_battery;

	/* SW Fuel gauge */
	int apsleep_battery_voltage_compensate;

	int bat_task_period;

	int zcv_int_enable;
};

#endif

enum FG_DAEMON_CTRL_CMD_FROM_USER {
	FG_DAEMON_CMD_GET_INIT_FLAG,
	FG_DAEMON_CMD_GET_SOC,
	FG_DAEMON_CMD_GET_DOD0,
	FG_DAEMON_CMD_GET_DOD1,
	FG_DAEMON_CMD_GET_HW_OCV,
	FG_DAEMON_CMD_GET_HW_FG_INIT_CURRENT,
	FG_DAEMON_CMD_GET_HW_FG_CURRENT,
	FG_DAEMON_CMD_GET_HW_FG_INIT_CURRENT_SIGN,
	FG_DAEMON_CMD_GET_HW_FG_CURRENT_SIGN,
	FG_DAEMON_CMD_GET_HW_FG_CAR_ACT,
	FG_DAEMON_CMD_GET_TEMPERTURE,
	FG_DAEMON_CMD_DUMP_REGISTER,
	FG_DAEMON_CMD_CHARGING_ENABLE,
	FG_DAEMON_CMD_GET_BATTERY_INIT_VOLTAGE,
	FG_DAEMON_CMD_GET_BATTERY_VOLTAGE,
	FG_DAEMON_CMD_FGADC_RESET,
	FG_DAEMON_CMD_GET_BATTERY_PLUG_STATUS,
	FG_DAEMON_CMD_GET_RTC_SPARE_FG_VALUE,
	FG_DAEMON_CMD_IS_CHARGER_EXIST,
	FG_DAEMON_CMD_IS_BATTERY_FULL,  /* bat_is_battery_full, */
	FG_DAEMON_CMD_SET_BATTERY_FULL, /* bat_set_battery_full, */
	FG_DAEMON_CMD_SET_RTC,		/* set RTC, */
	FG_DAEMON_CMD_SET_POWEROFF,     /* set Poweroff, */
	FG_DAEMON_CMD_IS_KPOC,		/* is KPOC, */
	FG_DAEMON_CMD_GET_BOOT_REASON,  /* g_boot_reason, */
	FG_DAEMON_CMD_GET_CHARGING_CURRENT,
	FG_DAEMON_CMD_GET_CHARGER_VOLTAGE,
	FG_DAEMON_CMD_GET_SHUTDOWN_COND,
	FG_DAEMON_CMD_GET_CUSTOM_SETTING,
	FG_DAEMON_CMD_GET_UI_SOC,
	FG_DAEMON_CMD_GET_CV_VALUE,
	FG_DAEMON_CMD_GET_DURATION_TIME,
	FG_DAEMON_CMD_GET_TRACKING_TIME,
	FG_DAEMON_CMD_GET_CURRENT_TH,
	FG_DAEMON_CMD_GET_CHECK_TIME,
	FG_DAEMON_CMD_GET_DIFFERENCE_VOLTAGE_UPDATE,
	FG_DAEMON_CMD_GET_AGING1_LOAD_SOC,
	FG_DAEMON_CMD_GET_AGING1_UPDATE_SOC,
	FG_DAEMON_CMD_GET_SHUTDOWN_SYSTEM_VOLTAGE,
	FG_DAEMON_CMD_GET_CHARGE_TRACKING_TIME,
	FG_DAEMON_CMD_GET_DISCHARGE_TRACKING_TIME,
	FG_DAEMON_CMD_GET_SHUTDOWN_GAUGE0,
	FG_DAEMON_CMD_GET_SHUTDOWN_GAUGE1_XMINS,
	FG_DAEMON_CMD_GET_SHUTDOWN_GAUGE1_MINS,
	FG_DAEMON_CMD_SET_SUSPEND_TIME,
	FG_DAEMON_CMD_SET_WAKEUP_SMOOTH_TIME,
	FG_DAEMON_CMD_SET_IS_CHARGING,
	FG_DAEMON_CMD_SET_RBAT,
	FG_DAEMON_CMD_SET_SWOCV,
	FG_DAEMON_CMD_SET_DOD0,
	FG_DAEMON_CMD_SET_DOD1,
	FG_DAEMON_CMD_SET_QMAX,
	FG_DAEMON_CMD_SET_SOC,
	FG_DAEMON_CMD_SET_UI_SOC,
	FG_DAEMON_CMD_SET_UI_SOC2,
	FG_DAEMON_CMD_SET_INIT_FLAG,
	FG_DAEMON_CMD_SET_DAEMON_PID,
	FG_DAEMON_CMD_NOTIFY_DAEMON,
	FG_DAEMON_CMD_CHECK_FG_DAEMON_VERSION,
	FG_DAEMON_CMD_SET_OAM_V_OCV,
	FG_DAEMON_CMD_SET_OAM_R,
	FG_DAEMON_CMD_GET_SUSPEND_TIME,
	FG_DAEMON_CMD_GET_SUSPEND_CAR,
	FG_DAEMON_CMD_IS_HW_OCV_UPDATE,
	FG_DAEMON_CMD_SET_SWSOC,
	FG_DAEMON_CMD_SET_HWSOC,
	FG_DAEMON_CMD_SET_VBATSOC,
	FG_DAEMON_CMD_SET_CAR_TUNE_VALUE,
	FG_DAEMON_CMD_GET_ZCV_INT_HW_OCV,

	FG_DAEMON_CMD_FROM_USER_NUMBER
};

/* ============================================================ */
/* External Variables */
/* ============================================================ */

extern struct battery_meter_custom_data batt_meter_cust_data;

#if !defined(CONFIG_MTK_HAFG_20)
#ifdef MTK_ENABLE_AGING_ALGORITHM
extern unsigned int suspend_time;
#endif
#endif
extern unsigned int _g_bat_sleep_total_time;

#if !defined(CONFIG_MTK_HAFG_20)
extern bool bat_spm_timeout;
#else
extern bool bat_spm_timeout;
extern unsigned int sleep_total_time;
#endif

extern char *saved_command_line;
extern enum BATTERY_VOLTAGE_ENUM cv_voltage;

extern unsigned int battery_tracking_time;
extern unsigned int wake_up_smooth_time;
extern enum kal_bool g_battery_soc_ready;
extern void bat_update_thread_wakeup(void);
#ifdef MTK_MULTI_BAT_PROFILE_SUPPORT
extern int IMM_GetOneChannelValue_Cali(int Channel, int *voltage);
#endif

/* ============================================================ */
/* External function */
/* ============================================================ */
extern signed int battery_meter_get_battery_voltage(enum kal_bool update);
extern signed int battery_meter_get_charging_current_imm(void);
extern signed int battery_meter_get_charging_current(void);
extern signed int battery_meter_get_battery_current(void);
extern enum kal_bool battery_meter_get_battery_current_sign(void);
extern signed int battery_meter_get_car(void);
extern signed int battery_meter_get_battery_temperature(void);
extern signed int battery_meter_get_charger_voltage(void);
extern signed int battery_meter_get_battery_percentage(void);
extern signed int battery_meter_initial(void);
extern signed int battery_meter_reset(void);
extern signed int battery_meter_sync(signed int bat_i_sense_offset);

extern signed int battery_meter_get_battery_zcv(void);
/* 15% zcv,  15% can be customized */
extern signed int battery_meter_get_battery_nPercent_zcv(void);
/* tracking point */
extern signed int battery_meter_get_battery_nPercent_UI_SOC(void);

extern signed int battery_meter_get_tempR(signed int dwVolt);
extern signed int battery_meter_get_tempV(void);
extern signed int battery_meter_get_QMAX25(void);
extern signed int battery_meter_get_VSense(void); /* isense voltage */
extern void battery_meter_smooth_uisoc2(void);
extern int wakeup_fg_algo(int flow_state);
extern int dod_init_in_kernel(void);

#if defined(CUST_CAPACITY_OCV2CV_TRANSFORM)
extern void battery_meter_set_reset_soc(enum kal_bool bUSE_UI_SOC);
extern signed int battery_meter_get_battery_soc(void);
#endif

#ifdef MTK_MULTI_BAT_PROFILE_SUPPORT
extern int IMM_GetOneChannelValue_Cali(int Channel, int *voltage);
#ifdef MTK_GET_BATTERY_ID_BY_AUXADC
extern unsigned int upmu_get_reg_value(unsigned int reg);
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int *rawdata);
extern int IMM_IsAdcInitReady(void);
extern unsigned int pmic_config_interface(unsigned int RegNum, unsigned int val,
					  unsigned int MASK,
					  unsigned int SHIFT);
extern unsigned int pmic_read_interface(unsigned int RegNum, unsigned int *val,
					unsigned int MASK, unsigned int SHIFT);
extern unsigned int get_pmic_mt6325_cid(void);
#endif
#endif

extern void fgauge_algo_run_get_init_data(void);
extern void battery_meter_set_init_flag(enum kal_bool flag);
extern void battery_meter_reset_sleep_time(void);
extern int battery_meter_get_low_battery_interrupt_status(void);
extern signed int battery_meter_meta_tool_cali_car_tune(int meta_current);
extern void mt_battery_set_init_vol(int init_voltage);

#if defined(CONFIG_MTK_HAFG_20)
extern struct battery_meter_custom_data batt_meter_cust_data;
extern struct battery_meter_table_custom_data batt_meter_table_cust_data;
extern unsigned int get_cv_voltage(void);
extern void battery_meter_recovery_run(int flow_state);
extern void battery_meter_get_init_value(signed int *voltage,
			enum kal_bool *is_charging, signed int *bat_current);
extern void battery_meter_set_r_bat(int r_bat);
extern void battery_meter_set_sw_ocv(int voltage);
extern int battery_meter_get_hw_ocv(void);
extern enum kal_bool is_recovery_mode(void);
extern void bmr_init(void);
extern void bmr_run(int flow_state);
extern void battery_meter_set_fg_int(void);
#endif

#endif /* #ifndef _BATTERY_METER_H */
