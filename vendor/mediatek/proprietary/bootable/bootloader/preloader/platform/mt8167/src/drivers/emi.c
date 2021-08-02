//=============================================================================
//  Include Files
//=============================================================================
#include "x_hal_io.h"
#include "dramc_common.h"
#include "emi.h"
#include "emi_hw.h"
#include "custom_emi.h"   // fix build error: emi_settings
#if __FLASH_TOOL_DA__
   #include "../auxadc/mtk_auxadc_sw.h"
#else
   #include "platform.h"
   #include "upmu_hw.h"
   #include "pmic.h"
   #include "dram_buffer.h"
#endif

#if CFG_BOOT_ARGUMENT
#define bootarg g_dram_buf->bootarg
#endif

#ifdef DRAM_ETT
static char UserCommand[128];
static unsigned char command_index = 0;
static int global_which_test=0;
static int voltage_sel = ETT_NVCORE_NVDRAM;
extern SAVE_TO_SRAM_FORMAT_CHANNEL_T gDRAM_CALIB_LOG;
static void TestMenu(void);
static void print_ETT_message(void);
static int complex_mem_test_ett(unsigned int start, unsigned int len);
#endif

#ifdef DRAM_CALIB_LOG
static U16 read_rtc(U16 unit);
void dram_klog_clean(void);
void dram_klog_init(void);
//int dram_klog_check(void);
//void dram_klog_print(void);
static DRAM_KLOG_HEAD *klog_head;
static DRAM_KLOG_TAIL *klog_tail;
static unsigned int *klog_data;
#endif //end #ifdef DRAM_CALIB_LOG
//=============================================================================
//  Definition
//=============================================================================

//=============================================================================
//  Global Variables
//=============================================================================
int emi_setting_index = -1;
extern DRAMC_CTX_T *psCurrDramCtx;
extern int Init_DRAM(EMI_SETTINGS *emi_set);
//=============================================================================
//  External references
//=============================================================================
extern DRAMC_CTX_T *psCurrDramCtx;
extern char* opt_dle_value;

void Dump_EMIRegisters(DRAMC_CTX_T *p);

#define EMI_APB_BASE    0x10205000

#if (fcFOR_CHIP_ID == MT8167)
void EMI_Init(EMI_SETTINGS *emi_set)
{
	*(volatile unsigned *)EMI_CONA = emi_set->EMI_CONA_VAL;
	*(volatile unsigned *)EMI_CONF = emi_set->EMI_CONF_VAL; //0x00421000 SCRAMBLE
	*(volatile unsigned *)EMI_CONH = emi_set->EMI_CONH_VAL; //0x00000003  DCM_RATIO

	*(volatile unsigned *)EMI_CONB	 = 0x17283544;//overhead 0-4
	*(volatile unsigned *)EMI_CONC	 = 0x0a1a0b1a;//overhead 5-8
	*(volatile unsigned *)EMI_COND	 = 0x3657587a;
	*(volatile unsigned *)EMI_CONE	 = 0xffff0848;
	*(volatile unsigned *)EMI_CONG	 = 0x2b2b2a38;
	*(volatile unsigned *)EMI_CONI	 = 0x00000006; //?
	*(volatile unsigned *)EMI_CONM	 = 0x000006b8;

	*(volatile unsigned *)EMI_TEST0	 = 0xcccccccc;
	*(volatile unsigned *)EMI_TEST1	 = 0xcccccccc;
	*(volatile unsigned *)EMI_TESTB	 = 0x00000027;
	*(volatile unsigned *)EMI_TESTC	 = 0x38460000;
	*(volatile unsigned *)EMI_TESTD	 = 0x00000000;

	*(volatile unsigned *)EMI_ARBI	 = 0x20406188; // olympus: 0x00006108;
	*(volatile unsigned *)EMI_SLCT	 = 0xff02ff00; // olympus: 0x0001ff00;
	*(volatile unsigned *)EMI_ARBA	 = 0xa0a07c4f; //APMCU => 15/64
	*(volatile unsigned *)EMI_ARBB	 = 0xa0a070dd; //MM => 29/64
	*(volatile unsigned *)EMI_ARBC	 = 0xffff7045; //Peri => 5/64
	//*(volatile unsigned *)EMI_ARBD	 = 0x400070cb; //8167 no MD
	//*(volatile unsigned *)EMI_ARBE	 = 0x2030604c; //8167 no MD
	*(volatile unsigned *)EMI_ARBF	 = 0xffff704a; //MFG => 10/64
	*(volatile unsigned *)EMI_ARBG	 = 0x400a7045; //Conn => 5/64

	mcSHOW_DBG_MSG(("EMI_CONA=0x%x\n",*(volatile unsigned *)EMI_CONA));
	mcSHOW_DBG_MSG(("EMI_CONF=0x%x\n",*(volatile unsigned *)EMI_CONF));
	mcSHOW_DBG_MSG(("EMI_CONH=0x%x\n",*(volatile unsigned *)EMI_CONH));

}
#endif

void Dump_EMIRegisters(DRAMC_CTX_T *p)
{

  U32 uiAddr;

  for (uiAddr=0; uiAddr<0x160; uiAddr+=4)
  {
    mcSHOW_DBG_MSG(("EMI offset:%x, value:%x\n", uiAddr, *(volatile unsigned *)(EMI_APB_BASE+uiAddr)));
  }
}

void Dump_Debug_Registers(DRAMC_CTX_T *p, int index)
{
#if 0
	mcSHOW_DBG_MSG(("debug index: %d\n", index));
	mcSHOW_DBG_MSG(("EMI_CONA=0x%x\n",*(volatile unsigned *)EMI_CONA));
	mcSHOW_DBG_MSG(("EMI_CONF=0x%x\n",*(volatile unsigned *)EMI_CONF));
	mcSHOW_DBG_MSG(("EMI_CONH=0x%x\n",*(volatile unsigned *)EMI_CONH));

	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DRAMC0_BASE + 0x004C), *(volatile unsigned *)(DRAMC0_BASE + 0x004C)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DRAMC0_BASE + 0x0058), *(volatile unsigned *)(DRAMC0_BASE + 0x0058)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DRAMC0_BASE + 0x0C00), *(volatile unsigned *)(DRAMC0_BASE + 0x0C00)));
    mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY1_BASE + 0x00a4), *(volatile unsigned *)(DDRPHY1_BASE + 0x00a4)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY1_BASE + 0x0124), *(volatile unsigned *)(DDRPHY1_BASE + 0x0124)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY2_BASE + 0x00a4), *(volatile unsigned *)(DDRPHY2_BASE + 0x00a4)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY2_BASE + 0x0124), *(volatile unsigned *)(DDRPHY2_BASE + 0x0124)));

	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY1_BASE + 0x0c14), *(volatile unsigned *)(DDRPHY1_BASE + 0x0c14)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY1_BASE + 0x0c94), *(volatile unsigned *)(DDRPHY1_BASE + 0x0124)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY2_BASE + 0x0c14), *(volatile unsigned *)(DDRPHY2_BASE + 0x0c14)));
	mcSHOW_DBG_MSG(("addr:%x, value:%x\n", (DDRPHY2_BASE + 0x0c94), *(volatile unsigned *)(DDRPHY2_BASE + 0x0c94)));

	mcSHOW_DBG_MSG(("0x10000000=0x%x\n",*(volatile unsigned *)0x10000000));
	mcSHOW_DBG_MSG(("0x102007F0=0x%x\n",*(volatile unsigned *)0x102007F0));
	mcSHOW_DBG_MSG(("0x102112A0=0x%x\n",*(volatile unsigned *)0x102112A0));
	mcSHOW_DBG_MSG(("0x102122A0=0x%x\n",*(volatile unsigned *)0x102122A0));
	mcSHOW_DBG_MSG(("0x1021100C=0x%x\n",*(volatile unsigned *)0x1021100C));
	mcSHOW_DBG_MSG(("0x102110B8=0x%x\n",*(volatile unsigned *)0x102110B8));
	mcSHOW_DBG_MSG(("0x102111BC=0x%x\n",*(volatile unsigned *)0x102111BC));

#endif
}

int mt_get_dram_type(void)
{
    EMI_SETTINGS *emi_set = mt_get_emisettings();
	return (emi_set->type & 0xF);
}

#ifdef DDR_RESERVE_MODE
extern U32 g_ddr_reserve_enable;
extern U32 g_ddr_reserve_success;
#define TIMEOUT 3
#endif

#ifdef DDR_RESERVE_MODE
extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;
#define TIMEOUT 3
//extern void before_Dramc_DDR_Reserved_Mode_setting(void);

#endif

void release_dram(void)
{
#ifdef DDR_RESERVE_MODE
    int i;
    int counter = TIMEOUT;

    Dramc_DDR_Reserved_Mode_setting();
    rgu_release_rg_dramc_conf_iso();//Release DRAMC/PHY conf ISO
    rgu_release_rg_dramc_iso();//Release PHY IO ISO
    rgu_release_rg_dramc_sref();//Let DRAM Leave SR

    // setup for EMI: touch center EMI and channel EMI to enable CLK
    print("[DDR reserve] EMI CONA: %x\n", *(volatile unsigned int*)EMI_CONA);
    for (i=0;i<10;i++);

    while(counter)
    {
        if(rgu_is_dram_slf() == 0) /* expect to exit dram-self-refresh */
            break;
        counter--;
    }

    if(counter == 0)
    {
        if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
        {
            print("[DDR Reserve] release dram from self-refresh FAIL!\n");
            g_ddr_reserve_success = 0;
        }
    }
    else
    {
         print("[DDR Reserve] release dram from self-refresh PASS!\n");
    }
    //Expect to Use LPDDR3200 and PHYPLL as output, so no need to handle
    //shuffle status since the status will be reset by system reset
    //There is an PLLL_SHU_GP in SPM which will reset by system reset
    return;
#endif
}

