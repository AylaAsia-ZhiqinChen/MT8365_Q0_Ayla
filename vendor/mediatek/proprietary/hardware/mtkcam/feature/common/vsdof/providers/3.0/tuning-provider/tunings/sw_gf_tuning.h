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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef SW_GF_TUNING_H_
#define SW_GF_TUNING_H_

#include "../stereo_tuning_base.h"
#include <stereo_tuning_provider.h>

struct SW_GF_TUNING_T
{
    MUINT32 coreNumber;
    GF_TUNING_T clearTable;
    GF_TUNING_T dispCtrlPoints;
    GF_TUNING_T blurGainTable;
    TUNING_PAIR_LIST_T params;
    vector<MUINT32> dofTable;
};

typedef map<int, SW_GF_TUNING_T> GF_MAP_T; //Scenario -> tuning

class SW_GFTuning : public StereoTuningBase
{
public:
    SW_GFTuning(Value &document);
    virtual ~SW_GFTuning();

    virtual const char *getTuningKey() { return STEREO_TUNING_NAME[E_TUNING_SW_GF]; }

    virtual bool retrieveTuningParams(TuningQuery_T &query);
    virtual void log(FastLogger &logger, bool inJSON=false);

protected:
    virtual void _initDefaultValues();
    virtual void _initDocument();
    virtual void _loadValuesFromDocument();

private:
    MUINT32 __getDofValue(MUINT32 DofLevel, vector<MUINT32> &table);

private:
    GF_MAP_T __gfMap;
    vector<std::string> __clearTableNames;
    vector<std::string> __dofTableNames;
    vector<std::string> __dispCtrlPointsNames;
    vector<std::string> __blurGainTableNames;

    const QUERY_INT_LIST_T __DEFAULT_CLEAR_TABLE = {3,3,3,4,5,6,7,8,9,10,12,13,16,19,24,30,39,54,80,135,153,177,208,249,255};
    const QUERY_INT_LIST_T __DEFAULT_DISP_CTRL_POINTS = {6,10,13,15,16,18,20,21,22,24,26,28,30,33,36,40,45,51,60,72,75,78,82,85,90};
    const QUERY_INT_LIST_T __DEFAULT_BLUR_GAIN_TABLE = {2,32,32,32,31,31,31,31,31,31,31,30,30,30,29,28,27,25,22,15,13,10,6,1,1};
};

#endif