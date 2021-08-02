#ifndef _hl7593_SW_H_
#define _hl7593_SW_H_

#define CFG_PRELOADER

#ifdef CFG_PRELOADER
#include <typedefs.h>
/* for reduce code size */
/*#define	EXTBUCK_hl7593	1*/
#endif

/*#define FAN53526_ID		0x8108*/
#define hl7593_ID		0xA801

/* Voltage setting */
#define hl7593_VSEL0		0x00
#define hl7593_VSEL1		0x01
/* Control register */
#define hl7593_CONTROL	0x02
/* IC Type */
#define hl7593_ID1		0x03
/* IC mask version */
#define hl7593_ID2		0x04
/* Monitor register */
#define hl7593_MONITOR	0x05




enum {
	hl7593_VDD2,
	hl7593_VDDQ,
	hl7593_MAX,
};

struct hl7593_chip {
	kal_uint8 id;
	char *name;
	kal_uint16 i2c_channel;
	kal_uint8 slave_addr;
	kal_uint8 buck_ctrl;
	kal_uint8 mode_shift;
	kal_uint8 en_shift;
	kal_uint32 chip_id;
};


extern void hl7593_driver_probe(void);
extern int hl7593_set_voltage(int id, unsigned long val);
extern unsigned long hl7593_get_voltage(int id); // if return 0 --> get fail
extern int hl7593_enable(int id, unsigned char en);
extern int hl7593_set_mode(int id, unsigned char mode);
extern int g_hl7593_hw_exist[hl7593_MAX];
#if defined(EXTBUCK_hl7593)
extern void hl7593_dump_register(int id);
extern int hl7593_is_enabled(int id); // if return -1 --> get fail
extern int is_hl7593_exist(int id);
#endif
#endif				/* _hl7593_SW_H_ */
