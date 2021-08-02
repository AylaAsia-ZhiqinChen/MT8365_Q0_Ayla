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

package android.bluetooth.mesh;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import android.os.Parcel;
import android.os.Parcelable;

/**
 *  Bluetooth Mesh internal constants definition
 */
public class MeshConstants {
    /**
     * Debug level logging
     */
    public static final boolean DEBUG = true;

    /**
     * Verbose level logging
     */
    public static final boolean VERBOSE = true;



    /** Mesh Role */
    public static final int MESH_ROLE_PROVISIONEE = 0;
    public static final int MESH_ROLE_PROVISIONER = 1;

    /** Mesh UUID Size */
    public static final int MESH_UUID_SIZE = 16;

    /** Mesh key Size */
    public static final int MESH_KEY_SIZE = 16;

    /** Feature mask */
    public static final int MESH_FEATURE_NONE   = 0x00;     /**< A bit field indicating no feature. */
    public static final int MESH_FEATURE_RELAY  = 0x01;    /**< A bit field indicating feature relay. */
    public static final int MESH_FEATURE_PROXY  = 0x02;    /**< A bit field indicating feature proxy. */
    public static final int MESH_FEATURE_FRIEND = 0x04;   /**< A bit field indicating feature friend. */
    public static final int MESH_FEATURE_LPN    = 0x08;      /**< A bit field indicating feature low power node. */

    /** Mesh Address type enum */
    public static final int MESH_ADDRESS_TYPE_UNASSIGNED   = 0;
    public static final int MESH_ADDRESS_TYPE_UNICAST      = 1;
    public static final int MESH_ADDRESS_TYPE_VIRTUAL      = 2;
    public static final int MESH_ADDRESS_TYPE_GROUP        = 3;

    /** Mesh BLE Addr Type */
    public static final int MESH_BLE_ADDR_TYPE_PUBLIC                = 0; /**< public address */
    public static final int MESH_BLE_ADDR_TYPE_RANDOM_STATIC         = 1; /**< random static address */
    public static final int MESH_BLE_ADDR_TYPE_RANDOM_RESOLVABLE     = 2; /**< random resolvable addresss */
    public static final int MESH_BLE_ADDR_TYPE_RANDOM_NON_RESOLVABLE = 3; /**< random non resolvable address */

    /** Mesh Gatt Service Type */
    public static final int MESH_GATT_SERVICE_PROXY     = 0; /**< Mesh proxy service */
    public static final int MESH_GATT_SERVICE_PROVISION = 1; /**< Mesh provisioning service */

    /** Invalid company id */
    public static final int MESH_MODEL_COMPANY_ID_NONE = 0xFFFF;

    /* The Algorithm values */
    public static final int MESH_PROV_CAPABILITY_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE = (1<<0);    /**< Capabilities bit indicating that the FIPS P256 Elliptic Curve algorithm is supported. */

    /** The Supported Public key OOB type */
    public static final int MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_INBAND = (0);    /**< Capabilities bit indicating that the public key is available in-band. If no public key type is set, this is the default */
    public static final int MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_OOB = (1<<0);    /**< Capabilities bit indicating that the public key is available OOB. */

    /** Provision OOB Info */
    public static final int MESH_PROV_OOB_INFO_FIELD_OTHER                      = (1 << 0x00);   /**< Other location */
    public static final int MESH_PROV_OOB_INFO_FIELD_ELECTRONIC_URI             = (1 << 0x01);   /**< Electronic / URI. */
    public static final int MESH_PROV_OOB_INFO_FIELD_2D_MACHINE_READABLE_CODE   = (1 << 0x02);   /**< 2D machine-readable code. */
    public static final int MESH_PROV_OOB_INFO_FIELD_BAR_CODE                   = (1 << 0x03);   /**< Bar code */
    public static final int MESH_PROV_OOB_INFO_FIELD_NFC                        = (1 << 0x04);   /**< Near Field Communication (NFC) */
    public static final int MESH_PROV_OOB_INFO_FIELD_NUMBER                     = (1 << 0x05);   /**< Number */
    public static final int MESH_PROV_OOB_INFO_FIELD_STRING                     = (1 << 0x06);   /**< String */
    public static final int MESH_PROV_OOB_INFO_FIELD_ON_BOX                     = (1 << 0x0B);   /**< On box */
    public static final int MESH_PROV_OOB_INFO_FIELD_INSIDE_BOX                 = (1 << 0x0C);   /**< Inside box */
    public static final int MESH_PROV_OOB_INFO_FIELD_ON_PIECE_OF_PAPER          = (1 << 0x0D);   /**< On piece of paper */
    public static final int MESH_PROV_OOB_INFO_FIELD_INSIDE_MANUAL              = (1 << 0x0E);   /**< Inside manual */
    public static final int MESH_PROV_OOB_INFO_FIELD_ON_DEVICE                  = (1 << 0x0F);   /**< On device */

