/*
 * Copyright (C) 2013 The Android Open Source Project
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


#define LOG_TAG "BluetoothMeshServiceJni"

#define LOG_NDEBUG 0

#include "com_android_bluetooth.h"
#include "hardware/bt_mesh.h"
#include "hardware/bt_mesh_param.h"
#include "utils/Log.h"
#include "android_runtime/AndroidRuntime.h"

#include <string.h>

#include <cutils/log.h>
#define info(fmt, ...)  ALOGI ("%s(L%d): " fmt,__func__, __LINE__,  ## __VA_ARGS__)
#define debug(fmt, ...) ALOGD ("%s(L%d): " fmt,__func__, __LINE__,  ## __VA_ARGS__)
#define warn(fmt, ...) ALOGW ("WARNING: %s(L%d): " fmt "##",__func__, __LINE__, ## __VA_ARGS__)
#define error(fmt, ...) ALOGE ("ERROR: %s(L%d): " fmt "##",__func__, __LINE__, ## __VA_ARGS__)
#define asrt(s) if(!(s)) ALOGE ("%s(L%d): ASSERT %s failed! ##",__func__, __LINE__, #s)

#define MSGOPCODE_PARAMS(msg_opcode) msg_opcode.opcode, msg_opcode.company_id

#define METADATA_PARAMS(meta_data) meta_data.src_addr, meta_data.dst_addr, \
    meta_data.appkey_index, meta_data.netkey_index, meta_data.rssi, meta_data.ttl

#define INIT_PROVISIONEE_PARAMS_SIZE 8
#define INIT_FRIEND_PARAMS_SIZE 3
#define INIT_CUSTOMIZE_PARAMS_SIZE 2


namespace android {
static jmethodID method_onMeshEnabled;
static jmethodID method_onConfigReset;
static jmethodID method_onFriendShipStatus;
static jmethodID method_onOTAEvent;
static jmethodID method_onProvScanComplete;
static jmethodID method_onProvScanResult;
static jmethodID method_onProvCapabilities;
static jmethodID method_onRequestOobPublicKey;
static jmethodID method_onRequestOobAuthValue;
static jmethodID method_onProvShowOobPublicKey;
static jmethodID method_onProvShowOobAuthValue;
static jmethodID method_onProvisionDone;
static jmethodID method_onAdvReport;
static jmethodID method_onKeyRefresh;
static jmethodID method_onIvUpdate;
static jmethodID method_onSeqChange;
static jmethodID method_onProvFactor;
static jmethodID method_onHeartbeat;
static jmethodID method_onBearerGattStatus;
static jmethodID method_onEvtErrorCode;
static jmethodID method_onMsgHandler;
static jmethodID method_onPublishTimeoutCallback;
static jmethodID method_onOTAMsgHandler;
static jmethodID method_CreateAccessMessageRxObject;


/**
 * Static variables
 */
static const btmesh_interface_t *sMeshIf = NULL;
static jobject mCallbacksObj = NULL;
static JNIEnv *sCallbackEnv = NULL;


/**
 * MESH callbacks
 */
static void btmesh_event_callbacks(meshif_bt_evt_t *evt)
{
    jint i;
    jint *p_value;
    sCallbackEnv = AndroidRuntime::getJNIEnv();
    ALOGI("%s: sCallbackEnv: %p", __func__, sCallbackEnv);

    if (NULL == evt)
    {
        ALOGE("btmesh_event_callbacks Event is NULL");
        return;
    }
    ALOGI("%s: evt_id:  %d ", __func__, evt->evt_id);
    switch (evt->evt_id)
    {
        case MESHIF_EVT_PROV_CAPABILITIES:
        {
            jint number_of_elements, algorithms, public_key_type, static_oob_type;
            jint output_oob_size, output_oob_action, input_oob_size, input_oob_action;

            number_of_elements = evt->evt.mesh_evt.mesh.prov_cap.cap.number_of_elements;
            algorithms = evt->evt.mesh_evt.mesh.prov_cap.cap.algorithms;
            public_key_type = evt->evt.mesh_evt.mesh.prov_cap.cap.public_key_type;
            static_oob_type = evt->evt.mesh_evt.mesh.prov_cap.cap.static_oob_type;
            output_oob_size = evt->evt.mesh_evt.mesh.prov_cap.cap.output_oob_size;
            output_oob_action = evt->evt.mesh_evt.mesh.prov_cap.cap.output_oob_action;
            input_oob_size = evt->evt.mesh_evt.mesh.prov_cap.cap.input_oob_size;
            input_oob_action = evt->evt.mesh_evt.mesh.prov_cap.cap.input_oob_action;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvCapabilities, number_of_elements, algorithms, public_key_type, static_oob_type,
                output_oob_size, output_oob_action, input_oob_size, input_oob_action);
            break;
        }
        case MESHIF_EVT_PROV_REQUEST_OOB_PUBLIC_KEY:
        {
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onRequestOobPublicKey);
            break;
        }
        case MESHIF_EVT_PROV_REQUEST_OOB_AUTH_VALUE:
        {
            jint action;
            jint method;
            jint size;
            action = evt->evt.mesh_evt.mesh.prov_request_auth.action;
            method = evt->evt.mesh_evt.mesh.prov_request_auth.method;
            size = evt->evt.mesh_evt.mesh.prov_request_auth.size;
            ALOGI("%s: MESHIF_EVT_PROV_REQUEST_OOB_AUTH_VALUE:  method %d, action %d ,  size %d", __func__,  method, action, size);
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onRequestOobAuthValue, method, action, size);
            break;
        }
        case MESHIF_EVT_PROV_SHOW_OOB_PUBLIC_KEY:
        {
            jintArray public_key;
            int key[MESHIF_PUBLIC_KEY_SIZE];

            public_key = sCallbackEnv->NewIntArray(MESHIF_PUBLIC_KEY_SIZE);
            p_value = sCallbackEnv->GetIntArrayElements(public_key, NULL);

            for(i=0;i<MESHIF_PUBLIC_KEY_SIZE;i++)
                key[i] = evt->evt.mesh_evt.mesh.prov_show_pk.pk[i];

            sCallbackEnv->SetIntArrayRegion(public_key, 0, MESHIF_PUBLIC_KEY_SIZE, key);

            ALOGI("%s: MESHIF_EVT_PROV_SHOW_OOB_PUBLIC_KEY: SDK:  key[0] = %d, key[1] = %d ; Service key[0] = %d, key[1] = %d",
                __func__, evt->evt.mesh_evt.mesh.prov_show_pk.pk[0], evt->evt.mesh_evt.mesh.prov_show_pk.pk[1],
                p_value[0], p_value[1]);

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvShowOobPublicKey, public_key);
            sCallbackEnv->DeleteLocalRef(public_key);
            break;
        }
        case MESHIF_EVT_PROV_SHOW_OOB_AUTH_VALUE:
        {
            jintArray auth_key;
            int key[MESHIF_AUTHENTICATION_SIZE];

            auth_key = sCallbackEnv->NewIntArray(MESHIF_AUTHENTICATION_SIZE);
            p_value = sCallbackEnv->GetIntArrayElements(auth_key, NULL);

            for(i=0;i<MESHIF_AUTHENTICATION_SIZE;i++)
                key[i] = evt->evt.mesh_evt.mesh.prov_show_auth.auth[i];
            sCallbackEnv->SetIntArrayRegion(auth_key, 0, MESHIF_AUTHENTICATION_SIZE, key);

            ALOGI("%s: MESHIF_EVT_PROV_SHOW_OOB_AUTH_VALUE: SDK:  key[0] = %d, key[1] = %d ; Service key[0] = %d, key[1] = %d",
                __func__, evt->evt.mesh_evt.mesh.prov_show_auth.auth[0], evt->evt.mesh_evt.mesh.prov_show_auth.auth[1],
                p_value[0], p_value[1]);

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvShowOobAuthValue, auth_key);
            sCallbackEnv->DeleteLocalRef(auth_key);
            break;
        }
        case MESHIF_EVT_PROV_DONE:
        {
            jintArray device_key;
            jint address;
            jboolean success;
            jboolean gatt_bearer;
            int key[MESHIF_DEVKEY_SIZE];

            device_key = sCallbackEnv->NewIntArray(MESHIF_DEVKEY_SIZE);
            p_value = sCallbackEnv->GetIntArrayElements(device_key, NULL);
            for(i=0;i<MESHIF_DEVKEY_SIZE;i++)
                key[i] = evt->evt.mesh_evt.mesh.prov_done.device_key[i];

            sCallbackEnv->SetIntArrayRegion(device_key, 0, MESHIF_DEVKEY_SIZE, key);

            ALOGI("%s: MESHIF_EVT_PROV_DONE: SDK:  key[0] = %d, key[1] = %d ; Service key[0] = %d, key[1] = %d",
                __func__, evt->evt.mesh_evt.mesh.prov_done.device_key[0], evt->evt.mesh_evt.mesh.prov_done.device_key[1],
                p_value[0], p_value[1]);

            address = evt->evt.mesh_evt.mesh.prov_done.address;
            success = evt->evt.mesh_evt.mesh.prov_done.success;
            gatt_bearer = evt->evt.mesh_evt.mesh.prov_done.gatt_bearer;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvisionDone, device_key, address, success, gatt_bearer);
            sCallbackEnv->DeleteLocalRef(device_key);
            break;
        }
        case MESHIF_EVT_PROV_SCAN_UD_RESULT:
        {
            jintArray device_uuid;
            jintArray uri_hash;
            jint oob_info;
            int uuid[MESHIF_UUID_SIZE];
            int uri[MESHIF_URI_HASH_LEN];

            device_uuid = sCallbackEnv->NewIntArray(MESHIF_UUID_SIZE);
            p_value = sCallbackEnv->GetIntArrayElements(device_uuid, NULL);

            for(i=0;i<MESHIF_UUID_SIZE;i++)
                uuid[i] = evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uuid[i];

            sCallbackEnv->SetIntArrayRegion(device_uuid, 0, MESHIF_UUID_SIZE, uuid);

            ALOGI("%s: MESHIF_EVT_PROV_SCAN_UD_RESULT: SDK:  uuid[0] = %d, uuid[1] = %d ; Service uuid[0] = %d, uuid[1] = %d",
                __func__, evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uuid[0], evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uuid[1],
                p_value[0], p_value[1]);

            oob_info = evt->evt.mesh_evt.mesh.prov_scan_ud.ud.oob_info;
            ALOGI("%s: MESHIF_EVT_PROV_SCAN_UD_RESULT: SDK:  oob_info = %d uri_hash_avaliable = %d", __func__,
                oob_info, evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uri_hash_avaliable);

            uri_hash = sCallbackEnv->NewIntArray(MESHIF_URI_HASH_LEN);
            if(evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uri_hash_avaliable) {
                p_value = sCallbackEnv->GetIntArrayElements(uri_hash, NULL);
                for(i=0;i<MESHIF_URI_HASH_LEN;i++)
                    uri[i] = evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uri_hash[i];
                sCallbackEnv->SetIntArrayRegion(uri_hash, 0, MESHIF_URI_HASH_LEN, uri);
                ALOGI("%s: MESHIF_EVT_PROV_SCAN_UD_RESULT: SDK:  uri_hash[0] = %d, uri_hash[1] = %d ; Service uri_hash[0] = %d, uri_hash[1] = %d",
                    __func__, evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uri_hash[0], evt->evt.mesh_evt.mesh.prov_scan_ud.ud.uri_hash[1],
                    p_value[0], p_value[1]);
            }
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvScanResult, device_uuid, oob_info, uri_hash);
            sCallbackEnv->DeleteLocalRef(uri_hash);
            sCallbackEnv->DeleteLocalRef(device_uuid);
            break;
        }
        case MESHIF_EVT_PROV_FACTOR:
        {
            jint type = evt->evt.mesh_evt.mesh.prov_factor.type;
            jint buf_len = evt->evt.mesh_evt.mesh.prov_factor.buf_len;
            jintArray buf;
            jint data[buf_len];

            buf = sCallbackEnv->NewIntArray(buf_len);
            p_value = sCallbackEnv->GetIntArrayElements(buf, NULL);
            for(i=0;i<buf_len;i++)
                data[i] = evt->evt.mesh_evt.mesh.prov_factor.buf[i];

            sCallbackEnv->SetIntArrayRegion(buf, 0, buf_len, data);
            ALOGI("%s: MESHIF_EVT_PROV_FACTOR: SDK:  buf[0] = %d, buf[1] = %d ; Service buf[0] = %d, buf[1] = %d",
                __func__, evt->evt.mesh_evt.mesh.prov_factor.buf[0], evt->evt.mesh_evt.mesh.prov_factor.buf[1],
                p_value[0], p_value[1]);

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvFactor, type, buf, buf_len);
            sCallbackEnv->DeleteLocalRef(buf);
            break;
        }
        case MESHIF_EVT_ADV_REPORT:
        {
            jintArray addr_array;
            jint addr_type = evt->evt.mesh_evt.mesh.adv_report.peer_addr.addr_type;
            jintArray data_array;
            jint rssi = evt->evt.mesh_evt.mesh.adv_report.rssi;
            jint report_type = evt->evt.mesh_evt.mesh.adv_report.type;
            jint len = evt->evt.mesh_evt.mesh.adv_report.dlen;
            int addr[MESHIF_BLE_ADDR_LEN];
            int data[len];

            addr_array = sCallbackEnv->NewIntArray(MESHIF_BLE_ADDR_LEN);
            p_value = sCallbackEnv->GetIntArrayElements(addr_array, NULL);
            for(i=0;i<MESHIF_BLE_ADDR_LEN;i++)
                addr[i] = evt->evt.mesh_evt.mesh.adv_report.peer_addr.addr[i];

            sCallbackEnv->SetIntArrayRegion(addr_array, 0, MESHIF_BLE_ADDR_LEN, addr);

            ALOGI("%s: MESHIF_EVT_ADV_REPORT: SDK addr[0] = %d, addr[1] = %d ; Service addr[0] = %d, addr[1] = %d", __func__,
                evt->evt.mesh_evt.mesh.adv_report.peer_addr.addr[0], evt->evt.mesh_evt.mesh.adv_report.peer_addr.addr[1],
                p_value[0], p_value[1]);

            data_array = sCallbackEnv->NewIntArray(evt->evt.mesh_evt.mesh.adv_report.dlen);
            p_value = sCallbackEnv->GetIntArrayElements(data_array, NULL);
            for(i=0;i<evt->evt.mesh_evt.mesh.adv_report.dlen;i++)
                data[i] = evt->evt.mesh_evt.mesh.adv_report.data[i];

            sCallbackEnv->SetIntArrayRegion(data_array, 0, len, data);

            ALOGI("%s: MESHIF_EVT_ADV_REPORT: SDK data[0] = %d, data[1] = %d, Service data[0] = %d, data[1] = %d", __func__,
                evt->evt.mesh_evt.mesh.adv_report.data[0], evt->evt.mesh_evt.mesh.adv_report.data[1], data[0], data[1]);

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onAdvReport, addr_type, addr_array, rssi, report_type, data_array);
            sCallbackEnv->DeleteLocalRef(addr_array);
            sCallbackEnv->DeleteLocalRef(data_array);
            break;
        }
        case MESHIF_EVT_KEY_REFRESH:
        {
            jint netkey_index = evt->evt.mesh_evt.mesh.key_refresh.netkey_index;
            jint phase = evt->evt.mesh_evt.mesh.key_refresh.phase;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onKeyRefresh, netkey_index, phase);
            break;
        }
        case MESHIF_EVT_IV_UPDATE:
        {
            jint iv_index = evt->evt.mesh_evt.mesh.iv_update.iv_index;
            jint state = evt->evt.mesh_evt.mesh.iv_update.state;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onIvUpdate, iv_index, state);
            break;
        }
        case MESHIF_EVT_SEQ_CHANGE:
        {
            jint seq_num = evt->evt.mesh_evt.mesh.seq_change.seq_num;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onSeqChange, seq_num);
            break;
        }
        case MESHIF_EVT_HEARTBEAT:
        {
            jint address = evt->evt.mesh_evt.mesh.heartbeat.address;
            jint active = evt->evt.mesh_evt.mesh.heartbeat.active;
            ALOGI("%s: MESHIF_EVT_HEARTBEAT: address %d, active %d", __func__, address, active);
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onHeartbeat, address, active);
            break;
        }
        case MESHIF_EVT_INIT_DONE:
        {
            ALOGD("%s: MESHIF_EVT_INIT_DONE, call onMeshEnabled", __func__);
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onMeshEnabled);
            break;
        }
        case MESHIF_EVT_CONFIG_RESET:
        {
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onConfigReset);
            break;
        }
        case MESHIF_EVT_FRIENDSHIP_STATUS:
        {
            jint addr = evt->evt.mesh_evt.mesh.friendship_status.address;
            jint status = evt->evt.mesh_evt.mesh.friendship_status.status;

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onFriendShipStatus, addr, status);
            break;
        }
        case MESHIF_EVT_OTA_EVENT:
        {
            jint i = 0;
            jint event_id =  evt->evt.mesh_evt.mesh.ota_evt.event_id;
            jint error_code = evt->evt.mesh_evt.mesh.ota_evt.error_code;
            jlong serial_number = evt->evt.mesh_evt.mesh.ota_evt.serial_number;
            jlong firmware_id = evt->evt.mesh_evt.mesh.ota_evt.firmware_id;
            jlong time_escaped = evt->evt.mesh_evt.mesh.ota_evt.time_escaped;
            jint nodes_num = evt->evt.mesh_evt.mesh.ota_evt.nodes_num;
            jint len = evt->evt.mesh_evt.mesh.ota_evt.nodes_num * 2;
            jint curr_block = evt->evt.mesh_evt.mesh.ota_evt.curr_block;
            jint total_block = evt->evt.mesh_evt.mesh.ota_evt.total_block;
            jint curr_chunk = evt->evt.mesh_evt.mesh.ota_evt.curr_chunk;
            jint chunk_mask = evt->evt.mesh_evt.mesh.ota_evt.chunk_mask;

            jintArray nodes_status = sCallbackEnv->NewIntArray(len);
            int nodes_data[len];
            for (i=0; i< len; i=i+2) {
                nodes_data[i] = evt->evt.mesh_evt.mesh.ota_evt.nodes_status[i/2].addr;
                nodes_data[i+1] = evt->evt.mesh_evt.mesh.ota_evt.nodes_status[i/2].status;
            }

            sCallbackEnv->SetIntArrayRegion(nodes_status, 0, len, nodes_data);

            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onOTAEvent, event_id, error_code, serial_number,
                firmware_id, time_escaped, nodes_num, curr_block, total_block, curr_chunk, chunk_mask, nodes_status);
            sCallbackEnv->DeleteLocalRef(nodes_status);
            break;
        }
        case MESHIF_EVT_PROV_UD_RESULT_COMPLETE:
        {
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onProvScanComplete);
            break;
        }
        case MESHIF_EVT_BEARER_GATT_STATUS:
        {
            jlong handle = evt->evt.mesh_evt.mesh.bearer_gatt_status.handle;
            jint status = evt->evt.mesh_evt.mesh.bearer_gatt_status.status;
            //for GATT disconnect and then connect immediately case
            usleep(100 * 1000);
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onBearerGattStatus, handle, status);
            break;
        }
        case MESHIF_EVT_ERROR_CODE:
        {
            jint type = evt->evt.mesh_evt.mesh.error_code.type;
            sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onEvtErrorCode, type);
            break;
        }
        default:
        {
            //We cannot return here. There is no data for these event, but they have to be notified to upper layer.
            break;
        }
    }
    free(evt);
}

