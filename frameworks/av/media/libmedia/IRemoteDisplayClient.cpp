/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <sys/types.h>

#include <media/IRemoteDisplayClient.h>
#include <gui/IGraphicBufferProducer.h>
#include <utils/String8.h>

namespace android {

enum {
    ON_DISPLAY_CONNECTED = IBinder::FIRST_CALL_TRANSACTION,
    ON_DISPLAY_DISCONNECTED,
    ON_DISPLAY_ERROR,
#ifdef MTK_AOSP_ENHANCEMENT
    ON_DISPLAY_KEY_EVENT,
    ON_DISPLAY_RTSPGENERIC_EVENT,
#endif
};

class BpRemoteDisplayClient: public BpInterface<IRemoteDisplayClient>
{
public:
    explicit BpRemoteDisplayClient(const sp<IBinder>& impl)
        : BpInterface<IRemoteDisplayClient>(impl)
    {
    }

    void onDisplayConnected(const sp<IGraphicBufferProducer>& bufferProducer,
            uint32_t width, uint32_t height, uint32_t flags, uint32_t session)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRemoteDisplayClient::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(bufferProducer));
        data.writeInt32(width);
        data.writeInt32(height);
        data.writeInt32(flags);
        data.writeInt32(session);
        remote()->transact(ON_DISPLAY_CONNECTED, data, &reply, IBinder::FLAG_ONEWAY);
    }

    void onDisplayDisconnected()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRemoteDisplayClient::getInterfaceDescriptor());
        remote()->transact(ON_DISPLAY_DISCONNECTED, data, &reply, IBinder::FLAG_ONEWAY);
    }

    void onDisplayError(int32_t error)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRemoteDisplayClient::getInterfaceDescriptor());
        data.writeInt32(error);
        remote()->transact(ON_DISPLAY_ERROR, data, &reply, IBinder::FLAG_ONEWAY);
    }
#ifdef MTK_AOSP_ENHANCEMENT
    void onDisplayKeyEvent(uint32_t uniCode, uint32_t flags){
        Parcel data, reply;
        data.writeInterfaceToken(IRemoteDisplayClient::getInterfaceDescriptor());
        data.writeInt32(uniCode);
        data.writeInt32(flags);
        remote()->transact(ON_DISPLAY_KEY_EVENT, data, &reply, IBinder::FLAG_ONEWAY);
    }

    void onDisplayGenericMsgEvent(uint32_t event){
        Parcel data, reply;
        data.writeInterfaceToken(IRemoteDisplayClient::getInterfaceDescriptor());
        data.writeInt32(event);
        remote()->transact(ON_DISPLAY_RTSPGENERIC_EVENT, data, &reply, IBinder::FLAG_ONEWAY);
    }

#endif
};

IMPLEMENT_META_INTERFACE(RemoteDisplayClient, "android.media.IRemoteDisplayClient");

// ----------------------------------------------------------------------

status_t BnRemoteDisplayClient::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case ON_DISPLAY_CONNECTED: {
            CHECK_INTERFACE(IRemoteDisplayClient, data, reply);
            sp<IGraphicBufferProducer> surfaceTexture(
                    interface_cast<IGraphicBufferProducer>(data.readStrongBinder()));
            uint32_t width = data.readInt32();
            uint32_t height = data.readInt32();
            uint32_t flags = data.readInt32();
            uint32_t session = data.readInt32();
            onDisplayConnected(surfaceTexture, width, height, flags, session);
            return NO_ERROR;
        }
        case ON_DISPLAY_DISCONNECTED: {
            CHECK_INTERFACE(IRemoteDisplayClient, data, reply);
            onDisplayDisconnected();
            return NO_ERROR;
        }
        case ON_DISPLAY_ERROR: {
            CHECK_INTERFACE(IRemoteDisplayClient, data, reply);
            int32_t error = data.readInt32();
            onDisplayError(error);
            return NO_ERROR;
        }
#ifdef MTK_AOSP_ENHANCEMENT
        case ON_DISPLAY_KEY_EVENT: {
            CHECK_INTERFACE(IRemoteDisplayClient, data, reply);
            uint32_t uniCode = data.readInt32();
            uint32_t flags = data.readInt32();
            onDisplayKeyEvent(uniCode, flags);
            return NO_ERROR;
        }
        case ON_DISPLAY_RTSPGENERIC_EVENT: {
            CHECK_INTERFACE(IRemoteDisplayClient, data, reply);
            uint32_t event = data.readInt32();
            onDisplayGenericMsgEvent(event);
            return NO_ERROR;
        }
#endif
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

} // namespace android