    /** OOB type description */
    public static final int MESH_PROV_START_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE  = (0x00);    /**< FIPS P256 Elliptic Curve */
    public static final int MESH_PROV_START_PUBLIC_KEY_NO_OOB                   = (0x00);    /**< No OOB Public Key is used */
    public static final int MESH_PROV_START_PUBLIC_KEY_OOB                      = (0x01);    /**< OOB Public Key is used */

    /** The Authentication Method values */
    public static final int MESH_PROV_START_AUTHEN_METHOD_NO_OOB                = (0x00);    /**< No OOB authentication is used */
    public static final int MESH_PROV_START_AUTHEN_METHOD_STATIC_OOB            = (0x01);    /**< Static OOB authentication is used */
    public static final int MESH_PROV_START_AUTHEN_METHOD_OUTPUT_OOB            = (0x02);    /**< Output OOB authentication is used */
    public static final int MESH_PROV_START_AUTHEN_METHOD_INPUT_OOB             = (0x03);    /**< Input OOB authentication is used */
    public static final int MESH_PROV_CAPABILITY_OOB_STATIC_TYPE_SUPPORTED      = (1<<0);    /**< Capabilities bit indicating that static OOB authentication is supported. */

    /** Sig Group address type description */
    public static final int MESH_ADDR_GROUP_PROXIES_VALUE   = 0xFFFC;  /**< All-proxies group address. */
    public static final int MESH_ADDR_GROUP_FRIENDS_VALUE   = 0xFFFD;  /**< All-friends group address. */
    public static final int MESH_ADDR_GROUP_RELAYS_VALUE    = 0xFFFE;  /**< All-relays group address. */
    public static final int MESH_ADDR_GROUP_NODES_VALUE     = 0xFFFF;  /**< All-nodes group address. */

    /** Primary network key */
    public static final int MESH_PRIMARY_NETWORK_KEY_INDEX = (0x0);    /**< Primary network key index in mesh, can't be modified.*/

    /** Element Location description */
    public static final int MESH_MODEL_ELEMENT_LOCATION_FIRST   = 0x0001;
    public static final int MESH_MODEL_ELEMENT_LOCATION_SECOND  = 0x0002;
    public static final int MESH_MODEL_ELEMENT_LOCATION_FRONT   = 0x0100;
    public static final int MESH_MODEL_ELEMENT_LOCATION_BACK    = 0x0101;
    public static final int MESH_MODEL_ELEMENT_LOCATION_TOP     = 0x0102;
    public static final int MESH_MODEL_ELEMENT_LOCATION_BOTTOM  = 0x0103;
    public static final int MESH_MODEL_ELEMENT_LOCATION_UPPER   = 0x0104;
    public static final int MESH_MODEL_ELEMENT_LOCATION_LOWER   = 0x0105;
    public static final int MESH_MODEL_ELEMENT_LOCATION_MAIN    = 0x0106;

    /** Provision factor */
    public static final int MESH_PROV_FACTOR_CONFIRMATION_KEY          = 0;
    public static final int MESH_PROV_FACTOR_RANDOM_PROVISIONER        = 1;
    public static final int MESH_PROV_FACTOR_RANDOM_DEVICE             = 2;
    public static final int MESH_PROV_FACTOR_CONFIRMATION_PROVISIONER  = 3;
    public static final int MESH_PROV_FACTOR_CONFIRMATION_DEVICE       = 4;
    public static final int MESH_PROV_FACTOR_PUB_KEY                   = 5;
    public static final int MESH_PROV_FACTOR_AUTHEN_VALUE              = 6;
    public static final int MESH_PROV_FACTOR_AUTHEN_RESULT             = 7;
    public static final int MESH_PROV_FACTOR_PROV_INVITE               = 8;

    /** NetKey or AppKey operation code */
    public static final int MESH_KEY_OP_ADD                 = 0;   /*add a netkey or appkey */
    public static final int MESH_KEY_OP_UPDATE              = 1;   /*update a netkey or appkey */
    public static final int MESH_KEY_OP_USE_NEW_NETKEY      = 2;   /*Key refreh phase2 - swithing to the new keys */
    public static final int MESH_KEY_OP_REVOKE_OLD_NETKEY   = 3;   /*Key refresh phase3 - revoking old keys*/

    /**Key refresh phase state*/
    public static final int MESH_KEY_REFRESH_STATE_NONE = 0;    /**< Key refresh phase 0. Indicates normal device operation. */
    public static final int MESH_KEY_REFRESH_STATE_1 = 1;       /**< Key refresh phase 1. Old keys are used for packet transmission, but new keys can be used to receive messages. */
    public static final int MESH_KEY_REFRESH_STATE_2 = 2;       /**< Key refresh phase 2. New keys are used for packet transmission, but old keys can be used to receive messages. */
    public static final int MESH_KEY_REFRESH_STATE_3 = 3;       /**< Key refresh phase 3. Used to complete a key refresh procedure and transition back to phase 0. */