void btmesh_ota_msg_handler_cb(uint16_t model_handle,
                             const meshif_access_message_rx_t *msg, const void *arg) {
    sCallbackEnv = AndroidRuntime::getJNIEnv();
    ALOGI("%s: sCallbackEnv: %p", __func__, sCallbackEnv);
    ALOGI("%s: model_handle: %d, buf_len: %d", __func__, model_handle, msg->buf_len);

    jint jbuf_len = (jint) msg->buf_len;
    jint i;
    int buf[jbuf_len];
    jintArray jbuf_array = NULL;

    if (jbuf_len != 0) {
        ALOGW("%s: msg buf len not 0", __func__);
        jbuf_array = sCallbackEnv->NewIntArray(jbuf_len);
        jint *p_value = sCallbackEnv->GetIntArrayElements(jbuf_array, NULL);
        for(i=0;i<jbuf_len;i++)
            buf[i] = msg->buf[i];

        sCallbackEnv->SetIntArrayRegion(jbuf_array, 0, jbuf_len, buf);

        ALOGI("%s: SDK:  buf[0] = %d, buf[1] = %d ; Service buf[0] = %d, buf[1] = %d",
          __func__, msg->buf[0], msg->buf[1], p_value[0], p_value[1]);
    }

    ScopedLocalRef<jobject> msgrx_obj(
        sCallbackEnv,
        sCallbackEnv->CallObjectMethod(
            mCallbacksObj, method_CreateAccessMessageRxObject,
            msg->opcode.opcode, msg->opcode.company_id,
            jbuf_array, msg->buf_len,
            msg->meta_data.src_addr, msg->meta_data.dst_addr,
            msg->meta_data.appkey_index, msg->meta_data.netkey_index,
            msg->meta_data.rssi, msg->meta_data.ttl));

    if (NULL != msgrx_obj.get()) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onOTAMsgHandler,
                                 model_handle, msgrx_obj.get());
    }

    if (jbuf_array != NULL) sCallbackEnv->DeleteLocalRef(jbuf_array);
}

void btmesh_access_msg_handler_cb(uint16_t model_handle,
                             const meshif_access_message_rx_t *msg, const void *arg) {
    sCallbackEnv = AndroidRuntime::getJNIEnv();
    ALOGI("%s: sCallbackEnv: %p", __func__, sCallbackEnv);
    ALOGI("%s: model_handle: %d, buf_len: %d", __func__, model_handle, msg->buf_len);

    jint jbuf_len = (jint) msg->buf_len;
    jint i;
    int buf[jbuf_len];
    jintArray jbuf_array = NULL;

    if (jbuf_len != 0) {
        ALOGW("%s: msg buf len %d", __func__, jbuf_len);
        jbuf_array = sCallbackEnv->NewIntArray(jbuf_len);

        for(i=0;i<jbuf_len;i++)
            buf[i] = msg->buf[i];

        sCallbackEnv->SetIntArrayRegion(jbuf_array, 0, jbuf_len, buf);
    }

    ScopedLocalRef<jobject> msgrx_obj(
        sCallbackEnv,
        sCallbackEnv->CallObjectMethod(
            mCallbacksObj, method_CreateAccessMessageRxObject,
            msg->opcode.opcode, msg->opcode.company_id,
            jbuf_array, msg->buf_len,
            msg->meta_data.src_addr, msg->meta_data.dst_addr,
            msg->meta_data.appkey_index, msg->meta_data.netkey_index,
            msg->meta_data.rssi, msg->meta_data.ttl));

    if (NULL != msgrx_obj.get()) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onMsgHandler,
                                 model_handle, msgrx_obj.get());
    }

    if (jbuf_array != NULL) sCallbackEnv->DeleteLocalRef(jbuf_array);
}

void btmesh_access_publish_timeout_cb(uint16_t model_handle, void * arg) {
    sCallbackEnv = AndroidRuntime::getJNIEnv();
    ALOGI("%s: sCallbackEnv: %p", __func__, sCallbackEnv);

    ALOGI("%s: model_handle:  %d ", __func__, model_handle);
    sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onPublishTimeoutCallback, model_handle);
}

void btmesh_health_client_evt_handler_cb(uint16_t model_handle,
                     const meshif_access_message_rx_t *msg, const meshif_health_client_evt_t *event) {
    btmesh_access_msg_handler_cb(model_handle, msg, NULL);
}

/**
 * Native function definitions
 */
static void classInitNative(JNIEnv* env, jclass clazz) {

    method_onMeshEnabled = env->GetMethodID(clazz, "onMeshEnabled", "()V");
    method_onConfigReset = env->GetMethodID(clazz, "onConfigReset", "()V");
    method_onFriendShipStatus = env->GetMethodID(clazz, "onFriendShipStatus", "(II)V");
    method_onOTAEvent = env->GetMethodID(clazz, "onOTAEvent", "(IIJJJIIIII[I)V");
    method_onProvScanComplete = env->GetMethodID(clazz, "onProvScanComplete", "()V");
    method_onProvScanResult = env->GetMethodID(clazz, "onProvScanResult", "([II[I)V");
    method_onProvCapabilities = env->GetMethodID(clazz, "onProvCapabilities", "(IIIIIIII)V");
    method_onRequestOobPublicKey = env->GetMethodID(clazz, "onRequestOobPublicKey", "()V");
    method_onRequestOobAuthValue = env->GetMethodID(clazz, "onRequestOobAuthValue", "(III)V");
    method_onProvShowOobPublicKey = env->GetMethodID(clazz, "onProvShowOobPublicKey", "([I)V");
    method_onProvShowOobAuthValue = env->GetMethodID(clazz, "onProvShowOobAuthValue", "([I)V");
    method_onProvisionDone = env->GetMethodID(clazz, "onProvisionDone", "([IIZZ)V");
    method_onAdvReport = env->GetMethodID(clazz, "onAdvReport", "(I[III[I)V");
    method_onKeyRefresh = env->GetMethodID(clazz, "onKeyRefresh", "(II)V");
    method_onIvUpdate = env->GetMethodID(clazz, "onIvUpdate", "(II)V");
    method_onSeqChange = env->GetMethodID(clazz, "onSeqChange", "(I)V");
    method_onProvFactor = env->GetMethodID(clazz, "onProvFactor", "(I[II)V");
    method_onHeartbeat = env->GetMethodID(clazz, "onHeartbeat", "(II)V");
    method_onBearerGattStatus = env->GetMethodID(clazz, "onBearerGattStatus", "(JI)V");
    method_onEvtErrorCode = env->GetMethodID(clazz, "onEvtErrorCode", "(I)V");
    method_onMsgHandler =
        env->GetMethodID(clazz, "onMsgHandler", "(ILandroid/bluetooth/mesh/BluetoothMeshAccessRxMessage;)V");
    method_onPublishTimeoutCallback =
        env->GetMethodID(clazz, "onPublishTimeoutCallback", "(I)V");
    method_onOTAMsgHandler =
        env->GetMethodID(clazz, "onOTAMsgHandler", "(ILandroid/bluetooth/mesh/BluetoothMeshAccessRxMessage;)V");
    method_CreateAccessMessageRxObject =
        env->GetMethodID(clazz, "CreateAccessMessageRxObject",
                         "(II[IIIIIIII)Landroid/bluetooth/mesh/BluetoothMeshAccessRxMessage;");

    ALOGI("%s: succeeds", __func__);
}

static const bt_interface_t* btIf;

static void initializeNative(JNIEnv *env, jobject object) {
    if(btIf)
        return;

    if ( (btIf = getBluetoothInterface()) == NULL) {
        error("Bluetooth module is not loaded");
        return;
    }

    if (sMeshIf != NULL) {
         ALOGW("Cleaning up Bluetooth MESH Interface before initializing...");
         sMeshIf->cleanup();
         sMeshIf = NULL;
    }

    if (mCallbacksObj != NULL) {
         ALOGW("Cleaning up Bluetooth MESH callback object");
         env->DeleteGlobalRef(mCallbacksObj);
         mCallbacksObj = NULL;
    }

    if ( (sMeshIf = (btmesh_interface_t *)
          btIf->get_profile_interface(BT_PROFILE_MESH_ID)) == NULL) {
        error("Failed to get Bluetooth MESH Interface");
        return;
    }

    meshif_status_code_t status;
    if ( (status = sMeshIf->init(btmesh_event_callbacks)) != MESHIF_SUCCESS) {
        error("Failed to initialize Bluetooth MESH, status: %d", status);
        sMeshIf = NULL;
        return;
    }

    mCallbacksObj = env->NewGlobalRef(object);
}

static jbyteArray callByteArrayGetter(JNIEnv* env, jobject object,
                                      const char* className,
                                      const char* methodName) {
  jclass myClass = env->FindClass(className);
  jmethodID myMethod = env->GetMethodID(myClass, methodName, "()[B");
  return (jbyteArray)env->CallObjectMethod(object, myMethod);
}
static jintArray callIntArrayGetter(JNIEnv* env, jobject object,
                                      const char* className,
                                      const char* methodName) {
  jclass myClass = env->FindClass(className);
  jmethodID myMethod = env->GetMethodID(myClass, methodName, "()[I");
  return (jintArray)env->CallObjectMethod(object, myMethod);
}

static jlong callLongGetter(JNIEnv* env, jobject object,
                                      const char* className,
                                      const char* methodName) {
  jclass myClass = env->FindClass(className);
  jmethodID myMethod = env->GetMethodID(myClass, methodName, "()J");
  return (jlong)env->CallLongMethod(object, myMethod);
}

static jint callIntGetter(JNIEnv* env, jobject object,
                                      const char* className,
                                      const char* methodName) {
  jclass myClass = env->FindClass(className);
  jmethodID myMethod = env->GetMethodID(myClass, methodName, "()I");
  return env->CallIntMethod(object, myMethod);
}

static jint callBooleanGetter(JNIEnv* env, jobject object,
                                      const char* className,
                                      const char* methodName) {
  jclass myClass = env->FindClass(className);
  jmethodID myMethod = env->GetMethodID(myClass, methodName, "()Z");
  return env->CallBooleanMethod(object, myMethod);
}

