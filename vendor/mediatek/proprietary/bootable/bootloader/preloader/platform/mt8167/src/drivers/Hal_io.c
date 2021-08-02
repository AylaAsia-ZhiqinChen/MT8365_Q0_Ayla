/** @file hal_io.cpp
 *  hal_io.cpp provides functions of register access
 */

#include "x_hal_io.h"
#include "dramc_common.h"
#include "dramc_pi_api.h"

#if 1//REG_ACCESS_PORTING_DGB
extern U8 RegLogEnable;
#endif

#if (fcFOR_CHIP_ID == MT8167)
const U32 arRegBaseAddrList[4] =
{
    Channel_A_DRAMC_NAO_BASE_ADDRESS,
    Channel_A_DRAMC_AO_BASE_ADDRESS,
    Channel_A_PHY_BASE_ADDRESS, 
    Channel_B_PHY_BASE_ADDRESS
};
#endif

DRAMC_CTX_T *pgCurrDramCtx;
void SaveCurrDramCtx(DRAMC_CTX_T *p)
{
    pgCurrDramCtx = p;
}

// Translate SW virtual base address to real HW base.
U32 u4RegBaseAddrTraslate(U32 u4reg_addr)
{
	U32 u4Offset = u4reg_addr & 0x1fff;
	U32 u4RegType = ((u4reg_addr-Channel_A_DRAMC_NAO_BASE_VIRTUAL) >> POS_BANK_NUM) & 0xf;

    if (u4reg_addr < Channel_A_DRAMC_NAO_BASE_VIRTUAL || u4reg_addr >= MAX_BASE_VIRTUAL) return u4reg_addr;

    if (u4RegType == 0)
    {
        // NAO Register
        if (u4Offset >= (DRAMC_REG_RK0_DQSOSC_STATUS-DRAMC_NAO_BASE_ADDRESS) && u4Offset < (DRAMC_REG_RK1_DQSOSC_STATUS-DRAMC_NAO_BASE_ADDRESS))
        {
            u4Offset += pgCurrDramCtx->rank * 0x100;
        }
    }
    else if (u4RegType == 1)
    {
        // AO Register
        if (u4Offset >= (DRAMC_REG_RK0_DQSOSC-DRAMC_AO_BASE_ADDRESS) && u4Offset < (DRAMC_REG_RK1_DQSOSC-DRAMC_AO_BASE_ADDRESS))
        {
            u4Offset += pgCurrDramCtx->rank * 0x100;
        }
        else if (u4Offset >= (DRAMC_REG_SHURK0_DQSCTL-DRAMC_AO_BASE_ADDRESS) && u4Offset < (DRAMC_REG_SHURK1_DQSCTL-DRAMC_AO_BASE_ADDRESS))
        {
            u4Offset += pgCurrDramCtx->rank * 0x100;
        }
    }
    else
    {
        if (u4Offset >= (DDRPHY_SHU1_R0_B0_DQ0-DDRPHY_BASE_ADDR) && u4Offset < (DDRPHY_SHU1_R1_B0_DQ0-DDRPHY_BASE_ADDR))
        {
            u4Offset += pgCurrDramCtx->rank * 0x100;
        }      
        else if (u4Offset >= (DDRPHY_MISC_STBERR_RK0_R-DDRPHY_BASE_ADDR) && u4Offset < (DDRPHY_MISC_STBERR_RK1_R-DDRPHY_BASE_ADDR))
        {
            u4Offset += pgCurrDramCtx->rank * 0x8;
        }
	   
    }
	
       //mcSHOW_DBG_MSG(("\n[u4RegBaseAddrTraslate]  0x%x => 0x%x(0x%x, 0x%x)", u4reg_addr , (arRegBaseAddrList[u4RegType] +u4Offset), arRegBaseAddrList[u4RegType], u4Offset));
	return (arRegBaseAddrList[u4RegType] +u4Offset); 
}

//-------------------------------------------------------------------------
/** ucDram_Register_Read
 *  DRAM register read (32-bit).
 *  @param  u4reg_addr    register address in 32-bit.
 *  @param  pu4reg_value  Pointer of register read value.
 *  @retval 0: OK, 1: FAIL 
 */
