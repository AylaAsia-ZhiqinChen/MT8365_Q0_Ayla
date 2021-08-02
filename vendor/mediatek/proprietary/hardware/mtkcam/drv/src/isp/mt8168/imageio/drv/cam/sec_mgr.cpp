#define LOG_TAG "SecMgr"

#include <dlfcn.h>
#include <stdlib.h>
#include <imageio/ispio_pipe_ports.h>
#include "isp_function.h"
#include "imageio_log.h"                    // Note: DBG_LOG_TAG/LEVEL will be used in header file, so header must be included after definition.
#include "sec_mgr.h"
#include "camera_isp.h"

#define CAM_NUM 1

#undef   DBG_LOG_TAG                        // Decide a Log TAG for current file.
#ifndef USING_MTK_LDVT   // Not using LDVT.
#define  DBG_LOG_TAG        ""
#else
#define  DBG_LOG_TAG        LOG_TAG
#endif

using namespace std;
using namespace NSCam::NSIoPipe;

DECLARE_DBG_LOG_VARIABLE(SecMgr);

#undef SecMgr_VRB
#undef SecMgr_DBG
#undef SecMgr_INF
#undef SecMgr_WRN
#undef SecMgr_ERR
#undef SecMgr_AST
#define SecMgr_VRB(fmt, arg...)        do { if (SecMgr_DbgLogEnable_VERBOSE) { BASE_LOG_VRB(fmt, ##arg); } } while(0)
#define SecMgr_DBG(fmt, arg...)        do { if (SecMgr_DbgLogEnable_DEBUG  ) { BASE_LOG_DBG(fmt, ##arg); } } while(0)
#define SecMgr_INF(fmt, arg...)        do { if (SecMgr_DbgLogEnable_INFO   ) { BASE_LOG_INF(fmt, ##arg); } } while(0)
#define SecMgr_WRN(fmt, arg...)        do { if (SecMgr_DbgLogEnable_WARN   ) { BASE_LOG_WRN(fmt, ##arg); } } while(0)
#define SecMgr_ERR(fmt, arg...)        do { if (SecMgr_DbgLogEnable_ERROR  ) { BASE_LOG_ERR(fmt, ##arg); } } while(0)
#define SecMgr_AST(cond, fmt, arg...)  do { if (SecMgr_DbgLogEnable_ASSERT ) { BASE_LOG_AST(cond, fmt, ##arg); } } while(0)

#define MTK_SEC_ISP_LIB            "uree_isp.so"
#define MTK_SEC_ISP_CREATE         "MtkSecISP_tlcHandleCreate"
#define MTK_SEC_ISP_OPEN           "MtkSecISP_tlcOpen"
#define MTK_SEC_ISP_CLOSE          "MtkSecISP_tlcClose"
#define MTK_SEC_ISP_INIT           "MtkSecISP_tlcInit"
#define MTK_SEC_ISP_UNINIT         "MtkSecISP_tlcUnInit"
#define MTK_SEC_ISP_CONFIG         "MtkSecISP_tlcSecConfig"
#define MTK_SEC_ISP_QUERYPA        "MtkSecISP_tlcQueryMVAFromHandle"
#define MTK_SEC_ISP_QUERYHWINFO    "MtkSecISP_tlcQueryHWInfo"
#define MTK_SEC_ISP_REGISTER_MEM   "MtkSecISP_tlcRegisterShareMem"
#define MTK_SEC_ISP_MIGRATE_TABLE  "MtkSecISP_tlcMigrateTable"
#define MTK_SEC_ISP_DUMP_SECBUFFER "MtkSecISP_tlcDumpSecmem"
#define MTK_SEC_ISP_SETSECCAM      "MtkSecISP_tlcSetSecCam"

typedef MUINT64 MtkSecISP_tlcHandleCreate();
typedef int MtkSecISP_tlcOpen(MUINT64);
typedef int MtkSecISP_tlcClose(MUINT64);
typedef int MtkSecISP_tlcInit(MUINT64);
typedef int MtkSecISP_tlcUnInit(MUINT64);
typedef int MtkSecISP_tlcSecConfig(MUINT64,MINT32,MINT32);
typedef int MtkSecISP_tlcQueryMVAFromHandle(MUINT64,MUINT64,MUINT64,TEE_MEM_TYPE);
typedef int MtkSecISP_tlcQueryHWInfo(MUINT64,MUINT64);
typedef int MtkSecISP_tlcRegisterShareMem(MUINT64, MUINT64);
typedef int MtkSecISP_tlcMigrateTable(MUINT64,SecMgr_SecInfo*);
typedef int MtkSecISP_tlcDumpSecmem(MUINT64,MUINT64,MUINT64,MUINT32);
typedef int MtkSecISP_tlcSetSecCam(MUINT64,SecMgr_CamInfo);