static meshif_address_type_t meshGetAddrType(uint16_t addr)
{
    if (addr == 0x0000) {
        return MESHIF_ADDRESS_TYPE_UNASSIGNED;
    } else if ((addr & 0x8000) == 0x0000) {
        return MESHIF_ADDRESS_TYPE_UNICAST;
    } else {
        if ((addr & 0xc000) == 0x8000) {
            return MESHIF_ADDRESS_TYPE_VIRTUAL;
        } else {
            return MESHIF_ADDRESS_TYPE_GROUP;
        }
    }
}

static int meshEnableNative(JNIEnv *env, jobject object, jobject initParams) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = -1;
    meshif_init_params_t init_params;
    jint i;

    memset(&init_params, 0, sizeof(init_params));

    jint role;
    jint* provisioneeParams = NULL;
    jint* deviceUuid = NULL;
    jint oobInfo;
    jint defaultTtl;
    jbyte* uri;
    jlong featureMask; //long
    jint* friendParams;
    jint* customizeParams;
    jintArray provisioneeParamsArray = NULL;
    jintArray deviceUuidArray = NULL;
    jbyteArray uriArray = NULL;
    jintArray friendParamsArray = NULL;
    jintArray customizeParamsArray = NULL;

    role = callIntGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getRole");

    ALOGI("%s: role %d", __func__, role);

    if (MESHIF_ROLE_PROVISIONER == role) {
        init_params.role = MESHIF_ROLE_PROVISIONER;
        // no need to init provisionee params
        init_params.provisionee = NULL;
    }
    else if (MESHIF_ROLE_PROVISIONEE == role) {
        init_params.role = MESHIF_ROLE_PROVISIONEE;

        //provisionee
        init_params.provisionee = (meshif_prov_provisionee_params_t *)malloc(sizeof(meshif_prov_provisionee_params_t));
        provisioneeParamsArray = callIntArrayGetter(
            env, initParams, "android/bluetooth/mesh/MeshInitParams", "getProvisioneeParams");
        if (provisioneeParamsArray != NULL) {
          provisioneeParams = env->GetIntArrayElements(provisioneeParamsArray, NULL);
          int len = env->GetArrayLength(provisioneeParamsArray);
          ALOGI("%s: provisioneParams len = %d", __func__, len);
          if (len != INIT_PROVISIONEE_PARAMS_SIZE) {
            ALOGE(
                "%s: wrong length of provisioneParams array, should be empty or %d bytes.",
                __func__, INIT_PROVISIONEE_PARAMS_SIZE);
            jniThrowIOException(env, EINVAL);
            goto done;
          }
          init_params.provisionee->cap.number_of_elements = (uint8_t)provisioneeParams[0];
          init_params.provisionee->cap.algorithms = (uint16_t)provisioneeParams[1];
          init_params.provisionee->cap.public_key_type = (uint8_t)provisioneeParams[2];
          init_params.provisionee->cap.static_oob_type = (uint8_t)provisioneeParams[3];
          init_params.provisionee->cap.output_oob_size = (uint8_t)provisioneeParams[4];
          init_params.provisionee->cap.output_oob_action = (uint16_t)provisioneeParams[5];
          init_params.provisionee->cap.input_oob_size = (uint8_t)provisioneeParams[6];
          init_params.provisionee->cap.input_oob_action = (uint16_t)provisioneeParams[7];
          ALOGI("%s: provisionee->cap:  number_of_elements %d, algorithms %d, public_key_type %d", __func__,
             init_params.provisionee->cap.number_of_elements, init_params.provisionee->cap.algorithms, init_params.provisionee->cap.public_key_type);
        } else
            ALOGW("%s: provisioneeParamsArray null!",__func__);
    }
    else {
        ALOGE("%s: Enable Mesh with wrong role %d", __func__, role);
        return -1;
    }

    //config
    init_params.config = (meshif_config_init_params_t *)malloc(sizeof(meshif_config_init_params_t));
    memset(init_params.config, 0, sizeof(meshif_config_init_params_t));

    deviceUuidArray = callIntArrayGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getDeviceUuid");
    if (deviceUuidArray != NULL) {
      deviceUuid = env->GetIntArrayElements(deviceUuidArray, NULL);
      int len = env->GetArrayLength(deviceUuidArray);
      ALOGI("%s: deviceUuid len = %d", __func__, len);
      if (len != MESHIF_UUID_SIZE) {
        ALOGE(
            "%s: wrong length of deviceUuid array, should be empty or %d bytes.",
            __func__, MESHIF_UUID_SIZE);
        jniThrowIOException(env, EINVAL);
        goto done;
      }
      for (i=0; i<len;i++)
          init_params.config->device_uuid[i] = (uint8_t) (*(deviceUuid + i) & 0xff);
    }
    else
        ALOGW("%s: deviceUuid null!",__func__);

    oobInfo = callIntGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getOobInfo");
    init_params.config->oob_info = oobInfo;

    defaultTtl = callIntGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getDefaultTtl");
    init_params.config->default_ttl = defaultTtl;

    ALOGI("%s: config:  oob_info %d, defaultTtl %d", __func__,
       init_params.config->oob_info, init_params.config->default_ttl);

    uriArray = callByteArrayGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getUri");
    if (uriArray != NULL) {
        uri = env->GetByteArrayElements(uriArray, NULL);
        int len = env->GetArrayLength(deviceUuidArray);
        ALOGI("%s: uriArray len %d", __func__, len);
        init_params.config->uri = (char *)malloc(len + 1); //"\0" in the end
        for (i=0; i<len;i++)
            init_params.config->uri[i] = (uint8_t) (*(uri + i) );
        init_params.config->uri[i] = 0;
    }
    else
        ALOGW("%s: uriArray null!",__func__);

    //feature_mask
    featureMask = callLongGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getFeatureMask");
    init_params.feature_mask = (uint32_t)featureMask;

    //friend_params
    init_params.friend_params = (meshif_friend_init_params_t *)malloc(sizeof(meshif_friend_init_params_t));
    friendParamsArray= callIntArrayGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getFriendInitParams");
    if (friendParamsArray != NULL) {
        friendParams = env->GetIntArrayElements(friendParamsArray, NULL);
        int len = env->GetArrayLength(friendParamsArray);
        ALOGI("%s: friendParams len = %d", __func__, len);
        if (len != INIT_FRIEND_PARAMS_SIZE) {
            ALOGE(
            "%s: wrong length of friendParams array, should be empty or %d bytes.",
            __func__, INIT_FRIEND_PARAMS_SIZE);
            jniThrowIOException(env, EINVAL);
            goto done;
        }
        init_params.friend_params->lpn_number = (uint8_t)(friendParams[0] & 0xff);
        init_params.friend_params->queue_size = (uint8_t)(friendParams[1] & 0xff);
        init_params.friend_params->subscription_list_size = (uint8_t)(friendParams[2] & 0xff);
        ALOGI("%s: friend_params:  lpn_number %d, queue_size %d, subscription_list_size %d", __func__,
            init_params.friend_params->lpn_number, init_params.friend_params->queue_size, init_params.friend_params->subscription_list_size);
    }
    else
        ALOGW("%s: friend_params null!",__func__);

    //customize_params
    init_params.customize_params = (meshif_customize_para_t *)malloc(sizeof(meshif_customize_para_t));
    customizeParamsArray = callIntArrayGetter(
        env, initParams, "android/bluetooth/mesh/MeshInitParams", "getCustomizeParams");
    if (customizeParamsArray != NULL) {
      customizeParams = env->GetIntArrayElements(customizeParamsArray, NULL);
      int len = env->GetArrayLength(customizeParamsArray);
      ALOGI("%s: customizeParams len = %d", __func__, len);
      if (len != INIT_CUSTOMIZE_PARAMS_SIZE) {
        ALOGE(
            "%s: wrong length of customizeParams array, len %d should be empty or %d bytes.",
            __func__, len, INIT_CUSTOMIZE_PARAMS_SIZE);
        jniThrowIOException(env, EINVAL);
        goto done;
      }
      init_params.customize_params->max_remote_node_cnt = (uint16_t)(customizeParams[0] & 0xffff);
      init_params.customize_params->save2flash = (uint8_t)(customizeParams[1] & 0xff);
      ALOGI("%s: customize_params:  max_remote_node_cnt %d, save2flash %d", __func__,
          init_params.customize_params->max_remote_node_cnt , init_params.customize_params->save2flash);
    }
    else
        ALOGW("%s: customize_params null!",__func__);

    ALOGI("%s call sdk enable", __func__);
    ret = sMeshIf->enable(&init_params);
    ALOGI("%s: ret = %d", __func__, ret);

done:
      if (init_params.provisionee)  free(init_params.provisionee);
      if (init_params.config && init_params.config->uri) free(init_params.config->uri);
      if (init_params.config) free(init_params.config);
      if (init_params.friend_params) free(init_params.friend_params);
      if (init_params.customize_params) free(init_params.customize_params);

      if (provisioneeParamsArray) env->ReleaseIntArrayElements(provisioneeParamsArray, provisioneeParams, 0);
      if (deviceUuidArray) env->ReleaseIntArrayElements(deviceUuidArray, deviceUuid, 0);
      if (uriArray) env->ReleaseByteArrayElements(uriArray, uri, 0);
      if (friendParamsArray ) env->ReleaseIntArrayElements(friendParamsArray, friendParams, 0);
      if (customizeParamsArray) env->ReleaseIntArrayElements(customizeParamsArray, customizeParams, 0);

    return ret;
}


static int meshDisableNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = sMeshIf->disable();
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static jint meshSetNetKeyNative(JNIEnv *env, jobject object, jint op, jintArray key, jint netIndex) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_netkey_t netkey;
    jint ret = JNI_TRUE;
    jint i;

    memset(&netkey, 0, sizeof(meshif_netkey_t));
    uint16_t len = (uint16_t)env->GetArrayLength(key);
    if (len != MESHIF_KEY_SIZE) {
        ALOGE("%s: error key size!", __func__);
        return -1;
    }
    jint* p_value = env->GetIntArrayElements(key, NULL);
    if (p_value == NULL) {
        ALOGE("%s: no key!", __func__);
        return -1;
    }
    netkey.network_key = (uint8_t *)malloc(MESHIF_KEY_SIZE);

    for (i=0; i<len;i++)
        netkey.network_key[i] = (uint8_t) (*(p_value + i) & 0xff);

    netkey.opcode = (meshif_key_opcode_t) op;
    netkey.key_index = (uint16_t)netIndex;
    ALOGI("%s: opcode %d, key_index %d, network_key[0] %d, network_key[1] %d", __func__,
        netkey.opcode , netkey.key_index, netkey.network_key[0], netkey.network_key[1]);
    ret = sMeshIf->set_netkey(&netkey);
    ALOGI("%s: ret = %d", __func__, ret);
    free(netkey.network_key);
    if (p_value) env->ReleaseIntArrayElements(key, p_value, 0);
    return ret;
}

static jint meshSetAppKeyNative(JNIEnv *env, jobject object, jint op,  jintArray key, jint netkey_index, jint appkey_index) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint i, ret;
    meshif_appkey_t appkey;

    memset(&appkey, 0, sizeof(meshif_appkey_t));

    uint16_t len = (uint16_t)env->GetArrayLength(key);
    if (len != MESHIF_KEY_SIZE) {
        ALOGE("%s: error key size!", __func__);
        return -1;
    }
    jint* p_value = env->GetIntArrayElements(key, NULL);
    if (p_value == NULL) {
        ALOGE("%s: no key!", __func__);
        return -1;
    }

    appkey.application_key = (uint8_t *)malloc(MESHIF_KEY_SIZE);
    for (i=0; i<len;i++)
        appkey.application_key[i] = (uint8_t) (*(p_value + i) & 0xff);

    appkey.opcode = (meshif_key_opcode_t) op;
    appkey.appkey_index = appkey_index;
    appkey.netkey_index = netkey_index;
    ALOGI("%s: opcode %d, appkey_index %d, netkey_index %d, application_key[0] %d, application_key[1] %d", __func__,
        appkey.opcode , appkey.appkey_index, appkey.netkey_index, appkey.application_key[0], appkey.application_key[1]);

    ret = sMeshIf->set_appkey(&appkey);
    ALOGI("%s: ret = %d", __func__, ret);

    free(appkey.application_key);
    if (p_value) env->ReleaseIntArrayElements(key, p_value, 0);
    return ret;
}

static void meshUnProvDevScanNative(JNIEnv *env, jobject object, jboolean start, jint duration) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return;
    }

    ALOGI("%s: start %d, duration %d, ", __func__, start, duration);

    sMeshIf->unprov_dev_scan((bool)start, duration);
    ALOGI("%s: return from unprov_dev_scan", __func__);
}

static jint meshInviteProvisioningNative(JNIEnv *env, jobject object, jintArray UUID , jint attentionDuration) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    uint8_t *target_uuid;
    jint i, ret;
    meshif_prov_invite_t invite;

    memset(&invite, 0, sizeof(meshif_prov_invite_t));

    uint16_t len = (uint16_t)env->GetArrayLength(UUID);
    jint* p_value = env->GetIntArrayElements(UUID, NULL);
    if (p_value == NULL) {
        ALOGE("%s: no key!", __func__);
        return -1;
    }

    target_uuid = (uint8_t *)malloc(len);
    for (i=0; i<len;i++)
        target_uuid[i] = (uint8_t) (*(p_value + i) & 0xff);

    invite.attention_duration = attentionDuration;
    ALOGI("%s: attentionDuration %d, target_uuid[0] %d, target_uuid[1] %d", __func__,
        attentionDuration, target_uuid[0], target_uuid[1]);

    ret = sMeshIf->invite_provisioning(target_uuid, &invite);
    ALOGI("%s: ret = %d", __func__, ret);

    free(target_uuid);
    if (p_value) env->ReleaseIntArrayElements(UUID, p_value, 0);
    return ret;
}

static jint meshStartProvisioningNative(JNIEnv *env, jobject object, jint algorithm, jint public_key, jint auth_method, jint auth_action, jint auth_size,
      jintArray netkey, jint netkey_index, jlong iv_index, jint addr, jint flags, jint mode) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint i, ret;
    meshif_prov_provisioner_params_t params;

    memset(&params, 0, sizeof(meshif_prov_provisioner_params_t));

    params.start.algorithm = algorithm;
    params.start.public_key = public_key;
    params.start.authentication_method = auth_method;
    params.start.authentication_action = auth_action;
    params.start.authentication_size = auth_size;

    uint16_t len = (uint16_t)env->GetArrayLength(netkey);
    if (len != MESHIF_KEY_SIZE) {
        ALOGE("%s: error key size!", __func__);
        return -1;
    }
    jint* p_value = env->GetIntArrayElements(netkey, NULL);
    if (p_value == NULL) {
        ALOGE("%s: no key!", __func__);
        return -1;
    }

    for (i=0; i<len;i++)
        params.data.netkey[i] = (uint8_t) (*(p_value + i) & 0xff);

    params.data.netkey_index = netkey_index;
    params.data.iv_index = iv_index;
    params.data.address = addr;
    params.data.flags = flags;
    ALOGI("%s: netkey_index %d, iv_index %d, address %d, flags %d", __func__,
        params.data.netkey_index, params.data.iv_index, params.data.address, params.data.flags);

    ret = sMeshIf->start_provisioning(&params, mode);
    ALOGI("%s: ret = %d", __func__, ret);

    if (p_value) env->ReleaseIntArrayElements(netkey, p_value, 0);
    return ret;
}

