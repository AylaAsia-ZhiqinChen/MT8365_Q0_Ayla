
bool bEnableMMDump;
bool bEnableMMPath;
MMDump* mmdump;

AppInsight::InfoDump appinsight;
video_encode_param* pEncParam;

void MtkOmxVencDebug();
void deMtkOmxVencDebug();

void ComponentInitDebug(AndroidVendorConfigExtensionStore* ext);
void ComponentDeInitDebug();

// InitHW before stream on
void Debug_OnInit(video_encode_param* param);
OMX_ERRORTYPE Debug_SetMode();

// DeInitHW after stream off
void Debug_OnDeInit();

bool Debug_Enabled();

void Debug_EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pInputBuf);
void Debug_FillThisBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf);

// before generating v4l2_buffer from omx buffer (may idendical to Debug_UpdateInputV4L2Buffer)
void Debug_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf);

void Debug_CollectBsBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf);

// before enque: after v4l2_buffer is generated
void Debug_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf);
void Debug_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf);

// after perform qbuf
void Debug_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf);
void Debug_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf);

// after acquire deque buffer, before calling Empty/FillBufferDone
void Debug_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf);
void Debug_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf, struct v4l2_buffer* dqbuf);

OMX_ERRORTYPE Debug_SetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);
OMX_ERRORTYPE Debug_GetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig);

OMX_ERRORTYPE Debug_GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE Debug_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam);
OMX_ERRORTYPE Debug_GetExtensionIndex(OMX_STRING parameterName, OMX_INDEXTYPE *pIndexType);

// MMDUMP
void Debug_MMDump_Init(video_encode_param* param);
void Debug_MMDump(OMX_BUFFERHEADERTYPE* ppInputBuf);
void Debug_MMPath(OMX_BUFFERHEADERTYPE* pInputBuf);

// MMDUMP
void Debug_AppInsight_Init(video_encode_param* param);
void Debug_AppInsight(OMX_BUFFERHEADERTYPE* ppInputBuf);