void check_ddr_reserve_status(void)
{
#ifdef DRAM_CALIB_LOG
    dram_klog_clean();
#endif

#ifdef DDR_RESERVE_MODE
    int counter = TIMEOUT;
    if(rgu_is_reserve_ddr_enabled())
    {
      g_ddr_reserve_enable = 1;
      if(rgu_is_reserve_ddr_mode_success())
      {
        while(counter)
        {
          if(rgu_is_dram_slf())
          {
            g_ddr_reserve_success = 1;
            break;
          }
          counter--;
        }
        if(counter == 0)
        {
          print("[DDR Reserve] ddr reserve mode success but DRAM not in self-refresh!\n");
          g_ddr_reserve_success = 0;
        }
      }
      else
      {
        print("[DDR Reserve] ddr reserve mode FAIL!\n");
        g_ddr_reserve_success = 0;
      }
	  /* release dram, no matter success or failed */
      release_dram();
    }
    else
    {
      print("[DDR Reserve] ddr reserve mode not be enabled yet\n");
      g_ddr_reserve_enable = 0;
    }
#endif
}

static int mt_get_dram_type_for_dis(void)
{
    int i;
    int type = 2;
    type = (emi_settings[0].type & 0xF);
    for (i = 0 ; i < num_of_emi_records; i++)
    {
      //print("[EMI][%d] type%d\n",i,type);
      if (type != (emi_settings[i].type & 0xF))
      {
          mcSHOW_DBG_MSG(("It's not allow to combine two type dram when combo discrete dram enable\n"));
          ASSERT(0);
          break;
      }
    }
    return type;
}

#if !__FLASH_TOOL_DA__

/*
 * setup block correctly, we should hander both 4GB mode and
 * non-4GB mode.
 */
void get_orig_dram_rank_info(dram_info_t *orig_dram_info)
{
	int i, j;
	int dram_rank_nr = 0;
	u64 base = DRAM_BASE;
	unsigned int rank_size[4] = {0};	/* fix uninitialized scalar variable*/

	dram_rank_nr = get_dram_rank_nr();
	if (dram_rank_nr < 0)
	{
		printf("[ERR] get_dram_rank_nr() < 0 !!!");
		dram_rank_nr = 0;
	}

	orig_dram_info->rank_num = (u32)dram_rank_nr;
	get_dram_rank_size(rank_size);

	orig_dram_info->rank_info[0].start = base;
	for (i = 0; i < orig_dram_info->rank_num; i++) {

		orig_dram_info->rank_info[i].size = (u64)rank_size[i];

		if (i > 0) {
			orig_dram_info->rank_info[i].start =
				orig_dram_info->rank_info[i - 1].start +
				orig_dram_info->rank_info[i - 1].size;
		}
		printf("orig_dram_info[%d] start: 0x%llx, size: 0x%llx\n",
				i, orig_dram_info->rank_info[i].start,
				orig_dram_info->rank_info[i].size);
	}

    for(j=i; j<4; j++)
    {
        orig_dram_info->rank_info[j].start = 0;
        orig_dram_info->rank_info[j].size = 0;
    }
}
#endif //#if !__FLASH_TOOL_DA__

#if 0
static char id[22];
static int emmc_nand_id_len=16;
static int fw_id_len;
static int enable_combo_dis = 0;
#endif

static int mt_get_mdl_number (void)
{
	return 0;
#if 0
    static int found = 0;
    static int mdl_number = -1;
    int i;
    int j;
    int has_emmc_nand = 0;
    int discrete_dram_num = 0;
    int mcp_dram_num = 0;

    unsigned int dram_type = 3, dram_pinmux = 2;
    DRAM_INFO_BY_MRR_T DramInfo;
    DRAM_DRAM_TYPE_T Dis_DramType = TYPE_LPDDR3;

    memset(&DramInfo, 0, sizeof(DramInfo));

    if (!(found))
    {
        int result=0;
        //platform_get_mcp_id (id, emmc_nand_id_len,&fw_id_len);
        for (i = 0 ; i < num_of_emi_records; i++)
        {
            if ((emi_settings[i].type & 0x0F00) == 0x0000)
            {
                discrete_dram_num ++;
            }
            else
            {
                mcp_dram_num ++;
            }
        }

        /*If the number >=2  &&
         * one of them is discrete DRAM
         * enable combo discrete dram parse flow
         * */
        if ((discrete_dram_num > 0) && (num_of_emi_records >= 2))
        {
            /* if we enable combo discrete dram
             * check all dram are all same type and not DDR3
             * */
            enable_combo_dis = 1;
            dram_type = emi_settings[0].type & 0x000F;
            dram_pinmux = emi_settings[0].PIN_MUX_TYPE & 0x0003;
            default_emi_setting.PIN_MUX_TYPE = dram_pinmux;
            for (i = 0 ; i < num_of_emi_records; i++)
            {
                if ((dram_type != (emi_settings[i].type & 0x000F)) || (dram_pinmux != (emi_settings[i].PIN_MUX_TYPE & 0x0003)))
                {
                    mcSHOW_DBG_MSG(("[EMI] Combo discrete dram only support when combo lists are all same dram type and dram pinmux."));
                    ASSERT(0);
                }
                if (((emi_settings[i].type & 0x000F) == TYPE_PCDDR3) || ((emi_settings[i].type & 0x000F) == TYPE_PCDDR4))
                {
                    // has PCDDR3/4, disable combo discrete drame, no need to check others setting
                    enable_combo_dis = 0;
                    break;
                }
                dram_type = emi_settings[i].type & 0x000F;
            }

        }
        mcSHOW_DBG_MSG(("[EMI] mcp_dram_num:%d,discrete_dram_num:%d,enable_combo_dis:%d\r\n",mcp_dram_num,discrete_dram_num,enable_combo_dis));
        /*
         *
         * 0. if there is only one discrete dram, use index=0 emi setting and boot it.
         * */
        if ((0 == mcp_dram_num) && (1 == discrete_dram_num))
        {
            mdl_number = 0;
            found = 1;
#if 0
		#if ENABLE_MRR_AFTER_FIRST_K
			Dis_DramType = mt_get_dram_type_for_dis();
			if (Dis_DramType == TYPE_LPDDR2 || Dis_DramType == TYPE_LPDDR3)
				Init_DRAM(Dis_DramType, CBT_NORMAL_MODE, &DramInfo);
		#endif
#endif
            return mdl_number;
        }

#if 1
        /* 1.
         * if there is MCP dram in the list, we try to find emi setting by emmc ID
         * */
        if (mcp_dram_num > 0)
        {
            result = platform_get_mcp_id (id, emmc_nand_id_len,&fw_id_len);

            for (i = 0; i < num_of_emi_records; i++)
            {
                if (emi_settings[i].type != 0)
                {
                    if ((emi_settings[i].type & 0xF00) != 0x000)
                    {
                        if (result == 0)
                        {   /* valid ID */

                            if ((emi_settings[i].type & 0xF00) == 0x100)
                            {
                                /* NAND */
                                if (memcmp(id, emi_settings[i].ID, emi_settings[i].id_length) == 0){
                                    memset(id + emi_settings[i].id_length, 0, sizeof(id) - emi_settings[i].id_length);
                                    mdl_number = i;
                                    found = 1;
                                    break; /* found */
                                }
                            }
                            else
                            {

                                /* eMMC */
                                if (memcmp(id, emi_settings[i].ID, emi_settings[i].id_length) == 0)
                                {
                                    print("fw id len:%d\n",emi_settings[i].fw_id_length);
                                    if (emi_settings[i].fw_id_length > 0)
                                    {
                                        char fw_id[6];
                                        memset(fw_id, 0, sizeof(fw_id));
                                        memcpy(fw_id,id+emmc_nand_id_len,fw_id_len);
                                        for (j = 0; j < fw_id_len;j ++){
                                            print("0x%x, 0x%x ",fw_id[j],emi_settings[i].fw_id[j]);
                                        }
                                        if(memcmp(fw_id,emi_settings[i].fw_id,fw_id_len) == 0)
                                        {
                                            mdl_number = i;
                                            found = 1;
                                            break; /* found */
                                        }
                                        else
                                        {
                                            print("[EMI] fw id match failed\n");
                                        }
                                    }
                                    else
                                    {
                                        mdl_number = i;
                                        found = 1;
                                        break; /* found */
                                    }
                                }
                                else{
                                      print("[EMI] index(%d) emmc id match failed\n",i);
                                }

                            }
                        }
                    }
                }
            }
        }
#endif

#if 1
        /* 2. find emi setting by MODE register 5
         * */
        // if we have found the index from by eMMC ID checking, we can boot android by the setting
        // if not, we try by vendor ID
        if ((0 == found) && (1 == enable_combo_dis))
        {
            EMI_SETTINGS *emi_set;
            int rank0_size=0, rank1_size=0;

            Dis_DramType = mt_get_dram_type_for_dis();

#if ENABLE_MRR_AFTER_FIRST_K
            Init_DRAM(Dis_DramType, CBT_NORMAL_MODE, &DramInfo);
#endif

            //try to find discrete dram by DDR2_MODE_REG5(vendor ID)
            for (i = 0; i < num_of_emi_records; i++)
            {
                mcSHOW_DBG_MSG(("emi_settings[%d].MODE_REG_5:%x,emi_settings[%d].type:%x, vender_id=%x\n", i, emi_settings[i].iLPDDR3_MODE_REG_5, i, emi_settings[i].type, DramInfo.u2MR5VendorID));
                //only check discrete dram type
                if ((emi_settings[i].type & 0x0F00) == 0x0000)
                {
                    //support for compol discrete dram
                    if (emi_settings[i].iLPDDR3_MODE_REG_5 == DramInfo.u2MR5VendorID)
                    {
                        rank0_size = DramInfo.u4MR8Density[0][0] + DramInfo.u4MR8Density[1][0];
                        rank1_size = DramInfo.u4MR8Density[0][1] + DramInfo.u4MR8Density[1][1];

                        mcSHOW_DBG_MSG(("emi_settings[%d].DRAM_RANK_SIZE[0]:0x%x, DRAM_RANK_SIZE[1]:0x%x, rank0_size:0x%x, rankl_size:0x%x\n",i,emi_settings[i].DRAM_RANK_SIZE[0], emi_settings[i].DRAM_RANK_SIZE[1], rank0_size, rank1_size));
                        if((emi_settings[i].DRAM_RANK_SIZE[0] == rank0_size) && (emi_settings[i].DRAM_RANK_SIZE[1] == rank1_size))
                        {
                            mdl_number = i;
                            found = 1;
                            break;
                        }
                    }
                }
            }
        }
#endif
        mcSHOW_DBG_MSG(("found:%d,i:%d\n",found,i));
        //while(1);
    }
    return mdl_number;
#endif
}