static jint meshSetProvisionFactorNative(JNIEnv *env, jobject object, jint type, jintArray buffer) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint i, ret;
    meshif_prov_factor_t prov_factor;

    memset(&prov_factor, 0, sizeof(meshif_prov_factor_t));

    uint16_t len = (uint16_t)env->GetArrayLength(buffer);
    jint* p_value = env->GetIntArrayElements(buffer, NULL);
    if (p_value == NULL) {
        ALOGE("%s: no data!", __func__);
        return -1;
    }
    for (i=0; i<len;i++)
        prov_factor.buf[i] = (uint8_t) (*(p_value + i) & 0xff);

    prov_factor.buf_len = len;
    prov_factor.type = (meshif_prov_factor_type_t) type;
    ALOGI("%s: type %d, len %d, buf[0] %d, buf[1] %d", __func__,
        prov_factor.type, prov_factor.buf_len, prov_factor.buf[0], prov_factor.buf[1]);

    ret = sMeshIf->set_prov_factor(&prov_factor);
    ALOGI("%s: ret = %d", __func__, ret);

    if (p_value) env->ReleaseIntArrayElements(buffer, p_value, 0);
    return ret;
}

static jboolean meshSetCompositionDataHeaderNative(JNIEnv *env, jobject object, jintArray data) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return JNI_FALSE;
    }

    meshif_model_data_t model_data;
    int i;
    jboolean ret = JNI_FALSE;

    memset(&model_data, 0, sizeof(model_data));
    model_data.opcode = MESHIF_MODEL_DATA_OP_SET_COMPOSITION_DATA_HEADER;

    uint16_t len = (uint16_t)env->GetArrayLength(data);
    jint* p_value = env->GetIntArrayElements(data, NULL);
    if (p_value == NULL) return JNI_FALSE;
    ALOGI("%s:len %d", __func__, len);

    model_data.data.composition_data.buf_len = len;
    model_data.data.composition_data.buf = (uint8_t *)malloc(len);
    for (i=0; i<len;i++) {
        model_data.data.composition_data.buf[i] = (uint8_t) (*(p_value + i) & 0xff);
        ALOGI("%s: data.buf[%d] %d", __func__, i, model_data.data.composition_data.buf[i]);
    }

    ret = sMeshIf->set_model_data(&model_data);
    ALOGI("%s: ret %d, opcode %d",
                __func__, ret, model_data.opcode);

    free((uint8_t *)model_data.data.composition_data.buf);
    if (p_value) env->ReleaseIntArrayElements(data, p_value, 0);
    return ret;
}

static jint meshAddElementNative(JNIEnv *env, jobject object, jint location) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_model_data_t model_data;
    uint16_t element_index;
    bool ret;

    memset(&model_data, 0, sizeof(model_data));
    model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_ELEMENT;

    model_data.data.element.location = (uint16_t)location;
    model_data.data.element.element_index = &element_index;
    ALOGI("%s: location %d", __func__, location);

    ret = sMeshIf->set_model_data(&model_data);
    ALOGI("%s: ret %d, opcode %d, element_index %d",
                __func__, ret, model_data.opcode, element_index);
    if (ret)
        return element_index;
    else
        return -1;
}

static jboolean meshSetElementAddrNative(JNIEnv *env, jobject object,jint addr) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return JNI_FALSE;
    }

    meshif_model_data_t model_data;
    jboolean ret = JNI_FALSE;

    memset(&model_data, 0, sizeof(model_data));
    model_data.opcode = MESHIF_MODEL_DATA_OP_SET_ELEMENT_ADDR;
    model_data.data.element_addr.unicast_addr = (uint16_t)addr;

    ret = sMeshIf->set_model_data(&model_data);
    ALOGI("%s: ret %d, opcode %d, addr %d",
                __func__, ret, model_data.opcode, addr);
    return ret;
}

