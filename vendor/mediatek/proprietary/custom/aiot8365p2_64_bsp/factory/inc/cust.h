#ifndef FTM_CUST_H
#define FTM_CUST_H

#define FEATURE_FTM_BATTERY
#define FEATURE_FTM_PMIC_632X
#define BATTERY_TYPE_Z3
#define FEATURE_FTM_SWCHR_I_68mohm
//#define FEATURE_FTM_HW_CANNOT_MEASURE_CURRENT
#define FEATURE_FTM_MAIN_CAMERA
#define FEATURE_FTM_SUB_CAMERA
//#define FEATURE_FTM_STROBE
#define FEATURE_FTM_AUDIO
//#define FEATURE_FTM_VIBRATOR
//#define FEATURE_FTM_SPK_OC
#define FEATURE_FTM_HEADSET
#define FEATURE_FTM_WAVE_PLAYBACK
#define FEATURE_FTM_GPS
//#define FEATURE_FTM_WIFI
#define FEATURE_FTM_FM
#define FEATURE_FTM_TOUCH
//#define FEATURE_FTM_BT
//#define FEATURE_FTM_HDMI
#define FEATURE_FTM_RTC
#define FEATURE_FTM_KEYS
#define FEATURE_FTM_BTS

#define FEATURE_FTM_LCD
#define FEATURE_FTM_BACKLIGHT
#define FEATURE_FTM_EMMC
#define FEATURE_FTM_CLEAREMMC

#define FEATURE_FTM_MEMCARD

#define FEATURE_FTM_IDLE
#define FEATURE_FTM_ACCDET
#define HEADSET_BUTTON_DETECTION

//#define FEATURE_FTM_SIM
#define FEATURE_FTM_OTG
#ifdef MTK_TB_WIFI_3G_MODE_3GDATA_ONLY
#define FEATURE_FTM_3GDATA_ONLY
#endif

#ifdef MTK_TB_WIFI_3G_MODE_3GDATA_SMS
#define FEATURE_FTM_3GDATA_SMS
#endif

#ifdef MTK_TB_WIFI_ONLY
#define FEATURE_FTM_WIFI_ONLY
#endif

//Add SMT RF Test
#ifndef MTK_TB_WIFI_ONLY
#define FEATURE_FTM_RF
#endif

#include "cust_font.h"		/* common part */
#include "cust_keys.h"		/* custom part */
#include "cust_lcd.h"		/* custom part */
#include "cust_led.h"		/* custom part */
#include "cust_touch.h"         /* custom part */

#endif /* FTM_CUST_H */
