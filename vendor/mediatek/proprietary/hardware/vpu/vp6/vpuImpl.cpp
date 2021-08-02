/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
#include <sys/types.h>
#include "vpuImpl.h"
#include "vpuCommon.h"
#include "vpu_buf.h"

// for EARA supportw
#include "earaUtil.h"

static int32_t gIonDevFD = -1;
#define DEFAULT_CALLER_NAME "defaultcaller"
//#define VPU_DUMP_SETT_INFO

//CHRISTODO, only for debug work around
static int work_around_index=0;
//#define VPU_FIX_M4U_MVA
/******************************************************************************
 *
 ******************************************************************************/
VpuAlgoImp::
VpuAlgoImp()
{
    mAvaiableRequest.clear();
    mInUseRequest.clear();
    memset(&mNative, 0, sizeof(vpu_algo_t));
    memset(mPorts, 0, sizeof(mPorts));
    mSize = 0;
    mNative.info_ptr = (uint64_t) malloc(1024);
    mNative.info_length = 1024;
    mbPortBuild = false;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuAlgoImp::
getProperty(
    const char *keyName,
    VpuProperty &prop
)
{
	MY_LOGW("3.0 not support this function");
    return false;
}
/******************************************************************************
 *
 ******************************************************************************/
VpuPort *
VpuAlgoImp::
getPort(
    const char *name
)
{
	MY_LOGW("3.0 not support this function");
    return NULL;
}
/******************************************************************************
 *
 ******************************************************************************/
VpuAlgoImp::
~VpuAlgoImp()
{
    vpuRequestList_t::iterator iter;
    MY_LOGV("delete algo: 0x%lx, algo name %s, id %d", (unsigned long)this, mNative.name, mNative.id);
    if (mNative.info_ptr != 0)
    {
        free((void *) mNative.info_ptr);
    }
    MY_LOGV("mAvaiableRequest: size %d, mInUseRequest: size %d", (int)mAvaiableRequest.size(), (int)mInUseRequest.size());
    for(iter = mAvaiableRequest.begin(); iter != mAvaiableRequest.end(); iter++)
        delete *iter;

    for(iter = mInUseRequest.begin(); iter != mInUseRequest.end(); iter++)
        delete *iter;

    mAvaiableRequest.clear();
    mInUseRequest.clear();
}
/******************************************************************************
 *
 ******************************************************************************/
VpuRequest *
VpuAlgoImp::
acquire()
{
	MY_LOGV("acquire +");
    std::unique_lock <std::mutex> l(mRequestMutex);
    VpuRequestImp *req;
    if (!mAvaiableRequest.empty())
    {
        vpuRequestList_t::iterator iter = mAvaiableRequest.begin();
        mInUseRequest.push_back(*iter);
        req = *iter;
        mAvaiableRequest.erase(iter);
        req->reset();
		MY_LOGV("acquire reset");
    }
    else
    {
        req = new VpuRequestImp(this);
        mInUseRequest.push_back(req);
    }
	MY_LOGV("acquire -");
    return req;
}
/******************************************************************************
 *
 ******************************************************************************/
void
VpuAlgoImp::
release(VpuRequest *request)
{
    std::unique_lock <std::mutex> l(mRequestMutex);
    mInUseRequest.remove((VpuRequestImp *)request);
    mAvaiableRequest.push_back((VpuRequestImp *)request);
}
/******************************************************************************
 *
 ******************************************************************************/
VpuRequestImp::
VpuRequestImp(VpuAlgoImp *algo)
    : mAlgo(algo),
      mBufferCount(0)
{
	memset(mRequestBuffer, 0, sizeof(mRequestBuffer));
	mSetting.share_fd= -1;
	mSetting.vMemBuf = NULL;
	mSetting.mva = 0x0;
	mSetting.va = 0x0;
	mSetting.size = 0;

	mNative.sett_ptr = 0;
	mNative.sett_length = 0;
	mNative.frame_magic = 0;
	mNative.buffer_count = 0;
	mNative.status  = VPU_REQ_STATUS_INVALID;
	mNative.priv    = 0;
	memset(mNative.buffers, 0, sizeof(mNative.buffers));
	mNative.user_id = NULL;
	mNative.request_id = (unsigned long)(&mNative);
	mNative.requested_core = eIndex_NONE;
	mNative.occupied_core = eIndex_NONE;

	mNative.power_param.opp_step = ePowerOppUnrequest;
	mNative.power_param.boost_value = ePowerBoostUnrequest;
	mNative.power_param.bw = 0x0;

	for(int i = 0;i<MAX_VPU_CORE_NUM;i++)
	{
		mNative.algo_id[i] = mAlgo->getId(i);
		MY_LOGV("VpuRequestImp algo_id[%d]:%d", i, mNative.algo_id[i]);
	}

	// for EARA support
	mModule = eModuleTypeCv;
	mExecTime = 0;
	mBandwidth = 0;
	mFastestTime = 0;
	mEaraParam.boost_value = ePowerBoostUnrequest;
	mJobPriority = 0;
	mSuggestTime = EARA_NOCONTROL_SUGGESTTIME;
	mOwner = NULL;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
prepareSettBuf(
	int size
)
{
	bool ret = true;
	if(mSetting.vMemBuf != NULL)
	{
		return ret;
	}
	else
	{
		VpuMemBuffer *vMemBuf;

		vMemBuf = mOwner->allocMem(size, 0, VPU_MEM_TYPE_DMA);


EXIT:
		mSetting.vMemBuf	= vMemBuf;
	    mSetting.share_fd   = vMemBuf->dma_fd;
	    mSetting.va         = (uint64_t)vMemBuf->va;
	    mSetting.mva        = vMemBuf->pa;
	    mSetting.size = size;

	    MY_LOGV("mSetting: mva 0x%x, va 0x%lx, share_fd %d", mSetting.mva, (unsigned long)mSetting.va, mSetting.share_fd);
		MY_LOGV("+ buffer size: %d/%d", mNative.buffer_count, mBufferCount);
		for(int i = 0;i<MAX_VPU_CORE_NUM;i++)
		{
			mNative.algo_id[i]      = mAlgo->getId(i);
			MY_LOGV("prepareSettBuf algo_id[%d]:%d", i, mNative.algo_id[i]);
		}
		MY_LOGV("- buffer size: %d/%d", mNative.buffer_count, mBufferCount);
	    mNative.sett_ptr     = mSetting.mva;
	    mNative.sett_length  = size;

		return ret;
	}
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
addBuffer(
    VpuBuffer &buffer
)
{
	bool result = true;
	int ret = 0;

    int buf_index = mBufferCount;
    vpu_request_buffer_t *req_buf = &mRequestBuffer[buf_index];
    vpu_buffer_t         *buf_n   = &mNative.buffers[buf_index];

    buf_n->format      = buffer.format == eFormatData      ? VPU_BUF_FORMAT_DATA     :
                         buffer.format == eFormatImageY8   ? VPU_BUF_FORMAT_IMG_Y8   :
                         buffer.format == eFormatImageYV12 ? VPU_BUF_FORMAT_IMG_YV12 :
                         buffer.format == eFormatImageNV21 ? VPU_BUF_FORMAT_IMG_NV21 : VPU_BUF_FORMAT_IMG_YUY2;
    buf_n->width       = buffer.width;
    buf_n->height      = buffer.height;
    buf_n->port_id     = buffer.port_id;
    buf_n->plane_count = buffer.planeCount;
    req_buf->plane_count = buffer.planeCount;

	if(mBufferCount == 32)
	{
		MY_LOGE("vpu buffer number would be larger than (%d) after this\n", mBufferCount);
		return false;
	}

	if(buffer.planeCount > 3)
	{
		MY_LOGE("vpu wrong planeCount(%d)\n", buffer.planeCount);
		return false;
	}
	else
	{
		for (unsigned int i = 0; i < buffer.planeCount; i++)
	    {
	        buf_n->planes[i].stride = buffer.planes[i].stride;
	        buf_n->planes[i].length = buffer.planes[i].length;
	        buf_n->planes[i].ptr    = -1;
	        req_buf->plane[i].share_fd   = buffer.planes[i].fd;
	        req_buf->plane[i].offset     = buffer.planes[i].offset;
			req_buf->plane[i].vMemBuf = NULL;
	    }

	    mNative.buffer_count = ++mBufferCount;
		MY_LOGD("buffer count 0x%x, (%d/%d)",&mNative, \
			mNative.buffer_count, mBufferCount);

		MY_LOGD("mmapMVA+ (%d/%d)", buf_n->port_id, buffer.port_id);
		ret = mmapMVA(); //map mva one time
		MY_LOGD("mmapMVA- (%d)", ret);
		if(ret != 0) {
			result = false;
		}
	}

    return result;
}


/******************************************************************************
 *
 ******************************************************************************/
void
VpuRequestImp::
addBuffer(
    VpuPort *port,
    VpuBuffer &buffer
)
{
	MY_LOGW("2.0 not support this function");
	return ;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
getProperty(
    const char *keyName,
    VpuProperty &prop
)
{
		MY_LOGW("3.0 do not support this function in 2.0 version");
		return false;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
setProperty(
    const char *keyName,
    const VpuProperty &prop
)
{
	MY_LOGW("3.0 not support this function");
	return false;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
setProperty(
    void* sett_ptr,
    int size
)
{
	MY_LOGD("+, size(%d)", size);
	bool ret;
	if(sett_ptr!=NULL)
	{
		ret = prepareSettBuf(size);
		if(ret)
		{
			MY_LOGD("1.1");
			memcpy((void *) (mSetting.va), (void*)(sett_ptr), size);
			MY_LOGD("1.2");
			#ifdef VPU_DUMP_SETT_INFO
			for(int i=0;i<size/sizeof(int);i++)
			{
				MY_LOGV("(%d), value: (0x%x/0x%x)", i, *((int*)sett_ptr+i), *((int*)(mSetting.va)+i));
			}
			#endif

			//need to cache flush before hw run
			if(this->cacheSync())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			MY_LOGE("prepareSettBuf fail \n");
			return false;
		}
	}
	else
	{
		MY_LOGE("setting_ptr is null \n");
		return false;
	}
}

bool
VpuRequestImp::
getProperty(
    void* sett_ptr,
    int size
)
{
    MY_LOGD("+, size(%d)", size);
    bool ret;

    if((sett_ptr == NULL) || (mSetting.va == NULL) || (size > mNative.sett_length))
    {
        return false;
    }

    if(!this->cacheInvalid())
    {
        return false;
    }

    memcpy((void*)(sett_ptr), (void *) (mSetting.va), size);
    return true;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
setExtraParam(
    VpuExtraParam extra_param
)
{
	MY_LOGD("%s+ 0x%x/%d/0x%x",__func__, extra_param.opp_step, extra_param.boost_value, extra_param.bw);

	mNative.power_param.opp_step = extra_param.opp_step;
	mNative.power_param.boost_value = extra_param.boost_value;
	mNative.power_param.bw = extra_param.bw;

	MY_LOGV("[earaVpu] setExtraParam: eara boost value = %d/%d", extra_param.boost_value, extra_param.eara_param.boost_value);
	memcpy(&mEaraParam, &extra_param.eara_param, sizeof(VpuEaraParam));

	return true;
}

/******************************************************************************
 *
 ******************************************************************************/
VpuRequestImp::
~VpuRequestImp()
{
    MY_LOGV("delete request: 0x%lx, request algo_id %d,%d", (unsigned long)this, mNative.algo_id[0], mNative.algo_id[1]);
    if (mNative.sett_length)
    {
		mOwner->freeMem(mSetting.vMemBuf);
    }
}

/******************************************************************************
 * for EARA support
 ******************************************************************************/
static uint64_t getEaraSuggestTime(void)
{
    char prop[100];
    uint64_t suggestTime = -2;

    property_get(PROPERTY_DEBUG_EARATIME, prop, "-2");

    suggestTime = (uint64_t)atoi(prop);

    MY_LOGD("[earaVpu][test] change eara earatime = %llu/%s", suggestTime, prop);

    return suggestTime;
}

static int getEaraPriority(void)
{
    char prop[100];
    int jobPriority = -2;

    property_get(PROPERTY_DEBUG_EARAPRIORITY, prop, "0");

    jobPriority = atoi(prop);

    MY_LOGD("[earaVpu][test] change eara earapriority = %d/%s", jobPriority, prop);

    return jobPriority;
}

static int getUserBoostValue(void)
{
    char prop[100];
    int boostValue = -1;

    property_get(PROPERTY_DEBUG_EARABOOSTVALUE, prop, "-1");

    boostValue = atoi(prop);

    MY_LOGD("[earaVpu][test] change user boostvalue = %d/%s", boostValue, prop);

    return boostValue;
}

static void initEaraDisable(void)
{
    char prop[100];

    property_get(PROPERTY_DEBUG_EARADISABLE, prop, "0");

    gEaraDisable = atoi(prop);

    return;
}

static inline void initVpuLogLevel(void)
{
    char prop[100];

    property_get(PROPERTY_DEBUG_LOGLEVEL, prop, "0");

    gVpuLogLevel = atoi(prop);

    return;
}

int getVpuLogLevel(void)
{
    return gVpuLogLevel;
}

void
VpuRequestImp::
getInferenceInfo(struct VpuInferenceInfo &info)
{
    ATRACE_CALL();
    info.execTime = mExecTime;
    info.bandwidth = mBandwidth;
}

bool
VpuRequestImp::
earaBegin()
{
    uint64_t suggestTime = EARA_NOCONTROL_SUGGESTTIME;
    int jobId = getJobIdFromPointer(this);
    uint8_t boostValue = 100;

    if(gEaraDisable)
    {
        return false;
    }

    ATRACE_CALL();

    //libvpu.so only control cv request for EARA
    if(mModule == eModuleTypeCv)
    {
        if(earaNotifyJobBegin(jobId, suggestTime))
        {
            MY_LOGW("[earaVpu] notify eara begin failed(%d)",jobId);
            return false;
        }

        // for EARA test
#ifdef VPU_EARA_TEST
        {
            uint64_t earaTime = 0;
            earaTime = getEaraSuggestTime();
            if(earaTime == -1)
            {
                MY_LOGD("[earaVpu] test eara dont control");
                MY_LOGE("[earaVpu] notify eara begin failed(%d)",jobId);
                return false;
            }
            else
            {
                suggestTime = earaTime;
                MY_LOGD("[earaVpu] test eara boostValue(%d)",boostValue);
            }
        }
#endif

        mSuggestTime = suggestTime;

        // if first time to run this request,
        if(mFastestTime == 0)
        {
            boostValue = 100;
        }
        else
        {
            //calculate boost value(0~100) for driver to decide opp table
            if(mSuggestTime == 0)
            {
                boostValue = 100;
            }
            else if(mSuggestTime == EARA_NOCONTROL_SUGGESTTIME)
            {
                boostValue = ePowerBoostUnrequest;
            }
            else
            {
                boostValue = mFastestTime * 100 / mSuggestTime;
                if(boostValue > 100)
                {
                    boostValue = 100;
                }
                else if(boostValue < 0)
                {
                    boostValue = 0;
                }
            }
        }

#ifdef VPU_EARA_TEST
        mNative.priority = (uint8_t)getEaraPriority();
#else
        mNative.priority = (uint8_t)mJobPriority;
#endif
        if(boostValue != ePowerBoostUnrequest)
        {
            uint8_t tmpBoostValue = calcBoostvalueFromOpp(mNative.power_param.opp_step);
            /* opp set */
            if(tmpBoostValue < boostValue)
            {
                boostValue = tmpBoostValue;
            }
        }

        mNative.power_param.boost_value = boostValue;

        MY_LOGI("[earaVpu] CV: update suggestTime(%d: %llu/%llu), opp(%d), boostValue(%d), priority(%d)",jobId, mFastestTime, mSuggestTime, mNative.power_param.opp_step, boostValue, mNative.priority);

        return true;
    }
    else
    {
        // for NN application, boost_value != ePowerBoostUnrequest mean eara want to control
        if(mEaraParam.boost_value != ePowerBoostUnrequest)
        {
            MY_LOGI("[earaVpu] NN: (%d/%d)", mEaraParam.job_priority, mEaraParam.boost_value);

            mNative.priority = (uint8_t)mEaraParam.job_priority;
            boostValue = mEaraParam.boost_value;
            mNative.power_param.boost_value = boostValue;

            return true;
        }
    }

    return false;
}

bool
VpuRequestImp::
earaEnd(int errorStatus)
{
    int jobId = getJobIdFromPointer(this);
    int32_t jobPriority = 0;
    int32_t boostVal = 100;

    if(gEaraDisable)
    {
        return false;
    }

    ATRACE_CALL();

    mExecTime = mNative.busy_time;
    mBandwidth = mNative.bandwidth;

    if(mModule == eModuleTypeCv)
    {
        //The first request should set boost value = 100, assume its bust time is fastest time
        if(mFastestTime == 0)
        {
            if(mNative.power_param.boost_value >= 0 && mNative.power_param.boost_value <= 100)
            {
                mFastestTime = mExecTime*mNative.power_param.boost_value/100;
            }
            MY_LOGD("[earaVpu] update mFastestTime(%d/%d/%d)", jobId, mFastestTime, mExecTime);
        }

        // get boost value from current setting
        if(mNative.power_param.boost_value >= 0 && mNative.power_param.boost_value <= 100)
        {
            boostVal = mNative.power_param.boost_value;
        }
        else
        {
            boostVal = calcBoostvalueFromOpp(mNative.power_param.opp_step);
            if(boostVal == ePowerBoostUnrequest)
            {
                boostVal = -1;
            }
        }

        if(earaNotifyJobEnd(jobId, mExecTime, mBandwidth, errorStatus, jobPriority, boostVal))
        {
            MY_LOGW("[earaVpu] notify eara end failed(%d)",jobId);
            return false;
        }

        if(mSuggestTime == EARA_NOCONTROL_SUGGESTTIME)
        {
            return false;
        }
        else
        {
            //MY_LOGD("[earaVpu] update jobPriority(%d/%d)",jobId, jobPriority);
            mJobPriority = jobPriority;
        }

        return true;
    }

    return false;
}

void
VpuRequestImp::
initEaraInfo(VpuModuleType module)
{
    ATRACE_CALL();
    if(module >= eModuleTypeNone || module < 0)
    {
        mModule = eModuleTypeCv;
    }
    else
    {
        mModule = module;
    }

    MY_LOGD("[earaVpu] module = %s", mModule==0?"CV":"NN");
}

void
VpuRequestImp::
clearEaraInfo()
{
    mNative.power_param.opp_step = ePowerOppUnrequest;
    mNative.power_param.boost_value = ePowerBoostUnrequest;
    mNative.power_param.bw = 0x0;

    // for EARA support
    mModule = eModuleTypeCv;
    mExecTime = 0;
    mBandwidth = 0;
    mFastestTime = 0;
    mEaraParam.boost_value = ePowerBoostUnrequest;
    mJobPriority = 0;
    mSuggestTime = EARA_NOCONTROL_SUGGESTTIME;

    return;
}

/******************************************************************************
 *
 ******************************************************************************/
VpuAlgoPool::
VpuAlgoPool()
{
	std::lock_guard<std::mutex> gLock(mAlgoMapMtx);
    mvVpuAlgoMap.clear();
}
/******************************************************************************
 *
 ******************************************************************************/
VpuAlgoPool::
~VpuAlgoPool()
{
    vpuAlgoMap_t::iterator iter;
	std::lock_guard<std::mutex> gLock(mAlgoMapMtx);
    for(iter = mvVpuAlgoMap.begin(); iter != mvVpuAlgoMap.end(); iter++)
    {
        delete iter->second;
    }
    mvVpuAlgoMap.clear();
}
/******************************************************************************
 *
 ******************************************************************************/
VpuAlgoPool*
VpuAlgoPool::getInstance()
{
    static VpuAlgoPool vpuAlgoPool;
    return &vpuAlgoPool;
}
/******************************************************************************
 *
 ******************************************************************************/
VpuAlgo*
VpuAlgoPool::
getAlgo(std::string name, int fd)
{
	int index;
	vpuAlgoMap_t::iterator iter;
	vpuRequestList_t listRequest;
	MY_LOGI("getAlgo(%s) in", name.c_str());
	std::lock_guard<std::mutex> gLock(mAlgoMapMtx);
	if ((iter = mvVpuAlgoMap.find(name)) == mvVpuAlgoMap.end())    // not in Pool
	{
		VpuAlgoImp *algo = new VpuAlgoImp();
		vpu_algo_t *algo_n = algo->getNative();
		strncpy(algo_n->name, name.c_str(), sizeof(algo_n->name));
		algo_n->name[sizeof(algo_n->name) -1] = '\0';
		MY_LOGV("algo: 0x%lx, algo name %s", (unsigned long)algo, algo_n->name);
		MY_LOGV("fd: %d, algo name %s", fd, algo_n->name);
		int ret = ioctl(fd, VPU_IOCTL_LOAD_ALG_TO_POOL, algo_n);
		if (ret < 0)
		{
			MY_LOGE("fail to get algo, %s, errno = %d", strerror(errno), errno);
			delete algo;
			return NULL;
		}
		mvVpuAlgoMap.insert(vpuAlgoPair_t(name, algo));
		return algo;
	}
	else
	{
		return iter->second;
	}
}

VpuCreateAlgo VpuAlgoPool::createAlgo(std::string name, void *buf, unsigned int len, unsigned int core, int fd, VpuStream *stream)
{
    vpu_create_algo_t *nativeAlgo;
    VpuMemBuffer *algoBuf;
    int ret = 0;

    /* check arguments */
    if(buf == nullptr || len == 0 || stream == nullptr)
    {
        MY_LOGE("VpuAlgoPool::createAlgo: invalid arguments\n");
        return 0;
    }

    /* alloc vpu_create_algo for kernel driver communication */
    nativeAlgo = (vpu_create_algo_t *)malloc(sizeof(vpu_create_algo_t));
    if(nativeAlgo == nullptr)
    {
        MY_LOGE("VpuAlgoPool::createAlgo: alloc memory for nativeAlgo failed\n");
        return 0;
    }
    memset(nativeAlgo, 0, sizeof(vpu_create_algo_t));

    /* alloc vpu visiable memory for algo */
    algoBuf = ((VpuStreamImp *)stream)->allocMem(len, 64, VPU_MEM_TYPE_DMA);
    if(algoBuf == 0)
    {
        MY_LOGE("VpuAlgoPool::createAlgo: alloc vpu memory for algo failed\n");
        goto vpu_mem_alloc_failed;
    }
    memcpy((void *)algoBuf->va, buf, len);

    /* assign vpu_create_algo's value */
    nativeAlgo->core = core;
    strncpy(nativeAlgo->name, name.c_str(), sizeof(nativeAlgo->name)-1);
    nativeAlgo->algo_length = len;
    nativeAlgo->algo_ptr = algoBuf->pa;

    /* call ioctl inform kernel driver to add algo */
    ret = ioctl(fd, VPU_IOCTL_CREATE_ALGO, nativeAlgo);
    if(ret < 0)
    {
        MY_LOGE("VpuAlgoPool::createAlgo: ioctl create algo failed\n");
        goto vpu_create_algo_failed;
    }
    MY_LOGI("VpuAlgoPool::createAlgo: create algo ok (%llx)", nativeAlgo);

    /* add algo buffer to list */
    mAlgoBuffer.push_back(algoBuf);

    return (VpuCreateAlgo)nativeAlgo;

vpu_create_algo_failed:
    if(((VpuStreamImp *)stream)->freeMem(algoBuf) != true)
    {
        MY_LOGE("VpuAlgoPool::createAlgo: release vpu mem failed\n");
    }

vpu_mem_alloc_failed:
    free(nativeAlgo);
    return 0;
}

bool VpuAlgoPool::freeAlgo(VpuCreateAlgo algoFd, int fd, VpuStream *stream)
{
    int ret = 0;
    vpu_create_algo_t *nativeAlgo;
    VpuMemBuffer *algoBuf;

    /* check arguments */
    if(algoFd == 0 || stream == nullptr)
    {
        MY_LOGE("VpuAlgoPool::freeAlgo: invalid arguments\n");
        return false;
    }

    nativeAlgo = (vpu_create_algo_t *)algoFd;

    ret = ioctl(fd, VPU_IOCTL_FREE_ALGO, algoFd);
    if(ret < 0)
    {
        MY_LOGE("VpuAlgoPool::freeAlgo: ioctl free algo failed\n");
        return false;
    }

    auto iter = mAlgoBuffer.begin();
    for(iter = mAlgoBuffer.begin(); iter != mAlgoBuffer.end(); iter++)
    {
        algoBuf = (VpuMemBuffer *)(*iter);
        if(algoBuf->pa == nativeAlgo->algo_ptr)
        {
            if(((VpuStreamImp *)stream)->freeMem(algoBuf) != true)
            {
                MY_LOGE("VpuAlgoPool::freeAlgo: free algo buffer failed\n");
                return false;
            }
            free(nativeAlgo);
            return true;
        }
    }

    MY_LOGW("VpuAlgoPool::freeAlgo: can not find algo buffer\n");
    return false;
}

/******************************************************************************
 *
 ******************************************************************************/
VpuUtil::
VpuUtil()
{
}
/******************************************************************************
 *
 ******************************************************************************/
uint32_t
VpuUtil::
mapPhyAddr(
    int fd
)
{
	return 0;
}
/******************************************************************************
 *
 ******************************************************************************/
VpuUtil::
~VpuUtil()
{
}

/******************************************************************************
 *
 ******************************************************************************/
VpuUtil*
VpuUtil::getInstance()
{
    static VpuUtil vpuUtil;
    return &vpuUtil;
}

/******************************************************************************
 *
 ******************************************************************************/
int
VpuRequestImp::
mmapMVA()
{
    int buf_idx, plane_idx;
    vpu_request_buffer_t *req_buf;
    vpu_buffer_t         *buf_n;
	int cnt = 0;
	VpuMemBuffer *vMemBuf;

    for (buf_idx = 0 ; buf_idx < mNative.buffer_count ; buf_idx++)
    {
        buf_n   = &mNative.buffers[buf_idx];
        req_buf = &mRequestBuffer [buf_idx];
        for (plane_idx = 0 ; plane_idx < buf_n->plane_count ; plane_idx++)
        {
			if(req_buf->plane[plane_idx].vMemBuf != NULL)
        	{}
			else
			{
				vMemBuf = mOwner->importMem(req_buf->plane[plane_idx].share_fd, 0xFF, VPU_MEM_TYPE_DMA);
	            req_buf->plane[plane_idx].vMemBuf = vMemBuf;
	            req_buf->plane[plane_idx].mva		 = vMemBuf->pa;
	            buf_n->planes[plane_idx].ptr		 = req_buf->plane[plane_idx].mva + req_buf->plane[plane_idx].offset;
	            MY_LOGV("get_phys: buffer[%d], plane[%d], mva 0x%lx", buf_idx, plane_idx, (unsigned long)buf_n->planes[plane_idx].ptr);
	        }
			mNative.buf_ion_infos[cnt] = (uint64_t)req_buf->plane[plane_idx].share_fd;
			MY_LOGD("mmapMVA cnt : %d, share_fd: %d, 0x%x\n", cnt, mNative.buf_ion_infos[cnt],
				req_buf->plane[plane_idx].share_fd);
			cnt++;
	    }
	}

	return 0;
}
/******************************************************************************
 *
 ******************************************************************************/
void
VpuRequestImp::
munmapMVA()
{
    int buf_idx, plane_idx;
    vpu_request_buffer_t *req_buf;
    vpu_buffer_t         *buf_n;

    for (buf_idx = 0 ; buf_idx < mNative.buffer_count ; buf_idx++)
    {
        buf_n   = &mNative.buffers[buf_idx];
        req_buf = &mRequestBuffer [buf_idx];
        for (plane_idx = 0 ; plane_idx < buf_n->plane_count ; plane_idx++)
        {
            if (buf_n->planes[plane_idx].ptr != (uint64_t)-1)
            {
                MY_LOGV("ion_free: buffer[%d], plane[%d]", buf_idx, plane_idx);
				mOwner->freeMem(req_buf->plane[plane_idx].vMemBuf);

                buf_n->planes[plane_idx].ptr = -1;
                req_buf->plane[plane_idx].vMemBuf = NULL;
            }
        }
    }
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuRequestImp::
cacheSync()
{
	return mOwner->syncMem(mSetting.vMemBuf, VPU_SYNC_TYPE_FLUSH);
}

bool
VpuRequestImp::
cacheInvalid()
{
	return mOwner->syncMem(mSetting.vMemBuf, VPU_SYNC_TYPE_INVALID);
}

/******************************************************************************
 *
 ******************************************************************************/
void
VpuRequestImp::
setOwner(void *pOwner)
{
	mOwner = (VpuStreamImp*)pOwner;
}

/******************************************************************************
 *
 ******************************************************************************/
void
VpuRequestImp::
release()
{
	if (mNative.sett_length)
	{
		mOwner->freeMem(mSetting.vMemBuf);
		mSetting.vMemBuf = NULL;
		mNative.sett_length = 0;
	}
}

/******************************************************************************
 *
 ******************************************************************************/
VpuStreamImp::
VpuStreamImp(const char* callername)
{
	VpuUtil *vpuUtil = NULL;

	mFd = open(VPU_DEV_PATH, O_RDONLY);
	if (mFd < 0)
	{
		MY_LOGE("===================================================================\n");
		MY_LOGE("===================================================================\n");
		MY_LOGE("===================================================================\n");
		MY_LOGE("open vpu fail, return %d, errno(%d):%s\n", mFd, errno, strerror(errno));
		MY_LOGE("===================================================================\n");
		MY_LOGE("===================================================================\n");
		MY_LOGE("===================================================================\n");
	}
	else
	{
		vpu_dev_debug_info dev_debug_info;
		dev_debug_info.dev_fd = mFd;
		strncpy(dev_debug_info.callername, callername, sizeof(dev_debug_info.callername));
		dev_debug_info.callername[sizeof(dev_debug_info.callername) -1] = '\0';
		int ret = ioctl(mFd, VPU_IOCTL_OPEN_DEV_NOTICE, &dev_debug_info);
	        if (ret < 0)
	        {
			MY_LOGE("fd(%d) fail to notice open device, %s, errno = %d",
					mFd, strerror(errno), errno);
	        }
	}
	MY_LOGI(" user_%s open mFd (%d)\n", callername, mFd);

	mCreateAlgoCnt = 0;
	mRequesetCnt = 0;

	vpuUtil = VpuUtil::getInstance();    // constructor
	if (vpuUtil == NULL)
	{
		MY_LOGE("vpuUtil is NULL!\n");
	}

	mMemAllocator = new vpuMemAllocator(mFd);

	//for EARA support
	startEaraInf(); //earaComm
}

/******************************************************************************
 *
 ******************************************************************************/
VpuAlgo *
VpuStreamImp::
getAlgo(
    char *name
)
{
    return VpuAlgoPool::getInstance()->getAlgo(std::string(name), mFd);
}

/******************************************************************************
 *
 ******************************************************************************/
VpuRequest *
VpuStreamImp::
acquire(
    VpuAlgo *algo,
    VpuModuleType module = eModuleTypeCv
)
{
	std::unique_lock <std::mutex> l(mRequestMutex);

	if (algo == NULL)
		return NULL;

	VpuRequest *req = (((VpuAlgoImp *)algo)->acquire());
	//VpuRequestImp *reqImp = dynamic_cast<VpuRequestImp *>(req);
	VpuRequestImp *reqImp = (VpuRequestImp *)(req);
	if (reqImp)
	{
		reqImp->setOwner(this);
		mRequesetCnt++;
	}
	else
	{
		MY_LOGE("[vpustream] acquire failed!\n");
	}
    //for EARA support
    ((VpuRequestImp *)req)->initEaraInfo(module);

	return req;
}
/******************************************************************************
 *
 ******************************************************************************/
void
VpuStreamImp::
release(
    VpuRequest *request
)
{
	std::unique_lock <std::mutex> l(mRequestMutex);

	if(request == NULL)
	{
		MY_LOGE("null request to release\n");
	}
	else
	{
		MY_LOGI("[vpu] release req_ref_0x%lx\n",
			(unsigned long)(((VpuRequestImp *)request)->getNative()->request_id));

		((VpuRequestImp *)request)->munmapMVA(); //unmap buffer first
		((VpuRequestImp *)request)->release();
	    ((VpuRequestImp *)request)->getAlgo()->release(request);
	    ((VpuRequestImp *)request)->clearEaraInfo();

		mRequesetCnt--;
	}
}

/******************************************************************************
 *
 ******************************************************************************/
bool
VpuStreamImp::
enque(
    VpuRequest *request,
    unsigned int core_index
)
{
	std::unique_lock <std::mutex> l(mRequestMutex);
	mRequests.push_back(request);
	mRequestCond.notify_one();
	//((VpuRequestImp *) request)->mmapMVA();
	vpu_request_t *req_n = ((VpuRequestImp *)request)->getNative();
	if(req_n->frame_magic == 65536)
		req_n->frame_magic = 0;
	else
		req_n->frame_magic += 1;
	MY_LOGI("[vpu] enque algo:ref_%d/0x%lx/%d, expected core:0x%x\n",
	req_n->algo_id[0], (unsigned long)req_n->request_id, req_n->frame_magic, core_index);
	{
		int i, j ;
		MY_LOGV("buffer_count = %d, sett_ptr 0x%lx, sett_length %d", \
		req_n->buffer_count, (unsigned long)req_n->sett_ptr, req_n->sett_length);
		if(req_n->buffer_count == 0)
		{
			MY_LOGW("buffer count = 0, WRN"); //testcase_max_power do not need buffers
			//return false;
		}
		for (i = 0 ; i < req_n->buffer_count; i++)
		{
			MY_LOGV("buffer[%d]: plane_count = %d, port_id %d, w %d, h %d, fmt %d", i, req_n->buffers[i].plane_count, req_n->buffers[i].port_id, req_n->buffers[i].width, req_n->buffers[i].height, req_n->buffers[i].format);
			for (j = 0 ; j < req_n->buffers[i].plane_count; j++)
			{
				MY_LOGV("plane[%d]: ptr 0x%lx, length %d, stride %d", j, (unsigned long)req_n->buffers[i].planes[j].ptr, req_n->buffers[i].planes[j].length, req_n->buffers[i].planes[j].stride);
			}
		}
	}

	//for EARA support
	((VpuRequestImp *) request)->earaBegin();

	req_n->requested_core = core_index;
	int ret = ioctl(mFd, VPU_IOCTL_ENQUE_REQUEST, req_n);
	if (ret < 0)
	{
		mRequests.pop_back();
		MY_LOGE("core(0x%x/0x%x) fail to enque, %s, errno = %d", \
		core_index, req_n->requested_core, strerror(errno), errno);
		return false;
	}

	return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuStreamImp::
tryEnque(
    VpuRequest *request
)
{
    MY_LOGV("%s, magicCore = 0x%x", __func__, VPU_TRYLOCK_CORENUM);
    return enque(request, VPU_TRYLOCK_CORENUM);
}
/******************************************************************************
 *
 ******************************************************************************/
VpuRequest *
VpuStreamImp::
deque()
{
	bool result = true;
    VpuRequest *req;
    {
        std::unique_lock <std::mutex> l(mRequestMutex);
        if (mRequests.size() == 0)
        {
            mRequestCond.wait(l);
        }
        req = mRequests.front();
        mRequests.erase(mRequests.begin());
    }

    vpu_request_t *req_n = ((VpuRequestImp *) req)->getNative();
    int ret = ioctl(mFd, VPU_IOCTL_DEQUE_REQUEST, req_n);
    if (ret < 0)
    {
        MY_LOGE("fail to deque, %s, errno = %d", strerror(errno), errno);

        result = false;
        //return NULL;
    }
    else
    {
	MY_LOGI("[vpu] algo_ref_%d/0x%lx/%d, occupied core_index 0x%x/0x%x, sts(%d)",
		req_n->algo_id[0], (unsigned long)req_n->request_id,
		req_n->frame_magic, req_n->requested_core, req_n->occupied_core, req_n->status);

	switch(req_n->status) {
	case VPU_REQ_STATUS_SUCCESS:
		result = true;
		break;
	case VPU_REQ_STATUS_BUSY:
	case VPU_REQ_STATUS_TIMEOUT:
	case VPU_REQ_STATUS_INVALID:
	case VPU_REQ_STATUS_FLUSH:
	case VPU_REQ_STATUS_FAILURE:
	default:
		MY_LOGE("[vpu] algo_ref_%d_%d, ocp core_index 0x%x/0x%x, FAILED sts(%d)",
			req_n->algo_id[0], req_n->frame_magic, req_n->requested_core, req_n->occupied_core, req_n->status);
		result = false;
		break;
	}
    }

    //for EARA support
    unsigned int coreIdx = 0xFFFF;
    struct vpu_lock_power earaPower;

    ((VpuRequestImp *) req)->earaEnd((int)result);

	//((VpuRequestImp *) req)->munmapMVA();
	if(result)
	{
    	return req;
	}
	else
	{
		return NULL;
	}
}
/******************************************************************************
 *
 ******************************************************************************/
void
VpuStreamImp::
flush()
{
    int ret = ioctl(mFd, VPU_IOCTL_FLUSH_REQUEST, 0);
    if (ret < 0)
    {
        MY_LOGE("fail to flush request, %s, errno = %d", strerror(errno), errno);
    }
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuStreamImp::
setPower(
    VpuPower& pwr,
    unsigned int core_index
)
{
#if 0
	vpu_power_t vpuPower;
	vpuPower.mode = pwr.mode == ePowerModeOn ? VPU_POWER_MODE_ON : VPU_POWER_MODE_DYNAMIC;
	vpuPower.opp  = pwr.opp;
	int ret = ioctl(mFd, VPU_IOCTL_SET_POWER, &vpuPower);
	if (ret < 0) {
		MY_LOGE("fail to setPower, %s, errno = %d", strerror(errno), errno);
		return false;
	}
#else
	struct vpu_power power_param;
	power_param.boost_value = pwr.boost_value;
	power_param.opp_step = pwr.opp;
	power_param.bw = 0;
	power_param.core = core_index;
	MY_LOGD("[vpu] setPower boost_value %d/core_%d\n", pwr.boost_value, core_index);
	int ret = ioctl(mFd, VPU_IOCTL_SET_POWER, &power_param);
	if (ret < 0)
	{
		MY_LOGE("fail to setPower, %s, errno = %d", strerror(errno), errno);
		return false;
	}
#endif
	return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
VpuStreamImp::getVpuStatus(
	std::vector<VpuStatus>& vpu_status_info
)
{
	int ret = 0;
	for(int i = vpu_status_info.size()-1 ; i>=0; i--)
		vpu_status_info.pop_back();

	/* individual cores */
	bool getmax = false;
	#if 0
	unsigned int index_max = static_cast<unsigned int>(eIndex_MAX);
	printf("eIndex_MAX(0x%x/0x%x), max_core_num(%d) \n",index_max, eIndex_MAX,max_core_num);
	do{
		if((int)(index_max) > 0) {
			max_core_num += 1;
			index_max = index_max >> 1;
		} else {
			getmax=true;
		}
		printf("index_max(0x%x), getmax(%d), max_core_num(%d)\n",index_max, getmax, max_core_num);

	}while(!getmax);
	printf("after eIndex_MAX(0x%x/0x%x), max_core_num(%d) \n",index_max, eIndex_MAX,max_core_num);
	#endif
	for(int i = 0 ; i < MAX_VPU_CORE_NUM ; i++)
	{
		vpu_status_t vpu_status;
		vpu_status.vpu_core_index = i; //kernel pool-based index, 0, 1, 2 //(0x1 << i);
		ret = ioctl(mFd, VPU_IOCTL_GET_CORE_STATUS, &vpu_status);
		if(ret < 0) {
			if (errno == EINVAL) {
				/*hw not support this core*/
				MY_LOGW("warning to get vpu_%d status, %s, errno = %d\n", vpu_status.vpu_core_index,
					strerror(errno), errno);
				continue;
			} else {
				MY_LOGE("fail to get vpu_%d status, %s, errno = %d\n", vpu_status.vpu_core_index,
					strerror(errno), errno);
        		return false;
			}
		}
		VpuStatus status;
		status.core_index = static_cast<VpuCoreIndex>(vpu_status.vpu_core_index);
		status.available = vpu_status.vpu_core_available;
		status.waiting_requests_num = vpu_status.pool_list_size;
		vpu_status_info.push_back(status);
	}

	/* common pool */
	if((int)vpu_status_info.size() > 0)
	{
		vpu_status_t vpu_status;
		vpu_status.vpu_core_index = static_cast<int>(eIndex_NONE);
		ret = ioctl(mFd, VPU_IOCTL_GET_CORE_STATUS, &vpu_status);
		if(ret < 0)
		{
			MY_LOGE("fail to get vpu status, %s, errno = %d", strerror(errno), errno);
			return false;
		}
		VpuStatus status;
		status.core_index = static_cast<VpuCoreIndex>(vpu_status.vpu_core_index);
		status.available = vpu_status.vpu_core_available;
		status.waiting_requests_num = vpu_status.pool_list_size;
		vpu_status_info.push_back(status);
	}
	else
	{
		MY_LOGE("do not support VPU HW..\n");
		return false;
	}

	return true;
}

/******************************************************************************
 * stream lib 3.0, support allocate memory for VPU
 ******************************************************************************/
VpuMemBuffer *VpuStreamImp::allocMem(size_t size, uint32_t align, enum vpuMemType type)
{
    return mMemAllocator->allocMem(size, align, type);
}

VpuMemBuffer *VpuStreamImp::importMem(int fd, uint32_t len, enum vpuMemType type)
{
    return mMemAllocator->importMem(fd, len, type);
}

bool VpuStreamImp::freeMem(VpuMemBuffer *buf)
{
    return mMemAllocator->freeMem(buf);
}

bool VpuStreamImp::syncMem(VpuMemBuffer *buf, enum vpuSyncType type)
{
    return mMemAllocator->syncMem(buf, type);
}

/******************************************************************************
 * stream lib 3.0, support dynamic load algo
 ******************************************************************************/
VpuCreateAlgo VpuStreamImp::createAlgo(char *name, char *buf, uint32_t len, uint32_t core)
{
    return VpuAlgoPool::getInstance()->createAlgo((std::string)name, buf, len, core, mFd, this);
}

bool VpuStreamImp::freeAlgo(VpuCreateAlgo algoFd)
{
    return VpuAlgoPool::getInstance()->freeAlgo(algoFd, mFd, this);
}

/******************************************************************************
 *
 ******************************************************************************/
VpuStreamImp::
~VpuStreamImp()
{
	int fd;
	fd = mFd;

	//for EARA support
	closeEaraInf();

	MY_LOGI("close mFd (%d)\n",mFd);
	int ret = ioctl(mFd, VPU_IOCTL_CLOSE_DEV_NOTICE, &fd);
    if (ret < 0)
    {
    	MY_LOGE("fd(%d/%d) fail to notice close device, %s, errno = %d",
			fd, mFd, strerror(errno), errno);
    }

    std::vector<VpuRequest *>::iterator iter;

    while(!mRequests.empty())
    {
        iter = mRequests.begin();
		release(*iter);
        mRequests.erase(iter);
    }

	if (mRequesetCnt != 0)
	{
		MY_LOGE("mRequesetCnt (%d) != 0\n", mRequesetCnt);
	}

	if (mCreateAlgoCnt != 0)
	{
		MY_LOGE("mCreateAlgoCnt (%d) != 0\n", mCreateAlgoCnt);
	}

	delete mMemAllocator;

	close(mFd);
}

/******************************************************************************
 *
 ******************************************************************************/
VpuStream *
VpuStream::
createInstance(void)
{
	return createInstance(DEFAULT_CALLER_NAME);
}

VpuStream *
VpuStream::
createInstance(const char* callername)
{
    /* get eara disable from property */
    initEaraDisable();
    /* init vpu stream log level from property */
    initVpuLogLevel();

#ifdef HAVE_EFUSE_BOND
	{
        #define DEV_IOC_MAGIC       'd'
        #define READ_DEV_DATA       _IOR(DEV_IOC_MAGIC,  1, unsigned int)

        typedef struct {
            unsigned int entry_num;
            unsigned int data[100];
        } DEVINFO_S;

        DEVINFO_S devinfo;
        int ret = 0;
        int fd = 0;

		int bonding_info=0;
		int mDisVPU = 0; /* vpu efuse value*/
		bool vpu_hw_support = true;
        fd = open("/proc/device-tree/chosen/atag,devinfo", O_RDONLY);
        if(fd < 0)
        {
            MY_LOGE("/proc/device-tree/chosen/atag,devinfo kernel open fail, errno(%d):%s",errno,strerror(errno));
			return NULL;
        }
        else
        {
            if ((ret = read(fd, (void *)&devinfo, sizeof(DEVINFO_S))) < 0)
            {
                MY_LOGE("Get Devinfo Data Fail:%d\n", ret);
				return NULL;
            }
            else
            {
            	switch (HAVE_EFUSE_BOND_VERSION) {
				case 71:
                bonding_info = devinfo.data[3];
					mDisVPU = (bonding_info & 0xC00)>>10;
					if(mDisVPU == 0x3)
						vpu_hw_support = false;
					break;
				default:
					break;
            	}
            }
            MY_LOGI("version(%d), Data:0x%x,0x%x, %d\n", HAVE_EFUSE_BOND_VERSION,
				bonding_info, mDisVPU, vpu_hw_support);
            close(fd);
        }
		if(vpu_hw_support)
		{
			return new VpuStreamImp(callername);
		}
		else
		{
			MY_LOGE("Do not support VPU HW:0x%x,0x%x, %d\n", bonding_info, mDisVPU, vpu_hw_support);
			return NULL;
		}
    }

#else
	MY_LOGD("vpu user createInstance\n");
	return new VpuStreamImp(callername);
#endif
}

/******************************************************************************
 *
 ******************************************************************************/
uint32_t
vpuMapPhyAddr(
    int fd
)
{
    return VpuUtil::getInstance()->mapPhyAddr(fd);
}