static jint meshAddModelNative(JNIEnv *env, jobject object, jobject model) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    bool ret = false;
    uint16_t handle = 0;
    jint modelOpcode;
    uint16_t elementIndex;
    meshif_model_data_t model_data;

    memset(&model_data, 0, sizeof(model_data));

    modelOpcode = callIntGetter(env, model, "android/bluetooth/mesh/MeshModel", "getModelOpcode");
    elementIndex = (uint16_t) callIntGetter(env, model, "android/bluetooth/mesh/MeshModel", "getElementIndex");
    ALOGI("%s: modelOpcode %d, elementIndex %d", __func__, modelOpcode, elementIndex);

    switch (modelOpcode) {
        case 0://MESHIF_MODEL_DATA_OP_SET_COMPOSITION_DATA_HEADER:
        case 1://MESHIF_MODEL_DATA_OP_ADD_ELEMENT:
        case 2://MESHIF_MODEL_DATA_OP_SET_ELEMENT_ADDR:
        {
            ALOGE("%s add model function not for opcode %d", __func__, modelOpcode);
            break;
        }
        case 3://MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_SERVER:
        {
            ALOGI("%s Add ConfigurationServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_SERVER;
            model_data.data.configuration_server.model_handle = &handle;
            model_data.data.configuration_server.callback = btmesh_access_msg_handler_cb;
            break;
        }
        case 4://MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT:
        {
            ALOGI("%s Add ConfigurationClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT;
            model_data.data.configuration_client.model_handle = &handle;
            model_data.data.configuration_client.element_index = elementIndex;
            model_data.data.configuration_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.configuration_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 5://MESHIF_MODEL_DATA_OP_ADD_HEALTH_SERVER:
        {
            ALOGI("%s Add HealthServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_HEALTH_SERVER;
            model_data.data.health_server.model_handle = &handle;
            model_data.data.health_server.element_index = elementIndex;
            model_data.data.health_server.callback = btmesh_access_msg_handler_cb;
            model_data.data.health_server.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 6://MESHIF_MODEL_DATA_OP_ADD_HEALTH_CLIENT:
        {
            ALOGI("%s Add HealthClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_HEALTH_CLIENT;
            model_data.data.health_client.model_handle = &handle;
            model_data.data.health_client.element_index = elementIndex;
            model_data.data.health_client.callback = btmesh_health_client_evt_handler_cb;
            break;
        }
        case 7://MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_SERVER:
        {
            ALOGI("%s Add GenericOnOffServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_SERVER;
            model_data.data.generic_onoff_server.model_handle = &handle;
            model_data.data.generic_onoff_server.callback = btmesh_access_msg_handler_cb;
            model_data.data.generic_onoff_server.element_index = elementIndex;
            model_data.data.generic_onoff_server.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 8://MESHIF_MODEL_DATA_OP_ADD_CTL_SETUP_SERVER:
        {
            ALOGI("%s Add CtlSetupServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_CTL_SETUP_SERVER;
            model_data.data.ctl_setup_server.model_handle = &handle;
            model_data.data.ctl_setup_server.element_index = elementIndex;
            model_data.data.ctl_setup_server.callback = btmesh_access_msg_handler_cb;
            model_data.data.ctl_setup_server.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 9://MESHIF_MODEL_DATA_OP_ADD_GENERIC_POWER_ONOFF_CLIENT:
        {
            ALOGI("%s Add GenericPowerOnOffClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_GENERIC_POWER_ONOFF_CLIENT;
            model_data.data.generic_power_onoff_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.generic_power_onoff_client.element_index = elementIndex;
            model_data.data.generic_power_onoff_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            model_data.data.generic_power_onoff_client.model_handle = &handle;;
            break;
        }
        case 10://MESHIF_MODEL_DATA_OP_ADD_LIGHTNESS_CLIENT:
        {
            ALOGI("%s Add LightnessClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_LIGHTNESS_CLIENT;
            model_data.data.lightness_client.model_handle = &handle;
            model_data.data.lightness_client.element_index = elementIndex;
            model_data.data.lightness_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.lightness_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 11://MESHIF_MODEL_DATA_OP_ADD_CTL_CLIENT:
        {
            ALOGI("%s Add CtlClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_CTL_CLIENT;
            model_data.data.ctl_client.model_handle = &handle;
            model_data.data.ctl_client.element_index = elementIndex;
            model_data.data.ctl_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.ctl_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 12://MESHIF_MODEL_DATA_OP_ADD_HSL_SETUP_SERVER:
        {
            ALOGI("%s Add HslSetupServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_HSL_SETUP_SERVER;
            model_data.data.hsl_setup_server.model_handle = &handle;
            model_data.data.hsl_setup_server.element_index = elementIndex;
            model_data.data.hsl_setup_server.callback = btmesh_access_msg_handler_cb;
            model_data.data.hsl_setup_server.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 13://MESHIF_MODEL_DATA_OP_ADD_HSL_CLIENT:
        {
            ALOGI("%s Add HslClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_HSL_CLIENT;
            model_data.data.hsl_client.model_handle = &handle;
            model_data.data.hsl_client.element_index = elementIndex;
            model_data.data.hsl_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.hsl_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 14://MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_SERVER:
        {
            ALOGI("%s Add GenericLevelServer Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_SERVER;
            model_data.data.generic_level_server.model_handle = &handle;
            model_data.data.generic_level_server.element_index = elementIndex;
            model_data.data.generic_level_server.callback = btmesh_access_msg_handler_cb;
            model_data.data.generic_level_server.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 15://MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_CLIENT:
        {
            ALOGI("%s Add GenericLevelClient Model with opcode %d", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_CLIENT;
            model_data.data.generic_level_client.model_handle = &handle;
            model_data.data.generic_level_client.element_index = elementIndex;
            model_data.data.generic_level_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.generic_level_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 16://MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_CLIENT:
        {
            ALOGI("%s Add GenericOnOffClient Model with opcode %d ", __func__, modelOpcode);
            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_CLIENT;
            model_data.data.generic_onoff_client.model_handle = &handle;
            model_data.data.generic_onoff_client.element_index = elementIndex;
            model_data.data.generic_onoff_client.callback = btmesh_access_msg_handler_cb;
            model_data.data.generic_onoff_client.publish_timeout_cb = btmesh_access_publish_timeout_cb;
            break;
        }
        case 500://MESHIF_MODEL_DATA_OP_ADD_MODEL:
        {
            ALOGI("%s Add Vendor Model with opcode %d", __func__, modelOpcode);

            uint32_t modelId = (uint32_t) callLongGetter(
                                        env, model, "android/bluetooth/mesh/MeshModel", "getModelID");
            uint16_t companyId = (uint16_t) callIntGetter(
                                        env, model, "android/bluetooth/mesh/MeshModel", "getCompanyID");
            uint8_t opcodeCount = (uint8_t) callIntGetter(
                                        env, model, "android/bluetooth/mesh/MeshModel", "getOpcodeCount");
            ALOGI("%s model_id 0x%x, company_id 0x%x, opcode_count %d", __func__, modelId, companyId, opcodeCount);

            model_data.opcode = MESHIF_MODEL_DATA_OP_ADD_MODEL;
            model_data.data.generic_model_data.model_handle = &handle;

            model_data.data.generic_model_data.model_params = (meshif_model_add_params_t *)malloc(sizeof(meshif_model_add_params_t));

            model_data.data.generic_model_data.model_params->model_id = modelId;
            model_data.data.generic_model_data.model_params->element_index = elementIndex;
            model_data.data.generic_model_data.model_params->opcode_count = opcodeCount;

            jintArray vendorMsgOpcodesArray = callIntArrayGetter(
                                        env, model, "android/bluetooth/mesh/MeshModel", "getVendorMsgOpcodes");
            if (vendorMsgOpcodesArray != NULL) {
                jint* msgOpcodes = env->GetIntArrayElements(vendorMsgOpcodesArray, NULL);
                int msgOpcodeLen = env->GetArrayLength(vendorMsgOpcodesArray);
                ALOGI("%s: vendorMsgOpcodesArray opcode length = %d", __func__, msgOpcodeLen);
                if (msgOpcodeLen > 0 && msgOpcodeLen == opcodeCount) {
                    model_data.data.generic_model_data.model_params->opcode_handlers =
                        (meshif_access_opcode_handler_t *)malloc(msgOpcodeLen * sizeof(meshif_access_opcode_handler_t));

                    for (int i = 0; i < msgOpcodeLen; i++) {
                        ALOGI("%s opcode%d 0x%x", __func__, i, *(msgOpcodes + i));
                        model_data.data.generic_model_data.model_params->opcode_handlers[i].opcode.opcode = *(msgOpcodes + i);
                        model_data.data.generic_model_data.model_params->opcode_handlers[i].opcode.company_id = companyId;
                        model_data.data.generic_model_data.model_params->opcode_handlers[i].handler = btmesh_access_msg_handler_cb;
                    }
                } else {
                    ALOGW("%s: no vendor message opcode or opcode length %d not match opcodeCount %d",
                        __func__, msgOpcodeLen, opcodeCount);
                }
                if (vendorMsgOpcodesArray) env->ReleaseIntArrayElements(vendorMsgOpcodesArray, msgOpcodes, 0);
            }
            else {
                ALOGW("%s: vendorMsgOpcodesArray is null!",__func__);
            }
            break;
        }
        default:
        {
            ALOGE("%s unkown Model with opcode %d", __func__, modelOpcode);
        }
    }

    ALOGI("%s: call sdk set_model_data:", __func__);
    ret = sMeshIf->set_model_data(&model_data);
    ALOGI("%s: ret %d, model opcode %d, handle 0x%x",
                __func__, ret, model_data.opcode, handle);

    if (!ret) {
        handle = -1;
        ALOGE("%s add model fail, model opcode %d", __func__, modelOpcode);
    }

    if (modelOpcode == MESHIF_MODEL_DATA_OP_ADD_MODEL) { //500
        if (model_data.data.generic_model_data.model_params && model_data.data.generic_model_data.model_params->opcode_handlers)
            free(model_data.data.generic_model_data.model_params->opcode_handlers);
        if (model_data.data.generic_model_data.model_params)
            free(model_data.data.generic_model_data.model_params);
    }
    return handle;
}

static  jint meshSendConfigMessageNative(JNIEnv *env, jobject object,
            jint dst, jint src, jint ttl, jint netKeyIndex, jint opcode, jobject configParam) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = -1;
    meshif_configuration_msg_tx_t param;
    memset(&param, 0, sizeof(meshif_configuration_msg_tx_t));

    meshif_heartbeat_publication_t heartbeat_publication;
    memset(&heartbeat_publication, 0, sizeof(meshif_heartbeat_publication_t));

    meshif_model_publication_state_t publication_state;
    memset(&publication_state, 0, sizeof(meshif_model_publication_state_t));

    meshif_heartbeat_subscription_t heartbeat_subscription;
    memset(&heartbeat_subscription, 0, sizeof(meshif_heartbeat_subscription_t));

    jintArray virtualUuidArrayInPubSet = NULL;
    jintArray virtualUuidArrayInSubAdd = NULL;
    jintArray virtualUuidArrayInSubDel = NULL;
    jintArray virtualUuidArrayInSubOw = NULL;
    jintArray netkeyArrayInNetkeyAdd = NULL;
    jintArray netkeyArrayInNetkeyUpdate = NULL;
    jintArray appkeyArrayInAppkeyAdd = NULL;
    jintArray appkeyArrayInAppkeyUpdate = NULL;

    jint* virtualUuidValueInPubSet = NULL;
    jint* virtualUuidValueInSubAdd = NULL;
    jint* virtualUuidValueInSubDel = NULL;
    jint* virtualUuidValueInSubOw = NULL;
    jint* netkeyValueInNetkeyAdd = NULL;
    jint* netkeyValueInNetkeyUpdate = NULL;
    jint* appkeyValueInAppkeyAdd = NULL;
    jint* appkeyValueInAppkeyUpdate = NULL;

    uint8_t appkey[MESHIF_KEY_SIZE];
    uint8_t netkey[MESHIF_KEY_SIZE];
    uint8_t virtual_uuid[MESHIF_UUID_SIZE];

    uint16_t len;

    param.opcode = opcode;
    ALOGI("%s opcode = 0x%X", __func__, param.opcode);
    ALOGI("%s dst = %d, src = %d, ttl = %d, netKeyIndex = %d, ", __func__, dst, src, ttl, netKeyIndex);
    switch (param.opcode) {

        case MESHIF_ACCESS_MSG_CONFIG_BEACON_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_BEACON_GET", __func__);
            param.data.beacon_get.meta.dst_addr = (uint16_t)dst;
            param.data.beacon_get.meta.src_addr = (uint16_t)src;
            param.data.beacon_get.meta.ttl = (uint8_t)ttl;
            param.data.beacon_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_BEACON_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_BEACON_SET", __func__);
            param.data.beacon_set.meta.dst_addr = (uint16_t)dst;
            param.data.beacon_set.meta.src_addr = (uint16_t)src;
            param.data.beacon_set.meta.ttl = (uint8_t)ttl;
            param.data.beacon_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.beacon_set.beacon = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getBeacon");
            ALOGI("%s beacon = %d", __func__, param.data.beacon_set.beacon);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_COMPOSITION_DATA_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_COMPOSITION_DATA_GET", __func__);
            param.data.composition_data_get.meta.dst_addr = (uint16_t)dst;
            param.data.composition_data_get.meta.src_addr = (uint16_t)src;
            param.data.composition_data_get.meta.ttl = (uint8_t)ttl;
            param.data.composition_data_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.composition_data_get.page = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getPage");
            ALOGI("%s page = %d", __func__, param.data.composition_data_get.page);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_GET", __func__);
            param.data.default_ttl_get.meta.dst_addr = (uint16_t)dst;
            param.data.default_ttl_get.meta.src_addr = (uint16_t)src;
            param.data.default_ttl_get.meta.ttl = (uint8_t)ttl;
            param.data.default_ttl_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_SET", __func__);
            param.data.default_ttl_set.meta.dst_addr = (uint16_t)dst;
            param.data.default_ttl_set.meta.src_addr = (uint16_t)src;
            param.data.default_ttl_set.meta.ttl = (uint8_t)ttl;
            param.data.default_ttl_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.default_ttl_set.TTL = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "geTTL");
            ALOGI("%s TTL = %d", __func__, param.data.default_ttl_set.TTL);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_GET", __func__);
            param.data.gatt_proxy_get.meta.dst_addr = (uint16_t)dst;
            param.data.gatt_proxy_get.meta.src_addr = (uint16_t)src;
            param.data.gatt_proxy_get.meta.ttl = (uint8_t)ttl;
            param.data.gatt_proxy_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_SET", __func__);
            param.data.gatt_proxy_set.meta.dst_addr = (uint16_t)dst;
            param.data.gatt_proxy_set.meta.src_addr = (uint16_t)src;
            param.data.gatt_proxy_set.meta.ttl = (uint8_t)ttl;
            param.data.gatt_proxy_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.gatt_proxy_set.gatt_proxy = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getGattProxy");
            ALOGI("%s gatt_proxy = %d", __func__, param.data.gatt_proxy_set.gatt_proxy);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_FRIEND_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_FRIEND_GET", __func__);
            param.data.friend_get.meta.dst_addr = (uint16_t)dst;
            param.data.friend_get.meta.src_addr = (uint16_t)src;
            param.data.friend_get.meta.ttl = (uint8_t)ttl;
            param.data.friend_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_FRIEND_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_FRIEND_SET", __func__);
            param.data.friend_set.meta.dst_addr = (uint16_t)dst;
            param.data.friend_set.meta.src_addr = (uint16_t)src;
            param.data.friend_set.meta.ttl = (uint8_t)ttl;
            param.data.friend_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.friend_set.mesh_friend = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getMeshFriend");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_RELAY_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_RELAY_GET", __func__);
            param.data.relay_get.meta.dst_addr = (uint16_t)dst;
            param.data.relay_get.meta.src_addr = (uint16_t)src;
            param.data.relay_get.meta.ttl = (uint8_t)ttl;
            param.data.relay_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_RELAY_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_RELAY_SET", __func__);
            param.data.relay_set.meta.dst_addr = (uint16_t)dst;
            param.data.relay_set.meta.src_addr = (uint16_t)src;
            param.data.relay_set.meta.ttl = (uint8_t)ttl;
            param.data.relay_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.relay_set.relay = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getRelay");
            param.data.relay_set.retransmit_count = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getRetransmitCount");
            param.data.relay_set.retransmit_interval_steps = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getRetransmitIntervalSteps");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_GET", __func__);
            param.data.model_pub_get.meta.dst_addr = (uint16_t)dst;
            param.data.model_pub_get.meta.src_addr = (uint16_t)src;
            param.data.model_pub_get.meta.ttl = (uint8_t)ttl;
            param.data.model_pub_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_pub_get.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.model_pub_get.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_SET", __func__);

            param.data.model_pub_set.state = &publication_state;
            param.data.model_pub_set.meta.dst_addr = (uint16_t)dst;
            param.data.model_pub_set.meta.src_addr = (uint16_t)src;
            param.data.model_pub_set.meta.ttl = (uint8_t)ttl;
            param.data.model_pub_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_pub_set.state->element_address = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.model_pub_set.state->publish_address.type = (meshif_address_type_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressType");
            param.data.model_pub_set.state->publish_address.value= (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressValue");

            virtualUuidArrayInPubSet = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getVirtualUUID");
            if (virtualUuidArrayInPubSet != NULL) {
                len = (uint16_t)env->GetArrayLength(virtualUuidArrayInPubSet);
                virtualUuidValueInPubSet = env->GetIntArrayElements(virtualUuidArrayInPubSet, NULL);
                if (virtualUuidValueInPubSet == NULL) {
                    ALOGE("%s: no virtual_uuid!", __func__);
                    param.data.model_pub_set.state->publish_address.virtual_uuid = NULL;
                }
                else {
                    param.data.model_pub_set.state->publish_address.virtual_uuid = virtual_uuid;
                    for (int i=0; i<len; i++)
                        param.data.model_pub_set.state->publish_address.virtual_uuid[i] = (uint8_t) (*(virtualUuidValueInPubSet + i) & 0xff);
                }
            }
            param.data.model_pub_set.state->appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");
            param.data.model_pub_set.state->friendship_credential_flag = (bool) callBooleanGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getFriendshipCredentialFlag");
            param.data.model_pub_set.state->publish_ttl = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getPublishTTL");
            param.data.model_pub_set.state->publish_period = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getPublishPeriod");
            param.data.model_pub_set.state->retransmit_count = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getRetransmitCount");
            param.data.model_pub_set.state->retransmit_interval_steps = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getRetransmitIntervalSteps");
            param.data.model_pub_set.state->model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;
        case MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD", __func__);

            param.data.model_sub_add.meta.dst_addr = (uint16_t)dst;
            param.data.model_sub_add.meta.src_addr = (uint16_t)src;
            param.data.model_sub_add.meta.ttl = (uint8_t)ttl;
            param.data.model_sub_add.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_sub_add.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");

            param.data.model_sub_add.address.type = (meshif_address_type_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressType");
            param.data.model_sub_add.address.value= (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressValue");

            virtualUuidArrayInSubAdd = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getVirtualUUID");
            if (virtualUuidArrayInSubAdd != NULL) {
                len = (uint16_t)env->GetArrayLength(virtualUuidArrayInSubAdd);
                virtualUuidValueInSubAdd = env->GetIntArrayElements(virtualUuidArrayInSubAdd, NULL);
                if (virtualUuidValueInSubAdd == NULL) {
                    ALOGE("%s: no virtual_uuid!", __func__);
                    param.data.model_sub_add.address.virtual_uuid = NULL;
                }
                else {
                    param.data.model_sub_add.address.virtual_uuid = virtual_uuid;
                    for (int i=0; i<len; i++)
                        param.data.model_sub_add.address.virtual_uuid[i] = (uint8_t) (*(virtualUuidValueInSubAdd + i) & 0xff);
                }
            }
            param.data.model_sub_add.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;
        case MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE", __func__);
            param.data.model_sub_del.meta.dst_addr = (uint16_t)dst;
            param.data.model_sub_del.meta.src_addr = (uint16_t)src;
            param.data.model_sub_del.meta.ttl = (uint8_t)ttl;
            param.data.model_sub_del.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_sub_del.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");

            param.data.model_sub_del.address.type = (meshif_address_type_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressType");
            param.data.model_sub_del.address.value= (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressValue");

            virtualUuidArrayInSubDel = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getVirtualUUID");
            if (virtualUuidArrayInSubDel != NULL) {
                len = (uint16_t)env->GetArrayLength(virtualUuidArrayInSubDel);
                virtualUuidValueInSubDel = env->GetIntArrayElements(virtualUuidArrayInSubDel, NULL);
                if (virtualUuidValueInSubDel == NULL) {
                    ALOGE("%s: no virtual_uuid!", __func__);
                    param.data.model_sub_del.address.virtual_uuid = NULL;
                }
                else {
                    param.data.model_sub_del.address.virtual_uuid = virtual_uuid;
                    for (int i=0; i<len; i++)
                        param.data.model_sub_del.address.virtual_uuid[i] = (uint8_t) (*(virtualUuidValueInSubDel + i) & 0xff);
                }
            }
            param.data.model_sub_del.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE", __func__);
            param.data.model_sub_ow.meta.dst_addr = (uint16_t)dst;
            param.data.model_sub_ow.meta.src_addr = (uint16_t)src;
            param.data.model_sub_ow.meta.ttl = (uint8_t)ttl;
            param.data.model_sub_ow.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_sub_ow.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");

            param.data.model_sub_ow.address.type = (meshif_address_type_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressType");
            param.data.model_sub_ow.address.value= (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAddressValue");

            virtualUuidArrayInSubOw = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getVirtualUUID");
            if (virtualUuidArrayInSubOw != NULL) {
                len = (uint16_t)env->GetArrayLength(virtualUuidArrayInSubOw);
                virtualUuidValueInSubOw = env->GetIntArrayElements(virtualUuidArrayInSubOw, NULL);
                if (virtualUuidValueInSubOw == NULL) {
                    ALOGE("%s: no virtual_uuid!", __func__);
                    param.data.model_sub_ow.address.virtual_uuid = NULL;
                }
                else {
                    param.data.model_sub_ow.address.virtual_uuid = virtual_uuid;
                    for (int i=0; i<len; i++)
                        param.data.model_sub_ow.address.virtual_uuid[i] = (uint8_t) (*(virtualUuidValueInSubOw + i) & 0xff);
                }
            }
            param.data.model_sub_ow.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL", __func__);
            param.data.model_sub_del_all.meta.dst_addr = (uint16_t)dst;
            param.data.model_sub_del_all.meta.src_addr = (uint16_t)src;
            param.data.model_sub_del_all.meta.ttl = (uint8_t)ttl;
            param.data.model_sub_del_all.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_sub_del_all.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.model_sub_del_all.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET", __func__);
            param.data.sig_model_sub_get.meta.dst_addr = (uint16_t)dst;
            param.data.sig_model_sub_get.meta.src_addr = (uint16_t)src;
            param.data.sig_model_sub_get.meta.ttl = (uint8_t)ttl;
            param.data.sig_model_sub_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.sig_model_sub_get.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.sig_model_sub_get.model_id = (uint16_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET", __func__);
            param.data.vendor_model_sub_get.meta.dst_addr = (uint16_t)dst;
            param.data.vendor_model_sub_get.meta.src_addr = (uint16_t)src;
            param.data.vendor_model_sub_get.meta.ttl = (uint8_t)ttl;
            param.data.vendor_model_sub_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.vendor_model_sub_get.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.vendor_model_sub_get.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETKEY_ADD:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETKEY_ADD", __func__);
            param.data.netkey_add.meta.dst_addr = (uint16_t)dst;
            param.data.netkey_add.meta.src_addr = (uint16_t)src;
            param.data.netkey_add.meta.ttl = (uint8_t)ttl;
            param.data.netkey_add.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.netkey_add.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");

            netkeyArrayInNetkeyAdd = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkey");
            len = (uint16_t) env->GetArrayLength(netkeyArrayInNetkeyAdd);
            netkeyValueInNetkeyAdd = env->GetIntArrayElements(netkeyArrayInNetkeyAdd, NULL);
            if (netkeyValueInNetkeyAdd == NULL || len != MESHIF_KEY_SIZE) {
                ALOGE("%s: no key or wrong length of netkey array %d bytes.", __func__, len);
                goto done;
            }
            param.data.netkey_add.netkey = netkey;
            for (int i=0; i<len; i++)
                param.data.netkey_add.netkey[i] = (uint8_t) (*(netkeyValueInNetkeyAdd + i) & 0xff);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETKEY_UPDATE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETKEY_UPDATE", __func__);
            param.data.netkey_update.meta.dst_addr = (uint16_t)dst;
            param.data.netkey_update.meta.src_addr = (uint16_t)src;
            param.data.netkey_update.meta.ttl = (uint8_t)ttl;
            param.data.netkey_update.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.netkey_update.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");

            netkeyArrayInNetkeyUpdate = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkey");
            len = (uint16_t) env->GetArrayLength(netkeyArrayInNetkeyUpdate);
            netkeyValueInNetkeyUpdate = env->GetIntArrayElements(netkeyArrayInNetkeyUpdate, NULL);
            if (netkeyValueInNetkeyUpdate == NULL || len != MESHIF_KEY_SIZE) {
                ALOGE("%s: no key or wrong length of netkey array %d bytes.", __func__, len);
                goto done;
            }
            param.data.netkey_update.netkey = netkey;
            for (int i=0; i<len; i++)
                param.data.netkey_update.netkey[i] = (uint8_t) (*(netkeyValueInNetkeyUpdate + i) & 0xff);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETKEY_DELETE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETKEY_DELETE", __func__);
            param.data.netkey_del.meta.dst_addr = (uint16_t)dst;
            param.data.netkey_del.meta.src_addr = (uint16_t)src;
            param.data.netkey_del.meta.ttl = (uint8_t)ttl;
            param.data.netkey_del.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.netkey_del.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETKEY_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETKEY_GET", __func__);
            param.data.netkey_get.meta.dst_addr = (uint16_t)dst;
            param.data.netkey_get.meta.src_addr = (uint16_t)src;
            param.data.netkey_get.meta.ttl = (uint8_t)ttl;
            param.data.netkey_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_APPKEY_ADD:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_APPKEY_ADD", __func__);
            param.data.appkey_add.meta.dst_addr = (uint16_t)dst;
            param.data.appkey_add.meta.src_addr = (uint16_t)src;
            param.data.appkey_add.meta.ttl = (uint8_t)ttl;
            param.data.appkey_add.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.appkey_add.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            param.data.appkey_add.appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");

            appkeyArrayInAppkeyAdd = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkey");
            len = (uint16_t) env->GetArrayLength(appkeyArrayInAppkeyAdd);
            appkeyValueInAppkeyAdd = env->GetIntArrayElements(appkeyArrayInAppkeyAdd, NULL);
            if (appkeyValueInAppkeyAdd == NULL || len != MESHIF_KEY_SIZE) {
                ALOGE("%s: no key or wrong length of netkey array %d bytes.", __func__, len);
                goto done;
            }
            param.data.appkey_add.appkey = appkey;
            for (int i=0; i<len; i++)
                param.data.appkey_add.appkey[i] = (uint8_t) (*(appkeyValueInAppkeyAdd + i) & 0xff);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_APPKEY_UPDATE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_APPKEY_UPDATE", __func__);
            param.data.appkey_update.meta.dst_addr = (uint16_t)dst;
            param.data.appkey_update.meta.src_addr = (uint16_t)src;
            param.data.appkey_update.meta.ttl = (uint8_t)ttl;
            param.data.appkey_update.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.appkey_update.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            param.data.appkey_update.appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");

            appkeyArrayInAppkeyUpdate = callIntArrayGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkey");
            len = (uint16_t) env->GetArrayLength(appkeyArrayInAppkeyUpdate);
            appkeyValueInAppkeyUpdate = env->GetIntArrayElements(appkeyArrayInAppkeyUpdate, NULL);
            if (appkeyValueInAppkeyUpdate == NULL || len != MESHIF_KEY_SIZE) {
                ALOGE("%s: no key or wrong length of netkey array %d bytes.", __func__, len);
                goto done;
            }
            param.data.appkey_update.appkey = appkey;
            for (int i=0; i<len; i++)
                param.data.appkey_update.appkey[i] = (uint8_t) (*(appkeyValueInAppkeyUpdate + i) & 0xff);
            break;

        case MESHIF_ACCESS_MSG_CONFIG_APPKEY_DELETE:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_APPKEY_DELETE", __func__);
            param.data.appkey_del.meta.dst_addr = (uint16_t)dst;
            param.data.appkey_del.meta.src_addr = (uint16_t)src;
            param.data.appkey_del.meta.ttl = (uint8_t)ttl;
            param.data.appkey_del.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.appkey_del.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            param.data.appkey_del.appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_APPKEY_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_APPKEY_GET", __func__);
            param.data.appkey_get.meta.dst_addr = (uint16_t)dst;
            param.data.appkey_get.meta.src_addr = (uint16_t)src;
            param.data.appkey_get.meta.ttl = (uint8_t)ttl;
            param.data.appkey_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.appkey_get.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_BIND:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_BIND", __func__);
            param.data.model_app_bind.meta.dst_addr = (uint16_t)dst;
            param.data.model_app_bind.meta.src_addr = (uint16_t)src;
            param.data.model_app_bind.meta.ttl = (uint8_t)ttl;
            param.data.model_app_bind.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_app_bind.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.model_app_bind.appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");
            param.data.model_app_bind.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_UNBIND:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_UNBIND", __func__);
            param.data.model_app_unbind.meta.dst_addr = (uint16_t)dst;
            param.data.model_app_unbind.meta.src_addr = (uint16_t)src;
            param.data.model_app_unbind.meta.ttl = (uint8_t)ttl;
            param.data.model_app_unbind.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.model_app_bind.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.model_app_bind.appkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getAppkeyIndex");
            param.data.model_app_bind.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_APP_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_APP_GET", __func__);
            param.data.sig_model_app_get.meta.dst_addr = (uint16_t)dst;
            param.data.sig_model_app_get.meta.src_addr = (uint16_t)src;
            param.data.sig_model_app_get.meta.ttl = (uint8_t)ttl;
            param.data.sig_model_app_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.sig_model_app_get.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.sig_model_app_get.model_id = (uint16_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_GET", __func__);
            param.data.vendor_model_app_get.meta.dst_addr = (uint16_t)dst;
            param.data.vendor_model_app_get.meta.src_addr = (uint16_t)src;
            param.data.vendor_model_app_get.meta.ttl = (uint8_t)ttl;
            param.data.vendor_model_app_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.vendor_model_app_get.element_addr = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getElementAddress");
            param.data.vendor_model_app_get.model_id = (uint32_t) callLongGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getModelId");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_GET", __func__);
            param.data.node_identity_get.meta.dst_addr = (uint16_t)dst;
            param.data.node_identity_get.meta.src_addr = (uint16_t)src;
            param.data.node_identity_get.meta.ttl = (uint8_t)ttl;
            param.data.node_identity_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.node_identity_get.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_SET", __func__);
            param.data.node_identity_set.meta.dst_addr = (uint16_t)dst;
            param.data.node_identity_set.meta.src_addr = (uint16_t)src;
            param.data.node_identity_set.meta.ttl = (uint8_t)ttl;
            param.data.node_identity_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.node_identity_set.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            param.data.node_identity_set.identity = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getIdentity");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NODE_RESET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NODE_RESET", __func__);
            param.data.node_reset.meta.dst_addr = (uint16_t)dst;
            param.data.node_reset.meta.src_addr = (uint16_t)src;
            param.data.node_reset.meta.ttl = (uint8_t)ttl;
            param.data.node_reset.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_GET", __func__);
            param.data.key_ref_pha_get.meta.dst_addr = (uint16_t)dst;
            param.data.key_ref_pha_get.meta.src_addr = (uint16_t)src;
            param.data.key_ref_pha_get.meta.ttl = (uint8_t)ttl;
            param.data.key_ref_pha_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.key_ref_pha_get.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_SET", __func__);
            param.data.key_ref_pha_set.meta.dst_addr = (uint16_t)dst;
            param.data.key_ref_pha_set.meta.src_addr = (uint16_t)src;
            param.data.key_ref_pha_set.meta.ttl = (uint8_t)ttl;
            param.data.key_ref_pha_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.key_ref_pha_set.netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            param.data.key_ref_pha_set.transition = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getTransition");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET", __func__);
            param.data.hb_pub_get.meta.dst_addr = (uint16_t)dst;
            param.data.hb_pub_get.meta.src_addr = (uint16_t)src;
            param.data.hb_pub_get.meta.ttl = (uint8_t)ttl;
            param.data.hb_pub_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET", __func__);
            param.data.hb_pub_set.publication = &heartbeat_publication;
            param.data.hb_pub_set.meta.dst_addr = (uint16_t)dst;
            param.data.hb_pub_set.meta.src_addr = (uint16_t)src;
            param.data.hb_pub_set.meta.ttl = (uint8_t)ttl;
            param.data.hb_pub_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.hb_pub_set.publication->destination = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getDestination");
            param.data.hb_pub_set.publication->count_log = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getCountLog");
            param.data.hb_pub_set.publication->period_log = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getPeriodLog");
            param.data.hb_pub_set.publication->ttl = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getTTL");
            param.data.hb_pub_set.publication->features = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getFeatures");
            param.data.hb_pub_set.publication->netkey_index = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getNetkeyIndex");
            break;
        case MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET", __func__);
            param.data.hb_sub_get.meta.dst_addr = (uint16_t)dst;
            param.data.hb_sub_get.meta.src_addr = (uint16_t)src;
            param.data.hb_sub_get.meta.ttl = (uint8_t)ttl;
            param.data.hb_sub_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET", __func__);
            param.data.hb_sub_set.subscription = &heartbeat_subscription;
            param.data.hb_sub_set.meta.dst_addr = (uint16_t)dst;
            param.data.hb_sub_set.meta.src_addr = (uint16_t)src;
            param.data.hb_sub_set.meta.ttl = (uint8_t)ttl;
            param.data.hb_sub_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.hb_sub_set.subscription->source = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getSource");
            param.data.hb_sub_set.subscription->destination = (uint16_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getDestination");
            param.data.hb_sub_set.subscription->period_log = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getPeriodLog");
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_GET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_GET", __func__);
            param.data.net_trans_get.meta.dst_addr = (uint16_t)dst;
            param.data.net_trans_get.meta.src_addr = (uint16_t)src;
            param.data.net_trans_get.meta.ttl = (uint8_t)ttl;
            param.data.net_trans_get.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            break;

        case MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_SET:
            ALOGI("%s: MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_SET", __func__);
            param.data.net_trans_set.meta.dst_addr = (uint16_t)dst;
            param.data.net_trans_set.meta.src_addr = (uint16_t)src;
            param.data.net_trans_set.meta.ttl = (uint8_t)ttl;
            param.data.net_trans_set.meta.msg_netkey_index = (uint16_t)netKeyIndex;
            param.data.net_trans_set.count = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getCount");
            param.data.net_trans_set.interval_steps = (uint8_t) callIntGetter(
                env, configParam, "android/bluetooth/mesh/ConfigMessageParams", "getIntervalSteps");
            break;

        default:
            ALOGE("%s: not support config msg %d", __func__, param.opcode);
            break;
    }
    ret = sMeshIf->model_cc_msg_tx(&param);
    ALOGI("%s: ret = %d", __func__, ret);

done:
    if (virtualUuidArrayInPubSet) env->ReleaseIntArrayElements(virtualUuidArrayInPubSet, virtualUuidValueInPubSet, 0);
    if (virtualUuidArrayInSubAdd) env->ReleaseIntArrayElements(virtualUuidArrayInSubAdd, virtualUuidValueInSubAdd, 0);
    if (virtualUuidArrayInSubDel) env->ReleaseIntArrayElements(virtualUuidArrayInSubDel, virtualUuidValueInSubDel, 0);
    if (virtualUuidArrayInSubOw) env->ReleaseIntArrayElements(virtualUuidArrayInSubOw, virtualUuidValueInSubOw, 0);
    if (netkeyArrayInNetkeyAdd) env->ReleaseIntArrayElements(netkeyArrayInNetkeyAdd, netkeyValueInNetkeyAdd, 0);
    if (netkeyArrayInNetkeyUpdate) env->ReleaseIntArrayElements(netkeyArrayInNetkeyUpdate, netkeyValueInNetkeyUpdate, 0);
    if (appkeyArrayInAppkeyAdd) env->ReleaseIntArrayElements(appkeyArrayInAppkeyAdd, appkeyValueInAppkeyAdd, 0);
    if (appkeyArrayInAppkeyUpdate) env->ReleaseIntArrayElements(appkeyArrayInAppkeyUpdate, appkeyValueInAppkeyUpdate, 0);

    return ret;
}

static jint meshSendPacketNative(JNIEnv *env, jobject object, jint dst, jint src, jint ttl, jint netKeyIndex, jint appKeyIndex, jintArray payload) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint i;
    jboolean ret = JNI_TRUE;
    meshif_tx_params_t param;

    ALOGI("%s: dst %d, src %d, ttl %d, netKeyIndex %d, appKeyIndex %d", __func__, dst, src, ttl, netKeyIndex, appKeyIndex);

    memset(&param, 0, sizeof(meshif_tx_params_t));

    param.dst.value = dst;
    param.dst.type = meshGetAddrType((uint16_t)dst);
    param.src = src;
    param.ttl = ttl;
    param.security.appidx = appKeyIndex;
    param.security.netidx = netKeyIndex;
    param.security.device_key = NULL;

    uint16_t len = (uint16_t)env->GetArrayLength(payload);
    jint* p_value = env->GetIntArrayElements(payload, NULL);
    if (p_value == NULL) return -1;

    param.data_len = len;
    param.data = (uint8_t *)malloc(param.data_len);
    for (i=0; i<len;i++)
        param.data[i] = (uint8_t) (*(p_value + i) & 0xff);

    uint16_t opcode = ((uint16_t) param.data[0]) << 8 | (uint16_t)param.data[1];
    ALOGI("%s opcode %d, len %d", __func__, opcode, len);
    ret = sMeshIf->send_packet(&param);
    ALOGI("%s: ret = %d", __func__, ret);

    free((uint8_t *)param.data);
    if (p_value) env->ReleaseIntArrayElements(payload, p_value, 0);
    return ret;
}

static jint meshPublishModelNative(JNIEnv *env, jobject object, jint modelHandle, jint opCode, jint companyId, jintArray buffer) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint i;
    jint ret;
    uint16_t handle = (uint16_t)modelHandle;

    meshif_access_message_tx_t tx;
    memset(&tx, 0, sizeof(meshif_access_message_tx_t));
    tx.opcode.company_id = companyId;
    tx.opcode.opcode= opCode;

    uint16_t len = (uint16_t)env->GetArrayLength(buffer);
    jint* p_value = env->GetIntArrayElements(buffer, NULL);
    if (p_value == NULL) return -1;

    tx.p_buffer = (uint8_t *)malloc(len);
    for (i=0; i<len;i++)
        tx.p_buffer[i] = (uint8_t) (*(p_value + i) & 0xff);
    tx.length = len;
    ret = sMeshIf->model_publish(handle, &tx);
    ALOGI("%s: ret = %d, company_id %d, opcode %d, handle %d, len %d",
        __func__, ret, tx.opcode.company_id, tx.opcode.opcode,handle, len);

    free((uint8_t *)tx.p_buffer);
    if (p_value) env->ReleaseIntArrayElements(buffer, p_value, 0);
    return ret;
}

static jint meshSetMeshModeNative(JNIEnv *env, jobject object, jint mode) {
    ALOGD("%s: set mesh mode %d", __func__, mode);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }


    jint ret = sMeshIf->set_mesh_mode((meshif_mesh_mode_t) mode);
    ALOGD("%s: ret = %d",__func__, ret);

    return ret;
}

static jboolean meshResetDataNative(JNIEnv *env, jobject object, jint sector) {
    ALOGD("%s, sector=0x%0x", __func__, (uint32_t) sector);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return JNI_FALSE;
    }

    jboolean ret = sMeshIf->data_reset((uint32_t) sector);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static jboolean meshSaveDataNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return JNI_FALSE;
    }

    jboolean ret = sMeshIf->data_save();

    ALOGI("%s: ret = %d", __func__, ret);

    return ret;
}

static jcharArray meshGetVersionNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return NULL;
    }

    char* version = NULL;
    jcharArray vArray = NULL;
    jchar *pvArray = NULL;

    version = (char*)malloc(64);
    if (version == NULL) {
        ALOGE("%s: version is null", __func__);
        return NULL;
    }
    memset(version, 0, 64);

    sMeshIf->version(version);
    int len = strlen(version);
    if (len == 0) {
        ALOGE("%s: version len 0", __func__);
        return NULL;
    }
    ALOGI("%s: version length = %d, version = %s", __func__, len, version);

    pvArray = (jchar*)malloc(len);
    if (pvArray == NULL) {
        ALOGE("%s: pvArray is null", __func__);
        return NULL;
    }
    for (int i=0; i<len; i++) {
        *(pvArray + i) = *(version + i);
    }

    vArray = env->NewCharArray(len);
    env->SetCharArrayRegion(vArray, 0, len, pvArray);

    if (version) free(version);
    if (pvArray) free(pvArray);
    return vArray;
}

static void meshDumpNative(JNIEnv *env, jobject object, jint type) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return;
    }

    sMeshIf->dump((meshif_dump_type_t) type);
    ALOGI("%s: return from dump", __func__);
}

static jint meshGetElementAddrNative(JNIEnv *env, jobject object, jint element_index) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret;

    ret = (jint)sMeshIf->get_element_address(element_index);
    ALOGI("%s: element_index %d, ret %d", __func__, element_index, ret);
    return ret;
}

static void meshSetDefaultTTLNative(JNIEnv *env, jobject object, jint ttl) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return;
    }

    ALOGI("%s: ttl %d", __func__, ttl);
    sMeshIf->set_default_ttl((uint8_t) ttl);
    ALOGI("%s: return from set_default_ttl", __func__);
}

static jint meshGetDefaultTTLNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ttl;

    ttl =  (jint) sMeshIf->get_default_ttl();
    ALOGI("%s: ttl %d", __func__, ttl);
    return ttl;
}

static jint meshSetIvNative(JNIEnv *env, jobject object, jlong ivIndex, jint ivPhase) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }
    jint ret;
    meshif_iv_t iv;
    memset(&iv, 0, sizeof(meshif_iv_t));
    iv.ivIndex = (uint32_t)ivIndex;
    iv.ivPhase = (uint8_t)ivPhase;
    ret =  (jint) sMeshIf->set_iv(&iv);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}
static jint meshAddDevkeyNative(JNIEnv *env, jobject object, jint unicastAddr, jintArray devicekey, jintArray uuid) {
    ALOGI("%s unicastAddr %d", __func__, unicastAddr);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }
    jint ret;
    jint i;
    jint* p_value = NULL;
    jint* p_uuid_value = NULL;
    meshif_device_info_t device_info;
    memset(&device_info, 0, sizeof(meshif_device_info_t));
    device_info.opcode = MESHIF_DEV_INFO_OP_ADD_DEVKEY;
    device_info.data.devkey.unicast_addr = (uint16_t)unicastAddr;

    if (devicekey != NULL) {
        uint16_t len = (uint16_t)env->GetArrayLength(devicekey);
        p_value = env->GetIntArrayElements(devicekey, NULL);
        if (p_value == NULL) return -1;
        if (len != MESHIF_KEY_SIZE) {
            ALOGE("%s invalid key size : %d", __func__, len);
            return -1;
        }
        for (i=0; i<len;i++)
            device_info.data.devkey.deviceKey[i] = (uint8_t) (*(p_value + i) & 0xff);
    } else {
        ALOGE("%s device key is NULL", __func__);
        return -1;
    }

    if (uuid != NULL) {
        uint16_t uuid_len = (uint16_t)env->GetArrayLength(uuid);
        p_uuid_value = env->GetIntArrayElements(uuid, NULL);
        if (p_uuid_value == NULL) return -1;
        if (uuid_len != MESHIF_UUID_SIZE) {
            ALOGE("%s invalid UUID size : %d", __func__, uuid_len);
            return -1;
        }
        for (i=0; i<uuid_len;i++)
            device_info.data.devkey.uuid[i] = (uint8_t) (*(p_uuid_value + i) & 0xff);
    } else {
        ALOGE("%s uuid is NULL", __func__);
        return -1;
    }

    ret =  (jint) sMeshIf->set_device_info(&device_info);
    ALOGI("%s: ret = %d", __func__, ret);

    if (p_value) env->ReleaseIntArrayElements(devicekey, p_value, 0);
    if (p_uuid_value) env->ReleaseIntArrayElements(uuid, p_uuid_value, 0);
    return ret;
}
static jintArray meshGetDevKeyNative(JNIEnv *env, jobject object, jint unicastAddr) {
    ALOGI("%s unicastAddr %d", __func__, unicastAddr);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return NULL;
    }
    jint* p_ret;
    jint ret , i;
    meshif_device_info_t device_info;
    memset(&device_info, 0, sizeof(meshif_device_info_t));
    device_info.opcode = MESHIF_DEV_INFO_OP_GET_DEVKEY;
    device_info.data.devkey.unicast_addr = (uint16_t)unicastAddr;

    ret =  (jint) sMeshIf->set_device_info(&device_info);
    ALOGI("%s: ret = %d", __func__, ret);
    if (ret > 0) {
        ALOGE("%s return error: %d", __func__, ret);
        return NULL;
    }
    int data[MESHIF_UUID_SIZE + MESHIF_KEY_SIZE];
    jintArray retArray = env->NewIntArray(MESHIF_UUID_SIZE + MESHIF_KEY_SIZE);
    p_ret = env->GetIntArrayElements(retArray, NULL);

    for (i=0; i<MESHIF_KEY_SIZE;i++)
        data[i] = device_info.data.devkey.deviceKey[i];

    for (i=0; i<MESHIF_UUID_SIZE;i++)
       data[i+MESHIF_KEY_SIZE] = device_info.data.devkey.uuid[i];

    sCallbackEnv->SetIntArrayRegion(retArray, 0, (MESHIF_UUID_SIZE + MESHIF_KEY_SIZE), data);

    return retArray;
}
static jint meshDelDevKeyNative(JNIEnv *env, jobject object, jint unicastAddr) {
    ALOGI("%s unicastAddr %d", __func__, unicastAddr);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }
    jint ret ;
    meshif_device_info_t device_info;
    memset(&device_info, 0, sizeof(meshif_device_info_t));
    device_info.opcode = MESHIF_DEV_INFO_OP_DELETE_DEVKEY;
    device_info.data.devkey.unicast_addr = (uint16_t)unicastAddr;

    ret =  (jint) sMeshIf->set_device_info(&device_info);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static jint meshModelAppBindNative(JNIEnv *env, jobject object, jint model_handle, jint appkey_index) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret;

    ret = sMeshIf->model_app_bind((uint16_t) model_handle, (uint16_t) appkey_index);
    ALOGI("%s: model_handle %d, appkey_index %d, ret %d", __func__, model_handle, appkey_index, ret);
    return ret;
}

static jint meshGetModelHandleNative(JNIEnv *env, jobject object, jlong model_id, jint element_idx) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    uint16_t modelHandle = sMeshIf->get_model_handle_by_elementIdx_and_modeId((uint32_t)model_id, (uint16_t)element_idx);
    ALOGI("%s: modelHandle %d", __func__, modelHandle);

    if (modelHandle == 0xFFFF) return -1;
    return (jint) modelHandle;
}

static jint meshAccessModelReplyNative(JNIEnv *env, jobject object, jint handle, jobject msg, jobject reply) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = -1;
    jint i;

    meshif_access_message_rx_t rxmsg;
    meshif_access_message_tx_t txreply;
    memset(&rxmsg, 0, sizeof(meshif_access_message_rx_t));
    memset(&txreply, 0, sizeof(meshif_access_message_tx_t));

    /*  rx message   */
    jint rx_opcode;
    jint rx_compant_id;
    jintArray rx_buf;
    jint rx_len;
    jint src_addr;
    jint dst_addr;
    jint appkey_index;
    jint netkey_index;
    jint rssi;
    jint ttl;
    jint* p_rxvalue = NULL;

    rx_opcode = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getOpCode");

     rx_compant_id = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getCompanyId");

    rx_buf = callIntArrayGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getBuffer");

    if (rx_buf != NULL) {
        rx_len = env->GetArrayLength(rx_buf);
        p_rxvalue = env->GetIntArrayElements(rx_buf, NULL);
        if (p_rxvalue != NULL) {
            rxmsg.buf = (uint8_t *)malloc(rx_len * sizeof(uint8_t));
            for (i=0; i<rx_len; i++)
                rxmsg.buf[i] = (uint8_t) (*(p_rxvalue + i) & 0xff);
            rxmsg.buf_len = (uint16_t)rx_len;
            ALOGI("%s BluetoothMeshAccessRxMessage buf_len %d", __func__, rxmsg.buf_len);

            if (rx_len > 2) {
                ALOGI("%s BluetoothMeshAccessRxMessage buf[0] %d, buf[1] %d, buf[2] %d !",
                    __func__, rxmsg.buf[0], rxmsg.buf[1], rxmsg.buf[2]);
            }
        } else {
            ALOGE("%s BluetoothMeshAccessRxMessage GetIntArrayElements  NULL !", __func__);
            rxmsg.buf_len = 0;
            rxmsg.buf = NULL;
        }
    } else {
        ALOGI("%s BluetoothMeshAccessRxMessage buf NULL !", __func__);
        rxmsg.buf_len = 0;
        rxmsg.buf = NULL;
    }

     src_addr = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getSrcAddr");
     dst_addr = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getDstAddr");

    appkey_index = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getAppKeyIndex");

    netkey_index = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getNetKeyIndex");

    rssi = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getRssi");

    ttl = callIntGetter(
        env, msg, "android/bluetooth/mesh/BluetoothMeshAccessRxMessage", "getTtl");

    rxmsg.opcode.opcode = (uint16_t)rx_opcode;
    rxmsg.opcode.company_id = (uint16_t)rx_compant_id;

    rxmsg.meta_data.src_addr = (uint16_t)src_addr;
    rxmsg.meta_data.dst_addr = (uint16_t)dst_addr;
    rxmsg.meta_data.appkey_index = (uint16_t)appkey_index;
    rxmsg.meta_data.netkey_index = (uint16_t)netkey_index;
    rxmsg.meta_data.rssi = (uint8_t)rssi;
    rxmsg.meta_data.ttl = (uint8_t)ttl;

    ALOGI("%s: rxmsg  buf_len %d, src_addr %d, dst_addr %d, appkey_index %d, netkey_index %d, rssi %d, ttl %d", __func__,
        rxmsg.buf_len, rxmsg.meta_data.src_addr, rxmsg.meta_data.dst_addr, rxmsg.meta_data.appkey_index,
        rxmsg.meta_data.netkey_index, rxmsg.meta_data.rssi, rxmsg.meta_data.ttl);

    /*  tx message   */
    jint tx_opcode;
    jint tx_compant_id;
    jintArray tx_buf;
    jint *p_txvalue = NULL;
    jint tx_length;

    tx_opcode = callIntGetter(
        env, reply, "android/bluetooth/mesh/BluetoothMeshAccessTxMessage", "getOpCode");

    tx_compant_id = callIntGetter(
        env, reply, "android/bluetooth/mesh/BluetoothMeshAccessTxMessage", "getCompanyId");

    tx_buf = callIntArrayGetter(
        env, reply, "android/bluetooth/mesh/BluetoothMeshAccessTxMessage", "getBuffer");

    txreply.opcode.opcode = tx_opcode;
    txreply.opcode.company_id = tx_compant_id;
    ALOGI("%s: txreply opcode %d, tx_compant_id %d", __func__,
        txreply.opcode.opcode, txreply.opcode.company_id);

    if(tx_buf != NULL) {
        tx_length = env->GetArrayLength(tx_buf);
        p_txvalue = env->GetIntArrayElements(tx_buf, NULL);
        if (p_txvalue != NULL) {
            txreply.p_buffer = (uint8_t *)malloc(tx_length * sizeof(uint8_t));
            for (i=0; i<tx_length; i++)
                txreply.p_buffer[i] = (uint8_t) (*(p_txvalue + i) & 0xff);
            txreply.length =(uint16_t) tx_length;
            ALOGI("%s BluetoothMeshAccessTxMessage tx_length %d", __func__, txreply.length);
            if (tx_length > 2) {
                ALOGI("%s BluetoothMeshAccessTxMessage buf[0] %d, buf[1] %d, buf[2] %d !",
                    __func__, txreply.p_buffer[0], txreply.p_buffer[1], txreply.p_buffer[2]);
            }
        } else {
            ALOGE("%s BluetoothMeshAccessTxMessage GetIntArrayElements  NULL !", __func__);
            txreply.p_buffer = NULL;
            txreply.length = 0;
        }
    } else {
        ALOGI("%s BluetoothMeshAccessTxMessage buf NULL !", __func__);
        txreply.p_buffer = NULL;
        txreply.length = 0;
    }

    ret = sMeshIf->access_model_reply(handle, &rxmsg, &txreply);
    ALOGI("%s: ret = %d", __func__, ret);

    if (rxmsg.buf) free(rxmsg.buf);
    if (txreply.p_buffer) free(txreply.p_buffer);
    if (p_rxvalue) env->ReleaseIntArrayElements(rx_buf, p_rxvalue, 0);
    if (p_txvalue) env->ReleaseIntArrayElements(tx_buf, p_txvalue, 0);
    return ret;
}

static void meshSetLogLevelNative(JNIEnv *env, jobject object, jlong level) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return;
    }

    sMeshIf->log_setlevel((uint32_t) level);
    ALOGI("%s: return from log_setlevel", __func__);
}


static jint meshGattConnectNative(JNIEnv *env, jobject object, jbyteArray address, jint addressType, jint serviceType) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_ble_addr_t bd_addr;
    jint ret = -1;

    jbyte* addrValue = env->GetByteArrayElements(address, NULL);
    jint len = env->GetArrayLength(address);
    if (addrValue == NULL || len != MESHIF_BLE_ADDR_LEN) {
      ALOGE("%s: address null or wrong address length %d.",__func__, len);
      jniThrowIOException(env, EINVAL);
      goto done;
    }

    for (int i=0; i<MESHIF_BLE_ADDR_LEN; i++)
        bd_addr.addr[i] = (uint8_t) (*(addrValue + i) & 0xff);

    bd_addr.addr_type = (meshif_ble_addr_type_t) addressType;

    ret = sMeshIf->gatt_connect(&bd_addr, (meshif_gatt_service_t) serviceType);
    ALOGI("%s: ret = %d", __func__, ret);

done:
    if (addrValue) env->ReleaseByteArrayElements(address, addrValue, 0);
    return ret;
}


static jint meshGattDisconnectNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = sMeshIf->gatt_disconnect();
    ALOGI("%s: ret = %d", __func__, ret);

    return ret;
}

static jint meshSetHeartbeatPeriodNative(JNIEnv *env, jobject object, jint num, jlong hbTimeout) {
    ALOGI("%s num = %d", __func__, num);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret = sMeshIf->set_heartbeat_period((uint8_t) num, (uint32_t) hbTimeout);
    ALOGI("%s: ret = %d", __func__, ret);

    return ret;
}

static jintArray meshOtaGetClientModelHandleNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return NULL;
    }

    jintArray retArray = env->NewIntArray(2);
    jint* ret;
    int data[2];
    uint16_t dist_client;
    uint16_t update_client;

    sMeshIf->ota_get_client_model_handle(&dist_client, &update_client);
    ret = env->GetIntArrayElements(retArray, NULL);
    data[0] = dist_client;
    data[1] = update_client;
    sCallbackEnv->SetIntArrayRegion(retArray, 0, 2, data);

    ALOGI("%s  SDK dist_client %d, update_client %d, Service dist_client %d, update_client %d", __func__, dist_client, update_client, ret[0], ret[1]);
    return retArray;
}