int get_dram_rank_nr (void)
{

    int index;
    int emi_cona;

#ifdef COMBO_MCP
  #ifdef DDR_RESERVE_MODE
    if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
    {
      emi_cona = *(volatile unsigned int*)(EMI_CONA);
    }
    else
  #endif
    {
      index = mt_get_mdl_number();
      if (index < 0 || index >=  num_of_emi_records)
      {
          return -1;
      }

      emi_cona = emi_settings[index].EMI_CONA_VAL;
    }
#else
    emi_cona = default_emi_setting.EMI_CONA_VAL;
#if CFG_FPGA_PLATFORM
    return 1;
#endif
#endif

    if ((emi_cona & (1 << 17)) != 0 || //for channel 0
        (emi_cona & (1 << 16)) != 0 )  //for channel 1
        return 2;
    else
        return 1;
}

void get_dram_rank_size_by_EMI_CONA (unsigned int dram_rank_size[])
{
    unsigned col_bit, row_bit, ch0_rank0_size, ch0_rank1_size, ch1_rank0_size, ch1_rank1_size;
#ifndef COMBO_MCP
    unsigned emi_cona = default_emi_setting.EMI_CONA_VAL;
    unsigned emi_conh = default_emi_setting.EMI_CONH_VAL;
#else
    unsigned emi_cona = *(volatile unsigned int*)(EMI_CONA);
    unsigned emi_conh = *(volatile unsigned int*)(EMI_CONH);
#endif

    dram_rank_size[0] = 0;
    dram_rank_size[1] = 0;

    ch0_rank0_size = (emi_conh >> 16) & 0xf;
    ch0_rank1_size = (emi_conh >> 20) & 0xf;
    ch1_rank0_size = (emi_conh >> 24) & 0xf;
    ch1_rank1_size = (emi_conh >> 28) & 0xf;

    //Channel 0
    {
        if(ch0_rank0_size == 0)
        {
            //rank 0 setting
            col_bit = ((emi_cona >> 4) & 0x03) + 9;
            row_bit = ((emi_cona >> 12) & 0x03) + 13;
            dram_rank_size[0] = (1 << (row_bit + col_bit)) * 4 * 8; // 4 byte * 8 banks
        }
        else
        {
            dram_rank_size[0] = (ch0_rank0_size * 256 << 20);
        }

        if (0 != (emi_cona &  (1 << 17)))   //rank 1 exist
        {
            if(ch0_rank1_size == 0)
            {
                col_bit = ((emi_cona >> 6) & 0x03) + 9;
                row_bit = ((emi_cona >> 14) & 0x03) + 13;
                dram_rank_size[1] = ((1 << (row_bit + col_bit)) * 4 * 8); // 4 byte * 8 banks
            }
            else
            {
                dram_rank_size[1] = (ch0_rank1_size * 256 << 20);
            }
        }
    }

    if(0 != (emi_cona & 0x01))     //channel 1 exist
    {
        if(ch1_rank0_size == 0)
        {
            //rank0 setting
            col_bit = ((emi_cona >> 20) & 0x03) + 9;
            row_bit = ((emi_cona >> 28) & 0x03) + 13;
            dram_rank_size[0] += ((1 << (row_bit + col_bit)) * 4 * 8); // 4 byte * 8 banks
        }
        else
        {
            dram_rank_size[0] += (ch1_rank0_size * 256 << 20);
        }

        if (0 != (emi_cona &  (1 << 16)))   //rank 1 exist
        {
            if(ch1_rank1_size == 0)
            {
                col_bit = ((emi_cona >> 22) & 0x03) + 9;
                row_bit = ((emi_cona >> 30) & 0x03) + 13;
                dram_rank_size[1] += ((1 << (row_bit + col_bit)) * 4 * 8); // 4 byte * 8 banks
            }
            else
            {
                dram_rank_size[1] += (ch1_rank1_size * 256 << 20);
            }
        }
    }

    mcSHOW_DBG_MSG(("DRAM rank0 size:0x%x\nDRAM rank1 size:0x%x\n", dram_rank_size[0], dram_rank_size[1]));
}

void get_dram_rank_size (unsigned int dram_rank_size[])
{

#ifdef COMBO_MCP
    int index, rank_nr, i;

  #ifdef DDR_RESERVE_MODE
    if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
    {
        get_dram_rank_size_by_EMI_CONA(dram_rank_size);
    }
    else
  #endif
    {
        index = mt_get_mdl_number();

        if (index < 0 || index >= num_of_emi_records)
        {
            return;
        }

        rank_nr = get_dram_rank_nr();

        for(i = 0; i < rank_nr; i++){
            dram_rank_size[i] = emi_settings[index].DRAM_RANK_SIZE[i];

            mcSHOW_DBG_MSG(("%d:dram_rank_size:%x\n",i,dram_rank_size[i]));
        }
    }
    return;
#else
    get_dram_rank_size_by_EMI_CONA(dram_rank_size);
    return;
#endif
}

DRAM_ADC_SIZE_T get_dram_size_by_adc(void)
{
    int ret = 0, data[4] = {0,0,0,0}, voltage = 0, rawvalue = 0;
    DRAM_ADC_SIZE_T size;

    ret = IMM_GetOneChannelValue(1,  data, &rawvalue);

    if (ret == 0)
    {
        voltage = rawvalue * 1500 / 4096;
        if ((voltage >= 0) && (voltage < 300))
        	size = DRAM_ADC_SIZE_1GB;
        else if ((voltage >= 300) && (voltage < 700))
        	size = DRAM_ADC_SIZE_2GB;
        else if ((voltage >= 700) && (voltage < 1100))
        	size = DRAM_ADC_SIZE_1_5GB;
        else if ((voltage > 1100) && (voltage <= 1450))
        	size = DRAM_ADC_SIZE_512MB;
        else
            size = DRAM_ADC_SIZE_1GB;
    }
    else
    {
        size = DRAM_ADC_SIZE_1GB;
    	print("get raw value error %d \n", ret);
    }

    mcSHOW_DBG_MSG(("get dram size from AUXADC1 %d %d\n", voltage, size));

    return size;
}

