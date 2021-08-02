
OMX_BOOL mSetWFDMode;
OMX_BOOL mWFDMode;

// for WFD dummy NAL
OMX_BOOL mEnableDummy;

pthread_mutex_t mDummyFrmLock;
DefaultKeyedVector<int, OMX_BUFFERHEADERTYPE*> mDummyBufferTable = DefaultKeyedVector<int, OMX_BUFFERHEADERTYPE*>(0);

void MtkOmxVencWFD();
void deMtkOmxVencWFD();
void WFD_NotifyStopCommand();
void WFD_HandleStateSet(OMX_U32 nNewState);
void ComponentInitWFD(AndroidVendorConfigExtensionStore* ext);
void ComponentDeInitWFD();
void WFD_OnInit(video_encode_param* param);
void WFD_OnDeInit();
OMX_ERRORTYPE WFD_EnableToggle();
OMX_ERRORTYPE WFD_DisableToggle();
OMX_ERRORTYPE WFD_SetMode();
OMX_ERRORTYPE WFD_SetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);
OMX_ERRORTYPE WFD_GetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);
OMX_ERRORTYPE WFD_GetParameter(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE WFD_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE WFD_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf);
OMX_ERRORTYPE WFD_DrawYUVStripeLine(
    unsigned char** pY, unsigned char** pU, unsigned char** pV, OMX_U32 iDrawWidth, OMX_U32 iDrawHeight);

void WFD_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf);
void WFD_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf);
OMX_ERRORTYPE WFD_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf);
OMX_ERRORTYPE WFD_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf);
OMX_ERRORTYPE WFD_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf);
OMX_ERRORTYPE WFD_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBufHdr, void* dqbuf);
OMX_ERRORTYPE WFD_HandleDequeFrame(OMX_BUFFERHEADERTYPE* pOutputBufHdr);
OMX_ERRORTYPE GenerateDummyBuffer(OMX_BUFFERHEADERTYPE* pDummyOutputBufHdr);
OMX_ERRORTYPE HandleWFDSetVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE* pBitrateType);
OMX_BOOL mIsSecureInst; // when it's true, means this obj supports secure path.
OMX_BOOL InitSecEncParams(void);
OMX_BOOL DeInitSecEncParams(void);
enum teeEnvType
{
    NONE_TEE = 0,
    TRUSTONIC_TEE = 1,
    INHOUSE_TEE = 2,
    MICROTRUST_TEE = 3,
};
teeEnvType mTeeEncType;
int setEncodeMode(teeEnvType teeType);