static jint meshOtaInitiatorOperationNative(JNIEnv *env, jobject object, jobject params) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    jint ret;
    jint i;
    jint len;
    jint opcode;
    meshif_ota_operation_params_t ota_params;

    memset(&ota_params, 0, sizeof(meshif_ota_operation_params_t));

    opcode = callIntGetter(
        env, params, "android/bluetooth/mesh/OtaOperationParams", "getOpcode");

    ALOGI("%s  opcode %d", __func__, opcode);
    ota_params.opcode = opcode;
    switch (opcode) {
        case MESHIF_OTA_INITIATOR_OP_REG_MSG_HANDLER:
            ota_params.params.msg_handler.appkey_index = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getAppkeyIndex");

            ota_params.params.msg_handler.ota_msg_handler = NULL;//btmesh_ota_msg_handler_cb;
            break;

        case MESHIF_OTA_INITIATOR_OP_FW_INFO_GET:
            ota_params.params.fw_info_get.node_addr = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getNodeAddr");
            ALOGI("%s  node_addr %d", __func__, ota_params.params.fw_info_get.node_addr);
            break;

        case MESHIF_OTA_INITIATOR_OP_START_DISTRIBUTION:
            jbyteArray nameArray;
            jbyte* name;
            jintArray objIdArray;
            jint* objId;
            jintArray updatersArray;
            jint* updaters;

            nameArray = callByteArrayGetter(
                env, params, "android/bluetooth/mesh/OtaOperationParams", "getObjFile");
            if (nameArray != NULL) {
                name = env->GetByteArrayElements(nameArray, NULL);
                len = env->GetArrayLength(nameArray);
                for (i=0; i<len;i++)
                    ota_params.params.start_params.obj_file[i] = *(name + i);
                ALOGI("%s  new_fw_file %s", __func__, ota_params.params.start_params.obj_file);
            }
            else
                ALOGE("%s: OTA Obj file name NULL!",__func__);

            ota_params.params.start_params.obj_size = (uint32_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getObjSize");
            ALOGI("%s FwSize %d", __func__, ota_params.params.start_params.obj_size);

            objIdArray = callIntArrayGetter(
                env, params, "android/bluetooth/mesh/OtaOperationParams", "getObjId");
            if (objIdArray != NULL) {
                objId = env->GetIntArrayElements(objIdArray, NULL);
                len = env->GetArrayLength(objIdArray);
                for (i=0; i<len;i++)
                    ota_params.params.start_params.obj_id[i] = (uint8_t)(*(objId + i));
            }
            else
                ALOGE("%s: OTA objId is NULL!",__func__);

            ota_params.params.start_params.new_fw_id = (uint32_t)callLongGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getFwId");
            ALOGI("%s FwId 0x%x", __func__, ota_params.params.start_params.new_fw_id);

            ota_params.params.start_params.appkey_index = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getAppkeyIndex");
            ALOGI("%s appkey_index %d", __func__, ota_params.params.start_params.appkey_index);

            ota_params.params.start_params.distributor_addr = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getDistributorAddr");
            ALOGI("%s distributor_addr %d", __func__, ota_params.params.start_params.distributor_addr);

            ota_params.params.start_params.group_addr = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getGroupAddr");
            ALOGI("%s group_addr %d", __func__, ota_params.params.start_params.group_addr);

            ota_params.params.start_params.updaters_num = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getUpdatersNum");
            ALOGI("%s updaters_num %d", __func__, ota_params.params.start_params.updaters_num);

            updatersArray = callIntArrayGetter(
                env, params, "android/bluetooth/mesh/OtaOperationParams", "getUpdaters");
            if (updatersArray != NULL) {
              updaters = env->GetIntArrayElements(updatersArray, NULL);
              len = env->GetArrayLength(updatersArray);
              ota_params.params.start_params.updaters = (uint16_t *)malloc(len * sizeof(uint16_t));
              for (i=0; i<len;i++) {
                  ota_params.params.start_params.updaters[i] = (uint16_t) (*(updaters + i) & 0xffff);
                  ALOGI("%s updater[%d]= 0x%x", __func__, i, ota_params.params.start_params.updaters[i]);
            }
            }
            ota_params.params.start_params.manual_apply = (bool)callBooleanGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getManualApply");
            ALOGI("%s manual_apply %d", __func__, ota_params.params.start_params.manual_apply);

            break;

        case MESHIF_OTA_INITIATOR_OP_STOP_DISTRIBUTION:
            ota_params.params.stop_params.new_fw_id = (uint32_t)callLongGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getFwId");
            ALOGI("%s FwId %u", __func__, ota_params.params.stop_params.new_fw_id);

            ota_params.params.stop_params.distributor_addr = (uint16_t)callIntGetter(
            env, params, "android/bluetooth/mesh/OtaOperationParams", "getDistributorAddr");
            ALOGI("%s distributor_addr %d", __func__, ota_params.params.stop_params.distributor_addr);

            break;

        case MESHIF_OTA_INITIATOR_OP_APPLY_DISTRIBUTION:
            ALOGI("%s APPLY_DISTRIBUTION ! ", __func__);
            break;
    }

    ret = sMeshIf->ota_initiator_operation(&ota_params);
    ALOGI("%s  ota_initiator_operation ret  %d", __func__, ret);
    return ret;
}