MtkSecISP_tlcHandleCreate           *g_isp_ca_handlecreate     = nullptr;       // SecMgr_OpenCA
MtkSecISP_tlcOpen                   *g_isp_ca_open             = nullptr;       // SecMgr_OpenCA
MtkSecISP_tlcClose                  *g_isp_ca_close            = nullptr;       // SecMgr_detach
MtkSecISP_tlcInit                   *g_isp_ca_init             = nullptr;       // SecMgr_InitCA
MtkSecISP_tlcUnInit                 *g_isp_ca_uninit           = nullptr;       // SecMgr_detach
MtkSecISP_tlcSecConfig              *g_isp_ca_config           = nullptr;       // SecMgr_SecConfig
MtkSecISP_tlcQueryMVAFromHandle     *g_isp_ca_querypa          = nullptr;       // SecMgr_QuerySecMVAFromHandle
MtkSecISP_tlcQueryHWInfo            *g_isp_ca_queryinfo        = nullptr;       // SecMgr_GetHWCapability
MtkSecISP_tlcRegisterShareMem       *g_isp_ca_registersharemem = nullptr;       // SecMgr_P1RegisterShareMem
MtkSecISP_tlcMigrateTable           *g_isp_ca_migratetable     = nullptr;       // SecMgr_P1MigrateTable
MtkSecISP_tlcDumpSecmem             *g_isp_ca_dumpsecbuffer    = nullptr;       // SecMgr_DumpSecBuffer
MtkSecISP_tlcSetSecCam              *g_isp_ca_setcaminfo       = nullptr;       // SecMgr_attach
struct Sec_CamState {
    struct SecMgr_CamInfo cam_info[CAM_NUM];
};

class SecMgrImp : public SecMgr
{
public:
    SecMgrImp();
    ~SecMgrImp(){}

    virtual MBOOL SecMgr_LoadCA();

    virtual MBOOL SecMgr_InitCA();

    virtual MBOOL SecMgr_OpenCA();

    virtual MBOOL SecMgr_GetHWCapability();

    virtual MBOOL SecMgr_detach(MUINT32 tg);

    virtual MBOOL SecMgr_attach(MUINT32 tg,MUINT32 sen_idx,vector<MUINT32> sec_port,MUINT32 burstq,MUINT32 tee_type,MUINT32 chk);

    virtual MBOOL SecMgr_QuerySecMVAFromHandle(MUINT32 HandlePA,MUINT32* SecMemPA);

    virtual MBOOL SecMgr_P1RegisterShareMem(SecMgr_SecInfo* secinfo);

    virtual MBOOL SecMgr_P1MigrateTable(SecMgr_SecInfo* secinfo);

    virtual MBOOL SecMgr_SecConfig(MUINT32 dma_port,MUINT32 tg_idx);

    virtual MBOOL SecMgr_DumpSecBuffer(MUINT64 HandlePA, void *dst_buf, MUINT32 size);

    virtual MUINT32 SecMgr_GetSecureCamStatus(ISP_HW_MODULE module); //For CQ protection

    virtual MBOOL SecMgr_SetDapcReg();

private:
    SecMgr_QueryInfo SecHwInfo[2];//0 for secure constraint; 1 for non-secure constraint

    ISP_HW_MODULE mModule[2]; //which CAM initialize secure CAM

    SecMgr_CamInfo CamInfo[2];

    IspDrv* m_pIspDrv;

    static MUINT32 Num_SecCam;

    MUINT32 dummy_sec_mva[3];

    vector<MUINT32> buf_sec_mva;

    vector<uint64_t> buf_sec_mva_idx;

    vector<MUINT32> lsc_buf_sec_mva;

    vector<uint64_t> lsc_buff_idx;  //lsc buff's VA

