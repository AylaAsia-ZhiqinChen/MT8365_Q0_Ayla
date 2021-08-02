#ifdef CFG_PRELOADER
#include <platform.h>
#include <gpio.h>
#endif
#include <i2c.h>
#include <hl7593.h>

#ifdef CFG_PRELOADER
#if CFG_FPGA_PLATFORM
#else
#include "cust_i2c.h"
#include "cust_gpio_usage.h"
#endif
#endif

/**********************************************************
  *   I2C Slave Setting
  *********************************************************/

/* VDD2 288x */
#define hl7593_CHIP_NAME_0		"hl7593_vdd2"
#define hl7593_I2C_CHANNEL_REG0	I2C0
/*#define FAN53526_SLAVE_ADDR_REG0	0xC2*/
#define hl7593_SLAVE_ADDR_REG0	0x57
#define hl7593_BUCK_CTRL_REG0		(hl7593_VSEL0)
#define hl7593_MODE_SHIFT_REG0	(0)
#define hl7593_EN_SHIFT_REG0		(7)
/* VDDQ 00x */
#define hl7593_CHIP_NAME_1		"hl7593_vddq"
#define hl7593_I2C_CHANNEL_REG1	I2C0
#define hl7593_SLAVE_ADDR_REG1	0x50
/*#define FAN53526_SLAVE_ADDR_REG1	0xC0*/
#define hl7593_BUCK_CTRL_REG1		(hl7593_VSEL1)
#define hl7593_MODE_SHIFT_REG1	(1)
#define hl7593_EN_SHIFT_REG1		(7)

#define EXTBUCK_hl7593	1
/**********************************************************
  *   Global Variable
  *********************************************************/
static struct mt_i2c_t hl7593_i2c;

int g_hl7593_driver_ready[hl7593_MAX] = {0};
int g_hl7593_hw_exist[hl7593_MAX] = {0};


#define hl7593_print(fmt, args...)	\
do {					\
    print(fmt, ##args);			\
} while (0)

#define DECL_CHIP(_id)			\
{					\
	.id = _id,			\
	.name = hl7593_CHIP_NAME_##_id,	\
	.i2c_channel = hl7593_I2C_CHANNEL_REG##_id,	\
	.slave_addr = hl7593_SLAVE_ADDR_REG##_id,	\
	.buck_ctrl = hl7593_BUCK_CTRL_REG##_id,	\
	.mode_shift = hl7593_MODE_SHIFT_REG##_id,	\
	.en_shift = hl7593_EN_SHIFT_REG##_id,		\
	.chip_id = 0,					\
}

static struct hl7593_chip hl7593_chip[hl7593_MAX] = {
	DECL_CHIP(0),
	DECL_CHIP(1),
};


/**********************************************************
  *
  *   [I2C Function For Read/Write hl7593]
  *
  *********************************************************/
kal_uint32 hl7593_write_byte(struct hl7593_chip *chip,
			       kal_uint8 addr, kal_uint8 value)
{
	int ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	if (chip == NULL) {
		hl7593_print("%s: chip not exist\n",__func__);
		return 0;
	}

	write_data[0] = addr;
	write_data[1] = value;

	hl7593_i2c.id = chip->i2c_channel;

	/* Since i2c will left shift 1 bit, we need to set hl7593 I2C address to >>1 */
	//hl7593_i2c.addr = (chip->slave_addr) >> 1;
	hl7593_i2c.addr = chip->slave_addr;
	hl7593_i2c.mode = ST_MODE;
	hl7593_i2c.speed = 100;
	hl7593_i2c.pushpull = 0;
	len = 2;

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to I2C mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_SDA1_);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_SCL1_);
#endif

	ret_code = i2c_write(&hl7593_i2c, write_data, len);
	/* hl7593_print("%s: i2c_write: ret_code: %d\n", __func__, ret_code); */

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* Switch to GPIO mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_GPIO);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_GPIO);
#endif

	if (ret_code == 0)
		return 1;	/* ok */
	else
		return 0;	/* fail */
}

