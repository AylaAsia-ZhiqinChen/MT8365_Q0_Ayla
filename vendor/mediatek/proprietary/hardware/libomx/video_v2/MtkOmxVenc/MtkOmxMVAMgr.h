#ifndef MTK_OMX_MVA_MGR
#define MTK_OMX_MVA_MGR

#include <pthread.h>

#include "osal_utils.h"
#include "MtkOmxBase.h"

#include "val_api_public.h"
#include "venc_drv_if_public.h"
#include "vdec_drv_if_public.h"
//#include "../MtkOmxVdecEx/MtkOmxVdecEx.h"

#include "properties.h"

typedef struct _VBufInfo
{
    VAL_UINT32_T u4OriVA;
    VAL_UINT32_T u4BuffHdr;
    VAL_UINT32_T u4VA;
    VAL_UINT32_T u4PA;
    VAL_UINT32_T u4BuffSize;
    void *pNativeHandle;

    unsigned long secure_handle;
    // for ion entry need return shared fd
    int         iIonFd;
} VBufInfo;

//-------- MtkOmxMVAMap --------
struct OmxMVAEntry
{
        void        *va;
        void        *pa;
        uint32_t    size;
        void        *bufHdr;
        void        *handle;
        bool        allocated;
        unsigned long rSecMemHandle;

        OmxMVAEntry() { va = NULL; pa = NULL; size = 0; bufHdr = NULL; handle = NULL; allocated = false; rSecMemHandle = NULL; }
        virtual ~OmxMVAEntry() {}

        virtual int setToVencBS(VENC_DRV_PARAM_BS_BUF_T *bsBuf);
        virtual int setToVencFrm(VENC_DRV_PARAM_FRM_BUF_T *frmBuf);
        virtual int setToVBufInfo(VBufInfo *bufInfo);

    private:
        OmxMVAEntry(const OmxMVAEntry &);
        OmxMVAEntry &operator=(const OmxMVAEntry &);
};

// the interface of mva map
struct OmxMVAMap
{
        OmxMVAMap() {}
        virtual ~OmxMVAMap() {}

        virtual int setBoolProperty(const char *key, OMX_BOOL value) = 0;
        //virtual int setCharProperty(const char *key, const char *value) = 0;
        virtual void *newEntry(int align, int size, void *bufHdr) = 0;
        virtual void *newEntryWithVa(void *va, int size, void *bufHdr) = 0;
        virtual void *newEntryWithHndl(void *handle, void *bufHdr) = 0;
        virtual void *newEntryWithHndlwithSize(void *handle, void *bufHdr, unsigned int desireSize) = 0;
        virtual void freeEntry(void *entry) = 0;
        virtual void dumpEntry(void *entry) { (void)entry; }
        virtual int getIonFd() { return 0; }

    private:
        OmxMVAMap(const OmxMVAMap &);
        OmxMVAMap &operator=(const OmxMVAMap &);
};

// ion map [
struct OmxIonMVAEntry : public OmxMVAEntry
{
        int                 fd;
        int                 srcFd;
        ion_user_handle_t   ionBufHndl;
        int                 ionDevFd;

        OmxIonMVAEntry() { fd = -1; srcFd = -1; ionBufHndl = NULL; ionDevFd = -1;/* ALOGD("c entry 0x%x", this); */ }
        virtual ~OmxIonMVAEntry() { /* ALOGD("r entry 0x%x", this); */ }

        int setToVencBS(VENC_DRV_PARAM_BS_BUF_T *bsBuf);
        int setToVencFrm(VENC_DRV_PARAM_FRM_BUF_T *frmBuf);
        int setToVBufInfo(VBufInfo *bufInfo);

    private:
        OmxIonMVAEntry(const OmxIonMVAEntry &);
        OmxIonMVAEntry &operator=(const OmxIonMVAEntry &);
};
struct OmxIonMVAMap : public OmxMVAMap
{
        OmxIonMVAMap(const char *ionName=NULL);
        //OmxIonMVAMap(const char *ionName);
        virtual ~OmxIonMVAMap();

