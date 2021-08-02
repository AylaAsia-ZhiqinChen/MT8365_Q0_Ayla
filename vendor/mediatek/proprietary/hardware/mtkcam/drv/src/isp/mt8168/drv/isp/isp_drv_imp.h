/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef ISP_DRV_IMP_H
#define ISP_DRV_IMP_H
//-----------------------------------------------------------------------------
//#include <utils/Errors.h>
#include <list>
#if defined (__ISP_USE_ION__)
    #include <linux/ion_drv.h>
    #include <ion.h>
    #include <linux/mtk_ion.h>
    #include <ion/ion.h>
#endif
//-----------------------------------------------------------------------------
using namespace std;
using namespace android;
//-----------------------------------------------------------------------------
#define ISP_DRV_DEV_NAME            "/dev/camera-isp"
#define ISP_DRV_VIR_DEFAULT_DATA    (0)
#define ISP_DRV_VIR_ADDR_ALIGN      (0x03) // 4 bytes alignment
#define ISP_DRV_CQ_DESCRIPTOR_ADDR_ALIGN      (0x07) // 8 bytes alignment

// for p2 tpipe dump information
#define DUMP_TPIPE_SIZE         100
#define DUMP_TPIPE_NUM_PER_LINE 10
#define UNUSED(x) (void)(x)

#if 1
typedef struct {
    MINT32    virtualAddrCq;
    MINT32    descriptorCq;
}ISP_DRV_CQ_MAPPING;
#else
typedef struct {
    ISP_DRV_CQ_ENUM             virtualAddrCq;
    ISP_DRV_DESCRIPTOR_CQ_ENUM  descriptorCq;
}ISP_DRV_CQ_MAPPING;
#endif


/////for tuning function
enum ETuningCtlByte
{
    eTuningCtlByte_P1 = 0,
    eTuningCtlByte_P1D,
    eTuningCtlByte_P2,
    eTuningCtlByte_Num
};

typedef struct
{
    EIspTuningMgrFuncBit eTuningFuncBit;
    ETuningCtlByte eTuningCtlByte;
    MINT32 camCtlEnP1Bit;
    MINT32 camCtlEnP1DBit;
    MINT32 camCtlEnP2Bit;
    //
    MINT32 camCtlEnP1DmaBit;
    MINT32 camCtlEnP1DDmaBit;
    MINT32 camCtlEnP2DmaBit;
    //
    CAM_MODULE_ENUM eTuningCqFunc1;  // cq function case1
    CAM_MODULE_ENUM eTuningCqFunc2;  // cq function case2
}ISP_TURNING_FUNC_BIT_MAPPING;


typedef struct
{
    MINT32          mIspFd;
    MINT32          mIonFd;
}ISP_DRV_FD_NODE_STRUCT;