static jint meshBearerAdvSetParams(JNIEnv *env, jobject object, jlong advPeriod, jint minInterval, jint maxInterval, jint resend,
                                                                  jlong scanPeriod, jint scanInterval, jint scanWindow) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_bearer_adv_params_t adv_params;

    adv_params.adv_period = (uint32_t) advPeriod;
    adv_params.min_interval = (uint16_t) minInterval;
    adv_params.max_interval = (uint16_t) maxInterval;
    adv_params.resend = (uint8_t) resend;

    meshif_bearer_scan_params_t scan_params;
    scan_params.scan_period = (uint32_t) scanPeriod;
    scan_params.scan_interval = (uint16_t) scanInterval;
    scan_params.scan_window = (uint16_t) scanWindow;

    ALOGD("%s: adv_period = %u, min_interval = %d, max_interval=%d, resend=%d, scan_period = %u, scan_interval = %d, scan_window = %d",
        __func__, adv_params.adv_period, adv_params.min_interval, adv_params.max_interval, adv_params.resend,
        scan_params.scan_period, scan_params.scan_interval, scan_params.scan_window);

    jint ret = sMeshIf->meshif_bearer_adv_set_params(&adv_params, &scan_params);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static jint meshSetScanParamsNative(JNIEnv *env, jobject object, jint scanInterval, jint scanWindow) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_bearer_scan_params_t scan_params;
    scan_params.scan_period = (uint32_t) 0;
    scan_params.scan_interval = (uint16_t) scanInterval;
    scan_params.scan_window = (uint16_t) scanWindow;

    ALOGD("%s: scan_interval = %d, scan_window = %d",__func__, scan_params.scan_interval, scan_params.scan_window);

    jint ret = sMeshIf->meshif_bearer_adv_set_params(NULL, &scan_params);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static jint meshSetSpecialPktParams(JNIEnv *env, jobject object, jboolean isSnIncrease, jint snIncreaseInterval, jint advInterval, jint advPeriod) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return -1;
    }

    meshif_special_pkt_params_t pkt_params;
    pkt_params.sn_increase_flag = isSnIncrease ? 1 : 0;
    pkt_params.sn_increase_interval = (uint32_t) snIncreaseInterval;
    pkt_params.adv_interval= (uint16_t)advInterval;
    pkt_params.adv_period= (uint32_t) advPeriod;
    ALOGD("%s: isSnIncrease = %d, sn_increase_flag = %d, sn_increase_interval = %d, adv_interval = %d, adv_period = %d",
        __func__, isSnIncrease, pkt_params.sn_increase_flag, pkt_params.sn_increase_interval, pkt_params.adv_interval, pkt_params.adv_period);

    jint ret = sMeshIf->meshif_set_special_pkt_params(&pkt_params);
    ALOGI("%s: ret = %d", __func__, ret);
    return ret;
}

