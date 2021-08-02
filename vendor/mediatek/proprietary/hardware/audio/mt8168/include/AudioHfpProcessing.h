#ifndef _AUDIO_HPF_PROCESSING_H
#define _AUDIO_HPF_PROCESSING_H

#include "AudioHfpProcessingInterface.h"
#include <sys/types.h>
#include <stdio.h>
#include "AudioCustParamClient.h"

extern "C" {
#include "enh_api.h"
}
#include "CFG_AUDIO_File.h"

namespace android
{

typedef struct _Sph_Enh_Ops {
    void    *handle;
    Word32(*ENH_API_Get_Memory)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl);
    Word16(*ENH_API_Alloc)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl, Word32 *mem_ptr);
    Word16(*ENH_API_Rst)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl);
    void (*ENH_API_Process)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl);
    Word16(*ENH_API_Free)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl);
    Word16(*ENH_API_Get_Version)(SPH_ENH_ctrl_struct *Sph_Enh_ctrl);
} Sph_Enh_Ops;

class AudioHfpProcessing : public AudioHfpProcessingInterface
{
    public:

        /**
         * a function for AudioHfpProcessing constructor
         */
        AudioHfpProcessing();

        /**
         * a function for ~AudioHfpProcessing destructor
         */
        virtual ~AudioHfpProcessing();

        /**
        * a destuctor for AudioAnalogControl, do nothing.
        * @return status_t
        */
        virtual status_t InitCheck() ;

        /**
        * a function of set control size of postprocessing
        * @param pdata
        * @param dataSize
        * @return status_t
        */
        status_t Set_Processing_CtrlStruct(void *pdata, uint32_t datasize);

        status_t Get_Processing_CtrlStruct(void *pdata, uint32_t datasize);

        void  Use_Default_CtrlStruct();

        status_t UpdateParameters();

        //parameters setting
        bool SetEnhPara(int mode, unsigned long *pEnhance_pars);

        bool SetAPPTable(int mode, unsigned int App_table);

        bool SetFeaCfgTable(int mode, long Fea_Cfg_table);

        bool SetMICDigitalGain(int mode, long gain);

        bool SetSampleRate(int mode, long sample_rate);

        bool SetFrameRate(int mode, long frame_rate);

        long GetFrameRate(void);

        long GetFrameSize(void);

        long GetPcmFormatBytes(void);

        //set MMI table, dynamic on/off
        bool SetDynamicFuncCtrl(const unsigned int func, const bool enable);

        bool SetDMNRPara(int mode, short *pDMNR_cal_data);

        bool SetCompFilter(int mode, short *pCompen_filter);

        bool SetDeviceMode(int mode, uint8_t devicemode);

        unsigned int PutUl1Data(void *buffer , unsigned int size);

        unsigned int PutUl2Data(void *buffer , unsigned int size);

        unsigned int PutDLData(void *buffer , unsigned int size);

        unsigned int PutDL_DelayData(void *buffer , unsigned int size);

        short* GutUl1Data(unsigned int *framecount);

        short* GutUl2Data(unsigned int *framecount);

        short* GutDLData(unsigned int *framecount);

        bool SetUpLinkTotalGain(int mode, uint8_t gain);

        bool ProCessing();

        bool Start();

        bool Stop();

        bool Reset();

    private:
        void Dump_EPL();

        // data structure with aec
        Sph_Enh_Ops mSphEnhOps;
        SPH_ENH_ctrl_struct mSph_Enh_ctrl;
        int *mSphCtrlBuffer;
        Word32 mMMI_ctrl_mask;
        Word32 mMIC_DG;
        Word32 mMMI_MIC_GAIN;
        Word32 mDevice_mode;

        AUDIO_HFP_PARAM_STRUCT mHfpProcessingParam;
        AudioCustParamClient *mAudioCustParamClient;

        // for EPL dumpAllowed
        static int mDumpFileNum;
        FILE *mAudioHfpEPLdumpFile;
};

}

#endif