        virtual int setBoolProperty(const char *key, OMX_BOOL value);
        //virtual int setCharProperty(const char *key, const char *value);
        virtual void *newEntry(int align, int size, void *bufHdr);
        virtual void *newEntryWithVa(void *va, int size, void *bufHdr);
        virtual void *newEntryWithHndl(void *handle, void *bufHdr);
        virtual void *newEntryWithHndlwithSize(void *handle, void *bufHdr, unsigned int desireSize);
        virtual void freeEntry(void *entry);
        virtual void dumpEntry(void *entry);
        int getIonFd();

    private:
        int mIonDevFd;
        static int mIonHandleCount;
        OMX_BOOL mIsSecure;
        OMX_BOOL mIsMCIMode;
        OMX_BOOL mIsClientLocally;
        OMX_BOOL mIsVdec;

        bool configIonBuffer(int ionFd, ion_user_handle_t handle);
        uint32_t getIonPhysicalAddress(int ionFd, ion_user_handle_t handle);
        int mapIon(OmxIonMVAEntry *pItem, int align);

        OmxIonMVAMap(const OmxIonMVAMap &);
        OmxIonMVAMap &operator=(const OmxIonMVAMap &);
};
// ] (ion map)

//-------- MtkOmxMVAMgr --------
struct OmxMVAManager
{
        OmxMVAManager();
        OmxMVAManager(const char *type);
        OmxMVAManager(const char *type, const char *String);
        virtual ~OmxMVAManager();

        inline const char *getType() { return mType; }
        inline uint32_t count() { return mEntryList.size(); }
        int setBoolProperty(const char *key, OMX_BOOL value);

        int newOmxMVAwithVA(void *va, int size, void *bufHdr);
        int newOmxMVAandVA(int align, int size, void *bufHdr, void **pVa);
        int newOmxMVAwithHndl(void *handle, void *bufHdr);
        int newOmxMVAwithHndlwithSize(void *handle, void *bufHdr, unsigned int desireSize);

        int freeOmxMVAByVa(void *va);
        int freeOmxMVAByHndl(void *handle);
        int freeOmxMVAAll(void);

        //deprecated, should call getOmxInfoFromVA
        int getOmxMVAFromVAToVencBS(void *va, VENC_DRV_PARAM_BS_BUF_T *bsBuf);
        int getOmxMVAFromVAToVencFrm(void *va, VENC_DRV_PARAM_FRM_BUF_T *frmBuf);
        int getOmxMVAFromVA(void *va, uint32_t *mva);

        int getOmxInfoFromVA(void* va, VBufInfo* info);
        int getOmxInfoFromHndl(void *handle, VBufInfo* info);

        //deprecated, should call getOmxInfoFromHndl
        int getOmxMVAFromHndlToVencBS(void *handle, VENC_DRV_PARAM_BS_BUF_T *bsBuf);
        int getOmxMVAFromHndlToVencFrm(void *handle, VENC_DRV_PARAM_FRM_BUF_T *frmBuf);
        //int getMapHndlFromIndex(int index, void *handle);
        //int getMapIndexFromBufHdr(void *bufHdr);

        void syncBufferCacheFrm(OMX_U8* buffer);
        void syncBufferCacheFrm(OMX_U8* buffer, bool flushAll);
        //int setSecHandleFromVa(void *va, unsigned long secHandle);
        //int setBufHdrFromVa(void *va, void *bufHdr);

        static OMX_U8 mEnableMoreLog;

    private:
        void init(const char *type, const char *ionName=NULL);
        //void init(const char *type, const char *String);
        int getMapIndexFromVa(void *va, bool lock);
        int getMapIndexFromHndl(void *handle, bool lock);

        OmxMVAEntry* getMapFromVa(void *va);
        OmxMVAEntry* getMapFromHndl(void *handle);


        char                    *mType;
        OmxMVAMap               *mMap;
        Vector<OmxMVAEntry *>   mEntryList;
        pthread_mutex_t mEntryListLock;

        void dumpList();

        OmxMVAManager(const OmxMVAManager &);
        OmxMVAManager &operator=(const OmxMVAManager &);
};

#endif//MTK_OMX_MVA_MGR

