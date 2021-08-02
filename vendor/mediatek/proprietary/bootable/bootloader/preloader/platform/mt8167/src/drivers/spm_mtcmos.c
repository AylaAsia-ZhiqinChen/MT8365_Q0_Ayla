#include <platform.h>
#include <spm.h>
#include <spm_mtcmos.h>
#include <spm_mtcmos_internal.h>
#include <timer.h>  //udelay
#include <pll.h>

/**************************************
 * for CPU MTCMOS
 **************************************/

#define spm_mtcmos_cpu_lock(x)
#define spm_mtcmos_cpu_unlock(x)

#define spm_mtcmos_noncpu_lock(x)   (*(&x) = 0)
#define spm_mtcmos_noncpu_unlock(x) (*(&x) = 0)

typedef int (*spm_cpu_mtcmos_ctrl_func)(int state, int chkWfiBeforePdn);
static spm_cpu_mtcmos_ctrl_func spm_cpu_mtcmos_ctrl_funcs[] =
{
    spm_mtcmos_ctrl_cpu0,
    spm_mtcmos_ctrl_cpu1,
    spm_mtcmos_ctrl_cpu2,
    spm_mtcmos_ctrl_cpu3,
};
int spm_mtcmos_ctrl_cpu(unsigned int cpu, int state, int chkWfiBeforePdn)
{
    return (*spm_cpu_mtcmos_ctrl_funcs[cpu])(state, chkWfiBeforePdn);
}

int spm_mtcmos_ctrl_cpu0(int state, int chkWfiBeforePdn)
{
    unsigned long flags;

    /* enable register control */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

    if (state == STA_POWER_DOWN)
    {
        if (chkWfiBeforePdn)
            while ((spm_read(SPM_SLEEP_TIMER_STA) & CA7_CPU0_STANDBYWFI) == 0);

        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | PWR_ISO);

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | SRAM_CKISO);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU0_L1_PDN, spm_read(SPM_CA7_CPU0_L1_PDN) | L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU0_L1_PDN) & L1_PDN_ACK) != L1_PDN_ACK);
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~PWR_RST_B);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | PWR_CLK_DIS);

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~PWR_ON);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU0) != 0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU0) != 0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_mtcmos_cpu_unlock(&flags);
    }
    else /* STA_POWER_ON */
    {
        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | PWR_ON);
        udelay(1);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU0) != CA7_CPU0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU0) != CA7_CPU0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~PWR_ISO);

        spm_write(SPM_CA7_CPU0_L1_PDN, spm_read(SPM_CA7_CPU0_L1_PDN) & ~L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU0_L1_PDN) & L1_PDN_ACK) != 0);
    #endif //#ifndef CFG_FPGA_PLATFORM
        udelay(1);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~SRAM_CKISO);

        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CA7_CPU0_PWR_CON, spm_read(SPM_CA7_CPU0_PWR_CON) | PWR_RST_B);

        spm_mtcmos_cpu_unlock(&flags);
    }

    return 0;
}

int spm_mtcmos_ctrl_cpu1(int state, int chkWfiBeforePdn)
{
    unsigned long flags;

    /* enable register control */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

    if (state == STA_POWER_DOWN)
    {
        if (chkWfiBeforePdn)
            while ((spm_read(SPM_SLEEP_TIMER_STA) & CA7_CPU1_STANDBYWFI) == 0);

        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | PWR_ISO);

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | SRAM_CKISO);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU1_L1_PDN, spm_read(SPM_CA7_CPU1_L1_PDN) | L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU1_L1_PDN) & L1_PDN_ACK) != L1_PDN_ACK);
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~PWR_RST_B);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | PWR_CLK_DIS);

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~PWR_ON);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU1) != 0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU1) != 0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_mtcmos_cpu_unlock(&flags);
    }
    else /* STA_POWER_ON */
    {
        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | PWR_ON);
        udelay(1);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU1) != CA7_CPU1) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU1) != CA7_CPU1));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~PWR_ISO);

        spm_write(SPM_CA7_CPU1_L1_PDN, spm_read(SPM_CA7_CPU1_L1_PDN) & ~L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU1_L1_PDN) & L1_PDN_ACK) != 0);
    #endif //#ifndef CFG_FPGA_PLATFORM
        udelay(1);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~SRAM_CKISO);

        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CA7_CPU1_PWR_CON, spm_read(SPM_CA7_CPU1_PWR_CON) | PWR_RST_B);

        spm_mtcmos_cpu_unlock(&flags);
    }

    return 0;
}

