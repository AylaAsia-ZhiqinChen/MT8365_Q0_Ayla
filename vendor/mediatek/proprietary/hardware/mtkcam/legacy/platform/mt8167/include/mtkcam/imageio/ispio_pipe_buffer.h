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

/**
* @file ispio_pipe_buffer.h
*
* ispio_pipe_buffer Header File
*/

#ifndef _ISPIO_PIPE_BUFFER_H_
#define _ISPIO_PIPE_BUFFER_H_

#include <mtkcam/imageio/ispio_stddef.h>
#include <sys/time.h>
#include <vector>
using namespace std;

namespace NSImageio
{
    namespace NSIspio
    {
        /**
             * @brief  Pipe Buffer Info
             */
        struct QBufInfo
        {
        public:

            MUINT32 u4User; ///< user-specific data. Cannot be modified by pipes
            MUINT32 u4Reserved; ///< reserved data. Cannot be modified by pipes
            MUINT32 u4BufIndex; ///< buffer index

            /**
                   *@var vBufInfo
                   * Vector of buffer information
                   * Note : The vector size depends on the image format. For example, the vector  must contain 3 buffer information for yuv420 3-plane_data
                   */
            vector<BufInfo> vBufInfo;

        public:

            /**
                   * @brief  Constructors
                   */
            QBufInfo(MUINT32 const _u4User = 0)
                : u4User(0)
                , u4Reserved(0)
                , u4BufIndex(0)
                , vBufInfo()
            {
                _u4User;
            }
        };

        /**
             * @brief  Pipe Buffer Info with Timestamp
             */
        struct QTimeStampBufInfo : public QBufInfo
        {
        public:
            MINT32 i4TimeStamp_sec; ///< time stamp in seconds.
            MINT32 i4TimeStamp_us;  ///<  time stamp in microseconds
            MUINT32 u4BufIndex;     ///< buffer index

        public:

            /**
                   * @brief  Constructor
                   */
            QTimeStampBufInfo(MUINT32 const _u4User = 0)
                : QBufInfo(_u4User)
                , i4TimeStamp_sec(0)
                , i4TimeStamp_us(0)
            {
                u4BufIndex = 0;
            }

        public:

            /**
                   * @brief  Get time stamp in ns
                   */
            inline MINT64   getTimeStamp_ns() const
            {
                return  i4TimeStamp_sec * 1000000000LL + i4TimeStamp_us * 1000LL;
            }

            /**
                   * @brief  Set time stamp
                   */
            inline MBOOL    setTimeStamp()
            {
                struct timeval tv;
                if  ( 0 == ::gettimeofday(&tv, NULL) )
                {
                    i4TimeStamp_sec = tv.tv_sec;
                    i4TimeStamp_us  = tv.tv_usec;
                    return  MTRUE;
                }
                return  MFALSE;
            }
        };


        /**
             * @brief  Memory info
             */
        struct MemInfo
        {
            MUINTPTR virtAddr;   ///< virtual address
            MUINTPTR phyAddr;    ///< physical address
            MUINT32 bufCnt;     ///< buffer count
            MUINT32 bufSize;    ///< buffer size

            /**
                   * @brief  Constructor
                   */
            MemInfo(MUINT32 const _virtAddr = 0,
                      MUINT32 const _phyAddr = 0,
                      MUINT32 const _bufCnt = 0,
                      MUINT32 const _bufSize = 0
                     )
                      : virtAddr(_virtAddr), phyAddr(_phyAddr),
                        bufCnt(_bufCnt), bufSize(_bufSize)
            {}
        };
    };  //namespace NSIspio
};  //namespace NSImageio

/*******************************************************************************
*
********************************************************************************/
namespace NSImageio_FrmB {
namespace NSIspio_FrmB   {
////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
* Pipe Buffer Info
********************************************************************************/
struct QBufInfo
{
public: ////    fields.
    /*
     * user-specific data. Cannot be modified by pipes.
     */
    MUINT32             u4User;
    /*
     * reserved data. Cannot be modified by pipes.
     */
    MUINT32             u4Reserved;
    //
    MUINT32             u4BufIndex;
    /*
     * vector of buffer information.
     * Note:
     *      The vector size depends on the image format. For example, the vector
     *      must contain 3 buffer information for yuv420 3-plane.
     */
    vector<BufInfo>     vBufInfo;
    //
public:     //// constructors.
    QBufInfo(MUINT32 const _u4User = 0)
        : u4User(0)
        , u4Reserved(0)
        , vBufInfo()
    {
        _u4User;
        u4BufIndex = 0;
    }
    //
};


/*******************************************************************************
* Pipe Buffer Info with Timestamp
********************************************************************************/
struct QTimeStampBufInfo : public QBufInfo
{
public: ////    fields.
    MINT32              i4TimeStamp_sec;//  time stamp in seconds.
    MINT32              i4TimeStamp_us; //  time stamp in microseconds
    //
    MUINT32             u4BufIndex; //buffer index
    //
public:     //// constructors.
    QTimeStampBufInfo(MUINT32 const _u4User = 0)
        : QBufInfo(_u4User)
        , i4TimeStamp_sec(0)
        , i4TimeStamp_us(0)
    {
        u4BufIndex = 0;
    }
    //
public: ////    operations.
    inline MINT64   getTimeStamp_ns() const
    {
        return  i4TimeStamp_sec * 1000000000LL + i4TimeStamp_us * 1000LL;
    }
    //
    inline MBOOL    setTimeStamp()
    {
        struct timeval tv;
        if  ( 0 == ::gettimeofday(&tv, NULL) )
        {
            i4TimeStamp_sec = tv.tv_sec;
            i4TimeStamp_us  = tv.tv_usec;
            return  MTRUE;
        }
        return  MFALSE;
    }
    //
};

//
//
struct MemInfo {
    MUINTPTR virtAddr;
    MUINTPTR phyAddr;
    MUINT32 bufCnt;
    MUINT32 bufSize;
    //
    MemInfo(MUINTPTR const _virtAddr = 0,
            MUINTPTR const _phyAddr = 0,
              MUINT32 const _bufCnt = 0,
              MUINT32 const _bufSize = 0
             )
              : virtAddr(_virtAddr), phyAddr(_phyAddr),
                bufCnt(_bufCnt), bufSize(_bufSize)
    {}
};


////////////////////////////////////////////////////////////////////////////////
};  //namespace NSIspio_FrmB
};  //namespace NSImageio_FrmB

#endif  //  _ISPIO_PIPE_BUFFER_H_