    TEE_MEM_TYPE tee_mem_type;

    vector<MUINT32> enable_secport;

    mutable Mutex m_lock;  // for multi-thread
};



static SecMgrImp gSecMgr;
static MUINT64 ispHandle;
static void* m_isp_ca;

MUINT32 SecMgrImp::Num_SecCam = 0;

SecMgrImp::SecMgrImp()
{
    memset(&SecHwInfo[0], 0, sizeof(SecMgr_QueryInfo));
    memset(&SecHwInfo[1], 0, sizeof(SecMgr_QueryInfo));
    memset(&CamInfo[0], 0, sizeof(SecMgr_CamInfo));
    memset(&CamInfo[1], 0, sizeof(SecMgr_CamInfo));
    this->mModule[0] = CAM_MAX;
    this->mModule[1] = CAM_MAX;
    this->m_pIspDrv = NULL;
    this->dummy_sec_mva[0] = 0;
    this->dummy_sec_mva[1] = 0;
    this->dummy_sec_mva[2] = 0;

    this->tee_mem_type = TEE_MAX;

    this->buf_sec_mva.clear();
    this->buf_sec_mva_idx.clear();
    this->lsc_buf_sec_mva.clear();
    this->lsc_buff_idx.clear();
    this->enable_secport.clear();
}

