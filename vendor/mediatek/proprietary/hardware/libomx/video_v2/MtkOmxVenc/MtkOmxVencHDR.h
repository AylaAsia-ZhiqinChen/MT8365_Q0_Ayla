
DescribeHDRStaticInfoParams mDescribeStaticInfo;
DescribeColorAspectsParams mDescribeColorAspects;
int mEnabledStatic;

OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* pEnabledHdr10Toggle;
int mEnabled10Plus;

// for UT
int mEnabled10PlusDummydata;

OmxMVAManager *mDynamicInfoBufferMVAMgr;

pthread_mutex_t mDynamicInfoBufferTableLock;
DefaultKeyedVector<int, OMX_U8*> mDynamicInfoBufferTable = DefaultKeyedVector<int, OMX_U8*>(0);

int mDynamicInfoBufferSize;
pthread_mutex_t mDynamicInfoFreeBufferListLock;
Vector<OMX_U8*> mDynamicInfoFreeBufferList;

void HDR_GetFreeInfoBuffer(OMX_U8** ppInfoBuffer, unsigned* pBufferSize);
void HDR_SetFreeInfoBuffer(OMX_U8* pInfoBuffer);

void MtkOmxVencHDR();
void deMtkOmxVencHDR();

void ComponentInitHDR(AndroidVendorConfigExtensionStore* ext);
void ComponentDeInitHDR();

// InitHW before stream on
void HDR_OnInit(video_encode_param* param);
OMX_ERRORTYPE HDR_SetMode();
void HDR_NotifyStopCommand();
void HDR_HandleStateSet(OMX_U32 nNewState);
void HDR_SetStaticInfo();

// DeInitHW after stream off
void HDR_OnDeInit();

bool HDR_Enabled();

void HDR_EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pInputBuf);
void HDR_FillThisBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf);

// before generating v4l2_buffer from omx buffer (may idendical to HDR_UpdateInputV4L2Buffer)
void HDR_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf);

void HDR_CollectBsBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf);

// before enque: after v4l2_buffer is generated
void HDR_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf);
void HDR_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf);

// after perform qbuf
void HDR_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf);
void HDR_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf);

// after acquire deque buffer, before calling Empty/FillBufferDone
void HDR_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf);
void HDR_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf, struct v4l2_buffer* dqbuf);

OMX_ERRORTYPE HDR_SetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);
OMX_ERRORTYPE HDR_GetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);

OMX_ERRORTYPE HDR_GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE HDR_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE HDR_GetExtensionIndex(OMX_STRING parameterName, OMX_INDEXTYPE *pIndexType);

OMX_ERRORTYPE HandleSetConfigDescribeHDRStaticInfo(DescribeHDRStaticInfoParams* pConfig);
OMX_ERRORTYPE HandleGetConfigDescribeHDRStaticInfo(DescribeHDRStaticInfoParams* pConfig);
OMX_ERRORTYPE HandleSetConfigDescribeColorAspects(DescribeColorAspectsParams* pConfig);
OMX_ERRORTYPE HandleGetConfigDescribeColorAspects(DescribeColorAspectsParams* pConfig);
OMX_ERRORTYPE HandleSetConfigDescribeHDR10PlusInfo(DescribeHDR10PlusInfoParams* pConfig);
OMX_ERRORTYPE HandleGetConfigDescribeHDR10PlusInfo(DescribeHDR10PlusInfoParams* pConfig);
OMX_ERRORTYPE HandleSetConfigHDRAndroidVendorExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* pConfig);
OMX_ERRORTYPE HandleGetConfigHDRAndroidVendorExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* pConfig);