    /** Mesh flash data record: always set (role | record_type), the most significant bit indicates the role*/
    public static final int MESH_FLASH_RECORD_ROLE_PROVISIONEE = 0x00000000;
    public static final int MESH_FLASH_RECORD_ROLE_PROVISIONER = 0x80000000;
    public static final int MESH_FLASH_RECORD_DATA             = 0x00000001;
    public static final int MESH_FLASH_RECORD_SEQ_NUM          = 0x00000002;
    public static final int MESH_FLASH_RECORD_REMOTE_NODE      = 0x00000004;
    //TODO Add more...
    public static final int MESH_FLASH_RECORD_ALL              = 0x7FFFFFFF;

    /** Mesh Dump type */
    public static final int MESH_DUMP_TYPE_UUID         = 0;
    public static final int MESH_DUMP_TYPE_NETWORK      = 1;
    public static final int MESH_DUMP_TYPE_TRANSPORT    = 2;
    public static final int MESH_DUMP_TYPE_CONFIG       = 3;
    public static final int MESH_DUMP_TYPE_MODEL        = 4;
    public static final int MESH_DUMP_TYPE_LPN          = 5;
    public static final int MESH_DUMP_TYPE_PROXY        = 6;
    public static final int MESH_DUMP_TYPE_ADV          = 7;
    public static final int MESH_DUMP_TYPE_ALL          = 8;

    /** Mesh bearer*/
    public static final int MESH_BEARER_ADV         = 0;
    public static final int MESH_BEARER_GATT      = 1;

    /**Mesh working mode*/
    public static final int MESH_MODE_OFF = 0;  //normal mode
    public static final int MESH_MODE_ON = 1;   //mesh working mode
    public static final int MESH_MODE_STANDBY = 2;  //standby mode

    /** special feature mask */
    public static final int MESH_FEATURE_MASK_HEARTBEAT = (1 << 0);   /**heartbeat feature */
    public static final int MESH_FEATURE_MASK_OTA       = (1 << 1);   /**OTA feature */


    /** Mesh Status */
    public static final int MESH_STATUS_SUCCESS             = 0;   /**< success */
    public static final int MESH_STATUS_ERROR_OOM           = 1;  /**< no memory */
    public static final int MESH_STATUS_ERROR_NULL          = 2;  /**< parameter is null pointer */
    public static final int MESH_STATUS_ERROR_INVALID_ADDR  = 3;  /**< invalid address */
    public static final int MESH_STATUS_ERROR_INVALID_TTL   = 4;  /**< invalid TTL value */
    public static final int MESH_STATUS_ERROR_INVALID_KEY   = 5;  /**< invalid key index */
    public static final int MESH_STATUS_ERROR_NOT_INIT      = 6;  /**< mesh core is not initialized */
    public static final int MESH_STATUS_ERROR_INVALID_STATE = 7;  /**< invalid state */
    public static final int MESH_STATUS_ERROR_INVALID_ROLE  = 8;  /**< invalid role */
    public static final int MESH_STATUS_ERROR_FAIL          = 9;  /**< operation failed */

    /** Mesh Acess Msg Status */
    public static final int MESH_ACCESS_MSG_STATUS_SUCCESS                            = 0; /**< Success */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_ADDRESS                    = 1; /**< Invalid Address */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_MODEL                      = 2;/**< Invalid Model */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_APPKEY                     = 3; /**< Invalid AppKey Index */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_NETKEY                     = 4; /**< Invalid NetKey Index */
    public static final int MESH_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES             = 5; /**< Insufficient Resources */
    public static final int MESH_ACCESS_MSG_STATUS_KEY_INDEX_ALREADY_STORED           = 6; /**< Key Index Already Stored */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_PUBLISH_PARAMS             = 7; /**< Invalid Publish Parameters */
    public static final int MESH_ACCESS_MSG_STATUS_NOT_A_SUBSCRIBE_MODEL              = 8; /**< Not a Subscribe Model */
    public static final int MESH_ACCESS_MSG_STATUS_STORAGE_FAILURE                    = 9; /**< Storage Failure */
    public static final int MESH_ACCESS_MSG_STATUS_FEATURE_NOT_SUPPORTED              = 10; /**< Feature Not Supported */
    public static final int MESH_ACCESS_MSG_STATUS_CANNOT_UPDATE                      = 11; /**< Cannot Update */
    public static final int MESH_ACCESS_MSG_STATUS_CANNOT_REMOVE                      = 12; /**< Cannot Remove */
    public static final int MESH_ACCESS_MSG_STATUS_CANNOT_BIND                        = 13; /**< Cannot Bind */
    public static final int MESH_ACCESS_MSG_STATUS_TEMPORARILY_UNABLE_TO_CHANGE_STATE = 14; /**< Temporarily Unable to Change State */
    public static final int MESH_ACCESS_MSG_STATUS_CANNOT_SET                         = 15; /**< Cannot Set */
    public static final int MESH_ACCESS_MSG_STATUS_UNSPECIFIED_ERROR                  = 16; /**< Unspecified Error */
    public static final int MESH_ACCESS_MSG_STATUS_INVALID_BINDING                    = 17; /**< Invalid Binding */