static void cleanupNative(JNIEnv *env, jobject object) {
    ALOGI("%s", __func__);
    if (!sMeshIf) {
        ALOGE("%s mesh interface NULL", __func__);
        return;
    }

    if (!btIf) {
        ALOGE("%s bt interface NULL", __func__);
        return;
    }

    if (sMeshIf != NULL) {
        sMeshIf->cleanup();
        ALOGI("%s: return from cleanup", __func__);
        sMeshIf = NULL;
    }

    if (mCallbacksObj != NULL) {
        env->DeleteGlobalRef(mCallbacksObj);
        mCallbacksObj = NULL;
    }
    btIf = NULL;
}


/**
 * JNI function definitinos
 */

// JNI functions defined in MeshService class.
static JNINativeMethod sMethods[] = {
    {"classInitNative", "()V", (void *) classInitNative},
    {"initializeNative", "()V", (void *) initializeNative},
    {"cleanupNative", "()V", (void *) cleanupNative},
    {"meshEnableNative", "(Landroid/bluetooth/mesh/MeshInitParams;)I", (void *) meshEnableNative},
    {"meshDisableNative", "()I", (void *) meshDisableNative},
    {"meshSetCompositionDataHeaderNative", "([I)Z", (void *) meshSetCompositionDataHeaderNative},
    {"meshAddElementNative", "(I)I", (void *) meshAddElementNative},
    {"meshSetElementAddrNative", "(I)Z", (void *) meshSetElementAddrNative},
    {"meshAddModelNative", "(Landroid/bluetooth/mesh/MeshModel;)I", (void *) meshAddModelNative},
    {"meshSetNetKeyNative", "(I[II)I", (void *) meshSetNetKeyNative},
    {"meshSetAppKeyNative", "(I[III)I", (void *) meshSetAppKeyNative},
    {"meshUnProvDevScanNative", "(ZI)V", (void *) meshUnProvDevScanNative},
    {"meshInviteProvisioningNative", "([II)I", (void *) meshInviteProvisioningNative},
    {"meshStartProvisioningNative", "(IIIII[IIJIII)I", (void *) meshStartProvisioningNative},
    {"meshSetProvisionFactorNative", "(I[I)I", (void *) meshSetProvisionFactorNative},
    {"meshSendConfigMessageNative", "(IIIIILandroid/bluetooth/mesh/ConfigMessageParams;)I", (void *) meshSendConfigMessageNative},
    {"meshSendPacketNative", "(IIIII[I)I", (void *) meshSendPacketNative},
    {"meshPublishModelNative", "(III[I)I", (void *) meshPublishModelNative},
    {"meshSetMeshModeNative", "(I)I", (void *) meshSetMeshModeNative},
    {"meshResetDataNative", "(I)Z", (void *) meshResetDataNative},
    {"meshSaveDataNative", "()Z", (void *) meshSaveDataNative},
    {"meshGetVersionNative", "()[C", (void *) meshGetVersionNative},
    {"meshDumpNative", "(I)V", (void *) meshDumpNative},
    {"meshGetElementAddrNative", "(I)I", (void *) meshGetElementAddrNative},
    {"meshSetDefaultTTLNative", "(I)V", (void *) meshSetDefaultTTLNative},
    {"meshGetDefaultTTLNative", "()I", (void *) meshGetDefaultTTLNative},
    {"meshSetIvNative", "(JI)I", (void *) meshSetIvNative},
    {"meshAddDevkeyNative", "(I[I[I)I", (void *) meshAddDevkeyNative},
    {"meshGetDevKeyNative", "(I)[I", (void *) meshGetDevKeyNative},
    {"meshDelDevKeyNative", "(I)I", (void *) meshDelDevKeyNative},
    {"meshModelAppBindNative", "(II)I", (void *) meshModelAppBindNative},
    {"meshGetModelHandleNative", "(JI)I", (void *) meshGetModelHandleNative},
    {"meshAccessModelReplyNative", "(ILandroid/bluetooth/mesh/BluetoothMeshAccessRxMessage;Landroid/bluetooth/mesh/BluetoothMeshAccessTxMessage;)I", (void *) meshAccessModelReplyNative},
    {"meshSetLogLevelNative", "(J)V", (void *) meshSetLogLevelNative},
    {"meshGattConnectNative", "([BII)I", (void *) meshGattConnectNative},
    {"meshGattDisconnectNative", "()I", (void *) meshGattDisconnectNative},
    {"meshSetHeartbeatPeriodNative", "(IJ)I", (void *) meshSetHeartbeatPeriodNative},
    {"meshOtaGetClientModelHandleNative", "()[I", (void *) meshOtaGetClientModelHandleNative},
    {"meshOtaInitiatorOperationNative", "(Landroid/bluetooth/mesh/OtaOperationParams;)I", (void *) meshOtaInitiatorOperationNative},
    {"meshBearerAdvSetParams", "(JIIIJII)I", (void *) meshBearerAdvSetParams},
    {"meshSetScanParamsNative", "(II)I", (void *) meshSetScanParamsNative},
    {"meshSetSpecialPktParams", "(ZIII)I", (void *) meshSetSpecialPktParams},
};

int register_com_android_bluetooth_mesh(JNIEnv* env)
{
    return jniRegisterNativeMethods(env, "com/android/bluetooth/mesh/MeshService",
                                    sMethods, NELEM(sMethods));
}
}
