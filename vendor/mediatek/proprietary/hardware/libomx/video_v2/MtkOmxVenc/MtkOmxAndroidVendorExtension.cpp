
#include "MtkOmxAndroidVendorExtension.h"
#include <stdio.h>
#include <string.h>

OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*
AndroidVendorConfigExtensionFactory::createAndroidVendorExtension(
    const char* extensionKey,
    std::initializer_list<AndroidVendorConfigExtensionFactory::ParamDescribtor> pParams)
{
    typedef AndroidVendorConfigExtensionFactory::ParamDescribtor ParamDescribtor;
    int paramSize = pParams.size();

    int backedSize =
        sizeof(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE)
        - (paramSize-1)*sizeof(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE::param);

    OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* ret =
        (OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*) malloc(backedSize);
    if (ret == NULL)
    {
        return NULL;
    }
    memset(ret, 0, backedSize);

    strncpy((char*)(&ret->cName), extensionKey, strlen(extensionKey));
    ret->nParamCount = paramSize;
    ret->nParamSizeUsed = paramSize; //(must be at least 1 and at most OMX_MAX_ANDROID_VENDOR_PARAMCOUNT)

    std::initializer_list<ParamDescribtor>::iterator it;
    int i;
    for(i=0, it = pParams.begin(); it!=pParams.end(); ++it, ++i)
    {
        const ParamDescribtor curr = *it;
        ret->param[i].eValueType = curr.type;

        int paramKeyLen = strlen(curr.key);
        strncpy((char*)(&ret->param[i].cKey), curr.key, paramKeyLen);
    }

    return ret;
}

AndroidVendorConfigExtensionStore::AndroidVendorConfigExtensionStore()
{
    INIT_MUTEX(mMutex);
}
AndroidVendorConfigExtensionStore::~AndroidVendorConfigExtensionStore()
{
    DESTROY_MUTEX(mMutex);
}

int AndroidVendorConfigExtensionStore::registerExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* ext)
{
    LOCK(mMutex);
    int currentIndex = mRegisteredExtensions.size();

    ext->nIndex = currentIndex;
    mRegisteredExtensions.push_back(ext);

    UNLOCK(mMutex);

    return currentIndex;
}

OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*
AndroidVendorConfigExtensionStore::getRegisteredExtension(unsigned int index)
{
    OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* ret = NULL;
    LOCK(mMutex);
    if(index < mRegisteredExtensions.size())
    {
        ret = mRegisteredExtensions[index];
    }
    UNLOCK(mMutex);

    return ret;
}
OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*
AndroidVendorConfigExtensionStore::getRegisteredExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* ext)
{
    return getRegisteredExtension(ext->nIndex);
}

bool AndroidVendorConfigExtensionStore::isIndexRegistered(unsigned int index)
{
    LOCK(mMutex);
    int currentIndex = mRegisteredExtensions.size();
    UNLOCK(mMutex);

    if(index < currentIndex)
    {
        return true;
    }

    return false;
}
bool AndroidVendorConfigExtensionStore::isIndexRegistered(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* ext)
{
    return isIndexRegistered(ext->nIndex);
}