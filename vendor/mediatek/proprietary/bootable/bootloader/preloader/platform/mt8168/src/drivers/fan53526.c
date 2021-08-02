#ifdef CFG_PRELOADER
#include <platform.h>
#include <gpio.h>
#endif
#include <i2c.h>
#include <fan53526.h>

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
#define FAN53526_CHIP_NAME_0		"fan53526_vdd2"
#define FAN53526_I2C_CHANNEL_REG0	I2C0
#define FAN53526_SLAVE_ADDR_REG0	0xC0
#define FAN53526_BUCK_CTRL_REG0		(FAN53526_VSEL0)
#define FAN53526_MODE_SHIFT_REG0	(0)
#define FAN53526_EN_SHIFT_REG0		(7)
/* VDDQ 00x */
#define FAN53526_CHIP_NAME_1		"fan53526_vddq"
#define FAN53526_I2C_CHANNEL_REG1	I2C0
#define FAN53526_SLAVE_ADDR_REG1	0xA0
#define FAN53526_BUCK_CTRL_REG1		(FAN53526_VSEL1)
#define FAN53526_MODE_SHIFT_REG1	(1)
#define FAN53526_EN_SHIFT_REG1		(7)

/**********************************************************
  *   Global Variable
  *********************************************************/
static struct mt_i2c_t fan53526_i2c;

int g_fan53526_driver_ready[FAN53526_MAX] = {0};
int g_fan53526_hw_exist[FAN53526_MAX] = {0};


#define fan53526_print(fmt, args...)	\
do {					\
    print(fmt, ##args);			\
} while (0)

#define DECL_CHIP(_id)			\
{					\
	.id = _id,			\
	.name = FAN53526_CHIP_NAME_##_id,	\
	.i2c_channel = FAN53526_I2C_CHANNEL_REG##_id,	\
	.slave_addr = FAN53526_SLAVE_ADDR_REG##_id,	\
	.buck_ctrl = FAN53526_BUCK_CTRL_REG##_id,	\
	.mode_shift = FAN53526_MODE_SHIFT_REG##_id,	\
	.en_shift = FAN53526_EN_SHIFT_REG##_id,		\
	.chip_id = 0,					\
}

static struct fan53526_chip fan53526_chip[FAN53526_MAX] = {
	DECL_CHIP(0),
	DECL_CHIP(1),
};


/**********************************************************
  *
  *   [I2C Function For Read/Write fan53526]
  *
  *********************************************************/
kal_uint32 fan53526_write_byte(struct fan53526_chip *chip,
			       kal_uint8 addr, kal_uint8 value)
{
	int ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	if (chip == NULL) {
		fan53526_print("%s: chip not exist\n",__func__);
		return 0;
	}

	write_data[0] = addr;
	write_data[1] = value;

	fan53526_i2c.id = chip->i2c_channel;

	/* Since i2c will left shift 1 bit, we need to set fan53526 I2C address to >>1 */
	fan53526_i2c.addr = (chip->slave_addr) >> 1;
	fan53526_i2c.mode = ST_MODE;
	fan53526_i2c.speed = 100;
	fan53526_i2c.pushpull = 0;
	len = 2;

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to I2C mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_SDA1_);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_SCL1_);
#endif

	ret_code = i2c_write(&fan53526_i2c, write_data, len);
	/* fan53526_print("%s: i2c_write: ret_code: %d\n", __func__, ret_code); */

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

kal_uint32 fan53526_read_byte(struct fan53526_chip *chip, kal_uint8 addr,
			      kal_uint8 *dataBuffer)
{
	int ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	if (chip == NULL) {
		fan53526_print("%s: chip not exist\n",__func__);
		return 0;
	}

	fan53526_i2c.id = chip->i2c_channel;
	/* Since i2c will left shift 1 bit, we need to set fan53526 I2C address to >>1 */
	fan53526_i2c.addr = (chip->slave_addr) >> 1;
	fan53526_i2c.mode = ST_MODE;
	fan53526_i2c.speed = 100;
	fan53526_i2c.pushpull = 0;
	len = 1;

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to I2C mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_SDA1_);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_SCL1_);
#endif

	ret_code = i2c_write_read(&fan53526_i2c, dataBuffer, len, len);