    /** Mesh Model Data Opcode  */
    public static final int MESH_MODEL_DATA_OP_SET_COMPOSITION_DATA_HEADER    = 0;
    public static final int MESH_MODEL_DATA_OP_ADD_ELEMENT                    = 1;
    public static final int MESH_MODEL_DATA_OP_SET_ELEMENT_ADDR               = 2;
    public static final int MESH_MODEL_DATA_OP_ADD_CONFIGURATION_SERVER       = 3;  /* ConfigurationServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT       = 4;  /* ConfigurationClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_HEALTH_SERVER              = 5;  /* HealthServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_HEALTH_CLIENT              = 6;  /* HealthClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_GENERIC_ONOFF_SERVER       = 7;  /* GenericOnOffServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_CTL_SETUP_SERVER           = 8;  /* CtlSetupServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_GENERIC_POWER_ONOFF_CLIENT = 9;  /* GenericPowerOnOffClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_LIGHTNESS_CLIENT           = 10; /* LightnessClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_CTL_CLIENT                 = 11; /* CtlClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_HSL_SETUP_SERVER           = 12; /* HslSetupServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_HSL_CLIENT                 = 13; /* HslClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_GENERIC_LEVEL_SERVER       = 14; /* GenericLevelServerModel */
    public static final int MESH_MODEL_DATA_OP_ADD_GENERIC_LEVEL_CLIENT       = 15; /* GenericLevelClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_GENERIC_ONOFF_CLIENT       = 16; /* GenericOnOffClientModel */
    public static final int MESH_MODEL_DATA_OP_ADD_MODEL                      = 500; /* VendorModel */


    /** Model Messages definition */
    /** Generic On Off Model Message Definition */
    public static final int MESH_MSG_GENERIC_ONOFF_GET = 0x8201;
    public static final int MESH_MSG_GENERIC_ONOFF_SET = 0x8202;
    public static final int MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE = 0x8203;
    public static final int MESH_MSG_GENERIC_ONOFF_STATUS = 0x8204;

    /** Generic Level Model Message Definition */
    public static final int MESH_MSG_GENERIC_LEVEL_GET = 0x8205;
    public static final int MESH_MSG_GENERIC_LEVEL_SET = 0x8206;
    public static final int MESH_MSG_GENERIC_LEVEL_SET_UNACKNOWLEDGED = 0x8207;
    public static final int MESH_MSG_GENERIC_LEVEL_STATUS = 0x8208;
    public static final int MESH_MSG_GENERIC_DELTA_SET = 0x8209;
    public static final int MESH_MSG_GENERIC_DELTA_SET_UNACKNOWLEDGED = 0x820A;
    public static final int MESH_MSG_GENERIC_MOVE_SET = 0x820B;
    public static final int MESH_MSG_GENERIC_MOVE_SET_UNACKNOWLEDGED = 0x820C;

    /** CTL Model Message Definition */
    public static final int MESH_MSG_LIGHT_CTL_GET = 0x825D;
    public static final int MESH_MSG_LIGHT_CTL_SET = 0x825E;
    public static final int MESH_MSG_LIGHT_CTL_SET_UNACKNOWLEDGED = 0x825F;
    public static final int MESH_MSG_LIGHT_CTL_STATUS = 0x8260;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_GET = 0x8261;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET = 0x8262;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_STATUS = 0x8263;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_SET = 0x8264;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED = 0x8265;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_STATUS = 0x8266;
    public static final int MESH_MSG_LIGHT_CTL_DEFAULT_GET = 0x8267;
    public static final int MESH_MSG_LIGHT_CTL_DEFAULT_STATUS = 0x8268;
    public static final int MESH_MSG_LIGHT_CTL_DEFAULT_SET = 0x8269;
    public static final int MESH_MSG_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED = 0x826A;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET = 0x826B;
    public static final int MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED = 0x826C;