U8 mt_get_dram_comsetting_type(EMI_SETTINGS *emi_set)
{
	U8 u1dramcom_type = DRAM_COMSETTING_DEFAULT;

	if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X16X2)
	{//DDR3
		if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 31))
		{//x32
			u1dramcom_type = DRAM_COMSETTING_DDR3_X32;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 30))
		{//x16
			u1dramcom_type = DRAM_COMSETTING_DDR3_X16;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 29))
		{//x8

		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
		{//ASYM
			u1dramcom_type = DRAM_COMSETTING_DDR3_ASYM;
		}
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR4X16X2)
	{//DDR4
		if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 31))
		{//x32
			u1dramcom_type = DRAM_COMSETTING_DDR4_X32;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 30))
		{//x16
			u1dramcom_type = DRAM_COMSETTING_DDR4_X16;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 29))
		{//x8

		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
		{//ASYM
			u1dramcom_type = DRAM_COMSETTING_DDR4_ASYM;
		}
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_LPDDR3_178BALL)
	{//LP3_178
		u1dramcom_type = DRAM_COMSETTING_LP3_178BALL;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_LPDDR3_168BALL)
	{//LP3_168
		u1dramcom_type = DRAM_COMSETTING_LP3_168BALL;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X8)
	{//DDR3x8
		u1dramcom_type = DRAM_COMSETTING_DDR3_X8;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X4)
	{//DDR3x4
		u1dramcom_type = DRAM_COMSETTING_DDR3_X4;
	}

	return u1dramcom_type;
}

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X4[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x00002020, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1GB
	{0x00002020, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1GB
	{0x00002020, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1GB
	{0x00002020, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X8[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x00002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//2GB
	{0x00003012, 0x00000003, {0x80000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1.5GB
	{0x00003012, 0x00000003, {0x80000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x00003012, 0x00000003, {0x80000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000}
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X16[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x00003010, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//256MB
	{0x00001010, 0x00000003, {0x10000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//128MB
	{0x00000010, 0x00000003, {0x08000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x00002010, 0x00000003, {0x20000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000}
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X32[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x00002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//2GB
	{0x00003012, 0x00000003, {0x80000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1.5GB
	{0x00002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x00002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000}
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR4X16[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x02003010, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//256MB
	{0x02001010, 0x00000003, {0x10000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x02002010, 0x00000003, {0x20000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x02002010, 0x00000003, {0x20000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000}
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR4X32[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0x02002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//2GB
	{0x02003012, 0x00000003, {0x80000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//1.5GB
	{0x02002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000},
	//512MB
	{0x02002012, 0x00000003, {0x40000000, 0x00000000, 0x00000000, 0x00000000}, 0x00000000, 0x00000000}
};

void EMI_Comsetting_Config(EMI_SETTINGS *emi_set)
{
	U8 u1dramcom_type;
	DRAM_ADC_SIZE_T dram_adc_size;
	DRAM_COM_SETTINGS *pDramCom = &DRAM_ComSetting_DDR3X32[0];
	DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

	if(dram_type == TYPE_LPDDR3 || dram_type == TYPE_LPDDR2)
    {//LP3 don't care ADC pin
    	dram_adc_size = DRAM_ADC_SIZE_MAX;
		u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
    }
    else
    {//PC3 & PC4 need check ADC pin
        dram_adc_size = get_dram_size_by_adc();
		u1dramcom_type = mt_get_dram_comsetting_type(emi_set);
    }

	switch (u1dramcom_type)
	{
		case DRAM_COMSETTING_DDR3_X4:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x4 "));
			pDramCom = &DRAM_ComSetting_DDR3X4[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X8:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x8 "));
			pDramCom = &DRAM_ComSetting_DDR3X8[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X16:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x16 "));
			pDramCom = &DRAM_ComSetting_DDR3X16[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X32:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x32 "));
			pDramCom = &DRAM_ComSetting_DDR3X32[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR4_X16:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 x16 "));
			pDramCom = &DRAM_ComSetting_DDR4X16[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR4_X32:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 x32 "));
			pDramCom = &DRAM_ComSetting_DDR4X32[dram_adc_size];
			break;
		case DRAM_COMSETTING_LP3_178BALL:
			mcSHOW_DBG_MSG(("[EMI] Use Common LP3 178BALL x32 "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emi gen
			break;
		case DRAM_COMSETTING_LP3_168BALL:
			mcSHOW_DBG_MSG(("[EMI] Use Common LP3 168BALL x32 "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DDR3_ASYM:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 ASYM "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DDR4_ASYM:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 ASYM "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DEFAULT:
		default:
			mcSHOW_DBG_MSG(("[EMI] Use default emigen "));
			break;
	}
	mcSHOW_DBG_MSG(("emi settings \r\n"));

	if(u1dramcom_type != DRAM_COMSETTING_DEFAULT)
	{
		emi_set->EMI_CONA_VAL = pDramCom->EMI_CONA_VAL;
		emi_set->EMI_CONH_VAL = pDramCom->EMI_CONH_VAL;
		emi_set->DRAM_RANK_SIZE[0] = pDramCom->DRAM_RANK_SIZE[0];
		emi_set->DRAM_RANK_SIZE[1] = pDramCom->DRAM_RANK_SIZE[1];
	}

	mcSHOW_DBG_MSG(("[EMI] Config emi settings:\n"));
	mcSHOW_DBG_MSG(("EMI_CONA=0x%x,   EMI_CONH=0x%x\n",emi_set->EMI_CONA_VAL, emi_set->EMI_CONH_VAL));
	mcSHOW_DBG_MSG(("EMI_RANK0=0x%x, EMI_RANK1=0x%x\n",emi_set->DRAM_RANK_SIZE[0], emi_set->DRAM_RANK_SIZE[1]));

}

void mt_set_emisettings(int index)
{
	emi_setting_index = index;
}

EMI_SETTINGS *mt_get_emisettings(void)
{
	EMI_SETTINGS *emi_set;
    if(emi_setting_index == -1)
        emi_set = &default_emi_setting;
    else
        emi_set = &emi_settings[emi_setting_index];
	return emi_set;
}

#if CFG_FPGA_PLATFORM
void mt_set_emi (void)
{
}
#else
void mt_set_emi(void)
{
    int index;
    EMI_SETTINGS *emi_set;
    DRAM_DRAM_TYPE_T dram_type;
#ifdef DRAM_ETT
    unsigned int rank_size[4] = {0, 0, 0, 0};
#endif

#ifdef DRAM_CALIB_LOG
    dram_klog_init();
#endif

#if !__FLASH_TOOL_DA__
#ifdef COMBO_MCP
    index = mt_get_mdl_number();
    mcSHOW_DBG_MSG(("[EMI] MDL number = %d \r\n", index));
    if (index < 0 || index >=  num_of_emi_records)
    {
        mcSHOW_DBG_MSG(("[EMI] setting failed 0x%x\r\n", index));
        ASSERT(0);
    }
    else
    {
		mt_set_emisettings(index);
		emi_set = mt_get_emisettings();
    }
#else
	emi_set = &default_emi_setting;
#endif
#else
	emi_set = mt_get_emisettings();
#endif//#if !__FLASH_TOOL_DA__

	dram_type = mt_get_dram_type();

#ifdef DDR_RESERVE_MODE
    if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==0)
        Before_Init_DRAM_While_Reserve_Mode_fail(dram_type);
#endif

	switch (dram_type)
	{
		case TYPE_DDR1:
			mcSHOW_DBG_MSG(("[EMI] DDR1\r\n"));
			break;
		case TYPE_LPDDR2:
			mcSHOW_DBG_MSG(("[EMI] LPDDR2\r\n"));
			break;
		case TYPE_LPDDR3:
			mcSHOW_DBG_MSG(("[EMI] LPDDR3\r\n"));
			break;
		case TYPE_PCDDR3:
			mcSHOW_DBG_MSG(("[EMI] PCDDR3\r\n"));
			break;
		case TYPE_PCDDR4:
			mcSHOW_DBG_MSG(("[EMI] PCDDR4\r\n"));
			break;
		default:
			mcSHOW_DBG_MSG(("[EMI] unknown dram type:%d\r\n", dram_type));
			break;
	}

#ifndef DRAM_HQA
	//set Vdram by ddr type
	//Vcore 1.15V
	//pmic_config_interface(0x312, 0x48, 0x7f, 0);
	//pmic_config_interface(0x314, 0x48, 0x7f, 0);
	if (dram_type == TYPE_LPDDR3 || dram_type == TYPE_LPDDR2)
	{
		//Vmem 1.20V
		pmic_config_interface(0x554, 0x0, 0x3, 4);
		pmic_config_interface(0x554, 0x0, 0xf, 8);
	}
	else if(dram_type == TYPE_PCDDR3)
	{
		//Vmem 1.35V
		pmic_config_interface(0x554, 0x1, 0x3, 4);
		pmic_config_interface(0x554, 0x0, 0xf, 8);
	}
	else //TYPE_PCDDR4
	{
		//Vmem 1.20V
		pmic_config_interface(0x554, 0x0, 0x3, 4);
		pmic_config_interface(0x554, 0x0, 0xf, 8);
	}
#else
	dram_HQA_adjust_voltage();
#endif

#ifdef DRAM_ETT
	TestMenu();
#endif

	EMI_Comsetting_Config(emi_set);

	EMI_Init(emi_set);

	Init_DRAM(emi_set);

#ifdef DRAM_ETT
    print_ETT_message();

	get_dram_rank_size(rank_size);

	if (global_which_test)
	{
		complex_mem_test_ett(DRAM_BASE,0x1000000);//fast test
		if(rank_size[1])
		{
			complex_mem_test_ett(DRAM_BASE + rank_size[0] - 0x1000000,0x2000000);//cross rank test
		}
		complex_mem_test_ett(DRAM_BASE, rank_size[0] + rank_size[1]);//full memory test
		global_which_test = 0x00;
	}
	mcSHOW_DBG_MSG(("\nETT done\n"));
#endif

#ifdef DRAM_CALIB_LOG
    klog_tail->dram_type = dram_type;
    klog_tail->check ^= klog_tail->dram_type;
#endif

}
#endif

uint32 mt_set_emis(uint8* emi, uint32 len, bool use_default) //array of emi setting.
{
    EMI_SETTINGS *v_emi_settings = (EMI_SETTINGS*)emi;
    unsigned int index;

    mcSHOW_DBG_MSG(("v_emi_settings->sub_version = 0x%x\n",v_emi_settings->sub_version));
    if(use_default)
    {
        if(v_emi_settings->sub_version == 0x1)
        {
            mcSHOW_DBG_MSG(("sub_version == 01, Using preloader tag\n"));
            memcpy(emi_settings,emi,len);
            num_of_emi_records = len/sizeof(EMI_SETTINGS);
            mcSHOW_DBG_MSG(("num_of_emi_records = %d\n",num_of_emi_records));
        }
        else
        {
            mcSHOW_DBG_MSG(("Use default EMI.\n"));
        }
        mt_set_emi();
    }
    else
    {
        mcSHOW_DBG_MSG(("EMI_TAG_Version == 28, Using preloader tag\n"));
        num_of_emi_records = len/sizeof(EMI_SETTINGS);
        if (num_of_emi_records > NUM_EMI_RECORD) {
            num_of_emi_records = NUM_EMI_RECORD;
            len = NUM_EMI_RECORD * sizeof(EMI_SETTINGS);
        }
        memcpy(emi_settings,emi,len);
        mcSHOW_DBG_MSG(("num_of_emi_records = %d\n",num_of_emi_records));
#ifdef COMBO_MCP
        index = mt_get_mdl_number();
        mcSHOW_DBG_MSG(("[EMI] MDL number = %d\r\n", index));
        if (index < 0 || index >=  num_of_emi_records)
        {
            mcSHOW_DBG_MSG(("[EMI] setting failed 0x%x\r\n", index));
            ASSERT(0);
        }
        else
    	{
			mt_set_emisettings(index);
    	}
#endif
        mt_set_emi();
    }
    mcSHOW_DBG_MSG(("EMI Setting OK.\n"));
    return 0;
}

#if defined(DRAM_HQA) || defined(DRAM_ETT)
static int calculate_vcore(unsigned int x)
{
    return (700+((625*x)/100));
}

static int calculate_vdram(unsigned int x)
{
	int vdram, cal = 0;
	pmic_read_interface(0x554, &vdram, 0x3, 4);
	switch(vdram)
	{
		case 1:
			vdram = 1350;
			break;
		case 2:
			vdram = 1500;
			break;
		case 3:
			vdram = 1800;
			break;
		case 0:
		default:
			vdram = 1200;
			break;
	}

	if (x < 8)
	{
		cal = -20 * x;
	}
	else
	{
		cal = 20 * (16-x);
	}
    return (vdram+cal);
}
static void set_vdram(unsigned int vdram)
{
	DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

	if (dram_type == TYPE_PCDDR3)
	{
		pmic_config_interface(0x554, 0x1, 0x3, 4);
	}
	else
	{
		pmic_config_interface(0x554, 0x0, 0x3, 4);
	}

    pmic_config_interface(0x554, vdram, 0xf, 8);
}

static void set_vcore(unsigned int vcore)
{
    pmic_config_interface(0x312, vcore, 0x7f, 0);
    pmic_config_interface(0x314, vcore, 0x7f, 0);
}

static unsigned int get_vdram(void)
{
    unsigned int vdram;
    pmic_read_interface(0x554, &vdram, 0xf, 8);
    return vdram;
}

static unsigned int get_vcore(void)
{
    unsigned int vcore;
    pmic_read_interface(0x312, &vcore, 0x7f, 0);
    return vcore;
}

static void get_voltage_info(VOLTAGE_LIST_T *voltage_list)
{
	DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

	voltage_list->vcore.HV = VCORE_HV;
	voltage_list->vcore.NV = VCORE_NV;
	voltage_list->vcore.LV = VCORE_LV;

	if(dram_type == TYPE_LPDDR3 || dram_type == TYPE_LPDDR2)
	{
		voltage_list->vdram.HV = VDRAM_HV_LPDDR3;
		voltage_list->vdram.NV = VDRAM_NV_LPDDR3;
		voltage_list->vdram.LV = VDRAM_LV_LPDDR3;
	}
	else if(dram_type == TYPE_PCDDR3)
	{
		voltage_list->vdram.HV = VDRAM_HV_PCDDR3;
		voltage_list->vdram.NV = VDRAM_NV_PCDDR3;
		voltage_list->vdram.LV = VDRAM_LV_PCDDR3;
	}
	else //dram_type == TYPE_PCDDR4
	{
		voltage_list->vdram.HV = VDRAM_HV_PCDDR4;
		voltage_list->vdram.NV = VDRAM_NV_PCDDR4;
		voltage_list->vdram.LV = VDRAM_LV_PCDDR4;
	}

    print("Vcore HV NV LV, Vdram HV NV LV\n");
    print("0x%x 0x%x 0x%x, 0x%x 0x%x 0x%x\n", voltage_list->vcore.HV, voltage_list->vcore.NV, voltage_list->vcore.LV,\
                                                voltage_list->vdram.HV, voltage_list->vdram.NV, voltage_list->vdram.LV);
}
#endif

#ifdef DRAM_HQA
static void print_HQA_voltage(void)
{
#if defined(HVCORE_HVDRAM)
    print("[HQA] Vcore HV, Vdram HV\n");
#elif defined(NVCORE_NVDRAM)
    print("[HQA] Vcore NV, Vdram NV\n");
#elif defined(LVCORE_LVDRAM)
    print("[HQA] Vcore LV, Vdram LV\n");
#elif defined(HVCORE_LVDRAM)
    print("[HQA] Vcore HV, Vdram LV\n");
#elif defined(LVCORE_HVDRAM)
    print("[HQA] Vcore LV, Vdram HV\n");
#elif defined(NVCORE_LVDRAM)
    print("[HQA] Vcore NV, Vdram LV\n");
#elif defined(NVCORE_HVDRAM)
    print("[HQA] Vcore NV, Vdram HV\n");
#else
    print("[HQA] Undefined HQA condition\n");
#endif

    print("[HQA] Vcore = %dmV\n", calculate_vcore(get_vcore()));
    print("[HQA] Vdram = %dmV\n", calculate_vdram(get_vdram()));
}

void dram_HQA_adjust_voltage(void)
{
	VOLTAGE_LIST_T voltage;

	get_voltage_info(&voltage);

#if defined(HVCORE_HVDRAM)
	set_vcore(voltage.vcore.HV);
	set_vdram(voltage.vdram.HV);
#elif defined(NVCORE_NVDRAM)
	set_vcore(voltage.vcore.NV);
	set_vdram(voltage.vdram.NV);
#elif defined(LVCORE_LVDRAM)
	set_vcore(voltage.vcore.LV);
	set_vdram(voltage.vdram.LV);
#elif defined(HVCORE_LVDRAM)
	set_vcore(voltage.vcore.HV);
	set_vdram(voltage.vdram.LV);
#elif defined(LVCORE_HVDRAM)
	set_vcore(voltage.vcore.LV);
	set_vdram(voltage.vdram.HV);
#elif defined(NVCORE_LVDRAM)
	set_vcore(voltage.vcore.NV);
	set_vdram(voltage.vdram.LV);
#elif defined(NVCORE_HVDRAM)
	set_vcore(voltage.vcore.NV);
	set_vdram(voltage.vdram.HV);
#else
	print("[HQA] Undefined HQA condition\n");
#endif
	print_HQA_voltage();
}
#endif //DRAM_HQA

#ifdef DRAM_ETT
static void toUpperString(char *str)
{
    char *temp = str;

    while (*temp) {
        if (*temp>='a' && *temp<='z') {
            *temp -= 'a'-'A';
        }
        temp++;
    }
}

static void pmic_voltage_read(void)
{
    print("Vcore(0x%x) = %d mV\n", get_vcore(), calculate_vcore(get_vcore()));
    print("Vdram(0x%x) = %d mV\n", get_vdram(), calculate_vdram(get_vdram()));
}

static unsigned int UART_Get_Command(void)
{
    unsigned char buff;
    int result_count;

    result_count = GetUARTBytes(&buff, 1, 1);

	if (buff == '=')
		return 0;

    while (!result_count) {
        if ((buff == 0x0d) || (buff == 0x0a)) {
            UserCommand[command_index] = '\0';
            command_index = 0;
            buff = '\n';
            PutUARTByte(buff);

            buff = '\r';
            PutUARTByte(buff);
			toUpperString(UserCommand);
            return 1;
        }

        /* check if the input char is backspace */
        if (buff == '\b') {
            /* check if any data in the command buffer */
            if (command_index) {
                /* put "backspace" */
                /* clear the char in the command buffer */
			      PutUARTByte('\b');
		          PutUARTByte(' ');
		   	      PutUARTByte('\b');

                UserCommand[--command_index] = 0;
            }
        } else {
            if (!result_count) {
                PutUARTByte(buff);
            }

            /* store the char in the command buffer */
            UserCommand[command_index++] = buff;
        }

		result_count = GetUARTBytes(&buff, 1, 1);
    }

    return 0;
}

//nAdjust -    0:Vcore++  1:Vcore--
static void pmic_Vcore_adjust(int nAdjust)
{
	unsigned int OldVcore = 0;

	switch(nAdjust)
	{
		case ETT_VCORE_INC:        // Vcore++
			pmic_read_interface(0x312, &OldVcore, 0x7f, 0);
			if (OldVcore < 0x7f)
				OldVcore += 1;
			pmic_config_interface(0x312, OldVcore, 0x7f, 0);
			pmic_read_interface(0x314, &OldVcore, 0x7f, 0);
			if (OldVcore < 0x7f)
				OldVcore += 1;
			pmic_config_interface(0x314, OldVcore, 0x7f, 0);
		break;
		case ETT_VCORE_DEC:        // Vcore--
			pmic_read_interface(0x312, &OldVcore, 0x7f, 0);
			if (OldVcore > 0)
				OldVcore -= 1;
			pmic_config_interface(0x312, OldVcore, 0x7f, 0);
			pmic_read_interface(0x314, &OldVcore, 0x7f, 0);
			if (OldVcore > 0)
				OldVcore -= 1;
			pmic_config_interface(0x314, OldVcore, 0x7f, 0);
		break;
		default :
		break;
	}

}

// nAdjust -    0:Vdram++  1:Vdram--
static void pmic_Vdram_adjust(int nAdjust)
{
    unsigned int OldVdram = 0;

    switch(nAdjust) {
		case ETT_VDRAM_INC:        // Vdram++
			pmic_read_interface(0x554, &OldVdram, 0xf, 8);
			if (OldVdram == 0)
				OldVdram = 0xf;
			else if (OldVdram != 0x8)
				OldVdram -= 1;
			pmic_config_interface(0x554, OldVdram, 0xf, 8);
			break;
		case ETT_VDRAM_DEC:        // Vdram--
			pmic_read_interface(0x554, &OldVdram, 0xf, 8);
			if (OldVdram == 0xf)
				OldVdram = 0x0;
			else if (OldVdram != 0x7)
				OldVdram += 1;
			pmic_config_interface(0x554, OldVdram, 0xf, 8);
		break;
		default :
		break;
	}
}


#define PATTERN1 0x5A5A5A5A
#define PATTERN2 0xA5A5A5A5
#define _errorExit(errcode) return(errcode)
#define dbg_print print
static unsigned int u4Process = 0x00;
static void mem_test_show_process(unsigned int index, unsigned int len)
{
#if 1
	 unsigned int u4NewProcess, u4OldProcess;

     u4NewProcess = (unsigned long long)index*100/(unsigned long long)len;
	// dbg_print("0x%x : 0x%x : 0x%x : %d\n", index,len, len-1, u4NewProcess);

	 if(index == 0x00) dbg_print("0%%");
     if(u4NewProcess != u4Process)
     {

	// dbg_print("1: 0x%x : 0x%x : 0x%x : %d : %d\n", index,len, len-1, u4NewProcess, u4Process);
		u4Process = u4NewProcess;
		if((u4Process%10) == 0x00)
		{
			dbg_print("\n%d%%", u4Process);
		}
		else
		{
			dbg_print(". ");
		}
     }

	 if((index != 0x00) && (index == (unsigned int)(len - 1)))
	 {
		u4Process = 0x00;
		 dbg_print("\n100%% \n");
	 }
#endif
}

static int complex_mem_test_ett(unsigned int start, unsigned int len)
{
    volatile unsigned char *MEM8_BASE = (volatile unsigned char *) start;
    volatile unsigned short *MEM16_BASE = (volatile unsigned short *) start;
    volatile unsigned int *MEM32_BASE = (volatile unsigned int *) start;
    volatile unsigned int *MEM_BASE = (volatile unsigned int *) start;
	volatile unsigned long long *MEM64_BASE = (volatile unsigned long long *)start;
    unsigned char pattern8;
    unsigned short pattern16;
    unsigned long long pattern64;
    unsigned int i, j, size, pattern32;
    unsigned int value, temp;

    size = len >> 2;

	*(volatile unsigned int *)0x10007000 = 0x22000000;
    dbg_print("memory test start address = 0x%x, test length = 0x%x\n", start, len);

    /* === Verify the tied bits (tied low) === */
    for (i = 0; i < size; i++)
    {
        MEM32_BASE[i] = 0;
    }

    for (i = 0; i < size; i++)
    {
		mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0)
        {
            dbg_print("Tied Low Test: Address %x not all zero, %x!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....32bits all zero test: Fail!\n\r");
            _errorExit(1);
        }
        else
        {
            MEM32_BASE[i] = 0xffffffff;
        }
    }
    if(i == size)	dbg_print("..32bits all zero test: Pass!\n\r");


    /* === Verify the tied bits (tied high) === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        temp = MEM32_BASE[i];
        if(temp != 0xffffffff)
        {
            dbg_print("Tied High Test: Address %x not equal 0xFFFFFFFF, %x!\n\r", &MEM32_BASE[i], temp);
            dbg_print("....32bits all one test: Fail!\n\r");
            _errorExit(2);
        }
        else
        {
            MEM32_BASE[i] = 0x00;
    }
    }
    if(i == size)	dbg_print("..32bits all one test: Pass!\n\r");

    /* === Verify pattern 1 (0x00~0xff) === */
    pattern8 = 0x00;
    for (i = 0; i < len; i++)
        MEM8_BASE[i] = pattern8++;

    pattern8 = 0x00;
    for (i = 0; i < len; i++)
    {
    	mem_test_show_process(i, len);
        if (MEM8_BASE[i] != pattern8++)
        {
            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM8_BASE[i], MEM8_BASE[i], --pattern8);
            dbg_print("....8bits 0x00~0xff pattern test: Fail!\n\r");
            _errorExit(3);
        }
    }
    if(i == len)
        dbg_print("..8bits 0x00~0xff pattern test: Pass!\n\r");


    /* === Verify pattern 3 (0x0000, 0x0001, 0x0002, ... 0xFFFF) === */
    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
        MEM16_BASE[i] = pattern16++;

    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
    {
    	mem_test_show_process(i, (len >> 1));
        temp = MEM16_BASE[i];
        if(temp != pattern16++)
        {

            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM16_BASE[i], temp, --pattern16);
            dbg_print("....16bits 0x00~0xffff pattern test: Fail!\n\r");
            _errorExit(4);
        }
    }
    if(i == (len >> 1))
        dbg_print("..16bits 0x00~0xffff pattern test: Pass!\n\r");

    /* === Verify pattern 4 (0x00000000, 0x00000001, 0x00000002, ... 0xFFFFFFFF) === */
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
        MEM32_BASE[i] = pattern32++;
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != pattern32++)
        {
            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i], --pattern32);
            dbg_print("....32bits 0x00~0xffffffff pattern test: Fail!\n\r");
            _errorExit(5);
        }
    }
    if(i == (len >> 2))
        dbg_print("..32bits 0x00~0xffffffff pattern test: Pass!\n\r");


    /* === Pattern 5: Filling memory range with 0xa5a5a5a5 === */
    for (i = 0; i < size; i++)
        MEM32_BASE[i] = 0xa5a5a5a5;

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 0h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5a5a5a5)
        {
            dbg_print("Address %x = %x, 0xa5a5a5a5 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5a5a5a5 pattern test: Fail!\n\r");
            _errorExit(6);
        }
        else
        {
            MEM8_BASE[i * 4] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5a5a5a5 pattern test: Pass!\n\r");

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 2h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5a5a500)
        {
            dbg_print("Address %x = %x, 0xa5a5a500 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5a5a500 pattern test: Fail!\n\r");
            _errorExit(7);
        }
        else
        {
            MEM8_BASE[i * 4 + 2] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5a5a500 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 1h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa500a500)
        {
            dbg_print("Address %x = %x, 0xa500a500 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa500a500 pattern test: Fail!\n\r");
            _errorExit(8);
        }
        else
        {
            MEM8_BASE[i * 4 + 1] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa500a500 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 3h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5000000)
        {
            dbg_print("Address %x = %x, 0xa5000000 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5000000 pattern test: Fail!\n\r");
            _errorExit(9);
        }
        else
        {
            MEM8_BASE[i * 4 + 3] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5000000 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with ffff Word Pattern at offset 1h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0x00000000)
        {
            dbg_print("Address %x = %x, 0x00000000 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0x00000000 pattern test: Fail!\n\r");
            _errorExit(10);
        }
    }
    if(i == size)
        dbg_print("..0x00000000 pattern test: Pass!\n\r");

    /************************************************
    * Additional verification
    ************************************************/
    /* === stage 1 => write 0 === */
    for (i = 0; i < size; i++)
    {
        MEM_BASE[i] = PATTERN1;
    }

    /* === stage 2 => read 0, write 0xF === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        value = MEM_BASE[i];
        if (value != PATTERN1)
        {
            dbg_print("\nStage 2 error. Addr = %x, value = %x\n", &(MEM_BASE[i]), value);
            _errorExit(11);
        }
        MEM_BASE[i] = PATTERN2;
    }

    /* === stage 3 => read F === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        value = MEM_BASE[i];
        if (value != PATTERN2)
        {
            dbg_print("\nStage 3 error. Addr = %x, value = %x\n", &(MEM_BASE[i]), value);
            _errorExit(12);
        }
    }

    dbg_print("..%x and %x Interleaving test: Pass!\n\r", PATTERN1, PATTERN2);

    return 0;
}

static void TestMenu(void)
{
	int needDelay=1, i;
	VOLTAGE_LIST_T voltage;

	get_voltage_info(&voltage);

    print("\n");
    print("G : Start the ETT test. %s\n",(needDelay)?"Delay 40s":"No Delay 40s");
    print("D : %s Delay 40s before start ETT...\n",(needDelay)?"Disable":"Enable");
    print("P : Print voltage settings\n");
    print("V : Voltage adjustment. \n");
    print("Please enter selection:");

    mtk_wdt_disable();

    while (1)
    {
        if ( UART_Get_Command() )
        {
            toUpperString(UserCommand);
            if ( !(strcmp((const char *)UserCommand, "D")) )
            {
    			if (needDelay == 1)
    			{
    				print("Disable Delay 40s before start ETT...\n");
    				needDelay = 0;
    			}
    			else
    			{
    				print("Enable Delay 40s before start ETT...\n");
    				needDelay = 1;
    			}
    		}
    		else if ( !(strcmp((const char *)UserCommand, "P")) )
    		{
    			pmic_voltage_read();
    		}
    		else if ( !(strcmp((const char *)UserCommand, "T")) )
    		{
    			global_which_test = 1;
    			//break;
    		}
    		else if ( !(strcmp((const char *)UserCommand, "G")) )
    		{
    			print("Start the test...\n");
    			if (needDelay == 1)
    			{
    				for (i = 0 ; i < 40; i++)
    				{
    					print(".");
    					mdelay(500);
    				}
    			}
    			break;
    		}
    		else if ( !(strcmp((const char *)UserCommand, "V")) )
    		{
    			print("\n");
    			print("Vcore (HV, NV, LV)=(%dmV, %dmV, %dmV)\n", calculate_vcore(voltage.vcore.HV), calculate_vcore(voltage.vcore.NV), calculate_vcore(voltage.vcore.LV));
				print("Vdram (HV, NV, LV)=(%dmV, %dmV, %dmV)\n", calculate_vdram(voltage.vdram.HV), calculate_vdram(voltage.vdram.NV), calculate_vdram(voltage.vdram.LV));

				print("======================================\n");
				print("%d (Vcore HV: %dmV	Vdram HV: %dmV)\n", ETT_HVCORE_HVDRAM, calculate_vcore(voltage.vcore.HV), calculate_vdram(voltage.vdram.HV));
				print("%d (Vcore NV: %dmV	Vdram NV: %dmV)\n", ETT_NVCORE_NVDRAM, calculate_vcore(voltage.vcore.NV), calculate_vdram(voltage.vdram.NV));
				print("%d (Vcore LV: %dmV	Vdram LV: %dmV)\n", ETT_LVCORE_LVDRAM, calculate_vcore(voltage.vcore.LV), calculate_vdram(voltage.vdram.LV));
				print("%d (Vcore HV; %dmV	Vdram LV: %dmV)\n", ETT_HVCORE_LVDRAM, calculate_vcore(voltage.vcore.HV), calculate_vdram(voltage.vdram.LV));
				print("%d (Vcore LV: %dmV	Vdram HV: %dmV)\n", ETT_LVCORE_HVDRAM, calculate_vcore(voltage.vcore.LV), calculate_vdram(voltage.vdram.HV));
				print("%d (Vcore NV: %dmV	Vdram LV: %dmV)\n", ETT_NVCORE_LVDRAM, calculate_vcore(voltage.vcore.NV), calculate_vdram(voltage.vdram.LV));
				print("%d (Vcore NV: %dmV	Vdram HV: %dmV)\n", ETT_NVCORE_HVDRAM, calculate_vcore(voltage.vcore.NV), calculate_vdram(voltage.vdram.HV));

    			print("======================================\n");
    			print("%d : (Vcore )  ++ ...\n", ETT_VCORE_INC);
    			print("%d : (Vcore )  -- ...\n", ETT_VCORE_DEC);
    			print("%d : (Vdram )  ++ ...\n", ETT_VDRAM_INC);
    			print("%d : (Vdram )  -- ...\n", ETT_VDRAM_DEC);

    			print("Please enter pattern selection:(%d-%d)", ETT_HVCORE_HVDRAM, ETT_VCORE_VDRAM_MAX-1);

    			while (1)
    			{
    				if ( UART_Get_Command() )
    				{
    					int  nSel = atoi(UserCommand);
    					if ( (nSel<ETT_HVCORE_HVDRAM) || (nSel>=ETT_VCORE_VDRAM_MAX))
    					{
    						nSel = ETT_NVCORE_NVDRAM;
    					}

                        if (nSel <= ETT_NVCORE_HVDRAM)
                            voltage_sel = nSel;

    					switch(nSel)
    					{
    						case ETT_HVCORE_HVDRAM:
								set_vcore(voltage.vcore.HV);
								set_vdram(voltage.vdram.HV);
							break;
    						case ETT_LVCORE_LVDRAM:
								set_vcore(voltage.vcore.LV);
								set_vdram(voltage.vdram.LV);
							break;
    						case ETT_HVCORE_LVDRAM:
								set_vcore(voltage.vcore.HV);
								set_vdram(voltage.vdram.LV);
							break;
    						case ETT_LVCORE_HVDRAM:
								set_vcore(voltage.vcore.LV);
								set_vdram(voltage.vdram.HV);
							break;
    						case ETT_NVCORE_LVDRAM:
								set_vcore(voltage.vcore.NV);
								set_vdram(voltage.vdram.LV);
							break;
    						case ETT_NVCORE_HVDRAM:
								set_vcore(voltage.vcore.NV);
								set_vdram(voltage.vdram.HV);
							break;
    						case ETT_VCORE_INC:
								pmic_Vcore_adjust(ETT_VCORE_INC);
							break;
    						case ETT_VCORE_DEC:
								pmic_Vcore_adjust(ETT_VCORE_DEC);
							break;
    						case ETT_VDRAM_INC:
								pmic_Vdram_adjust(ETT_VDRAM_INC);
							break;
    						case ETT_VDRAM_DEC:
								pmic_Vdram_adjust(ETT_VDRAM_DEC);
							break;
    						case ETT_NVCORE_NVDRAM:
							default:
								set_vcore(voltage.vcore.NV);
								set_vdram(voltage.vdram.NV);
							break;
    					}
    					pmic_voltage_read();
    					break;
    				}
    			}
    		}
    		print("\n");
    		print("G : Start the ETT test. %s\n",(needDelay)?"Delay 40s":"No Delay 40s");
    		print("D : %s Delay 40s before start ETT...\n",(needDelay)?"Disable":"Enable");
    		print("P : Print voltage settings\n");
    		print("V : Voltage adjustment. \n");
    		print("Please enter selection:");
    	}
    }
}

static void get_criteria_info(CRITERIA_LIST_T *criteria_list)
{
    DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

    criteria_list->HTLV.CA = 0;
    criteria_list->HTLV.RX = 0;
    criteria_list->HTLV.TX = 0;

    criteria_list->NTNV.CA = 0;
    criteria_list->NTNV.RX = 0;
    criteria_list->NTNV.TX = 0;

    criteria_list->LTHV.CA = 0;
    criteria_list->LTHV.RX = 0;
    criteria_list->LTHV.TX = 0;

    criteria_list->BUFF.CA = 0;
    criteria_list->BUFF.RX = 0;
    criteria_list->BUFF.TX = 0;

}

static void print_ETT_message(void)
{
    U8 u1BitIdx = 0, u1RankIdx = 0, support_rank_num;
    U8 u1MinWinBit, u1MinMarginBit, u1MinWin, u1MinMargin;
    U32 ca_warn = 0, rx_warn = 0, tx_warn = 0;
    U32 ca_fail = 0, rx_fail = 0, tx_fail = 0;
    DRAM_DATA_WIDTH_T data_width = DATA_WIDTH_16BIT;
    SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T *pWin;
    CRITERIA_SIGNAL_T *criteria_signal_list;
    CRITERIA_SIGNAL_T *buffer_signal_list;
    CRITERIA_LIST_T criteria;
    ETT_RESULT_T ett_ret_idx = ETT_FAIL;
    char *ett_ret[ETT_RET_MAX] = {"FAIL", "WARN", "PASS"};
    DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

    mcSHOW_DBG_MSG(("\n\n=================[ETT] information Begin=================\n"));

    get_criteria_info(&criteria);

    mcSHOW_DBG_MSG(("PASS CRITERIA: CA RX TX, BUFFER: CA RX TX\n"));

    if (voltage_sel == ETT_HVCORE_HVDRAM)
    {
        criteria_signal_list = &criteria.LTHV;
        mcSHOW_DBG_MSG(("HV: "));

    }
    else if (voltage_sel == ETT_LVCORE_LVDRAM)
    {
        criteria_signal_list = &criteria.HTLV;
        mcSHOW_DBG_MSG(("LV: "));
    }
    else
    {
        criteria_signal_list = &criteria.NTNV;
        mcSHOW_DBG_MSG(("NV: "));
    }
    buffer_signal_list = &criteria.BUFF;
    mcSHOW_DBG_MSG(("%d %d %d, ", criteria_signal_list->CA, criteria_signal_list->RX, criteria_signal_list->TX));
    mcSHOW_DBG_MSG(("%d %d %d\n", buffer_signal_list->CA, buffer_signal_list->RX, buffer_signal_list->TX));

    if (u4IO32ReadFldAlign(DRAMC_REG_DDRCONF0, DDRCONF0_DM64BITEN))
        data_width = DATA_WIDTH_32BIT;

    u4DRAMdebugLOgEnable2 = 0;

    mcSHOW_DBG_MSG2(("\n=================Impedance Begin=================\n"));
    mcSHOW_DBG_MSG2(("DRVP = %d, DRVN = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].SwImpedanceCal.DRVP, gDRAM_CALIB_LOG.RANK[u1RankIdx].SwImpedanceCal.DRVN));
    mcSHOW_DBG_MSG2(("=================Impedance Done=================\n"));

    if (dram_type == TYPE_LPDDR3)
    {
        mcSHOW_DBG_MSG(("\n=================CA Training Begin=================\n"));
        mcSHOW_DBG_MSG2(("CA_delay = %d, Clk_delay = %d, CS_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CA_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.Clk_delay, \
                                                                            gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CS_delay));

        if (criteria_signal_list->CA > buffer_signal_list->CA)
        {
            buffer_signal_list->CA = criteria_signal_list->CA - buffer_signal_list->CA;
        }
        else
        {
            buffer_signal_list->CA = 0;
        }

        mcSHOW_DBG_MSG(("CAx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<10; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->CA)
            {
                ett_ret_idx = ETT_FAIL;
                ca_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->CA)
            {
                ett_ret_idx = ETT_WARN;
                ca_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }
            mcSHOW_DBG_MSG(("CA%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                    pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CA_delay, pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("CA%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("CA%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================CA Training Done=================\n"));
    }

    if (dram_type != TYPE_LPDDR2)
    {
    mcSHOW_DBG_MSG2(("\n=================Write Leveling Begin=================\n"));
    mcSHOW_DBG_MSG2(("CA_Clk_delay = %d, Clk_delay = %d, Final_Clk_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.CA_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.Clk_delay, \
                                                                                gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.Final_Clk_delay));
    mcSHOW_DBG_MSG2(("DQS0_delay = %d, DQS1_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS0_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS1_delay));

    if (data_width == DATA_WIDTH_32BIT)
        mcSHOW_DBG_MSG2(("DQS2_delay = %d, DQS3_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS2_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS3_delay));

    mcSHOW_DBG_MSG2(("=================Write Leveling Done=================\n"));
    }

    support_rank_num = (U8)get_dram_rank_nr();

    for (u1RankIdx=RANK_0; u1RankIdx<support_rank_num; u1RankIdx++)
    {
        mcSHOW_DBG_MSG(("\n=================[Rank%d] Begin=================\n", u1RankIdx));

        mcSHOW_DBG_MSG2(("=================Gating Begin=================\n"));
        mcSHOW_DBG_MSG2(("DQS0 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_05T, \
                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[0]));
        mcSHOW_DBG_MSG2(("DQS1 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_05T, \
                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[1]));
        if (data_width == DATA_WIDTH_32BIT)
        {
            mcSHOW_DBG_MSG2(("DQS2 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_05T, \
                                                        gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[2]));
            mcSHOW_DBG_MSG2(("DQS3 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_05T, \
                                                        gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[3]));
        }

        mcSHOW_DBG_MSG2(("s1ChangeDQSINCTL = %d, reg_TX_dly_DQSgated_min = %d, u1TXDLY_Cal_min = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.s1ChangeDQSINCTL, \
                                                                                                  gDRAM_CALIB_LOG.RxdqsGatingPostProcess.reg_TX_dly_DQSgated_min, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u1TXDLY_Cal_min));
        mcSHOW_DBG_MSG2(("TX_dly_DQSgated_check_min = %d, TX_dly_DQSgated_check_max = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_min, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_max));
        mcSHOW_DBG_MSG2(("DQSINCTL = %d, RANKINCTL = %d, u4XRTR2R = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.DQSINCTL, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.RANKINCTL, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u4XRTR2R));

        mcSHOW_DBG_MSG2(("=================Gating Done=================\n"));

        mcSHOW_DBG_MSG2(("\n=================Datlat Begin=================\n"));
        mcSHOW_DBG_MSG2(("best_step = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].DATLAT.best_step));

        if (u1RankIdx == RANK_1)
        {
            mcSHOW_DBG_MSG2(("Rank0_Datlat = %d, Rank1_Datlat = %d, Final_Datlat = %d\n", gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank0_Datlat, gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank1_Datlat, \
                                                                                            gDRAM_CALIB_LOG.DualRankRxdatlatCal.Final_Datlat));
        }
        mcSHOW_DBG_MSG2(("=================Datlat Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================RX Begin=================\n"));
        mcSHOW_DBG_MSG2(("DQS0_delay = %d, DQS1_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS0_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS1_delay));

        if (data_width == DATA_WIDTH_32BIT)
            mcSHOW_DBG_MSG2(("DQS2_delay = %d, DQS3_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS2_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS3_delay));

        if (criteria_signal_list->RX > buffer_signal_list->RX)
        {
            buffer_signal_list->RX = criteria_signal_list->RX - buffer_signal_list->RX;
        }
        else
        {
            buffer_signal_list->RX = 0;
        }

        mcSHOW_DBG_MSG(("RX DQx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<data_width; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->RX)
            {
                ett_ret_idx = ETT_FAIL;
                rx_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->RX)
            {
                ett_ret_idx = ETT_WARN;
                rx_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }

            mcSHOW_DBG_MSG(("RX DQ%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                    pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQ_delay[u1BitIdx], pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("RX DQ%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("RX DQ%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================RX Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================TX Begin=================\n"));

        for (u1BitIdx=0; u1BitIdx<data_width/DQS_BIT_NUMBER; u1BitIdx++)
            mcSHOW_DBG_MSG2(("Byte%d Delay(LargeUI, SmallUI, PI) = (%d, %d, %d)\n", u1BitIdx, gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.Large_UI[u1BitIdx], \
                                                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.Small_UI[u1BitIdx], gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.PI[u1BitIdx]));

        if (criteria_signal_list->TX > buffer_signal_list->TX)
        {
            buffer_signal_list->TX = criteria_signal_list->TX - buffer_signal_list->TX;
        }
        else
        {
            buffer_signal_list->TX = 0;
        }

        mcSHOW_DBG_MSG(("TX DQx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<data_width; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->TX)
            {
                ett_ret_idx = ETT_FAIL;
                tx_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->TX)
            {
                ett_ret_idx = ETT_WARN;
                tx_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }

            mcSHOW_DBG_MSG(("TX DQ%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.PI_DQ_delay[u1BitIdx/DQS_BIT_NUMBER], pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("TX DQ%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("TX DQ%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================TX Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================[Rank%d] Done=================\n", u1RankIdx));
    }

    mcSHOW_DBG_MSG(("ca_warn = 0x%x, rx_warn = 0x%x, tx_warn = 0x%x\n", ca_warn, rx_warn, tx_warn));
    mcSHOW_DBG_MSG(("ca_fail = 0x%x, rx_fail = 0x%x, tx_fail = 0x%x\n", ca_fail, rx_fail, tx_fail));

    if (ca_fail || rx_fail || tx_fail)
    {
        mcSHOW_DBG_MSG(("\nFINAL ETT RESULT FAIL\n"));
    }

    else
    {
        mcSHOW_DBG_MSG(("\nFINAL ETT RESULT PASS\n"));
    }

    u4DRAMdebugLOgEnable2 = 1;
    mcSHOW_DBG_MSG(("\n=================[ETT] information Done=================\n"));
}
#endif //DRAM_ETT

#ifdef DRAM_CALIB_LOG
static U16 read_rtc(U16 unit)
{
    U32 time=0;
    pwrap_read((U32)unit, &time);
    return (U16)time;
}

void dram_klog_clean(void)
{
    print("[DRAM Klog] clean klog space in SRAM\n");
    memset((void*)CALIB_LOG_BASE, 0, CALIB_LOG_SIZE);
}

void dram_klog_init(void)
{
    U16 sec, min, hou, dom, mth, yea;

    klog_head = (DRAM_KLOG_HEAD*) CALIB_LOG_BASE;
    klog_data = (unsigned int*)(CALIB_LOG_BASE + sizeof(DRAM_KLOG_HEAD));
    klog_tail = (DRAM_KLOG_TAIL*)(CALIB_LOG_BASE + CALIB_LOG_SIZE - sizeof(DRAM_KLOG_TAIL));

    print("[DRAM Klog] head: 0x%x, data: 0x%x, tail: 0x%x\n", klog_head, klog_data, klog_tail);

    sec = read_rtc(RTC_TC_SEC);
    min = read_rtc(RTC_TC_MIN);
    hou = read_rtc(RTC_TC_HOU);
    dom = read_rtc(RTC_TC_DOM);
    mth = read_rtc(RTC_TC_MTH);
    yea = read_rtc(RTC_TC_YEA) + RTC_MIN_YEAR;

    klog_head->guard = CALIB_LOG_MAGIC;
    klog_head->rtc_yea_mth = (yea << 16) | mth;
    klog_head->rtc_dom_hou = (dom << 16) | hou;
    klog_head->rtc_min_sec = (min << 16) | sec;
    klog_tail->dram_type = 0;
    klog_tail->guard = CALIB_LOG_MAGIC;
    klog_tail->check = (klog_head->rtc_yea_mth) ^ (klog_head->rtc_dom_hou) ^ (klog_head->rtc_min_sec) ^
                       (klog_head->guard) ^ (klog_tail->guard);

    print("[DRAM Klog] init SRAM space for Klog at %d/%d/%d %d:%d:%d\n", yea, mth, dom, hou, min, sec);

    return;
}

int dram_klog_check(void)
{
    unsigned int i;
    unsigned int check = 0;

    for(i=0; i<CALIB_LOG_SIZE/4; i++)
        check ^= *((unsigned int*)CALIB_LOG_BASE+i);

    if(check != 0) {
        print("[DRAM Klog] wrong checksum\n");
        return -1;
    }

    print("[DRAM Klog] correct checksum\n");
    return 0;
}

int i4WriteSramLog(unsigned int u4Offset, unsigned int *pu4Src, unsigned int u4WordCnt)
{
    unsigned int i;

    if((u4Offset+u4WordCnt) > ((CALIB_LOG_SIZE-sizeof(DRAM_KLOG_HEAD)-sizeof(DRAM_KLOG_TAIL))/4)) {
        print("[DRAM Klog] fail to write Klog due to crossing the boundary\n");
        return -1;
    }
    else if(((unsigned int)0xffffffff-u4Offset) < u4WordCnt) {
        print("[DRAM Klog] fail to write Klog due to overflow\n");
        return -1;
    }
    else {
        for(i=0; i<u4WordCnt; i++) {
            klog_tail->check ^= *(klog_data+u4Offset+i) ^ *(pu4Src+i);
            *(klog_data+u4Offset+i) = *(pu4Src+i);
        }
    }

    if(klog_head->data_count < (u4Offset+u4WordCnt)) {
        klog_tail->check ^= klog_head->data_count ^ (u4Offset+u4WordCnt);
        klog_head->data_count = u4Offset+u4WordCnt;
    }

    return 0;
}

static void dram_klog_print(void)
{
    unsigned int *curr;

    for(curr=(unsigned int*)CALIB_LOG_BASE; curr<(unsigned int*)(CALIB_LOG_BASE+CALIB_LOG_SIZE); curr++) {
        print("[DRAM Klog] 0x%x: 0x%x\n", curr, *curr);
    }

    return;
}
#endif //DRAM_CALIB_LOG