int spm_mtcmos_ctrl_cpu2(int state, int chkWfiBeforePdn)
{
    unsigned long flags;

    /* enable register control */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

    if (state == STA_POWER_DOWN)
    {
        if (chkWfiBeforePdn)
            while ((spm_read(SPM_SLEEP_TIMER_STA) & CA7_CPU2_STANDBYWFI) == 0);

        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | PWR_ISO);

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | SRAM_CKISO);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU2_L1_PDN, spm_read(SPM_CA7_CPU2_L1_PDN) | L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU2_L1_PDN) & L1_PDN_ACK) != L1_PDN_ACK);
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~PWR_RST_B);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | PWR_CLK_DIS);

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~PWR_ON);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU2) != 0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU2) != 0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_mtcmos_cpu_unlock(&flags);
    }
    else /* STA_POWER_ON */
    {
        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | PWR_ON);
        udelay(1);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU2) != CA7_CPU2) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU2) != CA7_CPU2));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~PWR_ISO);

        spm_write(SPM_CA7_CPU2_L1_PDN, spm_read(SPM_CA7_CPU2_L1_PDN) & ~L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU2_L1_PDN) & L1_PDN_ACK) != 0);
    #endif //#ifndef CFG_FPGA_PLATFORM
        udelay(1);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~SRAM_CKISO);

        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CA7_CPU2_PWR_CON, spm_read(SPM_CA7_CPU2_PWR_CON) | PWR_RST_B);

        spm_mtcmos_cpu_unlock(&flags);
    }

    return 0;
}

int spm_mtcmos_ctrl_cpu3(int state, int chkWfiBeforePdn)
{
    unsigned long flags;

    /* enable register control */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

    if (state == STA_POWER_DOWN)
    {
        if (chkWfiBeforePdn)
            while ((spm_read(SPM_SLEEP_TIMER_STA) & CA7_CPU3_STANDBYWFI) == 0);

        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | PWR_ISO);

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | SRAM_CKISO);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU3_L1_PDN, spm_read(SPM_CA7_CPU3_L1_PDN) | L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU3_L1_PDN) & L1_PDN_ACK) != L1_PDN_ACK);
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~PWR_RST_B);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | PWR_CLK_DIS);

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~PWR_ON);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU3) != 0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU3) != 0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_mtcmos_cpu_unlock(&flags);
    }
    else /* STA_POWER_ON */
    {
        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | PWR_ON);
        udelay(1);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPU3) != CA7_CPU3) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPU3) != CA7_CPU3));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~PWR_ISO);

        spm_write(SPM_CA7_CPU3_L1_PDN, spm_read(SPM_CA7_CPU3_L1_PDN) & ~L1_PDN);
    #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPU3_L1_PDN) & L1_PDN_ACK) != 0);
    #endif //#ifndef CFG_FPGA_PLATFORM
        udelay(1);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~SRAM_CKISO);

        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CA7_CPU3_PWR_CON, spm_read(SPM_CA7_CPU3_PWR_CON) | PWR_RST_B);

        spm_mtcmos_cpu_unlock(&flags);
    }

    return 0;
}