    /** HSL Model Message Definition */
    public static final int MESH_MSG_LIGHT_HSL_GET = 0x826D;
    public static final int MESH_MSG_LIGHT_HSL_HUE_GET = 0x826E;
    public static final int MESH_MSG_LIGHT_HSL_HUE_SET = 0x826F;
    public static final int MESH_MSG_LIGHT_HSL_HUE_SET_UNACKNOWLEDGED = 0x8270;
    public static final int MESH_MSG_LIGHT_HSL_HUE_STATUS = 0x8271;
    public static final int MESH_MSG_LIGHT_HSL_SATURATION_GET = 0x8272;
    public static final int MESH_MSG_LIGHT_HSL_SATURATION_SET = 0x8273;
    public static final int MESH_MSG_LIGHT_HSL_SATURATION_SET_UNACKNOWLEDGED = 0x8274;
    public static final int MESH_MSG_LIGHT_HSL_SATURATION_STATUS = 0x8275;
    public static final int MESH_MSG_LIGHT_HSL_SET = 0x8276;
    public static final int MESH_MSG_LIGHT_HSL_SET_UNACKNOWLEDGED = 0x8277;
    public static final int MESH_MSG_LIGHT_HSL_STATUS = 0x8278;
    public static final int MESH_MSG_LIGHT_HSL_TARGET_GET = 0x8279;
    public static final int MESH_MSG_LIGHT_HSL_TARGET_STATUS = 0x827A;
    public static final int MESH_MSG_LIGHT_HSL_DEFAULT_GET = 0x827B;
    public static final int MESH_MSG_LIGHT_HSL_DEFAULT_STATUS = 0x827C;
    public static final int MESH_MSG_LIGHT_HSL_RANGE_GET = 0x827D;
    public static final int MESH_MSG_LIGHT_HSL_RANGE_STATUS = 0x827E;
    public static final int MESH_MSG_LIGHT_HSL_DEFAULT_SET = 0x827F;
    public static final int MESH_MSG_LIGHT_HSL_DEFAULT_SET_UNACKNOWLEDGED = 0x8280;
    public static final int MESH_MSG_LIGHT_HSL_RANGE_SET = 0x8281;
    public static final int MESH_MSG_LIGHT_HSL_RANGE_SET_UNACKNOWLEDGED = 0x8282;

    /** Health model message definition */
    public static final int MESH_MSG_HEALTH_CURRENT_STATUS = 0x04;
    public static final int MESH_MSG_HEALTH_FAULT_STATUS = 0x05;
    public static final int MESH_MSG_HEALTH_FAULT_CLEAR = 0x802F;
    public static final int MESH_MSG_HEALTH_FAULT_CLEAR_UNACKNOWLEDGED = 0x8030;
    public static final int MESH_MSG_HEALTH_FAULT_GET = 0x8031;
    public static final int MESH_MSG_HEALTH_FAULT_TEST = 0x8032;
    public static final int MESH_MSG_HEALTH_FAULT_TEST_UNACKNOWLEDGED = 0x8033;
    public static final int MESH_MSG_HEALTH_PERIOD_GET = 0x8034;
    public static final int MESH_MSG_HEALTH_PERIOD_SET = 0x8035;
    public static final int MESH_MSG_HEALTH_PERIOD_SET_UNACKNOWLEDGED = 0x8036;
    public static final int MESH_MSG_HEALTH_PERIOD_STATUS = 0x8037;
    public static final int MESH_MSG_HEALTH_ATTENTION_GET = 0x8004;
    public static final int MESH_MSG_HEALTH_ATTENTION_SET = 0x8005;
    public static final int MESH_MSG_HEALTH_ATTENTION_SET_UNACKNOWLEDGED = 0x8006;
    public static final int MESH_MSG_HEALTH_ATTENTION_STATUS = 0x8007;


