#include "tz_sec_reg.h"

#define MOD "[TZ_SEC_CFG]"

#define TEE_DEBUG
#ifdef TEE_DEBUG
#define DBG_MSG(str, ...) do {print(str, ##__VA_ARGS__);} while(0)
#else
#define DBG_MSG(str, ...) do {} while(0)
#endif

extern void tz_emi_mpu_init(u32 start, u32 end, u32 mpu_region);

void tz_sec_mem_init(u32 start, u32 end, u32 mpu_region)
{    
    tz_emi_mpu_init(start, end, mpu_region);
}

static unsigned int tz_uffs(unsigned int x)
{
	unsigned int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

static void tz_set_field(volatile u32 *reg, u32 field, u32 val)
{
    u32 tv;

    if (field == 0)
        return;
    tv  = (u32)*reg;
    tv &= ~(field);
    tv |= ((val) << (tz_uffs((unsigned int)field) - 1));
    *reg = tv;
}

#define set_field(r,f,v)                tz_set_field((volatile u32*)r,f,v)
#define TZ_SET_FIELD(reg,field,val)     set_field(reg,field,val)

void tz_sram_sec_init(u32 start)
{
    /* Set Region Address Info */
    WRITE_REGISTER_UINT32(SRAMROM_SEC_ADDR, (start & SRAMROM_SEC_ADDR_MASK));
    WRITE_REGISTER_UINT32(SRAMROM_SEC_ADDR1, (SRAMROM_REG_1_ADDR & SRAMROM_SEC_ADDR_MASK));
    WRITE_REGISTER_UINT32(SRAMROM_SEC_ADDR2, (SRAMROM_REG_2_ADDR & SRAMROM_SEC_ADDR_MASK));
    TZ_SET_FIELD(SRAMROM_SEC_ADDR, SRAMROM_REG_0_EN, ENABLE_SEC_SEC1_PROTECTION);

    DBG_MSG("%s SRAMROM Secure Addr 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_ADDR));
    DBG_MSG("%s SRAMROM Secure Addr 1 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_ADDR1));
    DBG_MSG("%s SRAMROM Secure Addr 2 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_ADDR2));

    /* Set permission for Region 0 */
    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SEC_SEC0_DOM0, PERMIT_S_RW_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SEC_SEC0_DOM1, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SEC_SEC0_DOM2, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SEC_SEC0_DOM3, PERMIT_S_BLOCK_NS_BLOCK);

    /* Set permission for Region 1 */
    TZ_SET_FIELD(SRAMROM_SEC_CTRL2, SRAMROM_SEC_SEC1_DOM0, PERMIT_S_RW_NS_RW);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL2, SRAMROM_SEC_SEC1_DOM1, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL2, SRAMROM_SEC_SEC1_DOM2, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL2, SRAMROM_SEC_SEC1_DOM3, PERMIT_S_BLOCK_NS_BLOCK);

    DBG_MSG("%s SRAMROM Secure Control 2 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_CTRL2));

    /* Set permission for Region 2 */
    TZ_SET_FIELD(SRAMROM_SEC_CTRL5, SRAMROM_SEC_SEC1_DOM0, PERMIT_S_RW_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL5, SRAMROM_SEC_SEC1_DOM1, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL5, SRAMROM_SEC_SEC1_DOM2, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL5, SRAMROM_SEC_SEC1_DOM3, PERMIT_S_BLOCK_NS_BLOCK);

    DBG_MSG("%s SRAMROM Secure Control 5 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_CTRL5));

    /* Set permission for Region 3 */
    TZ_SET_FIELD(SRAMROM_SEC_CTRL6, SRAMROM_SEC_SEC1_DOM0, PERMIT_S_RW_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL6, SRAMROM_SEC_SEC1_DOM1, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL6, SRAMROM_SEC_SEC1_DOM2, PERMIT_S_BLOCK_NS_BLOCK);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL6, SRAMROM_SEC_SEC1_DOM3, PERMIT_S_BLOCK_NS_BLOCK);
    DBG_MSG("%s SRAMROM Secure Control 6 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_CTRL6));

    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SW_ROM_PD, ENABLE_SEC_SEC1_PROTECTION);
    TZ_SET_FIELD(SRAMROM_SEC_CTRL, SRAMROM_SEC_SEC1_EN, ENABLE_SEC_SEC1_PROTECTION);
    DBG_MSG("%s SRAMROM Secure Control 0x%x\n", MOD, READ_REGISTER_UINT32(SRAMROM_SEC_CTRL));
}