//-----------------------------------------------------------------------------
class IspDrvImp : public IspDrv
{
    public:
        IspDrvImp();
        virtual ~IspDrvImp();
    //
    public:
        static IspDrv*  getInstance();
        virtual void    destroyInstance(void);
        virtual MINT32  isp_fd_open(const char* userName="");
        virtual MINT32  isp_fd_close(MINT32 mIspFd);
        virtual MBOOL   init(const char* userName="");
        virtual MBOOL   uninit(const char* userName="");
        virtual MBOOL   waitIrq(ISP_DRV_WAIT_IRQ_STRUCT* pWaitIrq);
        virtual MINT32   registerIrq(const char* userName) ;
        virtual MBOOL   markIrq(ISP_DRV_WAIT_IRQ_STRUCT Irqinfo) ;
        virtual MBOOL   flushIrq(ISP_DRV_WAIT_IRQ_STRUCT Irqinfo) ;
        virtual MBOOL   queryirqtimeinfo(ISP_DRV_WAIT_IRQ_STRUCT* Irqinfo) ;
        virtual MBOOL   readIrq(ISP_DRV_READ_IRQ_STRUCT* pReadIrq);
        virtual MBOOL   checkIrq(ISP_DRV_CHECK_IRQ_STRUCT CheckIrq);
        virtual MBOOL   clearIrq(ISP_DRV_CLEAR_IRQ_STRUCT ClearIrq);
        virtual MBOOL   unregisterIrq(ISP_DRV_WAIT_IRQ_STRUCT WaitIrq);
        virtual MBOOL   reset(MINT32 rstpath);
        virtual MBOOL   resetBuf(void);
        virtual MBOOL   checkCQBufAllocated(void);
        virtual MBOOL   readRegs(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count,
            MINT32                  caller=0);
        virtual MUINT32 readReg(MUINT32 Addr,MINT32 caller=0);
        virtual MBOOL   writeRegs(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count,
            MINT32                  userEnum=ISP_DRV_USER_OTHER,
            MINT32                  caller=0);
        virtual MBOOL   writeReg(
            MUINT32     Addr,
            unsigned long     Data,
            MINT32      userEnum=ISP_DRV_USER_OTHER,
            MINT32      caller=0);
        virtual MBOOL   holdReg(MBOOL En);
        virtual MBOOL   dumpReg(void);
        virtual MBOOL   checkTopReg(MUINT32 Addr) {return IspDrv::checkTopReg(Addr);}
        virtual isp_reg_t*   getCurHWRegValues();
#if defined(_use_kernel_ref_cnt_)
        virtual MBOOL   kRefCntCtrl(ISP_REF_CNT_CTRL_STRUCT* pCtrl);
#endif

