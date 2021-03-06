#ifndef __TRUSTZONE_TA_HDCP_TX__
#define __TRUSTZONE_TA_HDCP_TX__

#define TZ_TA_HDCP2_TX_UUID   "65e64a92-d60e-4d2d-bc38-a0a7ab721112"

/* Data Structure for Test TA */
/* You should define data structure used both in REE/TEE here
   N/A for Test TA */

/* Command for HDCP2_TX TA */
#define TZCMD_HDCP2_TX_SET_ENCKEY               0
#define TZCMD_HDCP2_TX_CHECK_RXID               1
#define TZCMD_HDCP2_TX_GET_ENC_KM               2
#define TZCMD_HDCP2_TX_KD_KEY_DEV               3
#define TZCMD_HDCP2_TX_COMPUTE_H                4
#define TZCMD_HDCP2_TX_COMPUTE_L                5
#define TZCMD_HDCP2_TX_GET_ENC_KS               6
#define TZCMD_HDCP2_TX_SET_PAIR_INFO            7
#define TZCMD_HDCP2_TX_INIT_AES                 8
#define TZCMD_HDCP2_TX_GET_ENC_DATA             9
#define TZCMD_HDCP2_TX_GET_PAIR_INFO           10
#define TZCMD_HDCP2_TX_QUERY_KEY_HAVE_SET      11
#define TZCMD_HDCP2_TX_VERIFY_SIGNATURE        12
#define TZCMD_HDCP2_TX_COMPUTE_V               13
#define TZCMD_HDCP2_TX_COMPUTE_2_2_H           14
#define TZCMD_HDCP2_TX_COMPUTE_2_12_V          15
#define TZCMD_HDCP2_TX_COMPUTE_M               16
#define TZCMD_HDCP2_TX_LOCALITY_CHECK          17
#define TZCMD_HDCP2_TX_GET_ENC_NATIVE_DATA     18
#define TZCMD_HDCP2_TX_2_2_KD_KEY_DEV          19
#define TZCMD_HDCP2_TX_GET_ENC_2_2_KS          20
#define TZCMD_HDCP2_TX_Enable_EBDKEY           21
#define TZCMD_HDCP2_TX_Set_HDCPState           22
#define TZCMD_HDCP2_TX_Set_HDCPVer             23
#define TZCMD_HDCP2_TX_COMPUTE_HMAC_RIV        24


typedef enum HDCP2_TX_KEY_SRC {
    CODED_FACSMILE_KEY = 0,
    CODED_LICENSE_KEY,
    DRM_KEY,
    EBD_KEY
} E_HDCP2_TX_KEY_SRC;

#endif /* __TRUSTZONE_TA_VDEC__ */