#if defined(GPIO_VMBUCK_I2C_SDA) & defined(GPIO_VMBUCK_I2C_SCL)
	/* switch to GPIO mode */
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SDA, GPIO_VMBUCK_I2C_SDA_M_GPIO);
	mt_set_gpio_mode(GPIO_VMBUCK_I2C_SCL, GPIO_VMBUCK_I2C_SCL_M_GPIO);
#endif

	/* fan53526_print("%s: i2c_read: ret_code: %d\n", __func__, ret_code); */

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
kal_uint32 fan53526_read_interface(struct fan53526_chip *chip, kal_uint8 RegNum,
				kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 fan53526_reg = 0;
	kal_uint32 ret = 0;

	/* fan53526_print("--------------------------------------------------PL\n"); */

	ret = fan53526_read_byte(chip, RegNum, &fan53526_reg);
	/* fan53526_print("[fan53526_read_interface] Reg[%x]=0x%x\n", RegNum, fan53526_reg); */

	fan53526_reg &= (MASK << SHIFT);
	*val = (fan53526_reg >> SHIFT);
	/* fan53526_print("[fan53526_read_interface] val=0x%x\n", *val); */

	return ret;
}

kal_uint32 fan53526_config_interface(struct fan53526_chip *chip, kal_uint8 RegNum,
				kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 fan53526_reg = 0;
	kal_uint32 ret = 0;

	/* fan53526_print("--------------------------------------------------PL\n"); */

	ret = fan53526_read_byte(chip, RegNum, &fan53526_reg);
	/* fan53526_print("[fan53526_config_interface] Reg[%x]=0x%x\n", RegNum, fan53526_reg); */

	fan53526_reg &= ~(MASK << SHIFT);
	fan53526_reg |= (val << SHIFT);

	ret = fan53526_write_byte(chip, RegNum, fan53526_reg);
	/* fan53526_print("[fan53526_config_interface] write Reg[%x]=0x%x\n", RegNum, fan53526_reg); */

	/* Check */
	/* fan53526_read_byte(RegNum, &fan53526_reg); */
	/* fan53526_print("[fan53526_config_interface] Check Reg[%x]=0x%x\n", RegNum, fan53526_reg); */

	return ret;
}

#if defined(EXTBUCK_FAN53526)
kal_uint32 fan53526_get_reg_value(struct fan53526_chip *chip, kal_uint32 reg)
{
	kal_uint32 ret = 0;
	kal_uint8 reg_val = 0;

	ret = fan53526_read_interface(chip, (kal_uint8) reg, &reg_val, 0xFF, 0x0);

	if (ret == 0)
		fan53526_print("ret=%d\n", ret);
	return reg_val;
}

void fan53526_hw_init(struct fan53526_chip *chip)
{
	fan53526_print("%s_hw_init\n", chip->name);
}
#endif

kal_uint8 fan53526_get_id1(struct fan53526_chip *chip)
{
	kal_uint8 val;

	fan53526_read_interface(chip, FAN53526_ID1, &val, 0xff, 0);
	return val;
}

kal_uint8 fan53526_get_id2(struct fan53526_chip *chip)
{
	kal_uint8 val;
	fan53526_read_interface(chip, FAN53526_ID2, &val, 0xff, 0);
	return val;
}

void update_fan53526_chip_id(struct fan53526_chip *chip)
{
	kal_uint32 id = 0;
	kal_uint32 id_l = 0;
	kal_uint32 id_r = 0;

	id_l = fan53526_get_id1(chip);
	id_r = fan53526_get_id2(chip);
	id = ((id_l << 8) | (id_r));

	chip->chip_id = id;
	fan53526_print("[%s] 0x%x, 0x%x, 0x%x\n", __func__, id_l, id_r, id);
}

void fan53526_hw_component_detect(struct fan53526_chip *chip)
{
	int ret = 0;
	unsigned char PGOOD = 0;
	int chip_id = 0;

	update_fan53526_chip_id(chip);
	ret = fan53526_read_interface(chip, 0x05, &PGOOD, 0x1, 7);

	if (chip->chip_id == FAN53526_ID && PGOOD) {
		g_fan53526_hw_exist[chip->id] = 1;
	} else {
		g_fan53526_hw_exist[chip->id] = 0;
	}
	fan53526_print("[%s] exist = %d, Chip ID = %x\n"
		     , __func__
		     , g_fan53526_hw_exist[chip->id], chip->chip_id);
}

