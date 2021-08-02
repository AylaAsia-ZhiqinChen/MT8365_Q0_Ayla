#include "platform.h"
#include "i2c.h"
#include "pmic.h"
#include "bq24296.h"

/**********************************************************
 *
 *	[I2C Slave Setting]
 *
 *********************************************************/
#define BQ24296_SLAVE_ADDR_WRITE   0xD6
#define BQ24296_SLAVE_ADDR_READ    0xD7

#ifdef I2C_SWITCHING_CHARGER_CHANNEL
#define BQ24296_BUSNUM I2C_SWITCHING_CHARGER_CHANNEL
#else
#define BQ24296_BUSNUM 1
#endif

/**********************************************************
 *
 *[Global Variable]
 *
 *********************************************************/
#define bq24296_REG_NUM 11
unsigned char bq24296_reg[bq24296_REG_NUM] = {0};

/**********************************************************
 *
 *	[I2C Function For Read/Write bq24296]
 *
 *********************************************************/

static struct mt_i2c_t bq24296_i2c;

kal_uint32 bq24296_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0]= addr;
	write_data[1] = value;

	bq24296_i2c.id = BQ24296_BUSNUM;
	bq24296_i2c.addr = (BQ24296_SLAVE_ADDR_WRITE >> 1);
	bq24296_i2c.mode = ST_MODE;
	bq24296_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&bq24296_i2c, write_data, len);

	if(I2C_OK != ret_code)
	print("%s: i2c_write: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

kal_uint32 bq24296_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	bq24296_i2c.id = BQ24296_BUSNUM;
	bq24296_i2c.addr = (BQ24296_SLAVE_ADDR_READ >> 1);
	bq24296_i2c.mode = ST_MODE;
	bq24296_i2c.speed = 100;
	len = 1;

	ret_code = i2c_write_read(&bq24296_i2c, dataBuffer, len, len);

	if(I2C_OK != ret_code)
	print("%s: i2c_read: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
unsigned int bq24296_read_interface(unsigned char RegNum, unsigned char *val, unsigned char MASK,
				  unsigned char SHIFT)
{
	unsigned char bq24296_reg = 0;
	int ret = 0;

	print("--------------------------------------------------\n");

	ret = bq24296_read_byte(RegNum, &bq24296_reg);

	print("[bq24296_read_interface] Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	bq24296_reg &= (MASK << SHIFT);
	*val = (bq24296_reg >> SHIFT);

	print("[bq24296_read_interface] val=0x%x\n", *val);
	return ret;
}

unsigned int bq24296_config_interface(unsigned char RegNum, unsigned char val, unsigned char MASK,
				    unsigned char SHIFT)
{
	unsigned char bq24296_reg = 0;
	int ret = 0;

	print("--------------------------------------------------\n");

	ret = bq24296_read_byte(RegNum, &bq24296_reg);
	print("[bq24296_config_interface] Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	bq24296_reg &= ~(MASK << SHIFT);
	bq24296_reg |= (val << SHIFT);

	ret = bq24296_write_byte(RegNum, bq24296_reg);
	print("[bq24296_config_interface] write Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	/* Check */
	/* bq24296_read_byte(RegNum, &bq24296_reg); */
	/* print("[bq24296_config_interface] Check Reg[%x]=0x%x\n", RegNum, bq24296_reg); */

	return ret;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/

/* CON1---------------------------------------------------- */

void bq24296_set_reg_rst(unsigned int val)
{
    kal_uint32 ret=0;

    ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_REG_RST_MASK),
                                    (kal_uint8)(CON1_REG_RST_SHIFT)
                                    );
}