        virtual MUINT32 pipeCountInc(EIspDrvPipePath ePipePath);
        virtual MUINT32 pipeCountDec(EIspDrvPipePath ePipePath);
        virtual MBOOL   ISPWakeLockCtrl(MBOOL WakeLockEn);
        virtual MBOOL loadInitSetting(void);
        // debug information
        virtual MBOOL dumpCQTable(ISP_DRV_CQ_ENUM cq,MINT32 burstQIdx=0, MUINT32 dupCqIdx=0, ISP_DRV_CQ_CMD_DESC_STRUCT* cqDesVa=0, MUINT32* cqVirVa=0);
        virtual MBOOL dumpDBGLog(MUINT32* P1,IspDumpDbgLogP2Package* pP2Package);
        //
        virtual MBOOL   rtBufCtrl(void *pBuf_ctrl);
        // tuning
        virtual MBOOL bypassTuningQue(ESoftwareScenario softScenario, MINT32 magicNum);
        virtual MBOOL deTuningQue(ETuningQueAccessPath path, ESoftwareScenario softScenario, MINT32 magicNum);
        virtual MBOOL enTuningQue(ETuningQueAccessPath path, ESoftwareScenario softScenario, MINT32 magicNum, EIspTuningMgrFunc updateFuncBit);
        virtual MBOOL deTuningQueByCq(ETuningQueAccessPath path, ISP_DRV_CQ_ENUM eCq, MINT32 magicNum);
        virtual MBOOL enTuningQueByCq(ETuningQueAccessPath path, ISP_DRV_CQ_ENUM eCq, MINT32 magicNum, EIspTuningMgrFunc updateFuncBit);
        virtual MBOOL setP2TuningStatus(ISP_DRV_CQ_ENUM cq, MBOOL en);
        virtual MBOOL getP2TuningStatus(ISP_DRV_CQ_ENUM cq);
        virtual MUINT64 getTuningUpdateFuncBit(ETuningQueAccessPath ePath, MINT32 magicNum, MINT32 cq,MUINT32 drvScenario);
        virtual MUINT32 getTuningTop(ETuningQueAccessPath ePath, ETuningTopEn top, MINT32 cq, MUINT32 magicNum=0);
        virtual MUINT32* getTuningBuf(ETuningQueAccessPath ePath, MINT32 cq);
        virtual MBOOL   getCqInfoFromScenario(ESoftwareScenario softScenario, ISP_DRV_CQ_ENUM &cq);
        virtual MBOOL   getP2cqInfoFromScenario(ESoftwareScenario softScenario, ISP_DRV_P2_CQ_ENUM &p2Cq);
        virtual MBOOL   getTuningTpipeFiled(ISP_DRV_P2_CQ_ENUM p2Cq, MUINT32* pTuningBuf, stIspTuningTpipeFieldInf &pTuningField);
        virtual MBOOL   mapCqToP2Cq(ISP_DRV_CQ_ENUM cq, ISP_DRV_P2_CQ_ENUM &p2Cq);
        //
        virtual MBOOL getIspCQModuleInfo(CAM_MODULE_ENUM eModule,ISP_DRV_CQ_MODULE_INFO_STRUCT &outInfo) {UNUSED(eModule); UNUSED(outInfo); return MFALSE;}//not support in phy
        virtual MBOOL getCqModuleInf(CAM_MODULE_ENUM moduleId, MUINT32 &addrOffset, MUINT32 &moduleSize);
        //
        // slow motion feature, support burst Queue control
        virtual MBOOL cqTableControl(ISP_DRV_CQTABLE_CTRL_ENUM cmd,int burstQnum);
        virtual MINT32 cqNumInfoControl(ISP_DRV_CQNUMINFO_CTRL_ENUM cmd,int newValue=0);
        //
        // dequeue mechanism in ihalpipewarpper
        virtual MBOOL   enqueP2Frame(unsigned long callerID,MINT32 p2burstQIdx,MINT32 p2dupCQIdx,MINT32 frameNum, MINT32 p2Scenario);
        virtual MBOOL   waitP2Deque();
        virtual MBOOL   dequeP2FrameSuccess(unsigned long callerID,MINT32 p2dupCQIdx, MINT32 p2Scenario);
        virtual MBOOL   dequeP2FrameFail(unsigned long callerID,MINT32 p2dupCQIdx, MINT32 p2Scenario);
        virtual MBOOL   waitP2Frame(unsigned long callerID,MINT32 p2dupCQIdx,MINT32 p2Scenario,MINT32 timeoutUs);
        virtual MBOOL   wakeP2WaitedFrames();
        virtual MBOOL   freeAllP2Frames();
        //update magic number
        virtual MBOOL   updateMagicNumber(MUINT32* value);
        //update/query register Scenario
        virtual MBOOL   updateScenarioValue(MUINT32 value);
        virtual MBOOL   queryScenarioValue(MUINT32& value);
        //
        //
#if defined(_rtbc_use_cq0c_)
        virtual MBOOL cqRingBuf(void *pBuf_ctrl);
#endif
        //temp remove later
         virtual MUINT32* getRegAddr(void);
        virtual isp_reg_t* getRegAddrMap(void);
        virtual MBOOL SetFPS(MUINT32 _fps);
        virtual MBOOL updateCq0bRingBuf(void *pOBval);
        virtual MBOOL getDeviceInfo(E_DEVICE_INFO eCmd, ISP_DRV_PASS1_PATH_ENUM path, MUINT8* pData);  // temp for hwsync use
        virtual MBOOL setDeviceInfo(E_DEVICE_INFO eCmd, ISP_DRV_PASS1_PATH_ENUM path, MUINT8* pData);  // temp for hwsync use
        virtual MBOOL setDapcReg(vector<MUINT32> reg_list);
        virtual MBOOL setDapcRegVal(ISP_DRV_DAPC_REG_STRUCT* DapcReg);
    protected:
        virtual MBOOL dumpP2DebugLog(IspDumpDbgLogP2Package* pP2Package);
        virtual MBOOL dumpP1DebugLog(MUINT32* P1);
    private:
        virtual MBOOL   searchTuningQue(ISP_DRV_P2_CQ_ENUM p2Cq, MINT32 magicNum, MINT32 &getQueIdx);
        virtual MBOOL   orderSearchTuningQue(ISP_DRV_P2_CQ_ENUM p2Cq, MINT32 magicNum, MUINT32 start, MUINT32 end, MINT32 &getQueIdx);
        static MUINT32* getMMapRegAddr(void) {return (mpIspHwRegAddr);}
#if defined(_rtbc_use_cq0c_)
        inline MBOOL _updateEnqCqRingBuf(void *pBuf_ctrl);
#endif
        MBOOL readRegsviaIO(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count);
        MBOOL writeRegsviaIO(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count);
    private:
        volatile MINT32 mInitCount;
        mutable Mutex   mLock;
        mutable Mutex   mFdLock;
        //
        /*imem*/
        IMemDrv*        m_pIMemDrv ;
        IMEM_BUF_INFO   m_ispVirRegBufInfo;
        IMEM_BUF_INFO   m_ispCQDescBufInfo;
        list<const char*> m_userNameList;
        list<ISP_DRV_FD_NODE_STRUCT> m_FdNd;
        //
        // tuning
        mutable android::Mutex   tuningDataLock;  // for tuning top
        mutable android::Mutex   tuningQueueIdxLock; // for tuning queue index control
        //
        //
        static MUINT32* mpIspHwRegAddr;//register map from mmap
        static MUINT32* mpIspHwRegAddr_camsv;//register map from mmap
        static MUINT32* mpIspHwRegAddr_seninf;//register map from mmap        
        //
        MUINT32 mIspOpenRetry;
};
//-----------------------------------------------------------------------------
class IspDrvVirImp : public IspDrv
{
    public:
        IspDrvVirImp();
        virtual ~IspDrvVirImp();
    //
    public:
    static IspDrv*  getInstance(MINT32 cq, MUINT32* ispVirRegAddr,isp_reg_t* ispVirRegMap);
        virtual void    destroyInstance(void);
        virtual MINT32 isp_fd_open(const char* userName=""){/*LOG_ERR("Not support");*/UNUSED(userName); return MTRUE;}
        virtual MINT32 isp_fd_close(MINT32 mIspFd){/*LOG_ERR("Not support");*/UNUSED(mIspFd); return MTRUE;}
        virtual MBOOL   init(const char* userName=""){ /*LOG_ERR("Not support");*/UNUSED(userName); return MTRUE;}
        virtual MBOOL   uninit(const char* userName=""){/*LOG_ERR("Not support");*/UNUSED(userName); return MTRUE;}
        virtual MBOOL   waitIrq(ISP_DRV_WAIT_IRQ_STRUCT* pWaitIrq){/*LOG_ERR("Not support");*/UNUSED(pWaitIrq); return MTRUE;}
        virtual MINT32   registerIrq(const char* userName) {/*LOG_ERR("Not support");*/UNUSED(userName); return MTRUE;}
        virtual MBOOL   markIrq(ISP_DRV_WAIT_IRQ_STRUCT Irqinfo) {/*LOG_ERR("Not support");*/UNUSED(Irqinfo); return MTRUE;}
        virtual MBOOL   flushIrq(ISP_DRV_WAIT_IRQ_STRUCT Irqinfo) {/*LOG_ERR("Not support");*/UNUSED(Irqinfo); return MTRUE;}
        virtual MBOOL   queryirqtimeinfo(ISP_DRV_WAIT_IRQ_STRUCT* Irqinfo) {/*LOG_ERR("Not support");*/UNUSED(Irqinfo); return MTRUE;}
        virtual MBOOL   readIrq(ISP_DRV_READ_IRQ_STRUCT* pReadIrq){/*LOG_ERR("Not support");*/UNUSED(pReadIrq); return MTRUE;}
        virtual MBOOL   checkIrq(ISP_DRV_CHECK_IRQ_STRUCT CheckIrq){/*LOG_ERR("Not support");*/UNUSED(CheckIrq); return MTRUE;}
        virtual MBOOL   clearIrq(ISP_DRV_CLEAR_IRQ_STRUCT ClearIrq){/*LOG_ERR("Not support");*/UNUSED(ClearIrq); return MTRUE;}
    virtual MBOOL   unregisterIrq(ISP_DRV_WAIT_IRQ_STRUCT WaitIrq){/*LOG_ERR("Not support");*/UNUSED(WaitIrq); return MTRUE;}
        virtual MBOOL   reset(MINT32 rstpath);
        virtual MBOOL   resetBuf(void){/*LOG_ERR("Not support");*/return MTRUE;}
    virtual MBOOL checkCQBufAllocated(void){/*LOG_ERR("Not support");*/return MTRUE;}
        virtual MBOOL   readRegs(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count,
            MINT32                  caller=0);
        virtual MUINT32 readReg(MUINT32 Addr,MINT32 caller=0);
        virtual MBOOL   writeRegs(
            ISP_DRV_REG_IO_STRUCT*  pRegIo,
            MUINT32                 Count,
            MINT32                  userEnum=ISP_DRV_USER_OTHER,
            MINT32                  caller=0);
        virtual MBOOL   writeReg(
            MUINT32     Addr,
            unsigned long     Data,
            MINT32      userEnum=ISP_DRV_USER_OTHER,
            MINT32      caller=0);
        virtual MBOOL   holdReg(MBOOL En){/*LOG_ERR("Not support");*/UNUSED(En); return MTRUE;}
        virtual MBOOL   dumpReg(void){/*LOG_ERR("Not support");*/return MTRUE;}
        virtual MBOOL   checkTopReg(MUINT32 Addr) {return IspDrv::checkTopReg(Addr);}
        virtual isp_reg_t* getCurHWRegValues() {return NULL;}
        //
        virtual MBOOL   rtBufCtrl(void *pBuf_ctrl){UNUSED(pBuf_ctrl); return MTRUE;}
        virtual MUINT32 pipeCountInc(EIspDrvPipePath ePipePath){UNUSED(ePipePath); return MTRUE;};
        virtual MUINT32 pipeCountDec(EIspDrvPipePath ePipePath){UNUSED(ePipePath); return MTRUE;};
        //
        virtual MBOOL   ISPWakeLockCtrl(MBOOL WakeLockEn){UNUSED(WakeLockEn); return MTRUE;};
        virtual MBOOL loadInitSetting(void){return MTRUE;};
        // debug information
        virtual MBOOL dumpCQTable(ISP_DRV_CQ_ENUM cq,MINT32 burstQIdx=0,MUINT32 dupCqIdx=0, ISP_DRV_CQ_CMD_DESC_STRUCT* cqDesVa=0, MUINT32* cqVirVa=0)
                                 {UNUSED(cq); UNUSED(burstQIdx); UNUSED(dupCqIdx); UNUSED(cqDesVa); UNUSED(cqVirVa); return MTRUE;};
        virtual MBOOL dumpDBGLog(MUINT32* P1,IspDumpDbgLogP2Package* pP2Package){UNUSED(P1); UNUSED(pP2Package); return MTRUE;};
        //
        // tuning
        virtual MBOOL   bypassTuningQue(ESoftwareScenario softScenario, MINT32 magicNum){UNUSED(softScenario); UNUSED(magicNum); return MFALSE;};
        virtual MBOOL   deTuningQue(ETuningQueAccessPath path, ESoftwareScenario softScenario, MINT32 magicNum)
                                   {UNUSED(path); UNUSED(softScenario); UNUSED(magicNum); return MFALSE;};
        virtual MBOOL   enTuningQue(ETuningQueAccessPath path, ESoftwareScenario softScenario, MINT32 magicNum, EIspTuningMgrFunc updateFuncBit)
                                   {UNUSED(path); UNUSED(softScenario); UNUSED(magicNum); UNUSED(updateFuncBit); return MFALSE;};
        virtual MBOOL   deTuningQueByCq(ETuningQueAccessPath path, ISP_DRV_CQ_ENUM eCq, MINT32 magicNum)
                                       {UNUSED(path); UNUSED(eCq); UNUSED(magicNum); return MFALSE;};
        virtual MBOOL   enTuningQueByCq(ETuningQueAccessPath path, ISP_DRV_CQ_ENUM eCq, MINT32 magicNum, EIspTuningMgrFunc updateFuncBit)
                                       {UNUSED(path); UNUSED(eCq); UNUSED(magicNum); UNUSED(updateFuncBit); return MFALSE;};
        virtual MBOOL setP2TuningStatus(ISP_DRV_CQ_ENUM cq, MBOOL en){UNUSED(cq); UNUSED(en); return MFALSE;};
        virtual MBOOL getP2TuningStatus(ISP_DRV_CQ_ENUM cq){UNUSED(cq); return MFALSE;};
        virtual MUINT64 getTuningUpdateFuncBit(ETuningQueAccessPath ePath, MINT32 magicNum, MINT32 cq,MUINT32 drvScenario)
                                              {UNUSED(ePath); UNUSED(magicNum); UNUSED(cq); UNUSED(drvScenario); return eIspTuningMgrFunc_Null;};
        virtual MUINT32 getTuningTop(ETuningQueAccessPath ePath, ETuningTopEn top, MINT32 cq, MUINT32 magicNum=0)
                                    {UNUSED(ePath); UNUSED(top); UNUSED(cq); UNUSED(magicNum); return -1;};
        virtual MUINT32* getTuningBuf(ETuningQueAccessPath ePath, MINT32 cq){UNUSED(ePath); UNUSED(cq); return NULL;};
        virtual MBOOL   getCqInfoFromScenario(ESoftwareScenario softScenario, ISP_DRV_CQ_ENUM &cq){UNUSED(softScenario); UNUSED(cq); return MFALSE;};
        virtual MBOOL   getP2cqInfoFromScenario(ESoftwareScenario softScenario, ISP_DRV_P2_CQ_ENUM &p2Cq){UNUSED(softScenario); UNUSED(p2Cq); return MFALSE;};
        virtual MBOOL   getTuningTpipeFiled(ISP_DRV_P2_CQ_ENUM p2Cq, MUINT32* pTuningBuf, stIspTuningTpipeFieldInf &pTuningField)
                                           {UNUSED(p2Cq); UNUSED(pTuningBuf); UNUSED(pTuningField); return MFALSE;};
        virtual MBOOL   mapCqToP2Cq(ISP_DRV_CQ_ENUM cq, ISP_DRV_P2_CQ_ENUM &p2Cq){UNUSED(cq); UNUSED(p2Cq); return MFALSE;};
        //
        virtual MBOOL getCqModuleInf(CAM_MODULE_ENUM moduleId, MUINT32 &addrOffset, MUINT32 &moduleSize);
        // slow motion feature, support burst Queue control
        virtual MBOOL cqTableControl(ISP_DRV_CQTABLE_CTRL_ENUM cmd,int burstQnum) {UNUSED(cmd); UNUSED(burstQnum); return MFALSE;};
        virtual MINT32 cqNumInfoControl(ISP_DRV_CQNUMINFO_CTRL_ENUM cmd,int newValue=0) {UNUSED(cmd); UNUSED(newValue); return MFALSE;};
        //
        // dequeue mechanism in ihalpipewarpper
        virtual MBOOL   enqueP2Frame(unsigned long callerID,MINT32 p2burstQIdx,MINT32 p2dupCQIdx,MINT32 frameNum, MINT32 p2Scenario) {
            UNUSED(callerID);UNUSED(p2burstQIdx);UNUSED(p2dupCQIdx);UNUSED(frameNum);UNUSED(p2Scenario);
            return MFALSE;
        };
        virtual MBOOL   waitP2Deque() {return MFALSE;};
        virtual MBOOL   dequeP2FrameSuccess(unsigned long callerID,MINT32 p2dupCQIdx,MINT32 p2Scenario) {
            UNUSED(callerID);UNUSED(p2dupCQIdx);UNUSED(p2Scenario);
            return MFALSE;
        };
        virtual MBOOL   dequeP2FrameFail(unsigned long callerID,MINT32 p2dupCQIdx,MINT32 p2Scenario) {
            UNUSED(callerID);UNUSED(p2dupCQIdx);UNUSED(p2Scenario);
            return MFALSE;
        };
        virtual MBOOL   waitP2Frame(unsigned long callerID,MINT32 p2dupCQIdx,MINT32 p2Scenario,MINT32 timeoutUs) {
            UNUSED(callerID);UNUSED(p2dupCQIdx);UNUSED(timeoutUs);UNUSED(p2Scenario);
            return MFALSE;
        };
        virtual MBOOL   wakeP2WaitedFrames() {return MFALSE;};
        virtual MBOOL   freeAllP2Frames() {return MFALSE;};
        //update magic number
        virtual MBOOL   updateMagicNumber(MUINT32* value) {UNUSED(value); return MFALSE;};
        //
        //update/query register Scenario
        virtual MBOOL   updateScenarioValue(MUINT32 value) {UNUSED(value); return MFALSE;};
        virtual MBOOL   queryScenarioValue(MUINT32& value) {UNUSED(value); return MFALSE;};
        //
        //
        //temp remove later
        virtual isp_reg_t* getRegAddrMap(void){return NULL;}
        //
        virtual MBOOL getIspCQModuleInfo(CAM_MODULE_ENUM eModule,ISP_DRV_CQ_MODULE_INFO_STRUCT &outInfo);
        virtual MBOOL SetFPS(MUINT32 _fps){UNUSED(_fps); return MFALSE;}
        virtual MBOOL updateCq0bRingBuf(void *pOBval){UNUSED(pOBval); return MTRUE;};
        virtual MBOOL getDeviceInfo(E_DEVICE_INFO eCmd, ISP_DRV_PASS1_PATH_ENUM path, MUINT8* pData) {return MTRUE;};  // temp for hwsync use
        virtual MBOOL setDeviceInfo(E_DEVICE_INFO eCmd, ISP_DRV_PASS1_PATH_ENUM path, MUINT8* pData) {return MTRUE;};  // temp for hwsync use
        virtual MBOOL setDapcReg(vector<MUINT32> reg_list){return MFALSE;}
        virtual MBOOL setDapcRegVal(ISP_DRV_DAPC_REG_STRUCT* DapcReg){return MFALSE;}
    protected:
        virtual MBOOL dumpP2DebugLog(IspDumpDbgLogP2Package* pP2Package){UNUSED(pP2Package); return MTRUE;};
        virtual MBOOL dumpP1DebugLog(MUINT32* P1){UNUSED(P1); return MTRUE;};
    public:
        isp_reg_t* mpIspVirRegMap;//for calculating offset in register read/write macro
    private:
        virtual MBOOL   searchTuningQue(ISP_DRV_P2_CQ_ENUM p2Cq, MINT32 magicNum, MINT32 &getQueIdx)
                                       {UNUSED(p2Cq); UNUSED(magicNum); UNUSED(getQueIdx); return MFALSE;};
        virtual MBOOL   orderSearchTuningQue(ISP_DRV_P2_CQ_ENUM p2Cq, MINT32 magicNum, MUINT32 start, MUINT32 end, MINT32 &getQueIdx)
                                            {UNUSED(p2Cq); UNUSED(magicNum); UNUSED(start); UNUSED(end); UNUSED(getQueIdx); return MFALSE;};
        virtual MUINT32* getRegAddr(void);
        mutable Mutex   mLock;
        MUINT32 *  mpIspVirRegBuffer;
};
//-----------------------------------------------------------------------------
#endif