static struct fan53526_chip *fan53526_find_chip(int id)
{
	struct fan53526_chip *chip;
	int i;

	for (i = 0; i < FAN53526_MAX; i++) {
		chip = &fan53526_chip[i];
		if (chip->id == id)
			return chip;
	}
	return NULL;
}

#if defined(EXTBUCK_FAN53526)
void fan53526_dump_register(int id)
{
	kal_uint8 i = 0x0;
	kal_uint8 i_max = 0x5;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	for (i = 0x0; i <= i_max; i++) {
		fan53526_print("[0x%x]=0x%x ", i, fan53526_get_reg_value(chip, i));
	}
}

int is_fan53526_exist(int id)
{
	fan53526_print("g_fan53526_hw_exist=%d\n", g_fan53526_hw_exist[id]);

	return g_fan53526_hw_exist[id];
}

int fan53526_is_enabled(int id)
{
	int ret = 1;
	unsigned char en;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	ret = fan53526_read_interface(chip, chip->buck_ctrl, &en, 0x1, 7);

	if (ret == 0)
		return -1;
	return en;
}
#endif

int fan53526_enable(int id, unsigned char en)
{
	int ret = 1;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	ret = fan53526_config_interface(chip, chip->buck_ctrl, en, 0x1, 7);

	return ret;
}

int fan53526_set_voltage(int id, unsigned long val)
{
	int ret = 1;
	unsigned long reg_val = 0;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	reg_val = (val - 600000) / 6250;

	if (reg_val > 127)
		reg_val = 127;

	ret = fan53526_config_interface(chip, chip->buck_ctrl, reg_val, 0x7F, 0);


	return ret;
}

unsigned long fan53526_get_voltage(int id)
{
	unsigned int ret = 0, vol = 0;
	unsigned char step = 0;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	ret = fan53526_read_interface(chip, chip->buck_ctrl, &step, 0x7F, 0);

	vol = ((step * 6250 ) + 600000);

	return vol;
}

/* mode = 3 force PWM mode no matter vsel H/L */
/* mode = 0 auto mode      */
int fan53526_set_mode(int id, unsigned char mode)
{
	int ret;
	struct fan53526_chip *chip = fan53526_find_chip(id);

	if (mode != 0 && mode != 3 ) {
		fan53526_print("[%s] error mode = %d only 0 or 3\n", __func__, mode);
		return -1;
	}

	ret = fan53526_config_interface(chip, 0x02, mode, 0x3, 0);

	return ret;
}

void fan53526_driver_probe(void)
{
	int id = 0;
	struct fan53526_chip *chip;

	fan53526_print("[%s]\n", __func__);
	for (id = 0; id < FAN53526_MAX; id++) {
		chip = &fan53526_chip[id];
		fan53526_hw_component_detect(chip);
		if (g_fan53526_hw_exist[id] == 1) {
#if defined(EXTBUCK_FAN53526)
			fan53526_hw_init(chip);
			fan53526_dump_register(id);
#endif
			g_fan53526_driver_ready[id] = 1;

			fan53526_print("[%s] PL g_fan53526_%d_hw_exist=%d, g_fan53526_driver_ready=%d\n"
				     , __func__
				     , id
				     , g_fan53526_hw_exist[id]
				     , g_fan53526_driver_ready[id]);
		} else {
			fan53526_print("[%s] PL %s is not exist\n"
				     , __func__
				     , chip->name);
		}
	}
	/* initial setting */
	if (g_fan53526_hw_exist[FAN53526_VDD2]) {
		fan53526_set_voltage(FAN53526_VDD2, 1125000);
		fan53526_print("fan53526_vdd2=%d uV\n", fan53526_get_voltage(FAN53526_VDD2));
	}
	if (g_fan53526_hw_exist[FAN53526_VDDQ]) {
		fan53526_print("fan53526_vddq=%d uV\n", fan53526_get_voltage(FAN53526_VDDQ));
	}
}
