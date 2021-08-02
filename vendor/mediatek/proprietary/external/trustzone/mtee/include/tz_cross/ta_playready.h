#ifndef __TRUSTZONE_TA_PLAYREADY__
#define __TRUSTZONE_TA_PLAYREADY__

#define TZ_TA_PLAYREADY_UUID         "b25bf100-d276-11e2-8b8b-0800200c9a66"

/*
    get model/unit certificate
    get model/unit private key
    query whether certificates and keys have been cached
*/
typedef enum {
    PLAYREADY_PROVISIONED_MODEL_CERT = 1,    // a. get clear model cert; b. cache clear model cert in TEE.
    PLAYREADY_PROVISIONED_MODEL_KEY,         // decrypt 'encryped zgpriv' in TEE
    PLAYREADY_PROVISIONED_UNIT_CERT,         // a. get clear unit cert; b. cache clear unit cert in TEE.
    PLAYREADY_PROVISIONED_UNIT_KEY,          // decrypt 'encryped zupriv' in TEE
    PLAYREADY_PROVISIONED_QUERY,             // query whether certificates and keys have been cached
    PLAYREADY_PROVISIONED_CLEAR_KEY,         // set clear zgpriv to TEE. used for File System Mode.(Can't get encrypted key from KB)
} KeyCertLProvCMDs;

#define PRIVKEYSIZE         32
typedef struct {
    unsigned int    cbPRModelCert;
    unsigned char  *pbPRModelCert;
    unsigned int    cbPRZgpriv;
    unsigned char   pbPRZgpriv[PRIVKEYSIZE];
    unsigned int    cbPRUnitCert;
    unsigned char  *pbPRUnitCert;
    unsigned int    cbPRZupriv;
    unsigned char   pbPRZupriv[2][PRIVKEYSIZE];
} KeyCert_T;

enum {
    PLAYREADY_HDCP_HDMI_QUERY = 1,  // Query HDCP version of HDMI
    PLAYREADY_HDCP_WFD_QUERY,       // Query HDCP version of WFD
};

// Commands for Playready TA
typedef enum {
    TZCMD_PLAYREADY_BASE                             = 0x00000000,
    TZCMD_PLAYREADY_PROVISIONED_DATA_GET             = 0x00000001,
    TZCMD_PLAYREADY_ECCP256_KEYPAIR_GEN              = 0x00000002,
    TZCMD_PLAYREADY_ECCP256_KEY_SET                  = 0x00000003,
    TZCMD_PLAYREADY_ECDSAP256_SIGN                   = 0x00000004,
    TZCMD_PLAYREADY_ECCP256_DECRYPT                  = 0x00000005,
    TZCMD_PLAYREADY_OMAC1_KEY_SET                    = 0x00000006,
    TZCMD_PLAYREADY_OMAC1_VERIFY                     = 0x00000007,
    TZCMD_PLAYREADY_OMAC1_SIGN                       = 0x00000008,
    TZCMD_PLAYREADY_COPYBYTE                         = 0x00000009,
    TZCMD_PLAYREADY_CONTENTKEY_AESCTR_SET            = 0x0000000a,
    TZCMD_PLAYREADY_CONTENT_AESCTR_DECRYPT           = 0x0000000b,
    TZCMD_PLAYREADY_AESECB_KEY_SET                   = 0x0000000c,
    TZCMD_PLAYREADY_AESECB_ENCRYPT                   = 0x0000000d,
    TZCMD_PLAYREADY_AESECB_DECRYPT                   = 0x0000000e,
    TZCMD_PLAYREADY_GET_KFKEY                        = 0x0000000f,
    TZCMD_PLAYREADY_AESCBC_KEY_SET                   = 0x00000010,
    TZCMD_PLAYREADY_AESCBC_ENCRYPT                   = 0x00000011,
    TZCMD_PLAYREADY_AESCBC_DECRYPT                   = 0x00000012,
    TZCMD_PLAYREADY_HANDLE_CONTENT_AESCTR_DECRYPT    = 0x00000013,
    TZCMD_PLAYREADY_KEYFILE_DECRYPT                  = 0x00000014,
    TZCMD_PLAYREADY_KEYFILE_ENCRYPT                  = 0x00000015,
    TZCMD_PLAYREADY_TOKENTIME_COMPARE                = 0x00000016,
    TZCMD_PLAYREADY_TOKENTIME_UPDATE                 = 0x00000017,
    TZCMD_PLAYREADY_MACHINEDATETIME_CHECK            = 0x00000019,
    TZCMD_PLAYREADY_GET_HWID                         = 0x00000020,
    TZCMD_PLAYREADY_COPY_TO_SECURE_BUFFER            = 0x00000021,
    TZCMD_PLAYREADY_HDCP_VERSION_GET                 = 0x00000022,
} PlayReadyTACMDs;

// For customer
typedef enum {
    PR_TEE_SUCCESS          = 0,
    PR_TEE_BUFFERTOOSMALL   = -1,
    PR_TEE_INVALID_PARAM    = -2,
    PR_TEE_PROT_ENCERROR    = -3,
} PR_TEE_RESULT;

#ifndef DRM_ID_SIZE
#define DRM_ID_SIZE  16
#endif

/**********************************************************************
**
** Function:    _teePlayReady_GetUniqueID
**
** Synopsis:    Get Unique ID
**
** Arguments:
**
** [f_pbUniqueId]         -- output, buffer for Unique ID. size is DRM_ID_SIZE(16Byte).
** [f_pcbLength]          -- output, return the Unique ID size.
**
** Returns:                 PR_TEE_SUCCESS on success.
**                          PR_TEE_BUFFERTOOSMALL if the buffer of f_pbUniqueId is too small.
**                          PR_TEE_PROT_ENCERROR  if generate UID fail.
**
**********************************************************************/
extern int _teePlayReady_GetUniqueID(unsigned char *f_pbUniqueId, unsigned int *f_pcbLength);

#endif /* __TRUSTZONE_TA_PLAYREADY__ */