kal_uint32 hl7593_read_byte(struct hl7593_chip *chip, kal_uint8 addr,
			      kal_uint8 *dataBuffer)
{
	int ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	if (chip == NULL) {
		hl7593_print("%s: chip not exist\n",__func__);
		return 0;
	}

	hl7593_i2c.id = chip->i2c_channel;
	/* Since i2c will left shift 1 bit, we need to set hl7593 I2C address to >>1 */
	//hl7593_i2c.addr = (chip->slave_addr) >> 1;
	hl7593_i2c.addr = chip->slave_addr;

	hl7593_i2c.mode = ST_MODE;
	hl7593_i2c.speed = 100;
	hl7593_i2c.pushpull = 0;
	len = 1;

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to I2C mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_SDA1_);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_SCL1_);
#endif

	ret_code = i2c_write_read(&hl7593_i2c, dataBuffer, len, len);

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to GPIO mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_GPIO);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_GPIO);
#endif

	/* hl7593_print("%s: i2c_read: ret_code: %d\n", __func__, ret_code); */

	if (ret_code == 0)
		return 1;	/* ok */
	else
		return 0;	/* fail */
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
kal_uint32 hl7593_read_interface(struct hl7593_chip *chip, kal_uint8 RegNum,
				kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 hl7593_reg = 0;
	kal_uint32 ret = 0;

	/* hl7593_print("--------------------------------------------------PL\n"); */

	ret = hl7593_read_byte(chip, RegNum, &hl7593_reg);
	hl7593_print("[hl7593_read_interface] Reg[%x]=0x%x\n", RegNum, hl7593_reg); // ziyi modify 

	hl7593_reg &= (MASK << SHIFT);
	*val = (hl7593_reg >> SHIFT);
	hl7593_print("[hl7593_read_interface] val=0x%x\n", *val);  // ziyi modify

	return ret;
}

kal_uint32 hl7593_config_interface(struct hl7593_chip *chip, kal_uint8 RegNum,
				kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 hl7593_reg = 0;
	kal_uint32 ret = 0;

	/* hl7593_print("--------------------------------------------------PL\n"); */

	ret = hl7593_read_byte(chip, RegNum, &hl7593_reg);
	/* hl7593_print("[hl7593_config_interface] Reg[%x]=0x%x\n", RegNum, hl7593_reg); */

	hl7593_reg &= ~(MASK << SHIFT);
	hl7593_reg |= (val << SHIFT);

	ret = hl7593_write_byte(chip, RegNum, hl7593_reg);
	/* hl7593_print("[hl7593_config_interface] write Reg[%x]=0x%x\n", RegNum, hl7593_reg); */

	/* Check */
	/* hl7593_read_byte(RegNum, &hl7593_reg); */
	/* hl7593_print("[hl7593_config_interface] Check Reg[%x]=0x%x\n", RegNum, hl7593_reg); */

	return ret;
}

#if defined(EXTBUCK_hl7593)
kal_uint32 hl7593_get_reg_value(struct hl7593_chip *chip, kal_uint32 reg)
{
	kal_uint32 ret = 0;
	kal_uint8 reg_val = 0;

	ret = hl7593_read_interface(chip, (kal_uint8) reg, &reg_val, 0xFF, 0x0);

	if (ret == 0)
		hl7593_print("ret=%d\n", ret);
	return reg_val;
}

void hl7593_hw_init(struct hl7593_chip *chip)
{
	hl7593_print("%s_hw_init\n", chip->name);
}
#endif

kal_uint8 hl7593_get_id1(struct hl7593_chip *chip)
{
	kal_uint8 val;

	hl7593_read_interface(chip, hl7593_ID1, &val, 0xff, 0);
	return val;
}

kal_uint8 hl7593_get_id2(struct hl7593_chip *chip)
{
	kal_uint8 val;
	hl7593_read_interface(chip, hl7593_ID2, &val, 0xff, 0);
	return val;
}

void update_hl7593_chip_id(struct hl7593_chip *chip)
{
	kal_uint32 id = 0;
	kal_uint32 id_l = 0;
	kal_uint32 id_r = 0;

	id_l = hl7593_get_id1(chip);
	id_r = hl7593_get_id2(chip);
	id = ((id_l << 8) | (id_r));

	chip->chip_id = id;
	hl7593_print("[%s] 0x%x, 0x%x, 0x%x\n", __func__, id_l, id_r, id);
}

void hl7593_hw_component_detect(struct hl7593_chip *chip)
{
	int ret = 0;
	unsigned char PGOOD = 0;
	int chip_id = 0;

	update_hl7593_chip_id(chip);
	ret = hl7593_read_interface(chip, 0x05, &PGOOD, 0x1, 7);
	hl7593_print("PGOOD = %d, chip_id = %d\n", PGOOD,chip->chip_id); // ziyi add

	if (chip->chip_id == hl7593_ID && PGOOD) {
		g_hl7593_hw_exist[chip->id] = 1;
	} else {
		g_hl7593_hw_exist[chip->id] = 0;
	}
	hl7593_print("[%s] exist = %d, Chip ID = %x\n"
		     , __func__
		     , g_hl7593_hw_exist[chip->id], chip->chip_id);
}

