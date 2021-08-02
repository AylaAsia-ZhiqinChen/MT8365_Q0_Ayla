#ifndef _SEC_MGR_H_
#define _SEC_MGR_H_

#include <vector>
#include <drv/isp_drv.h>


//using namespace NSCam::NSIoPipe;

#define DAPC_NUM_CQ 15

//////////////////////////////////////////////////////////////////////////////////////////////
typedef enum{
    CAM_A   = 0,
    CAM_MAX,
    CAMSV_START = CAM_MAX,
    CAMSV_0 = CAMSV_START,
    CAMSV_1,
    CAMSV_MAX,
    MAX_ISP_HW_MODULE = CAMSV_MAX
}ISP_HW_MODULE;

struct SecMgr_QueryInfo{
	unsigned int Num_of_Cam;
	unsigned int CAM_CTL_EN_P1;
	unsigned int CAM_CTL_EN_P1_DMA;
	unsigned int CAM_CTL_EN_P1_D;
	unsigned int CAM_CTL_EN_P1_DMA_D;
	unsigned int CAM_CTL_FMT_SEL_P1;
	unsigned int CAM_CTL_FMT_SEL_P1_D;
	unsigned int CAM_CTL_SEL_P1;
	unsigned int CAM_CTL_SEL_P1_D;
	unsigned int CAM_CTL_SEL_GLOBAL;
	unsigned int CAM_AWB_WIN_SIZE; 	      //AA_R1_AA_AWB_WIN_SIZE
	unsigned int CAM_AWB_WIN_PIT;
	unsigned int CAM_AWB_ROT;		      //LCES_R1_LCES_OUT_SIZE
	unsigned int CAM_AWB_L0_X;
	unsigned int CAM_RSV_46F4;		      //FLK_R1_FLK_SIZE
	unsigned int CAM_FLK_SIZE;

	unsigned int SecReg_ADDR_CQ[DAPC_NUM_CQ];	   //record all 2nd DAPC register address
	unsigned int SecOn;

};

enum SECMGRCmd {
    SECMGRCmd_QUERY_AF_BLK2_CONSTRAINT         = 0x0001,
    SECMGRCmd_QUERY_AWB_WIN_SIZE_CONSTRAINT    = 0x0002,
    SECMGRCmd_QUERY_AWB_WIN_PIT_CONSTRAINT     = 0x0003,
    SECMGRCmd_QUERY_PS_AWB_WIN_SIZE_CONSTRAINT = 0x0004,
    SECMGRCmd_QUERY_PS_AWB_WIN_PIT_CONSTRAINT  = 0x0005,
    SECMGRCmd_QUERY_FLK_CONSTRAINT             = 0x0006,
    SECMGRCmd_QUERY_MODULE_EN_CONSTRAINT       = 0x0007,
    SECMGRCmd_QUERY_DMA_EN_CONSTRAINT          = 0x0008,
    SECMGRCmd_QUERY_MODULE_EN2_CONSTRAINT      = 0x0009,
    SECMGRCmd_QUERY_SEL_CONSTRAINT             = 0x000A,
    SECMGRCmd_QUERY_NUM_CAM_CONSTRAINT         = 0x000B,
    SECMGRCmd_MAX
};

typedef enum{
    SECMEM_CQ_DESCRIPTOR_TABLE = 0,
    SECMEM_VIRTUAL_REG_TABLE,
    SECMEM_FRAME_HEADER,
    SECMEM_LSC,
    SECMEM_BPC,
    SECMEM_MAX
}SEC_MEM_TYPE;

typedef enum {
    CAM_PORT_IMGO = 0,
    CAM_PORT_RRZO,
   	CAM_PORT_AAO,
    CAM_PORT_LCS,
    CAM_PORT_ESFKO,
    CAM_PORT_CAM_SV0,
    CAM_PORT_CAM_SV1,
    CAM_PORT_LSCI,
    CAM_PORT_LSCI_D,
    CAM_PORT_AFO,

    CAM_PORT_SPARE,
    CAM_PORT_BPCI,
    CAM_PORT_BPCI_D,
    CAM_PORT_UFDI,
    CAM_PORT_IMGI,
    CAM_PORT_IMG2O,
    CAM_PORT_IMG3O,
    CAM_PORT_WPE0_I,
    CAM_PORT_WPE1_I,
    CAM_PORT_WPE_O,

    CAM_PORT_FD0_I,
    CAM_PORT_FD1_I,
    CAM_PORT_FD0_O,
    CAM_PORT_FD1_O,
    CAM_PORT_MAX
}SEC_DMA_PORT;

typedef enum {
    SECTABLE_CQ_DESCRIPTOR = 0,
    SECTABLE_VIRTUAL_REG,
    SECTABLE_FRAME_HEADER,
    SECTABLE_LSC,
    SECTABLE_BPC,
    SECTABLE_MAX
}SEC_TABLE_TYPE;

typedef enum {
    TEE_PROTECT = 0,
    TEE_SECURE,
    TEE_MAX
} TEE_MEM_TYPE;

typedef struct{
    SEC_MEM_TYPE type;
    MUINT32 buff_size;
    uint64_t buff_va;
    MUINT32 buff_sec_mva;
    MINT32  memID;
}SecMgr_SubSecInfo;

typedef struct SecMgr_SecInfo{
    SEC_MEM_TYPE type;
    MUINT32 module;
    MUINT32 cq;
    MUINT32 dummy;
    MUINT32 dupq;
    MUINT32 buff_size;
    uint64_t buff_va;
    MUINT32 port;
    MUINT32 buff_sec_mva;
    MINT32  memID;
    SecMgr_SubSecInfo sub;
    SecMgr_SecInfo(){
        type = SECMEM_MAX;
        module = CAM_A;
        cq = 0;
        dummy = 0;
        dupq = 0;
        buff_size = 0;
        buff_va = 0;
        port = 0;
        memID = 0;
        sub.type = SECMEM_MAX;
        sub.buff_size = 0;
        sub.buff_va = 0;
        sub.buff_sec_mva = 0;
        sub.memID = 0;
    }
}SecMgr_SecInfo;


struct SecMgr_CamInfo{
    uint32_t CamModule;
    uint32_t SecTG;
    uint32_t DevID;
    uint32_t chk_handle;
};


class SecMgr
{
public:
    SecMgr(){}
    virtual ~SecMgr(){}

    static SecMgr* SecMgr_Init();

    static SecMgr* SecMgr_GetMgrObj();

    static MUINT32 SecMgr_GetSecureCamStatus(ISP_HW_MODULE module); //For CQ check

    virtual MBOOL SecMgr_GetHWCapability();

    virtual MBOOL SecMgr_detach(MUINT32 tg);

    virtual MBOOL SecMgr_attach(MUINT32 tg,MUINT32 sen_idx,vector<MUINT32> sec_port,MUINT32 burstq,MUINT32 tee_type,MUINT32 chk);

    virtual MBOOL SecMgr_QuerySecMVAFromHandle(MUINT32 HandlePA,MUINT32* SecMemPA);

    virtual MBOOL SecMgr_P1RegisterShareMem(SecMgr_SecInfo* secinfo);

    virtual MBOOL SecMgr_P1MigrateTable(SecMgr_SecInfo* secinfo);

    virtual MBOOL SecMgr_SecConfig(MUINT32 dma_port,MUINT32 tg_idx);

    virtual MBOOL SecMgr_DumpSecBuffer(MUINT64 HandlePA, void *dst_buf, MUINT32 size);

    virtual MBOOL SecMgr_SetDapcReg();
};
#endif