int spm_mtcmos_ctrl_cpusys0(int state, int chkWfiBeforePdn)
{
    unsigned long flags;

    /* enable register control */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

    if (state == STA_POWER_DOWN)
    {
        //TODO: add per cpu power status check?

        if (chkWfiBeforePdn)
            while ((spm_read(SPM_SLEEP_TIMER_STA) & CA7_CPUTOP_STANDBYWFI) == 0);

        //XXX: no dbg0 mtcmos on k2
        //spm_mtcmos_ctrl_dbg0(state);

        //XXX: no async adb on k2
        //spm_topaxi_prot(CA7_PDN_REQ, 1);

        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | PWR_ISO);

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | SRAM_CKISO);
    #if 1
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPUTOP_L2_PDN, spm_read(SPM_CA7_CPUTOP_L2_PDN) | L2_SRAM_PDN);
      #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPUTOP_L2_PDN) & L2_SRAM_PDN_ACK) != L2_SRAM_PDN_ACK);
      #endif //#ifndef CFG_FPGA_PLATFORM
        //XXX: only in kernel
        //ndelay(1500);
        udelay(2);
    #else
        //XXX: only in kernel
        //ndelay(100);
        udelay(1);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~SRAM_ISOINT_B);
        spm_write(SPM_CA7_CPUTOP_L2_SLEEP, spm_read(SPM_CA7_CPUTOP_L2_SLEEP) & ~L2_SRAM_SLEEP_B);
      #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPUTOP_L2_SLEEP) & L2_SRAM_SLEEP_B_ACK) != 0);
      #endif //#ifndef CFG_FPGA_PLATFORM
    #endif

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~PWR_RST_B);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | PWR_CLK_DIS);

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~PWR_ON);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPUTOP) != 0) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPUTOP) != 0));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_mtcmos_cpu_unlock(&flags);
    }
    else /* STA_POWER_ON */
    {
        spm_mtcmos_cpu_lock(&flags);

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | PWR_ON);
        udelay(1);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | PWR_ON_2ND);
    #ifndef CFG_FPGA_PLATFORM
        while (((spm_read(SPM_PWR_STATUS) & CA7_CPUTOP) != CA7_CPUTOP) || ((spm_read(SPM_PWR_STATUS_2ND) & CA7_CPUTOP) != CA7_CPUTOP));
    #endif //#ifndef CFG_FPGA_PLATFORM

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~PWR_ISO);

    #if 1
        spm_write(SPM_CA7_CPUTOP_L2_PDN, spm_read(SPM_CA7_CPUTOP_L2_PDN) & ~L2_SRAM_PDN);
      #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPUTOP_L2_PDN) & L2_SRAM_PDN_ACK) != 0);
      #endif //#ifndef CFG_FPGA_PLATFORM
    #else
        spm_write(SPM_CA7_CPUTOP_L2_SLEEP, spm_read(SPM_CA7_CPUTOP_L2_SLEEP) | L2_SRAM_SLEEP_B);
      #ifndef CFG_FPGA_PLATFORM
        while ((spm_read(SPM_CA7_CPUTOP_L2_SLEEP) & L2_SRAM_SLEEP_B_ACK) != L2_SRAM_SLEEP_B_ACK);
      #endif //#ifndef CFG_FPGA_PLATFORM
    #endif
        //XXX: only in kernel
        //ndelay(900);
        udelay(1);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | SRAM_ISOINT_B);
        //XXX: only in kernel
        //ndelay(100);
        udelay(1);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~SRAM_CKISO);

        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CA7_CPUTOP_PWR_CON, spm_read(SPM_CA7_CPUTOP_PWR_CON) | PWR_RST_B);

        spm_mtcmos_cpu_unlock(&flags);

        //XXX: no async adb on k2
        //spm_topaxi_prot(CA7_PDN_REQ, 0);

        //XXX: no dbg0 mtcmos on k2
        //spm_mtcmos_ctrl_dbg0(state);
    }

    return 0;
}

bool spm_cpusys0_can_power_down(void)
{
    return !(spm_read(SPM_PWR_STATUS) & (CA15_CPU0 | CA15_CPU1 | CA15_CPU2 | CA15_CPU3 | CA15_CPUTOP | CA7_CPU1 | CA7_CPU2 | CA7_CPU3)) &&
           !(spm_read(SPM_PWR_STATUS_2ND) & (CA15_CPU0 | CA15_CPU1 | CA15_CPU2 | CA15_CPU3 | CA15_CPUTOP | CA7_CPU1 | CA7_CPU2 | CA7_CPU3));
}


/**************************************
 * for non-CPU MTCMOS
 **************************************/
