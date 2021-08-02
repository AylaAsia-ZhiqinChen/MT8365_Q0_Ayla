#ifndef _fan53526_SW_H_
#define _fan53526_SW_H_

#define CFG_PRELOADER

#ifdef CFG_PRELOADER
#include <typedefs.h>
/* for reduce code size */
/*#define	EXTBUCK_FAN53526	1*/
#endif

#define FAN53526_ID		0x8108

/* Voltage setting */
#define FAN53526_VSEL0		0x00
#define FAN53526_VSEL1		0x01
/* Control register */
#define FAN53526_CONTROL	0x02
/* IC Type */
#define FAN53526_ID1		0x03
/* IC mask version */
#define FAN53526_ID2		0x04
/* Monitor register */
#define FAN53526_MONITOR	0x05




enum {
	FAN53526_VDD2,
	FAN53526_VDDQ,
	FAN53526_MAX,
};

struct fan53526_chip {
	kal_uint8 id;
	char *name;
	kal_uint16 i2c_channel;
	kal_uint8 slave_addr;
	kal_uint8 buck_ctrl;
	kal_uint8 mode_shift;
	kal_uint8 en_shift;
	kal_uint32 chip_id;
};


extern void fan53526_driver_probe(void);
extern int fan53526_set_voltage(int id, unsigned long val);
extern unsigned long fan53526_get_voltage(int id); // if return 0 --> get fail
extern int fan53526_enable(int id, unsigned char en);
extern int fan53526_set_mode(int id, unsigned char mode);
extern int g_fan53526_hw_exist[FAN53526_MAX];
#if defined(EXTBUCK_FAN53526)
extern void fan53526_dump_register(int id);
extern int fan53526_is_enabled(int id); // if return -1 --> get fail
extern int is_fan53526_exist(int id);
#endif
#endif				/* _fan53526_SW_H_ */
