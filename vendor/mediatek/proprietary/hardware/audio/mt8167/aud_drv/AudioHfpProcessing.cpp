#define LOG_TAG "AudioHfpProcessing"
//#define LOG_NDEBUG 0

#include "AudioHfpProcessing.h"
#include "AudioCustParam.h"
#include "AudioSpeechEnhLayer.h"
#include "AudioUtility.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define EnhanceModeParasExtNum 32
#define EnhanceParasNum  (SPEECH_PARA_NUM + SPEECH_COMMON_NUM + EnhanceModeParasExtNum)  // (16+12+32)

#define DMNRCalDataNum  76
#define CompenFilterNum  270

#define RecBufSize20ms  960
#define RecBufSize10ms  480

#define EPLBufSize 4160
#define MaxVMSize   1922    //960*2 +2 for 48K
#define VMAGC1  3847
#define VMAGC2  3848

#define HFP_PCM_FORMAT_SIZE (2)  // short = 2 bytes
#define HFP_UL1_BUFFER_FRAMESIZE (320)
#define HFP_UL2_BUFFER_FRAMESIZE (320)
#define HFP_DL_BUFFER_FRAMESIZE (320)
#define HFP_DLDELAY_BUFFER_FRAMESIZE (320)

#define HFP_UL1_SAMPLE_OFFSET (0)
#define HFP_UL2_SAMPLE_OFFSET (HFP_UL1_SAMPLE_OFFSET + HFP_UL1_BUFFER_FRAMESIZE)
#define HFP_DL_SAMPLE_OFFSET (HFP_UL2_SAMPLE_OFFSET + HFP_UL2_BUFFER_FRAMESIZE)
#define HFP_DLDELAY_SAMPLE_OFFSET (HFP_DL_SAMPLE_OFFSET + HFP_DL_BUFFER_FRAMESIZE)

#define HFP_EPL_BUFFER_SIZE (4160*2)


static const char *AudioHfpEPLBuffer = "/sdcard/mtklog/audio_dump/AudioHfpEPLBuffer_Dump";