#define MFG_ASYNC_PWR_STA_MASK (0x1 << 25)
#define MFG_2D_PWR_STA_MASK    (0x1 << 24)
#define VDE_PWR_STA_MASK       (0x1 << 7)
#define ISP_PWR_STA_MASK       (0x1 << 5)
#define MFG_PWR_STA_MASK       (0x1 << 4)
#define DIS_PWR_STA_MASK       (0x1 << 3)
#define CONN_PWR_STA_MASK      (0x1 << 1)

#define SRAM_PDN               (0xf << 8)
#define MFG_SRAM_PDN           (0xf << 8)
#define MFG_2D_SRAM_PDN        (0xf << 8)
#define MFG_ASYNC_SRAM_PDN     0
#define VDE_SRAM_PDN           (0x1 << 8)
#define CONN_SRAM_PDN          (0x1 << 8)

#define VDE_SRAM_ACK           (0x1 << 12)
#define ISP_SRAM_ACK           (0xf << 12)
#define DIS_SRAM_ACK           (0xf << 12)
#define MFG_SRAM_ACK           (0xf << 12)
#define MFG_2D_SRAM_ACK        (0xf << 12)
#define MFG_ASYNC_SRAM_ACK     0
#define CONN_SRAM_ACK          0

#define DISP_PROT_MASK         0x0802 // bit 1, 11
#define MFG_PROT_MASK          0x0020 // bit 5
#define CONN_PROT_MASK         0x0310 // bit 4, 8, 9