    /** Config message definition */
    public static final int MESH_MSG_CONFIG_BEACON_GET = 0x8009;
    public static final int MESH_MSG_CONFIG_BEACON_SET = 0x800A;
    public static final int MESH_MSG_CONFIG_BEACON_STATUS = 0x800B;
    public static final int MESH_MSG_CONFIG_COMPOSITION_DATA_GET = 0x8008;
    public static final int MESH_MSG_CONFIG_COMPOSITION_DATA_STATUS = 0x02;
    public static final int MESH_MSG_CONFIG_DEFAULT_TTL_GET = 0x800C;
    public static final int MESH_MSG_CONFIG_DEFAULT_TTL_SET = 0x800D;
    public static final int MESH_MSG_CONFIG_DEFAULT_TTL_STATUS = 0x800E;
    public static final int MESH_MSG_CONFIG_GATT_PROXY_GET = 0x8012;
    public static final int MESH_MSG_CONFIG_GATT_PROXY_SET = 0x8013;
    public static final int MESH_MSG_CONFIG_GATT_PROXY_STATUS = 0x8014;
    public static final int MESH_MSG_CONFIG_FRIEND_GET = 0x800F;
    public static final int MESH_MSG_CONFIG_FRIEND_SET = 0x8010;
    public static final int MESH_MSG_CONFIG_FRIEND_STATUS = 0x8011;
    public static final int MESH_MSG_CONFIG_MODEL_PUBLICATION_GET = 0x8018;
    public static final int MESH_MSG_CONFIG_MODEL_PUBLICATION_SET = 0x03;
    public static final int MESH_MSG_CONFIG_MODEL_PUBLICATION_STATUS = 0x8019;
    public static final int MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET = 0x801A;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD = 0x801B;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE = 0x801C;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL = 0x801D;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE = 0x801E;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS = 0x801F;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD = 0x8020;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE = 0x8021;
    public static final int MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE = 0x8022;
    public static final int MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET = 0x8023;
    public static final int MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET = 0x8024;
    public static final int MESH_MSG_CONFIG_NETWORK_TRANSMIT_STATUS = 0x8025;
    public static final int MESH_MSG_CONFIG_RELAY_GET = 0x8026;
    public static final int MESH_MSG_CONFIG_RELAY_SET = 0x8027;
    public static final int MESH_MSG_CONFIG_RELAY_STATUS = 0x8028;
    public static final int MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET = 0x8029;
    public static final int MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST = 0x802A;
    public static final int MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET = 0x802B;
    public static final int MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST = 0x802C;
    public static final int MESH_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_GET = 0x802D;
    public static final int MESH_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_STATUS = 0x802E;
    public static final int MESH_MSG_CONFIG_NETKEY_ADD = 0x8040;
    public static final int MESH_MSG_CONFIG_NETKEY_DELETE = 0x8041;
    public static final int MESH_MSG_CONFIG_NETKEY_GET = 0x8042;
    public static final int MESH_MSG_CONFIG_NETKEY_LIST = 0x8043;
    public static final int MESH_MSG_CONFIG_NETKEY_STATUS = 0x8044;
    public static final int MESH_MSG_CONFIG_NETKEY_UPDATE = 0x8045;
    public static final int MESH_MSG_CONFIG_APPKEY_ADD = 0x00;
    public static final int MESH_MSG_CONFIG_APPKEY_UPDATE = 0x01;
    public static final int MESH_MSG_CONFIG_APPKEY_DELETE = 0x8000;
    public static final int MESH_MSG_CONFIG_APPKEY_GET = 0x8001;
    public static final int MESH_MSG_CONFIG_APPKEY_LIST = 0x8002;
    public static final int MESH_MSG_CONFIG_APPKEY_STATUS = 0x8003;
    public static final int MESH_MSG_CONFIG_MODEL_APP_BIND = 0x803D;
    public static final int MESH_MSG_CONFIG_MODEL_APP_STATUS = 0x803E;
    public static final int MESH_MSG_CONFIG_MODEL_APP_UNBIND = 0x803F;
    public static final int MESH_MSG_CONFIG_SIG_MODEL_APP_GET = 0x804B;
    public static final int MESH_MSG_CONFIG_SIG_MODEL_APP_LIST = 0x804C;
    public static final int MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET = 0x804D;
    public static final int MESH_MSG_CONFIG_VENDOR_MODEL_APP_LIST = 0x804E;
    public static final int MESH_MSG_CONFIG_NODE_IDENTITY_GET = 0x8046;
    public static final int MESH_MSG_CONFIG_NODE_IDENTITY_SET = 0x8047;
    public static final int MESH_MSG_CONFIG_NODE_IDENTITY_STATUS = 0x8048;
    public static final int MESH_MSG_CONFIG_NODE_RESET = 0x8049;
    public static final int MESH_MSG_CONFIG_NODE_RESET_STATUS = 0x804A;
    public static final int MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET = 0x8015;
    public static final int MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET = 0x8016;
    public static final int MESH_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS = 0x8017;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET = 0x8038;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET = 0x8039;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS = 0x06;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET = 0x803A;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET = 0x803B;
    public static final int MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS = 0x803C;


    /** Model ID */
    /** Foundation model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER = 0x0000;    /**< The model ID of the configuration server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT = 0x0001;    /**< The model ID of the configuration client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_HEALTH_SERVER = 0x0002;    /**< The model ID of the health  server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_HEALTH_CLIENT = 0x0003;    /**< The model ID of the health client. */

    /** Generic model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER = 0x1000;    /**< The model ID of the generic onoff server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT = 0x1001;    /**< The model ID of the generic onoff client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_SERVER = 0x1002;    /**< The model ID of the generic level server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT = 0x1003;    /**< The model ID of the generic level clinet. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_DEFAULT_TRANSITION_TIME_SERVER = 0x1004;    /**< The model ID of the generic default transition time server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT = 0x1005;    /**< The model ID of the generic default transition time client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SERVER = 0x1006;    /**< The model ID of the generic power onoff server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SERVER = 0x1007;    /**< The model ID of the generic power onoff setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_CLIENT = 0x1008;    /**< The model ID of the generic power onoff client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SERVER = 0x1009;    /**< The model ID of the generic power level server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SERVER = 0x100a;    /**< The model id of the generic power level setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_CLIENT = 0x100b;    /**< The model ID of the generic power level client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_BATTERY_SERVER = 0x100c;    /**< The model ID of the generic battery server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_BATTERY_CLIENT = 0x100d;    /**< The model ID of the generic battery client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_SERVER = 0x100e;    /**< The model ID of the generic location server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_SETUP_SERVER = 0x100f;    /**< The model ID of the generic location server server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_CLIENT = 0x1010;    /**< The model ID of the generic location client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_ADMIN_PROPERTY_SERVER = 0x1011;    /**< The model ID of the generic admin property server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_MANUFACTURER_PROPERTY_SERVER = 0x1012;    /**< The model ID of the generic manufacturer property server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_USER_PROPERTY_SERVER = 0x1013;    /**< The model ID of the generic user property server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_CLIENT_PROPERTY_SERVER = 0x1014;    /**< The model ID of the generic client property server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_GENERIC_PROPERTY_CLIENT = 0x1015;    /**< The model ID of the generic property client. */