static struct hl7593_chip *hl7593_find_chip(int id)
{
	struct hl7593_chip *chip;
	int i;

	for (i = 0; i < hl7593_MAX; i++) {
		chip = &hl7593_chip[i];
		if (chip->id == id)
			return chip;
	}
	return NULL;
}

#if defined(EXTBUCK_hl7593)
void hl7593_dump_register(int id)
{
	kal_uint8 i = 0x0;
	kal_uint8 i_max = 0x5;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	for (i = 0x0; i <= i_max; i++) {
		hl7593_print("[0x%x]=0x%x ", i, hl7593_get_reg_value(chip, i));
	}
}

int is_hl7593_exist(int id)
{
	hl7593_print("g_hl7593_hw_exist=%d\n", g_hl7593_hw_exist[id]);

	return g_hl7593_hw_exist[id];
}

int hl7593_is_enabled(int id)
{
	int ret = 1;
	unsigned char en;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	ret = hl7593_read_interface(chip, chip->buck_ctrl, &en, 0x1, 7);

	if (ret == 0)
		return -1;
	return en;
}
#endif

int hl7593_enable(int id, unsigned char en)
{
	int ret = 1;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	ret = hl7593_config_interface(chip, chip->buck_ctrl, en, 0x1, 7);

	hl7593_print("[%s] id =%d, en =%d, ret =%d \n", __func__, id, en, ret);

	return ret;
}

int hl7593_set_voltage(int id, unsigned long val)
{
	int ret = 1;
	unsigned long reg_val = 0;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	reg_val = (val - 600000) / 6250;

	if (reg_val > 127)
		reg_val = 127;

	ret = hl7593_config_interface(chip, chip->buck_ctrl, reg_val, 0x7F, 0);

	hl7593_print("[%s] id = %d, set_val = %d \n", __func__, id, val);


	return ret;
}

unsigned long hl7593_get_voltage(int id)
{
	unsigned int ret = 0, vol = 0;
	unsigned char step = 0;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	ret = hl7593_read_interface(chip, chip->buck_ctrl, &step, 0x7F, 0);

	vol = ((step * 6250 ) + 600000);

	hl7593_print("[%s] id = %d, get_val = %d \n", __func__, id, vol);

	return vol;
}

/* mode = 3 force PWM mode no matter vsel H/L */
/* mode = 0 auto mode      */
int hl7593_set_mode(int id, unsigned char mode)
{
	int ret;
	struct hl7593_chip *chip = hl7593_find_chip(id);

	if (mode != 0 && mode != 3 ) {
		hl7593_print("[%s] error mode = %d only 0 or 3\n", __func__, mode);
		return -1;
	}

	ret = hl7593_config_interface(chip, 0x02, mode, 0x3, 0);

	return ret;
}

void hl7593_driver_probe(void)
{
	int id = 0;
	struct hl7593_chip *chip;

	hl7593_print("[%s]\n", __func__);
	for (id = 0; id < hl7593_MAX; id++) {
		chip = &hl7593_chip[id];
		hl7593_hw_component_detect(chip);
		if (g_hl7593_hw_exist[id] == 1) {
#if defined(EXTBUCK_hl7593)
			hl7593_hw_init(chip);
			hl7593_dump_register(id);
#endif
			g_hl7593_driver_ready[id] = 1;

			hl7593_print("[%s] PL g_hl7593_%d_hw_exist=%d, g_hl7593_driver_ready=%d\n"
				     , __func__
				     , id
				     , g_hl7593_hw_exist[id]
				     , g_hl7593_driver_ready[id]);
		} else {
			hl7593_print("[%s] PL %s is not exist\n"
				     , __func__
				     , chip->name);
		}
	}
	/* initial setting */
	if (g_hl7593_hw_exist[hl7593_VDD2]) {
		hl7593_set_voltage(hl7593_VDD2, 1125000);
		hl7593_print("hl7593_vdd2=%d uV\n", hl7593_get_voltage(hl7593_VDD2));
	}
	if (g_hl7593_hw_exist[hl7593_VDDQ]) {
		hl7593_print("hl7593_vddq=%d uV\n", hl7593_get_voltage(hl7593_VDDQ));
	}
}