int spm_mtcmos_ctrl_vdec(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) | VDE_SRAM_PDN);

        while ((spm_read(SPM_VDE_PWR_CON) & VDE_SRAM_ACK) != VDE_SRAM_ACK) {
        }

        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_VDE_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_VDE_PWR_CON, val);

        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & VDE_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & VDE_PWR_STA_MASK)) {
        }
    } else {
        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) | PWR_ON);
        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & VDE_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & VDE_PWR_STA_MASK)) {
        }

        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) | PWR_RST_B);

        spm_write(SPM_VDE_PWR_CON, spm_read(SPM_VDE_PWR_CON) & ~VDE_SRAM_PDN);

        while ((spm_read(SPM_VDE_PWR_CON) & VDE_SRAM_ACK)) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_isp(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) | SRAM_PDN);

        while ((spm_read(SPM_ISP_PWR_CON) & ISP_SRAM_ACK) != ISP_SRAM_ACK) {
        }

        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_ISP_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_ISP_PWR_CON, val);

        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & ISP_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & ISP_PWR_STA_MASK)) {
        }
    } else {    /* STA_POWER_ON */
        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) | PWR_ON);
        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & ISP_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & ISP_PWR_STA_MASK)) {
        }

        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) | PWR_RST_B);

        spm_write(SPM_ISP_PWR_CON, spm_read(SPM_ISP_PWR_CON) & ~SRAM_PDN);

        while ((spm_read(SPM_ISP_PWR_CON) & ISP_SRAM_ACK)) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_disp(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;
    int count = 0;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | DISP_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & DISP_PROT_MASK) != DISP_PROT_MASK) {
            count++;
            if (count > 1000)
                break;
        }
        count = 0;

        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) | SRAM_PDN);

        while ((spm_read(SPM_DIS_PWR_CON) & DIS_SRAM_ACK) != DIS_SRAM_ACK) {
            count++;
            if (count > 1000)
                break;
        }

        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_DIS_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_DIS_PWR_CON, val);

        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & DIS_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & DIS_PWR_STA_MASK)) {
        }
    } else {    /* STA_POWER_ON */
        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) | PWR_ON);
        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & DIS_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & DIS_PWR_STA_MASK)) {
        }

        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) | PWR_RST_B);

        spm_write(SPM_DIS_PWR_CON, spm_read(SPM_DIS_PWR_CON) & ~SRAM_PDN);

        while ((spm_read(SPM_DIS_PWR_CON) & DIS_SRAM_ACK)) {
            count++;
            if (count > 1000)
                break;
        }

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~DISP_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & DISP_PROT_MASK) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_mfg_async(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) | MFG_ASYNC_SRAM_PDN);

        while ((spm_read(SPM_MFG_ASYNC_PWR_CON) & MFG_ASYNC_SRAM_ACK) != MFG_ASYNC_SRAM_ACK) {
        }

        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_MFG_ASYNC_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_MFG_ASYNC_PWR_CON, val);

        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & MFG_ASYNC_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & MFG_ASYNC_PWR_STA_MASK)) {
        }
    } else {    /* STA_POWER_ON */
        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) | PWR_ON);
        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & MFG_ASYNC_PWR_STA_MASK) ||
                !(spm_read(SPM_PWR_STATUS_2ND) & MFG_ASYNC_PWR_STA_MASK)) {
        }

        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) | PWR_RST_B);

        spm_write(SPM_MFG_ASYNC_PWR_CON, spm_read(SPM_MFG_ASYNC_PWR_CON) & ~MFG_ASYNC_SRAM_PDN);

        while ((spm_read(SPM_MFG_ASYNC_PWR_CON) & MFG_ASYNC_SRAM_ACK)) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_mfg_2d(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) | MFG_2D_SRAM_PDN);

        while ((spm_read(SPM_MFG_2D_PWR_CON) & MFG_2D_SRAM_ACK) != MFG_2D_SRAM_ACK) {
        }

        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_MFG_2D_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_MFG_2D_PWR_CON, val);

        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & MFG_2D_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & MFG_2D_PWR_STA_MASK)) {
        }
    } else {    /* STA_POWER_ON */
        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) | PWR_ON);
        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & MFG_2D_PWR_STA_MASK) ||
                !(spm_read(SPM_PWR_STATUS_2ND) & MFG_2D_PWR_STA_MASK)) {
        }

        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) | PWR_RST_B);

        spm_write(SPM_MFG_2D_PWR_CON, spm_read(SPM_MFG_2D_PWR_CON) & ~MFG_2D_SRAM_PDN);

        while ((spm_read(SPM_MFG_2D_PWR_CON) & MFG_2D_SRAM_ACK)) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_mfg(int state)
{
    int err = 0, count = 0;
    volatile unsigned int val;
    unsigned long flags;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | MFG_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & MFG_PROT_MASK) != MFG_PROT_MASK) {
            count++;
            if (count > 1000)
                break;
        }

        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) | MFG_SRAM_PDN);

        while ((spm_read(SPM_MFG_PWR_CON) & MFG_SRAM_ACK) != MFG_SRAM_ACK) {
        }

        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_MFG_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_MFG_PWR_CON, val);

        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & MFG_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & MFG_PWR_STA_MASK)) {
        }
    } else {    /* STA_POWER_ON */
        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) | PWR_ON);
        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & MFG_PWR_STA_MASK) ||
                !(spm_read(SPM_PWR_STATUS_2ND) & MFG_PWR_STA_MASK)) {
        }

        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) | PWR_RST_B);

        spm_write(SPM_MFG_PWR_CON, spm_read(SPM_MFG_PWR_CON) & ~MFG_SRAM_PDN);

        while ((spm_read(SPM_MFG_PWR_CON) & MFG_SRAM_ACK)) {
        }

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~MFG_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & MFG_PROT_MASK) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}

int spm_mtcmos_ctrl_connsys(int state)
{
    int err = 0;
    volatile unsigned int val;
    unsigned long flags;
    int count = 0;

    spm_mtcmos_noncpu_lock(flags);

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | CONN_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & CONN_PROT_MASK) != CONN_PROT_MASK) {
            count++;
            if (count > 1000)
                break;
        }

        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) | CONN_SRAM_PDN);

        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_CONN_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_CONN_PWR_CON, val);

        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & CONN_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & CONN_PWR_STA_MASK)) {
        }
    } else {
        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) | PWR_ON);
        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & CONN_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & CONN_PWR_STA_MASK)) {
        }

        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) | PWR_RST_B);

        spm_write(SPM_CONN_PWR_CON, spm_read(SPM_CONN_PWR_CON) & ~CONN_SRAM_PDN);

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~CONN_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & CONN_PROT_MASK) {
        }
    }

    spm_mtcmos_noncpu_unlock(flags);

    return err;
}