    /** Sensors model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_SENSOR_SERVER = 0x1101;    /**< The model ID of the sensor server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SENSOR_SETUP_SERVER = 0x1102;    /**< The model ID of the sensor setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SENSOR_CLIENT = 0x1103;    /**< The model ID of the sensor client. */

    /** Time and Scenes model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_TIME_SERVER = 0x1200;    /**< The model ID of the time server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_TIME_SETUP_SERVER = 0x1201;    /**< The model ID of the time setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_TIME_CLIENT = 0x1202;    /**< The model ID of the time client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCENE_SERVER = 0x1203;    /**< The model ID of the scene server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCENE_SETUP_SERVER = 0x1204;    /**< The model ID of the scene setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCENE_CLIENT = 0x1205;    /**< The model ID of the scene client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCHEDULER_SERVER = 0x1206;    /**< The model ID of the scheduler server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCHEDULER_SETUP_SERVER = 0x1207;    /**< The model ID of the scheduler setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_SCHEDULER_CLIENT = 0x1208;    /**< The model ID of the scheduler client. */

    /** Lighting model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_SERVER = 0x1300;    /**< The model ID of the light lightness server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SERVER = 0x1301;    /**< The model ID of the light lightness setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_CLIENT = 0x1302;    /**< The model ID of the light lightness client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_SERVER = 0x1303;    /**< The model ID of the light CTL server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_SETUP_SERVER = 0x1304;    /**< The model ID of the light CTL setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_CLIENT = 0x1305;    /**< The model ID of the light CTL client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_TEMPERATURE_SERVER = 0x1306;    /**< The model ID of the light CTL temperature server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SERVER = 0x1307;    /**< The model ID of the light HSL server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SETUP_SERVER = 0x1308;    /**< The model ID of the light HSL setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_CLIENT = 0x1309;    /**< The model ID of the light HSL client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_HUE_SERVER = 0x130a;    /**< The model ID of the light HSL HUE server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SATURATION_SERVER = 0x130b;    /**< The model ID of the light XYL server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_SERVER = 0x130c;    /**< The model ID of the light XYL setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_SETUP_SERVER = 0x130d;    /**< The model ID of the light XYL setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_CLIENT = 0x130e;    /**< The model ID of the light XYL client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_SERVER = 0x130f;    /**< The model ID of the light LC server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_SETUP_SERVER = 0x1310;    /**< The model ID of the light LC setup server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_CLIENT = 0x1311;    /**< The model ID of the light LC client. */

    /** Firmware update model ID */
    public static final int MESH_MODEL_SIG_MODEL_ID_FIRMWARE_UPDATE_SERVER = 0xFE00;    /**< The model ID of the firmware update server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_FIRMWARE_UPDATE_CLIENT = 0xFE01;    /**< The model ID of the firmware update client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_FIRMWARE_DISTRIBUTION_SERVER = 0xFE02;    /**< The model ID of the firmware distribution server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_FIRMWARE_DISTRIBUTION_CLIENT = 0xFE03;    /**< The model ID of the firmware distribution client. */
    public static final int MESH_MODEL_SIG_MODEL_ID_OBJECT_TRANSFER_SERVER = 0xFF00;    /**< The model ID of the object transfer server. */
    public static final int MESH_MODEL_SIG_MODEL_ID_OBJECT_TRANSFER_CLIENT = 0xFF01;    /**< The model ID of the object transfer client. */


    /**Light Lightness Message Definition */
    public static final int MESH_MSG_LIGHT_LIGHTNESS_GET = 0x824B;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_SET = 0x824C;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED = 0x824D;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_STATUS = 0x824E;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET = 0x824F;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET = 0x8250;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED = 0x8251;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LINEAR_STATUS = 0x8252;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LAST_GET = 0x8253;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_LAST_STATUS = 0x8254;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET = 0x8255;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_STATUS = 0x8256;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET = 0x8257;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_RANGE_STATUS = 0x8258;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET = 0x8259;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKNOWLEDGED = 0x825A;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET = 0x825B;
    public static final int MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET_UNACKNOWLEDGED = 0x825C;

    /** OTA operation ID */
    public static final int MESH_OTA_INITIATOR_OP_REG_MSG_HANDLER        = 0;
    public static final int MESH_OTA_INITIATOR_OP_FW_INFO_GET            = 1;
    public static final int MESH_OTA_INITIATOR_OP_START_DISTRIBUTION     = 2;
    public static final int MESH_OTA_INITIATOR_OP_STOP_DISTRIBUTION      = 3;
    public static final int MESH_OTA_OTA_INITIATOR_OP_APPLY_DISTRIBUTION = 4;

