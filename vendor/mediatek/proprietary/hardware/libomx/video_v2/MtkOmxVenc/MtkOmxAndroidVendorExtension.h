#ifndef __MTK_OMX_ANDROID_VENDOR_EXT_H__
#define __MTK_OMX_ANDROID_VENDOR_EXT_H__

#include <log/log.h>
#include <cutils/properties.h>
#include <utils/Vector.h>
#include <vector>
#include <initializer_list>

#include "osal_utils.h"
#include "OMX_IndexExt.h"

class AndroidVendorConfigExtensionFactory
{
public:
    typedef struct {
        const char* key;
        OMX_ANDROID_VENDOR_VALUETYPE type;
    } ParamDescribtor;

    static OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*
    createAndroidVendorExtension(
        const char* extensionKey,
        std::initializer_list<ParamDescribtor> pParams );
};

class AndroidVendorConfigExtensionStore
{
public:
    AndroidVendorConfigExtensionStore();
    ~AndroidVendorConfigExtensionStore();

    int registerExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*);

    OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* getRegisteredExtension(unsigned int);
    OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* getRegisteredExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*);

    bool isIndexRegistered(unsigned int);
    bool isIndexRegistered(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*);

private:
    pthread_mutex_t mMutex;
    std::vector<OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*> mRegisteredExtensions;
};

#endif