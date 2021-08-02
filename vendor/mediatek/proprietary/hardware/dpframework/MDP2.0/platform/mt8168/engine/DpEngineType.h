#ifndef __DP_ENGINE_TYPE_H__
#define __DP_ENGINE_TYPE_H__

#include "DpConfig.h"
#include "DpDataType.h"

#include "cmdq_v3_def.h"
#include "mdp_def_ex.h"

#define DP_GET_ENGINE_NAME_CASE(_value, _name)          \
    case (_value):                                      \
    _name = (char*)#_value;                                    \
    break;
#define DP_GET_ENGINE_NAME(value, name)                 \
{                                                       \
    switch(value)                                       \
    {                                                   \
        DP_GET_ENGINE_NAME_CASE(tIMGI, name)            \
        DP_GET_ENGINE_NAME_CASE(tIMGO, name)            \
        DP_GET_ENGINE_NAME_CASE(tIMG2O, name)           \
        DP_GET_ENGINE_NAME_CASE(tCAMIN, name)           \
        DP_GET_ENGINE_NAME_CASE(tRDMA0, name)           \
        DP_GET_ENGINE_NAME_CASE(tCCORR0, name)          \
        DP_GET_ENGINE_NAME_CASE(tSCL0, name)            \
        DP_GET_ENGINE_NAME_CASE(tSCL1, name)            \
        DP_GET_ENGINE_NAME_CASE(tTDSHP0, name)          \
        DP_GET_ENGINE_NAME_CASE(tCOLOR0, name)          \
        DP_GET_ENGINE_NAME_CASE(tWROT0, name)           \
        DP_GET_ENGINE_NAME_CASE(tJPEGENC, name)         \
        DP_GET_ENGINE_NAME_CASE(tVENC, name)            \
        DP_GET_ENGINE_NAME_CASE(tJPEGDEC, name)         \
        DP_GET_ENGINE_NAME_CASE(tRDMA1, name)           \
        DP_GET_ENGINE_NAME_CASE(tWROT1, name)           \
        default:                                        \
        name = (char*)"tNone";                          \
        assert(0);                                      \
        break;                                          \
    }                                                   \
}

// Invalid engine type
#define tNone       -1
//CAM
#define tWPEI         tNone
#define tWPEO         tNone
#define tWPEI2        tNone
#define tWPEO2        tNone
#define tIMGI         CMDQ_ENG_ISP_IMGI    /* 0 */
#define tIMGO         CMDQ_ENG_ISP_IMGO    /* 1 */
#define tIMG2O        CMDQ_ENG_ISP_IMG2O   /* 2 */
//IPU
#define tIPUI         tNone
#define tIPUO         tNone
//MDP
#define tCAMIN        CMDQ_ENG_MDP_CAMIN   /* 3 */
#define tCAMIN2       tNone
#define tRDMA0        CMDQ_ENG_MDP_RDMA0   /* 4 */
#define tRDMA1        CMDQ_ENG_MDP_RDMA1   /* 44 */
#define tAAL0         tNone
#define tCCORR0       CMDQ_ENG_MDP_CCORR0  /* 5 */
#define tSCL0         CMDQ_ENG_MDP_RSZ0    /* 6 */
#define tSCL1         CMDQ_ENG_MDP_RSZ1    /* 7 */
#define tSCL2         tNone
#define tTDSHP0       CMDQ_ENG_MDP_TDSHP0  /* 8 */
#define tCOLOR0       CMDQ_ENG_MDP_COLOR0  /* 9 */
#define tWROT0        CMDQ_ENG_MDP_WROT0   /* 10 */
#define tWROT1        CMDQ_ENG_MDP_WROT1   /* 46 */
#define tWDMA         tNone
#define tPATH0_SOUT   tNone
#define tPATH1_SOUT   tNone
//DISP
#define tTO_WROT      tNone
#define tTO_WROT_SOUT tNone
#define tTO_DISP0     tNone
#define tTO_DISP1     tNone
#define tTO_WDMA      tNone
#define tTO_RSZ       tNone
//JPEG
#define tJPEGENC      CMDQ_ENG_JPEG_ENC    /* 12 */
#define tVENC         CMDQ_ENG_VIDEO_ENC   /* 13 */
#define tTotal        (tWROT1+1)
#define tJPEGDEC      CMDQ_ENG_JPEG_DEC    /* 14 */
#define tJPEGREMDC    CMDQ_ENG_JPEG_REMDC  /* 15 */
//PQ
#define tCOLOR_EX     CMDQ_ENG_DISP_COLOR0 /* 18 */
#define tOVL0_EX      CMDQ_ENG_DISP_OVL0   /* 23 */
#define tWDMA_EX      CMDQ_ENG_DISP_WDMA0  /* 21 */

#endif  // __DP_ENGINE_TYPE_H__