    /** Mesh Friendship Status */
    public static final int MESH_FRIENDSHIP_TERMINATED             = 0; /**< The friendship is terminated. */
    public static final int MESH_FRIENDSHIP_ESTABLISHED            = 1; /**< The friendship is successfully established. */
    public static final int MESH_FRIENDSHIP_ESTABLISH_FAILED       = 2; /**< The friendship is not established. */
    public static final int MESH_FRIENDSHIP_REQUEST_FRIEND_TIMEOUT = 3; /**< Request friend procedure timeout. The status is only received when low power feature in use. */
    public static final int MESH_FRIENDSHIP_SELECT_FRIEND_TIMEOUT  = 4; /**< Select friend procedure timeout. The status is only received when low power feature in use. */

    /** Mesh OTA Event ID */
    public static final int MESH_OTA_EVENT_DISTRIBUTION_STARTING  = 0; /**< Event id for informing status of a new distribution was starting.*/
    public static final int MESH_OTA_EVENT_DISTRIBUTION_STARTED   = 1; /**< Event id for informing status of a new distribution was started.*/
    public static final int MESH_OTA_EVENT_DISTRIBUTION_ONGOING   = 2; /**< Event id for informing status of the distribution was ongoing.*/
    public static final int MESH_OTA_EVENT_DISTRIBUTION_STOP      = 3; /**< Event id for informing status of the distirbution was stopped.*/
    public static final int MESH_OTA_EVENT_DISTRIBUTION_QUEUED    = 4; /**< Event id for informing status of a new distribution was queued.*/
    public static final int MESH_OTA_EVENT_DISTRIBUTION_DFU_READY = 5; /**< Event id for informing status of the distribution was dfu ready.*/

    /** Mesh OTA Status Error Code */
    public static final int MESH_OTA_ERROR_CODE_SUCCESS           = 0; /**< Error code of indicating success.*/
    public static final int MESH_OTA_ERROR_CODE_WRONG_FIRMWARE_ID = 1; /**< Error code of inidcating wrong firmware id.*/
    public static final int MESH_OTA_ERROR_CODE_BUSY              = 2; /**< Error code of inidcating busy of distributor*/
    public static final int MESH_OTA_ERROR_CODE_NO_RESPONSE       = 3; /**< Error code of inidcating no response of distributor*/
    public static final int MESH_OTA_ERROR_CODE_USER_STOP         = 4; /**< Error code of inidcating user interuption*/

    /** Mesh OTA Node Update Status */
    public static final int MESH_OTA_NODE_UPDATE_STATUS_SUCCESS     = 0;
    public static final int MESH_OTA_NODE_UPDATE_STATUS_IN_PROGRESS = 1;
    public static final int MESH_OTA_NODE_UPDATE_STATUS_CANCELED    = 2; /**< Failed with some problem.*/
    public static final int MESH_OTA_NODE_UPDATE_STATUS_DFU_READY   = 3;

    /** Mesh Report Type */
    public static final int MESH_REPORT_TYPE_IND         = 0x00; /**< Type for ADV_IND found (passive).*/
    public static final int MESH_REPORT_TYPE_DIRECT_IND  = 0x01; /**< Type for ADV_DIRECT_IND found (passive).*/
    public static final int MESH_REPORT_TYPE_SCAN_IND    = 0x02; /**< Type for ADV_SCAN_IND found (passive).*/
    public static final int MESH_REPORT_TYPE_NONCONN_IND = 0x03; /**< Type for ADV_NONCONN_IND found (passive).*/
    public static final int MESH_REPORT_TYPE_SCAN_RSP    = 0x04; /**< Type for SCAN_RSP found (active).*/

    /** Mesh IV Update State */
    public static final int MESH_IV_UPDATE_STATE_NORMAL      = 0; /**< Indicates IV update is in normal operation. */
    public static final int MESH_IV_UPDATE_STATE_IN_PROGRESS = 1; /**< Indicates IV update is in progress. */

    /** Mesh Bearer Gatt Status */
    public static final int MESH_BEARER_GATT_STATUS_CONNECTED         = 0; /**< Bearer GATT is connected. */
    public static final int MESH_BEARER_GATT_STATUS_DISCONNECTED      = 1; /**< Bearer GATT is disconnected. */
    public static final int MESH_BEARER_GATT_STATUS_NO_SERVICE        = 2; /**< Bearer GATT failed to be established because the specified service was not found. */
    public static final int MESH_BEARER_GATT_STATUS_NO_CHARACTERISTIC = 3; /**< Bearer GATT failed to be established because the specified characteristics were not found. */
    public static final int MESH_BEARER_GATT_STATUS_WRITE_CCCD_FAILED = 4; /**< Bearer GATT failed to be established because writing the CCCD failed. */
    public static final int MESH_BEARER_GATT_STATUS_CONNECT_FAILED = 5;  /**< Bearer GATT connect fail with 0x3e error. */

    /** Mesh Event Error Code */
    public static final int MESH_ERROR_NO_RESOURCE_TO_ADD_REPLAYPROTECTION = 0;


}
