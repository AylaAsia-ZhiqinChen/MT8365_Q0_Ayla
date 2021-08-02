###################################################################
# Audio Feature Option Configuration
###################################################################
CFG_MTK_AUDIO_FRAMEWORK_SUPPORT = yes
CFG_AUDIO_LOG_DEBUG = yes
CFG_AUDIO_DEMO = no
CFG_AUDIO_IPC_UT = no
CFG_AUDIO_DSP_STATE = yes
CFG_HW_RES_MGR = yes
CFG_HOST_READY_NOTIFY_SUPPORT = yes
CFG_VA_PROCESS_CLI_SUPPORT = yes
CFG_AUDIO_VA_VAD_DUMMY = yes
CFG_AUDIO_VA_AEC_DUMMY = yes
CFG_AUDIO_VA_PREPROC_MTKFFP = no
CFG_AUDIO_VA_WW_DUMMY = yes
CFG_AUDIO_DEBUG_DUMP = no
CFG_DSP_ULPLL_SUPPORT = yes

###################################################################
# Voice Assistant Configuration
###################################################################
#config va task reserve sram size
CFG_VA_TASK_RESERVE_SRAM = 131072

#VA Process Control
#config which process is default on
CFG_VA_VAD_DEFAULT_ON = yes
CFG_VA_WW_DEFAULT_ON = yes
CFG_VA_PREPROC_DEFAULT_ON = yes

#VAD, WW always on and in one config
CFG_VA_VAD_ALWAYS_ON = 1
CFG_VA_WW_ALWAYS_ON = 0
CFG_VA_VAD_WW_IN_ONE = 0

#VA in buffer length, mili-second
CFG_VA_VAD_BUF_LEN = 500
CFG_VA_VAD_BUF_TYPE = ADSP_MEM_TASK_RESERVE
CFG_VA_VAD_BITWIDTH = 16
CFG_VA_VAD_CHNUM = 1

#VA Preprocessing OUT buffer
CFG_VA_PREPROC_BUF_TYPE = ADSP_MEM_NORMAL_CACHE
#if use AEC, AEC's config
CFG_VA_AEC_OUT_CH_NUM = 2
CFG_AUDIO_ECHO_REF_CH = 0

#VA in buffer length, mili-second
#if CFG_VA_VAD_WW_IN_ONE, WW_BUF_LEN&TYPE will not be used
CFG_VA_WW_BUF_LEN = 3000
CFG_VA_WW_BUF_TYPE = ADSP_MEM_NORMAL_CACHE
#VA wakeword pre-roll size, mili-second
CFG_VA_WW_PRE_ROLL_LEN = 500
CFG_VA_WW_BITWIDTH = 16
CFG_VA_WW_CHNUM = 1

CFG_VA_PROCESS_SPEED_UP_EN = 0

#VA VOICE UPLOAD BUFFER CONFIG
CFG_VA_VOICE_UPLOAD_CH_NUM = 1
CFG_VA_VOICE_UPLOAD_BITWIDTH = 16
CFG_VA_UPLOAD_SPEED_UP_EN = 1
CFG_VA_UPLOAD_BUF_LEN = 2000

# VA State Control
CFG_VA_WW_TIMEOUT_EN = 1
CFG_VA_WW_TIMEOUT_LEN = 2000

CFG_VA_IDLE_DSP_CLK = DSP_CLK_13M
CFG_VA_VAD_DSP_CLK  = DSP_CLK_26M
CFG_VA_WW_DSP_CLK   = DSP_CLK_PLL
CFG_VA_VOICE_UPLOAD_DSP_CLK = DSP_CLK_PLL

CFG_VA_IDLE_SYS_HW = DSP_SYS_HW_NONE
CFG_VA_VAD_SYS_HW  = DSP_SYS_HW_NONE
CFG_VA_WW_SYS_HW   = DSP_SYS_HW_DRAM
CFG_VA_VOICE_UPLOAD_SYS_HW = DSP_SYS_HW_DRAM

# DSP State Control
CFG_DSP_SWITCH_STATE_EN = no
CFG_DSP_SWITCH_TIMEOUT_LEN = 1000
CFG_DSP_SWITCH_DSP_CLK = DSP_CLK_26M
CFG_DSP_SWITCH_SYS_HW = DSP_SYS_HW_26M

CFG_DSP_PLL_VALUE = 400000000