//-------------------------------------------------------------------------
// This function need to be porting by BU requirement
U32 u4Dram_Register_Read(U32 u4reg_addr)
{
    U32 u4reg_value;
    
    #if (fcFOR_CHIP_ID == MT8167)
    u4reg_addr = u4RegBaseAddrTraslate(u4reg_addr);
    #endif

   u4reg_value = (*(volatile unsigned int *)(u4reg_addr));

   return u4reg_value;
}


//-------------------------------------------------------------------------
/** ucDram_Register_Write
 *  DRAM register write (32-bit).
 *  @param  u4reg_addr    register address in 32-bit.
 *  @param  u4reg_value   register write value.
 *  @retval 0: OK, 1: FAIL 
 */
//-------------------------------------------------------------------------

// This function need to be porting by BU requirement
U8 ucDram_Register_Write(U32 u4reg_addr, U32 u4reg_value)
{
    U8 ucstatus;
    ucstatus = 0;

    #if (fcFOR_CHIP_ID == MT8167)
    u4reg_addr = u4RegBaseAddrTraslate(u4reg_addr);
    #endif
    
    (*(volatile unsigned int *)u4reg_addr) = u4reg_value;// real chip
    dsb();
        
#if REG_ACCESS_PORTING_DGB
	if(RegLogEnable)
	{
		mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_DBG]   ucDramC_Register_Write Reg(0x%X) = 0x%X\n",u4reg_addr,  u4reg_value));
	}
#endif

	//print("  *((UINT32P)(%s + 0x%x)) = 0x%x;\n", 
	//				(u4reg_addr>>12)== 0x10207 ? "DRAMC0_BASE" : ((u4reg_addr>>12)== 0x10211 ? "DDRPHY1_BASE" : ((u4reg_addr>>12)== 0x10212 ? "DDRPHY2_BASE" : "DRAMC_NAO_BASE")), 
	//				u4reg_addr&0xfff, u4reg_value);
	//print("*((UINT32P)(0x%x)) = 0x%x; //0x%x\n", u4reg_addr, u4reg_value, u4Dram_Register_Read(u4reg_addr));
    return ucstatus;
}

void vIO32Write4BMsk(U32 reg32, U32 val32, U32 msk32)
{
    U32 u4Val;
    
    val32 &=msk32;
    
    u4Val = u4Dram_Register_Read(reg32);
    u4Val = ((u4Val &~msk32)|val32);   
    ucDram_Register_Write(reg32, u4Val);
}

void vIO32Write4B_All(U32 reg32, U32 val32)
{
    U8 ii, u1AllCount = 1;
    U32 u4RegType = (reg32 & (0xf <<POS_BANK_NUM));
    
    if(u4RegType >=Channel_A_PHY_BASE_VIRTUAL)//PHY 
    {
        reg32 &= 0xffff;
        u1AllCount =2;
        reg32 += Channel_A_PHY_BASE_VIRTUAL;
    }

	for(ii=0; ii< u1AllCount; ii++)
	{
	    vIO32Write4B(reg32+((U32)ii<<POS_BANK_NUM), val32);
	}   


#if 0
    // debug
    //mcSHOW_DBG_MSG(("RISCWrite : address %05x data %08x\n", reg32&0x000fffff, val32));
    //mcSHOW_DBG_MSG(("RISCWrite : address %05x data %08x\n", (reg32+((U32)1<<POS_BANK_NUM))&0x000fffff, val32));
    //mcSHOW_DBG_MSG(("RISCWrite : address %05x data %08x\n", (reg32+((U32)2<<POS_BANK_NUM))&0x000fffff, val32));
#endif
}

void vIO32Write4BMsk_All(U32 reg32, U32 val32, U32 msk32)
{
    U32 u4Val;
    U8 ii, u1AllCount = 1;
    U32 u4RegType = (reg32 & (0xf <<POS_BANK_NUM));

    if(u4RegType >=Channel_A_PHY_BASE_VIRTUAL)//PHY 
    {
        reg32 &= 0xffff;
        u1AllCount =2;
        reg32 += Channel_A_PHY_BASE_VIRTUAL;
    }

    for(ii=0; ii< u1AllCount; ii++)
    {
        u4Val = u4Dram_Register_Read(reg32+((U32)ii<<POS_BANK_NUM));    
        u4Val = ((u4Val &~msk32)|val32);    
        ucDram_Register_Write(reg32+((U32)ii<<POS_BANK_NUM), u4Val);
    }
}