namespace android
{

int AudioHfpProcessing::mDumpFileNum = 0;

AudioHfpProcessing::AudioHfpProcessing()
{
    memset(&mSph_Enh_ctrl, 0, sizeof(SPH_ENH_ctrl_struct));
    mMMI_ctrl_mask = 0xFFFFFFFF;
    mMIC_DG = 0;
    mMMI_MIC_GAIN = 120;
    mDevice_mode = 0;
    mAudioHfpEPLdumpFile = NULL;
    mSphCtrlBuffer = NULL;

    /* dlopen */
    const char *error;
    const char *funName = NULL;

    mAudioCustParamClient = NULL;
    mAudioCustParamClient = AudioCustParamClient::GetInstance();

    mSphEnhOps.handle = dlopen("libspeech_enh_lib.so", RTLD_LAZY);
    if (!mSphEnhOps.handle) {
        ALOGE("%s(), dlopen fail! (%s)\n", __FUNCTION__, dlerror());
        ASSERT(0);
        return;
    }
    dlerror();    /* Clear any existing error */

    /* dlsym */
    funName = "ENH_API_Get_Memory";
    mSphEnhOps.ENH_API_Get_Memory = (Word32(*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }

    funName = "ENH_API_Alloc";
    mSphEnhOps.ENH_API_Alloc = (Word16(*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl, Word32 * mem_ptr)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }

    funName = "ENH_API_Rst";
    mSphEnhOps.ENH_API_Rst = (Word16(*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }

    funName = "ENH_API_Process";
    mSphEnhOps.ENH_API_Process = (void (*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }

    funName = "ENH_API_Free";
    mSphEnhOps.ENH_API_Free = (Word16(*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }

    funName = "ENH_API_Get_Version";
    mSphEnhOps.ENH_API_Get_Version = (Word16(*)(SPH_ENH_ctrl_struct * Sph_Enh_ctrl)) dlsym(mSphEnhOps.handle, funName);
    error = dlerror();
    if (error != NULL) {
        ALOGE("%s(), dlsym %s fail. (%s)\n", __FUNCTION__, funName, error);
        ASSERT(0);
        return;
    }
}

AudioHfpProcessing::~AudioHfpProcessing()
{
    if (mAudioHfpEPLdumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpEPLdumpFile);
        mAudioHfpEPLdumpFile = NULL;
    }

    if (mSphCtrlBuffer) {
        mSphEnhOps.ENH_API_Free(&mSph_Enh_ctrl);
        free(mSphCtrlBuffer);
        mSphCtrlBuffer = NULL;
    }

    if (mSphEnhOps.handle) {
        dlclose(mSphEnhOps.handle);
        mSphEnhOps.handle = NULL;
    }
}

status_t AudioHfpProcessing::InitCheck()
{
    if (mAudioCustParamClient->GetAudioHFPParamFromNV(&mHfpProcessingParam) == 0) {
        ALOGW("%s GetAudioHFPParamFromNV fail", __FUNCTION__);
    }

    return NO_ERROR;
}

status_t AudioHfpProcessing::UpdateParameters()
{
    if (mAudioCustParamClient->GetAudioHFPParamFromNV(&mHfpProcessingParam) == 0) {
            ALOGW("%s GetAudioHFPParamFromNV fail", __FUNCTION__);
    }

#if LOG_NDEBUG == 0
    int i, j;
    for (i = 0; i < SPEECH_COMMON_NUM; i+=6) {
        ALOGE("%s speech_common_para[%d:%d] = %u-%u-%u-%u-%u-%u", __FUNCTION__, i, i+5,
              mHfpProcessingParam.speech_common_para[i], mHfpProcessingParam.speech_common_para[i+1],
              mHfpProcessingParam.speech_common_para[i+2], mHfpProcessingParam.speech_common_para[i+3],
              mHfpProcessingParam.speech_common_para[i+4], mHfpProcessingParam.speech_common_para[i+5]);
    }

    for (i = 0; i < NUM_AUDIO_HFP_DEVICE; i++) {
        for (j = 0; j < SPEECH_PARA_NUM; j+=8) {
            ALOGE("%s speech_mode_para[%d][%d:%d] = %u-%u-%u-%u-%u-%u-%u-%u", __FUNCTION__, i, j, j+7,
                mHfpProcessingParam.speech_mode_para[i][j], mHfpProcessingParam.speech_mode_para[i][j+1],
                mHfpProcessingParam.speech_mode_para[i][j+2], mHfpProcessingParam.speech_mode_para[i][j+3],
                mHfpProcessingParam.speech_mode_para[i][j+4], mHfpProcessingParam.speech_mode_para[i][j+5],
                mHfpProcessingParam.speech_mode_para[i][j+6], mHfpProcessingParam.speech_mode_para[i][j+7]);
        }
    }

    for (i = 0; i < NUM_AUDIO_HFP_DEVICE; i++) {
        for (j = 0; j < WB_FIR_NUM; j+=10) {
            ALOGE("%s in_fir[%d][%d:%d] = %u-%u-%u-%u-%u-%u-%u-%u-%u-%u", __FUNCTION__, i, j, j+9,
                mHfpProcessingParam.in_fir[i][j], mHfpProcessingParam.in_fir[i][j+1],
                mHfpProcessingParam.in_fir[i][j+2], mHfpProcessingParam.in_fir[i][j+3],
                mHfpProcessingParam.in_fir[i][j+4], mHfpProcessingParam.in_fir[i][j+5],
                mHfpProcessingParam.in_fir[i][j+6], mHfpProcessingParam.in_fir[i][j+7],
                mHfpProcessingParam.in_fir[i][j+8], mHfpProcessingParam.in_fir[i][j+9]);
        }
        for (j = 0; j < WB_FIR_NUM; j+=10) {
            ALOGE("%s out_fir[%d][%d:%d] = %u-%u-%u-%u-%u-%u-%u-%u-%u-%u", __FUNCTION__, i, j, j+9,
                mHfpProcessingParam.out_fir[i][j], mHfpProcessingParam.out_fir[i][j+1],
                mHfpProcessingParam.out_fir[i][j+2], mHfpProcessingParam.out_fir[i][j+3],
                mHfpProcessingParam.out_fir[i][j+4], mHfpProcessingParam.out_fir[i][j+5],
                mHfpProcessingParam.out_fir[i][j+6], mHfpProcessingParam.out_fir[i][j+7],
                mHfpProcessingParam.out_fir[i][j+8], mHfpProcessingParam.out_fir[i][j+9]);
        }
    }
#endif

    return NO_ERROR;
}

status_t AudioHfpProcessing::Set_Processing_CtrlStruct(void *pdata, uint32_t datasize)
{
    if (datasize != sizeof(AUDIO_HFP_PARAM_STRUCT))
        return BAD_VALUE;

    memcpy((void *)&mSph_Enh_ctrl, pdata, sizeof(AUDIO_HFP_PARAM_STRUCT));
    return NO_ERROR;
}

status_t AudioHfpProcessing::Get_Processing_CtrlStruct(void *pdata, uint32_t datasize)
{
    if (datasize != sizeof(AUDIO_HFP_PARAM_STRUCT))
        return BAD_VALUE;

    memcpy(pdata, (void*)&mSph_Enh_ctrl, sizeof(AUDIO_HFP_PARAM_STRUCT));
    return NO_ERROR;
}

//parameters setting
bool AudioHfpProcessing::SetEnhPara(int mode, unsigned long *pEnhance_pars)
{
    (void)mode;
    memcpy(&mSph_Enh_ctrl.enhance_pars, pEnhance_pars, EnhanceParasNum * sizeof(unsigned long));
    return true;
}

bool AudioHfpProcessing::SetAPPTable(int mode, unsigned int App_table)
{
    (void)mode;
    mSph_Enh_ctrl.App_table = App_table;
    return true;
}

bool AudioHfpProcessing::SetFeaCfgTable(int mode, long Fea_Cfg_table)
{
    (void)mode;
    mSph_Enh_ctrl.Fea_Cfg_table = Fea_Cfg_table;
    return true;
}

bool AudioHfpProcessing::SetMICDigitalGain(int mode, long gain)
{
    ALOGD("%s mode = %d gain = %d", __FUNCTION__, mode, gain);
    mMIC_DG = gain;
    return true;
}

bool AudioHfpProcessing::SetSampleRate(int mode, long sample_rate)
{
    (void)mode;
    mSph_Enh_ctrl.sample_rate = sample_rate;
    return true;
}

bool AudioHfpProcessing::SetFrameRate(int mode, long frame_rate)
{
    (void)mode;
    (void)frame_rate;
    mSph_Enh_ctrl.frame_rate = 20;
    return true;
}

long AudioHfpProcessing::GetFrameRate()
{
    return mSph_Enh_ctrl.frame_rate;
}

long AudioHfpProcessing::GetFrameSize()
{
    return HFP_UL1_BUFFER_FRAMESIZE;
}

long AudioHfpProcessing::GetPcmFormatBytes()
{
    return HFP_PCM_FORMAT_SIZE;
}

//set MMI table, dynamic on/off
bool AudioHfpProcessing::SetDynamicFuncCtrl(const unsigned int func, const bool enable)
{
    const bool current_state = ((mMMI_ctrl_mask & func) > 0);
    ALOGD("%s %x(%x) enable(%d) current_state(%d)", __FUNCTION__, mMMI_ctrl_mask, func, enable, current_state);

    if (enable == false) {
        mMMI_ctrl_mask &= (~func);
    }
    else {
        mMMI_ctrl_mask |= func;
        //normal/handsfree mode DMNR are exclusive
        if (func == HANDSFREE_DMNR) {
            mMMI_ctrl_mask &= (~NORMAL_DMNR);
        }
        else if (func == NORMAL_DMNR)
        {
            mMMI_ctrl_mask &= (~HANDSFREE_DMNR);
        }
    }
    mSph_Enh_ctrl.MMI_ctrl = mMMI_ctrl_mask;
    ALOGD("%s mMMI_ctrl_mask = %x", __FUNCTION__, mMMI_ctrl_mask);
    return true;
}

bool AudioHfpProcessing::SetDMNRPara(int mode, short *pDMNR_cal_data)
{
    (void)mode;
    memcpy(&mSph_Enh_ctrl.DMNR_cal_data, pDMNR_cal_data, DMNRCalDataNum * sizeof(short));
    return true;
}

bool AudioHfpProcessing::SetCompFilter(int mode, short *pCompen_filter)
{
    (void)mode;
    memcpy(&mSph_Enh_ctrl.Compen_filter, pCompen_filter, CompenFilterNum * sizeof(short));
    return true;
}

bool AudioHfpProcessing::SetDeviceMode(int mode, uint8_t devicemode)
{
    ALOGD("%s mode = %d devicemode = %d", __FUNCTION__, mode, devicemode);
    mDevice_mode = devicemode;
    return true;
}

unsigned int AudioHfpProcessing::PutUl1Data(void* buffer , unsigned int size)
{
    short *UL1Data = mSph_Enh_ctrl.PCM_buffer;
    memcpy(UL1Data, buffer, (HFP_UL1_BUFFER_FRAMESIZE*HFP_PCM_FORMAT_SIZE));
    return size;
}

unsigned int AudioHfpProcessing::PutUl2Data(void* buffer , unsigned int size)
{
    short *UL2Data = (mSph_Enh_ctrl.PCM_buffer + HFP_UL2_SAMPLE_OFFSET);
    memcpy(UL2Data, buffer, (HFP_UL2_BUFFER_FRAMESIZE*HFP_PCM_FORMAT_SIZE));
    return size;
}

unsigned int AudioHfpProcessing::PutDLData(void* buffer , unsigned int size)
{
    short *DLData = (mSph_Enh_ctrl.PCM_buffer + HFP_DL_SAMPLE_OFFSET);
    memcpy(DLData, buffer, (HFP_DL_BUFFER_FRAMESIZE*HFP_PCM_FORMAT_SIZE));
    return size;
}

unsigned int AudioHfpProcessing::PutDL_DelayData(void* buffer , unsigned int size)
{
    short *DLDelayData = (mSph_Enh_ctrl.PCM_buffer + HFP_DLDELAY_SAMPLE_OFFSET);
    memcpy(DLDelayData, buffer, (HFP_DLDELAY_BUFFER_FRAMESIZE*HFP_PCM_FORMAT_SIZE));
    return size;
}

short* AudioHfpProcessing::GutUl1Data(unsigned int *framecount)
{
    short *UL1Data = mSph_Enh_ctrl.PCM_buffer;
    *framecount = HFP_UL1_BUFFER_FRAMESIZE;
    return UL1Data;
}

short* AudioHfpProcessing::GutUl2Data(unsigned int *framecount)
{
    short *UL2Data = (mSph_Enh_ctrl.PCM_buffer + HFP_UL2_SAMPLE_OFFSET);
    *framecount = HFP_UL2_BUFFER_FRAMESIZE;
    return UL2Data;
}

short* AudioHfpProcessing::GutDLData(unsigned int *framecount)
{
    short *DLData = (mSph_Enh_ctrl.PCM_buffer + HFP_DL_SAMPLE_OFFSET);
    *framecount = HFP_DL_BUFFER_FRAMESIZE;
    return DLData;
}

bool AudioHfpProcessing::SetUpLinkTotalGain(int mode, uint8_t gain)
{
    ALOGD("%s mode = %d gain = %d", __FUNCTION__, mode, gain);
    mMMI_MIC_GAIN = gain;
    return true;
}

bool AudioHfpProcessing::ProCessing()
{
    //process the fill in buffer
    ALOGV("%s", __FUNCTION__);
    mSphEnhOps.ENH_API_Process(&mSph_Enh_ctrl);
    Dump_EPL();
    return NO_ERROR;
}

bool AudioHfpProcessing::Start()
{
    ALOGD("%s",__FUNCTION__);

    int index = 0;
    unsigned long HDRecordEnhanceParas[EnhanceParasNum] = {0};
    short HDRecordCompenFilter[CompenFilterNum] = {0};
    short DMNRParam[DMNRCalDataNum] = {0};
    unsigned int hfpDevice;

    if (mDevice_mode == ROUTE_HEADSET) {
        hfpDevice = AUDIO_HFP_DEVICE_HEADSET;
    } else if (mDevice_mode == ROUTE_NORMAL) {
        hfpDevice = AUDIO_HFP_DEVICE_NORMAL;
    } else {
        hfpDevice = AUDIO_HFP_DEVICE_SPEAKER;
    }

    // parameters
    for (index = 0; index < SPEECH_PARA_NUM; index++)
    {
        // HFP parameters
        HDRecordEnhanceParas[index] = mHfpProcessingParam.speech_mode_para[hfpDevice][index];
    }
    for (index = SPEECH_PARA_NUM; index < SPEECH_PARA_NUM + EnhanceModeParasExtNum; index++)
    {
        // Fill with 0 for parameter extension
        HDRecordEnhanceParas[index] = 0;
    }
    for (index = SPEECH_PARA_NUM + EnhanceModeParasExtNum; index < EnhanceParasNum; index++)
    {
        // Common parameters
        HDRecordEnhanceParas[index] = mHfpProcessingParam.speech_common_para[index - (SPEECH_PARA_NUM + EnhanceModeParasExtNum)];
    }

    SetEnhPara(0, HDRecordEnhanceParas);

    // set FIR filter
    memcpy(&HDRecordCompenFilter[0], &mHfpProcessingParam.in_fir[hfpDevice][0], sizeof(short)*WB_FIR_NUM);
    memcpy(&HDRecordCompenFilter[0+WB_FIR_NUM], &mHfpProcessingParam.in_fir[hfpDevice][0], sizeof(short)*WB_FIR_NUM);
    memcpy(&HDRecordCompenFilter[0+WB_FIR_NUM*2], &mHfpProcessingParam.out_fir[hfpDevice][0], sizeof(short)*WB_FIR_NUM);
    SetCompFilter(0, HDRecordCompenFilter);

    mSph_Enh_ctrl.frame_rate = 20;
    mSph_Enh_ctrl.sample_rate = 16000;
    mSph_Enh_ctrl.App_table = 2; // voip mode
    mSph_Enh_ctrl.MIC_DG = mMIC_DG;
    mSph_Enh_ctrl.MMI_MIC_GAIN = mMMI_MIC_GAIN;
    mSph_Enh_ctrl.Fea_Cfg_table = 511;
    mSph_Enh_ctrl.Device_mode = mDevice_mode;
    mSph_Enh_ctrl.RCV_DG = 0xE3D;
    mSph_Enh_ctrl.Near_end_vad = 0;
    mSph_Enh_ctrl.SCH_mem = NULL;
    mSph_Enh_ctrl.MMI_ctrl = mMMI_ctrl_mask;

    // set DMNR disable
    memset(DMNRParam, 0, sizeof(DMNRParam));
    SetDMNRPara(0, DMNRParam);
    SetDynamicFuncCtrl(NORMAL_DMNR, false);
    SetDynamicFuncCtrl(HANDSFREE_DMNR, false);

    uint32_t enhMemSize = mSphEnhOps.ENH_API_Get_Memory(&mSph_Enh_ctrl);
    //uint32_t enhMemSize = 1024; //for build pass by zj
    mSphCtrlBuffer = (int *)malloc(enhMemSize);
    if (mSphCtrlBuffer == NULL) {
        ALOGE("%s malloc fail", __FUNCTION__);
        return false;
    }
    memset(mSphCtrlBuffer, 0, enhMemSize);
    mSphEnhOps.ENH_API_Alloc(&mSph_Enh_ctrl, (Word32 *)mSphCtrlBuffer);
    mSphEnhOps.ENH_API_Rst(&mSph_Enh_ctrl);

    if (mAudioHfpEPLdumpFile == NULL)
    {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s%d.pcm", AudioHfpEPLBuffer, mDumpFileNum);
        mAudioHfpEPLdumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamin_epl_propty);
        if (mAudioHfpEPLdumpFile)
            ALOGD("%s dumpFileName = %s", __FUNCTION__, dumpFileName.string());
    }
    mDumpFileNum++;

    return true;
}

bool AudioHfpProcessing::Stop()
{
    ALOGD("%s",__FUNCTION__);

    if (mSphCtrlBuffer)
    {
        mSphEnhOps.ENH_API_Free(&mSph_Enh_ctrl);
        free(mSphCtrlBuffer);
        mSphCtrlBuffer = NULL;
    }

    if (mAudioHfpEPLdumpFile)
    {
        AudioCloseDumpPCMFile(mAudioHfpEPLdumpFile);
        mAudioHfpEPLdumpFile = NULL;
    }
    return true;
}

bool AudioHfpProcessing::Reset()
{
    ALOGD("%s",__FUNCTION__);
    mSphEnhOps.ENH_API_Rst(&mSph_Enh_ctrl);
    return true;
}

void AudioHfpProcessing::Dump_EPL()
{
    if (mAudioHfpEPLdumpFile)
    {
        ALOGV("%s size = %d sampleRate = %d MMI_MIC_GAIN = %d", __FUNCTION__, HFP_EPL_BUFFER_SIZE,
              mSph_Enh_ctrl.sample_rate, mSph_Enh_ctrl.MMI_MIC_GAIN);
        AudioDumpPCMData((void *)mSph_Enh_ctrl.EPL_buffer, HFP_EPL_BUFFER_SIZE, mAudioHfpEPLdumpFile);
    }
}

}