MBOOL SecMgrImp::SecMgr_LoadCA()
{
    SecMgr_DBG("+");
    MBOOL ret = MTRUE;
    m_isp_ca = dlopen(MTK_SEC_ISP_LIB, RTLD_NOW);

    if(nullptr == m_isp_ca)
    {
        SecMgr_ERR("ISP CA LIB open failed (%s)",dlerror());
        return MFALSE;
    }

    SecMgr_DBG("ISP CA LIB open success");

    if (nullptr == g_isp_ca_handlecreate)
    {
        g_isp_ca_handlecreate = (MtkSecISP_tlcHandleCreate*) dlsym(m_isp_ca, MTK_SEC_ISP_CREATE);
        if (nullptr == g_isp_ca_handlecreate)
        {
            SecMgr_ERR("Load CA create handle failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_open)
    {
        g_isp_ca_open = (MtkSecISP_tlcOpen*) dlsym(m_isp_ca, MTK_SEC_ISP_OPEN);
        if (nullptr == g_isp_ca_open)
        {
            SecMgr_ERR("Load CA open failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_close)
    {
        g_isp_ca_close = (MtkSecISP_tlcClose*) dlsym(m_isp_ca, MTK_SEC_ISP_CLOSE);
        if (nullptr == g_isp_ca_close)
        {
            SecMgr_ERR("Load CA close failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_init)
    {
        g_isp_ca_init = (MtkSecISP_tlcInit*) dlsym(m_isp_ca, MTK_SEC_ISP_INIT);
        if (nullptr == g_isp_ca_init)
        {
            SecMgr_ERR("Load CA init failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_uninit)
    {
        g_isp_ca_uninit = (MtkSecISP_tlcUnInit*) dlsym(m_isp_ca, MTK_SEC_ISP_UNINIT);
        if (nullptr == g_isp_ca_uninit)
        {
            SecMgr_ERR("Load CA uninit failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_config)
    {
        g_isp_ca_config = (MtkSecISP_tlcSecConfig*) dlsym(m_isp_ca, MTK_SEC_ISP_CONFIG);
        if (nullptr == g_isp_ca_config)
        {
            SecMgr_ERR("Load CA config failed!");
            ret = MFALSE;
            goto Exit;
        }
    }
    if (nullptr == g_isp_ca_querypa)
    {
        g_isp_ca_querypa = (MtkSecISP_tlcQueryMVAFromHandle*) dlsym(m_isp_ca, MTK_SEC_ISP_QUERYPA);
        if (nullptr == g_isp_ca_querypa)
        {
            SecMgr_ERR("Load CA querypa failed!");
            ret = MFALSE;
            goto Exit;
        }
    }
    if (nullptr == g_isp_ca_queryinfo)
    {
        g_isp_ca_queryinfo = (MtkSecISP_tlcQueryHWInfo*) dlsym(m_isp_ca, MTK_SEC_ISP_QUERYHWINFO);
        if (nullptr == g_isp_ca_queryinfo)
        {
            SecMgr_ERR("Load CA queryinfo failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_registersharemem)
    {
        g_isp_ca_registersharemem = (MtkSecISP_tlcRegisterShareMem*) dlsym(m_isp_ca, MTK_SEC_ISP_REGISTER_MEM);
        if (nullptr == g_isp_ca_registersharemem)
        {
            SecMgr_ERR("Load CA register share mem failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_migratetable)
    {
        g_isp_ca_migratetable = (MtkSecISP_tlcMigrateTable*) dlsym(m_isp_ca, MTK_SEC_ISP_MIGRATE_TABLE);
        if (nullptr == g_isp_ca_migratetable)
        {
            SecMgr_ERR("Load CA migrate table failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_dumpsecbuffer)
    {
        g_isp_ca_dumpsecbuffer = (MtkSecISP_tlcDumpSecmem*) dlsym(m_isp_ca, MTK_SEC_ISP_DUMP_SECBUFFER);
        if (nullptr == g_isp_ca_dumpsecbuffer)
        {
            SecMgr_ERR("Load CA dump secbuffer failed!");
            ret = MFALSE;
            goto Exit;
        }
    }

    if (nullptr == g_isp_ca_setcaminfo)
    {
        g_isp_ca_setcaminfo = (MtkSecISP_tlcSetSecCam*) dlsym(m_isp_ca, MTK_SEC_ISP_SETSECCAM);
        if (nullptr == g_isp_ca_setcaminfo)
        {
            SecMgr_ERR("Load CA set caminfo failed!");
            ret = MFALSE;
            goto Exit;
        }
    }


Exit:
    if (ret == MFALSE)
    {
        dlclose(m_isp_ca);
        m_isp_ca = NULL;
    }
    return ret;
}


MBOOL SecMgrImp::SecMgr_OpenCA()
{
    ispHandle = g_isp_ca_handlecreate();
    if(ispHandle == NULL){
        SecMgr_ERR("ISP CA handle create failed!");
        return MFALSE;
    }
    SecMgr_DBG("SecMgr_OpenCA+ (0x%x)",ispHandle);
    if (!g_isp_ca_open(ispHandle))
    {
        SecMgr_ERR("ISP CA open failed!");
        return MFALSE;
    }
    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_InitCA()
{
    SecMgr_DBG("SecMgr_InitCA+ (0x%x)",ispHandle);
    if (!g_isp_ca_init(ispHandle))
    {
        SecMgr_ERR("ISP CA init failed!");
        return MFALSE;
    }
    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_QuerySecMVAFromHandle(MUINT32 HandlePA,MUINT32* SecMemPA)
{
    if(this->buf_sec_mva.size() < 1){
        if (!g_isp_ca_querypa(ispHandle, HandlePA, (MUINT64)SecMemPA, this->tee_mem_type))
        {
            SecMgr_ERR("Query secmem PA failed!");
            return MFALSE;
        }

       this->buf_sec_mva.push_back(*SecMemPA);
       this->buf_sec_mva_idx.push_back(HandlePA);

    }else{
        vector<uint64_t>::iterator iter=std::find(this->buf_sec_mva_idx.begin(), this->buf_sec_mva_idx.end(), HandlePA);
        if (iter == buf_sec_mva_idx.end()) {
            if (!g_isp_ca_querypa(ispHandle, HandlePA, (MUINT64)SecMemPA, this->tee_mem_type))
            {
                SecMgr_ERR("Query secmem PA failed!");
                return MFALSE;
            }
            this->buf_sec_mva.push_back(*SecMemPA);
            this->buf_sec_mva_idx.push_back(HandlePA);
        }
        else {
            *SecMemPA = this->buf_sec_mva.at(iter - this->buf_sec_mva_idx.begin());
        }
    }
    SecMgr_DBG("SecMgr_QuerySecMVAFromHandle HandlePA:0x%lx SecMemPA:0x%x",HandlePA, *SecMemPA);

    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_GetHWCapability()
{
    SecMgr_DBG("+ (0x%x)",ispHandle);

    this->SecHwInfo[0].SecOn = 1;
    if (!g_isp_ca_queryinfo(ispHandle,(MUINT64)&this->SecHwInfo[0]))
    {
        SecMgr_ERR("ISP CA get secure HW capability failed!");
        return MFALSE;
    }
    SecMgr_DBG("SecHwInfo: nCam=0x%x EN_P1=0x%x EN_DMA=0x%x EN_P1_D=0x%x EN_DMA_D=0x%x FMT_SEL=0x%x FMT_SEL_D=0x%x SEL_P1=0x%x SEL_P1_D=0x%x SEL_GLB=0x%x",
        this->SecHwInfo[0].Num_of_Cam,
        this->SecHwInfo[0].CAM_CTL_EN_P1,
        this->SecHwInfo[0].CAM_CTL_EN_P1_DMA,
        this->SecHwInfo[0].CAM_CTL_EN_P1_D,
        this->SecHwInfo[0].CAM_CTL_EN_P1_DMA_D,
        this->SecHwInfo[0].CAM_CTL_FMT_SEL_P1,
        this->SecHwInfo[0].CAM_CTL_FMT_SEL_P1_D,
        this->SecHwInfo[0].CAM_CTL_SEL_P1,
        this->SecHwInfo[0].CAM_CTL_SEL_P1_D,
        this->SecHwInfo[0].CAM_CTL_SEL_GLOBAL);
    for(MUINT i=0;i<DAPC_NUM_CQ;i++)
        SecMgr_DBG("SecReg[%d]:0x%x",i,this->SecHwInfo[0].SecReg_ADDR_CQ[i]);

    SecMgr_SetDapcReg();
    return MTRUE;
}

#define str(x)  ({\
   static char _str[48];\
   _str[0] = '\0';\
   switch(x){\
       case SECMEM_CQ_DESCRIPTOR_TABLE:\
           sprintf(_str,"SECMEM_CQ_DESCRIPTOR_TABLE");\
           break;\
       case SECMEM_VIRTUAL_REG_TABLE:\
           sprintf(_str,"SECMEM_VIRTUAL_REG_TABLE");\
           break;\
       case SECMEM_FRAME_HEADER:\
           sprintf(_str,"SECMEM_FRAME_HEADER");\
           break;\
       case SECMEM_LSC:\
           sprintf(_str,"SECMEM_LSC");\
           break;\
       case SECMEM_BPC:\
           sprintf(_str,"SECMEM_BPC");\
           break;\
   }\
   _str;\
})

MBOOL SecMgrImp::SecMgr_P1RegisterShareMem(SecMgr_SecInfo* secinfo)
{
   static MUINT32 fh_idx = 0;
   MUINT32 _port = 0;

   switch(secinfo->type)
   {
       case SECMEM_CQ_DESCRIPTOR_TABLE:
       case SECMEM_VIRTUAL_REG_TABLE:
            SecMgr_DBG("+ module:0x%x cq:%d dup:%d dmy:%d type:%d buff_va:0x%llx size:0x%x memID:0x%x",
            secinfo->module,secinfo->cq,secinfo->dupq,secinfo->dummy,secinfo->type,secinfo->buff_va,secinfo->buff_size,secinfo->memID);
            SecMgr_DBG("+ module:0x%x cq:%d dup:%d dmy:%d sub-type:%d buff_va:0x%llx size:0x%x memID:0x%x",
            secinfo->module,secinfo->cq,secinfo->dupq,secinfo->dummy,secinfo->sub.type,secinfo->sub.buff_va,secinfo->sub.buff_size,secinfo->sub.memID);

            if (!g_isp_ca_registersharemem(ispHandle,(MUINT64)secinfo))
            {
                SecMgr_ERR("Share mem register failed (module:0x%x type:%d buff:0x%llx size:0x%x)",
                            secinfo->module,secinfo->type,secinfo->buff_va,secinfo->buff_size);
                return MFALSE;
            }
            break;
       case SECTABLE_LSC:
            SecMgr_DBG("+ module:0x%x type:%d buff:0x%llx size:0x%x memID:0x%x",
                secinfo->module,secinfo->type,secinfo->buff_va,secinfo->buff_size,secinfo->memID);

            if(this->lsc_buff_idx.size() < 1){
                if (!g_isp_ca_registersharemem(ispHandle,(MUINT64)secinfo))
                {
                    SecMgr_ERR("Share mem register failed (module:0x%x type:%d buff:0x%llx size:0x%x)",
                            secinfo->module,secinfo->type,secinfo->buff_va,secinfo->buff_size);
                    return MFALSE;
                }

                this->lsc_buff_idx.push_back(secinfo->buff_va);
                this->lsc_buf_sec_mva.push_back(secinfo->buff_sec_mva);

            }else{
                vector<uint64_t>::iterator iter=std::find(this->lsc_buff_idx.begin(), this->lsc_buff_idx.end(), secinfo->buff_va);
                if (iter == lsc_buff_idx.end()) {
                    if (!g_isp_ca_registersharemem(ispHandle,(MUINT64)secinfo))
                    {
                        SecMgr_ERR("Share mem register failed (module:0x%x type:%d buff:0x%llx size:0x%x)",
                                secinfo->module,secinfo->type,secinfo->buff_va,secinfo->buff_size);
                        return MFALSE;
                    }
                    this->lsc_buff_idx.push_back(secinfo->buff_va);
                    this->lsc_buf_sec_mva.push_back(secinfo->buff_sec_mva);
                }
                else {
                    secinfo->buff_sec_mva = this->lsc_buf_sec_mva.at(iter - this->lsc_buff_idx.begin());
                }
           }
           break;
       default:
            SecMgr_ERR("Unsupport table type (%d)",secinfo->type);
            return MFALSE;
            break;
   }

    return MTRUE;
}


MBOOL SecMgrImp::SecMgr_P1MigrateTable(SecMgr_SecInfo* secinfo)
{
   SecMgr_DBG("+ module:0x%x type:%d",secinfo->module,secinfo->type);

   int* buf = (int*)secinfo->buff_va;
   for(int i=0;i<24; i+=4)
       SecMgr_DBG("SecMgr_P1MigrateTable 0x%08x 0x%08x 0x%08x 0x%08x \n", *(buf+i), *(buf+i+1), *(buf+i+2), *(buf+i+3));

   SecMgr_SecInfo sinfo;

   sinfo.type = secinfo->type;
   sinfo.module = secinfo->module;

   switch(secinfo->type)
   {
       case SECMEM_CQ_DESCRIPTOR_TABLE:
       case SECMEM_VIRTUAL_REG_TABLE:
            sinfo.cq = secinfo->cq;
            sinfo.dupq = secinfo->dupq;
            sinfo.dummy = secinfo->dummy;
            sinfo.sub.type = secinfo->sub.type;
            sinfo.buff_va = secinfo->buff_va;
            SecMgr_DBG("type %x module %x cq %x dummy %x dupq %x buff_size %x buff_va %llu port %x buff_sec_mva %x",
                sinfo.type, sinfo.module,
                sinfo.cq, sinfo.dummy,
                sinfo.dupq, sinfo.buff_size,
                sinfo.buff_va, sinfo.port,
                sinfo.buff_sec_mva);

            break;
       case SECTABLE_LSC:
            sinfo.buff_va = secinfo->buff_va;
            break;
       default:
           SecMgr_ERR("not supported");
           return MFALSE;
   }

   if (!g_isp_ca_migratetable(ispHandle,&sinfo))
   {
       SecMgr_ERR("Share mem migration failed (module:0x%x type:%d)",secinfo->module,secinfo->type);
       return MFALSE;
   }
    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_SecConfig(MUINT32 dma_port,MUINT32 tg_idx)
{
    SEC_DMA_PORT _port = CAM_PORT_MAX;
    _port = CAM_PORT_RRZO;
    if (!g_isp_ca_config(ispHandle,_port,tg_idx))
    {
        SecMgr_ERR("ISP CA Config DMA Port(%d) failed!",_port);
        return MFALSE;
    }

    _port = CAM_PORT_IMGO;
    if (!g_isp_ca_config(ispHandle,_port,tg_idx))
    {
        SecMgr_ERR("ISP CA Config DMA Port(%d) failed!",_port);
        return MFALSE;
    }

    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_SetDapcReg()
{
    SecMgr_DBG("+");

    vector<MUINT32> reg_list;
    ISP_DRV_DAPC_REG_STRUCT DAPCReg;

    this->m_pIspDrv = IspDrv::createInstance();
    //this->m_pIspDrv->init("SecMgr");

    for(MUINT32 i=0;i<DAPC_NUM_CQ;i++){
        reg_list.push_back(this->SecHwInfo[0].SecReg_ADDR_CQ[i]);
    }
    DAPCReg.CAM_CTL_EN_P1       = (this->SecHwInfo[0].CAM_CTL_EN_P1);
    DAPCReg.CAM_CTL_EN_P1_DMA   = (this->SecHwInfo[0].CAM_CTL_EN_P1_DMA);
    DAPCReg.CAM_CTL_EN_P1_D     = (this->SecHwInfo[0].CAM_CTL_EN_P1_D);
    DAPCReg.CAM_CTL_EN_P1_DMA_D = (this->SecHwInfo[0].CAM_CTL_EN_P1_DMA_D);
    DAPCReg.CAM_CTL_FMT_SEL_P1  = (this->SecHwInfo[0].CAM_CTL_FMT_SEL_P1);
    DAPCReg.CAM_CTL_FMT_SEL_P1_D = (this->SecHwInfo[0].CAM_CTL_FMT_SEL_P1_D);
    DAPCReg.CAM_CTL_SEL_P1      = (this->SecHwInfo[0].CAM_CTL_SEL_P1);
    DAPCReg.CAM_CTL_SEL_P1_D    = (this->SecHwInfo[0].CAM_CTL_SEL_P1_D);
    DAPCReg.CAM_CTL_SEL_GLOBAL  = (this->SecHwInfo[0].CAM_CTL_SEL_GLOBAL);

    this->m_pIspDrv->setDapcReg(reg_list);
    this->m_pIspDrv->setDapcRegVal(&DAPCReg);
    //this->m_pIspDrv->uninit("SecMgr");
    //this->m_pIspDrv->destroyInstance();
    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_DumpSecBuffer(MUINT64 HandlePA, void *dst_buf, MUINT32 size)
{
    SecMgr_INF("+ HandlePA(0x%llx) dst(0x%llx) size(0x%x)",HandlePA,dst_buf,size);

    if (!g_isp_ca_dumpsecbuffer(ispHandle,HandlePA,(MUINT64)dst_buf,size))
    {
        SecMgr_ERR("ISP CA dump sec buff failed!");
        return MFALSE;
    }
    return MTRUE;
}

MBOOL SecMgrImp::SecMgr_attach(MUINT32 tg,MUINT32 sen_idx,vector<MUINT32> sec_port,MUINT32 burstq,MUINT32 tee_type,MUINT32 chk)
{
    SecMgr_INF("+ TG:0x%x SenID:0x%x TEE_Type:0x%x chk:0x%lx port.size:%d",tg,sen_idx,tee_type,chk, sec_port.size());

    if(burstq > 1)
    {
        SecMgr_ERR("Failed - Secure Cam DO NOT support subsample mode(0x%x)",burstq);
        return MFALSE;
    }
    if (MFALSE == this->SecMgr_SecConfig(0, tg)) {
        SecMgr_ERR("SecMgr_SecConfig DMA fail");
        return MFALSE;
    }
    for(MUINT32 j=0;j<sec_port.size();j++) {
        this->enable_secport.push_back(sec_port.at(j));
    }

    switch(tg)
    {
        case 0:
            this->CamInfo[0].CamModule = CAM_A;
            this->CamInfo[0].SecTG = tg;
            this->CamInfo[0].DevID = sen_idx;
            this->CamInfo[0].chk_handle = chk;

            if (!g_isp_ca_setcaminfo(ispHandle,this->CamInfo[0]))
            {
                SecMgr_ERR("ISP CA set caminfo failed(0x%x)",tg);
                goto EXIT;
            }

            this->Num_SecCam++;
            SecMgr_INF("Secure Cam(0x%x) attached(0x%x)",this->CamInfo[0].CamModule,this->Num_SecCam);
            break;
        default:
            SecMgr_ERR("Unsupport tg type (%d)",tg);
            goto EXIT;
            break;
    }
    SecMgr_INF("Secure Cam %d %d %d",this->CamInfo[0].CamModule,this->CamInfo[0].SecTG,this->CamInfo[0].DevID);

    switch(tee_type)
    {
        case 1:
            this->tee_mem_type = TEE_PROTECT;
            break;
        case 2:
            this->tee_mem_type = TEE_SECURE;
            break;
        default:
            SecMgr_ERR("Unsupport TEE mem type (%d)",tee_type);
            goto EXIT;
            break;
    }
/*
    if (MFALSE == gSecMgr.SecMgr_GetHWCapability()){
        SecMgr_ERR("Get secure constraints failure");
        goto EXIT;
    }
*/
    return MTRUE;
EXIT:
    enable_secport.clear();
    return MFALSE;
}

MBOOL SecMgrImp::SecMgr_detach(MUINT32 tg)
{
    SecMgr_INF("+SecMgr_detach SenIdx:0x%x",tg);

    if(this->Num_SecCam == 1){

        if (!g_isp_ca_uninit(ispHandle))
        {
            SecMgr_ERR("ISP CA unint failed!");
            return MFALSE;
        }

        if (!g_isp_ca_close(ispHandle))
        {
            SecMgr_ERR("ISP CA close failed!");
            return MFALSE;
        }

        g_isp_ca_handlecreate     = nullptr;
        g_isp_ca_open             = nullptr;
        g_isp_ca_close            = nullptr;
        g_isp_ca_init             = nullptr;
        g_isp_ca_uninit           = nullptr;
        g_isp_ca_config           = nullptr;
        g_isp_ca_querypa          = nullptr;
        g_isp_ca_queryinfo        = nullptr;
        g_isp_ca_registersharemem = nullptr;
        g_isp_ca_migratetable     = nullptr;
        g_isp_ca_dumpsecbuffer    = nullptr;
        g_isp_ca_setcaminfo       = nullptr;

        memset(SecHwInfo, 0, sizeof(SecMgr_QueryInfo)*2);
        memset(CamInfo, 0, sizeof(SecMgr_CamInfo)*2);
        this->mModule[0] = CAM_MAX;
        this->mModule[1] = CAM_MAX;

        this->dummy_sec_mva[0] = 0;
        this->dummy_sec_mva[1] = 0;
        this->dummy_sec_mva[2] = 0;

        this->m_pIspDrv = NULL;
        this->Num_SecCam = 0;
        this->tee_mem_type = TEE_MAX;
        this->buf_sec_mva.clear();
        this->buf_sec_mva_idx.clear();
        this->lsc_buf_sec_mva.clear();
        this->lsc_buff_idx.clear();
        this->enable_secport.clear();
        ispHandle = NULL;
        if(m_isp_ca){
            dlclose(m_isp_ca);
           m_isp_ca = NULL;
        }

    }
    else if(this->Num_SecCam > 1){
        this->Num_SecCam--;
    }else{
        SecMgr_ERR("SecMgr detach failed:Secure Cam (0x%x) is not enabled before",tg);
        return MFALSE;
    }

    return MTRUE;
}

MUINT32 SecMgr::SecMgr_GetSecureCamStatus(ISP_HW_MODULE module)
{
    return gSecMgr.SecMgr_GetSecureCamStatus(module);
}

SecMgr* SecMgr::SecMgr_Init()
{
    MINT32 ret = 0;
    DBG_LOG_CONFIG(imageio, SecMgr);

    SecMgr_INF("+ (0x%x)",ispHandle);

    if (MFALSE == gSecMgr.SecMgr_LoadCA()){
        SecMgr_ERR("Load CA library failure");
        return NULL;
    }

    if (MFALSE == gSecMgr.SecMgr_OpenCA()){
        SecMgr_ERR("Open CA session failure");
        return NULL;
    }

    if (MFALSE == gSecMgr.SecMgr_InitCA()){
        SecMgr_ERR("Init CA failure");
        return NULL;
    }

    return (SecMgr*)&gSecMgr;
}

SecMgr* SecMgr::SecMgr_GetMgrObj()
{
    SecMgr_DBG("+ (0x%x)",ispHandle);
    SecMgr_DBG("gSecMgrf:0x%x",&gSecMgr);
    return (SecMgr*)&gSecMgr;
}

MUINT32 SecMgrImp::SecMgr_GetSecureCamStatus(ISP_HW_MODULE module)
{
    SecMgr_DBG("+ (0x%x)",ispHandle);
    MUINT32 lock = 0;
    switch(module){
        default:
            if(this->Num_SecCam > 0)
               lock = 1;
            break;
    }
    return lock;
}

 
