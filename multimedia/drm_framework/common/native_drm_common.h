/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Drm
 * @{
 *
 * @brief Provides APIs of Drm.
 * @kit Drm.
 * @since 11
 * @version 1.0
 */

/**
 * @file native_drm_common.h
 *
 * @brief Defines the Drm common struct.
 * @library libnative_drm.z.so
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */

#ifndef NATIVE_DRM_COMMON_H
#define NATIVE_DRM_COMMON_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Enumerates event types of listener.
  * @brief Content potection level.
  * @since 11
  * @version 1.0
*/
typedef enum DRM_ListenerType {
    /**
     * DRM event base.
     */
    LISTENER_DRM_EVENT = 200,
    /**
     * Provision required event.
     */
    LISTENER_PROVISION_REQUIRED = 201,
    /**
     * Media key required event.
     */
    LISTENER_KEY_REQUIRED = 202,
    /**
     * Media key expired event.
     */
    LISTENER_KEY_EXPIRED = 203,
    /**
     * Vendor defined event.
     */
    LISTENER_VENDOR_DEFINED = 204,
    /**
     * Expiration update event.
     */
    LISTENER_EXPIRATION_UPDATE = 206,
  } DRM_ListenerType;

/**
 * @brief Content potection level.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_ContentProtectionLevel {
    /**
     * Content potection level unknown.
     */
    CONTENT_PROTECTION_LEVEL_UNKNOWN = 0,
    /**
     * Content potection level software crypto.
     */
    CONTENT_PROTECTION_LEVEL_SW_CRYPTO,
    /**
     * Content potection level hardware crypto.
     */
    CONTENT_PROTECTION_LEVEL_HW_CRYPTO,
    /**
     * Content potection level enhanced hardware crypto.
     */
    CONTENT_PROTECTION_LEVEL_ENHANCED_HW_CRYPTO,
    /**
     * Content potection level max stub.
     */
    CONTENT_PROTECTION_LEVEL_MAX,
} DRM_ContentProtectionLevel;

/**
 * @brief Media key type.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_MediaKeyType {
    /**
     * Media key type offline.
     */
    MEDIA_KEY_TYPE_OFFLINE = 0,
    /**
     * Media key type online
     */
    MEDIA_KEY_TYPE_ONLINE,
} DRM_MediaKeyType;

/**
 * @brief Media key request type.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_MediaKeyRequestType {
    /**
     * Media key request type unknown.
     */
    MEDIA_KEY_REQUEST_TYPE_UNKNOWN = 0,
    /**
     * Media key request type initial.
     */
    MEDIA_KEY_REQUEST_TYPE_INITIAL,
    /**
     * Media key request type renewal.
     */
    MEDIA_KEY_REQUEST_TYPE_RENEWAL,
    /**
     * Media key request type release.
     */
    MEDIA_KEY_REQUEST_TYPE_RELEASE,
    /**
     * Media key request type none.
     */
    MEDIA_KEY_REQUEST_TYPE_NONE,
    /**
     * Media key request type update.
     */
    MEDIA_KEY_REQUEST_TYPE_UPDATE,
} DRM_MediaKeyRequestType;

/**
 * @brief Offline media key status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_OfflineMediaKeyStatus {
    /**
     * Offline media key status unknown.
     */
    OFFLINE_MEDIA_KEY_STATUS_UNKNOWN = 0,
    /**
     * Offline media key status usable.
     */
    OFFLINE_MEDIA_KEY_STATUS_USABLE,
    /**
     * Offline media key status inactive.
     */
    OFFLINE_MEDIA_KEY_STATUS_INACTIVE,
} DRM_OfflineMediaKeyStatus;

/**
 * @brief Certificate status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_CertificateStatus {
    /**
     * Device already provisioned.
     */
    CERT_STATUS_PROVISIONED = 0,
    /**
     * Device not provisioned.
     */
    CERT_STATUS_NOT_PROVISIONED,
    /**
     * Cert already expired.
     */
    CERT_STATUS_EXPIRED,
    /**
     * Certs are invalid.
     */
    CERT_STATUS_INVALID,
    /**
     * Get certs status failed.
     */
    CERT_STATUS_UNAVAILABLE,
} DRM_CertificateStatus;

/**
 * @brief Offline media key status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum DRM_MediaKeyStatus {
    /**
     * Media key status OK.
     */
    MEDIA_KEY_STATUS_OK = 0,
    /**
     * Media key is invalid e.g. not exist.
     */
    MEDIA_KEY_STATUS_UNAVAILABLE = 1,
} DRM_MediaKeyStatus;

/**
 * @brief Unsigned char buffer.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_Uint8Buffer {
    /**
     * Unsigned char buffer addr.
     */
    unsigned char *buffer;
    /**
     * Unsigned char buffer len.
     */
    uint32_t bufferLen;
} DRM_Uint8Buffer;

/**
 * @brief Char buffer.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_CharBuffer {
    /**
     * Char buffer addr.
     */
    char *buffer;
    /**
     * Char buffer len.
     */
    uint32_t bufferLen;
} DRM_CharBuffer;

/**
 * @brief Char-char buffer pair.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_CharBufferPair {
    /* Name buffer in chars. */
    DRM_CharBuffer name;
    /* Value buffer in chars. */
    DRM_CharBuffer value;
} DRM_CharBufferPair;

/**
 * @brief Unsignedchar-char buffer.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_Uint8CharBufferPair {
    /* Key buffer in Uint8Array. */
    DRM_Uint8Buffer key;
    /* Value buffer in chars. */
    DRM_CharBuffer value;
} DRM_Uint8CharBufferPair;

/**
 * @brief Media key request info.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_MediaKeyRequestInfo {
    /**
     * Offline or online media key type.
     */
    DRM_MediaKeyType type;
    /**
     * Initial data format as PSSH after base64 encoding.
     */
    DRM_Uint8Buffer data;
    /**
     * Media content mime type.
     */
    DRM_CharBuffer mimeType;
    /**
     * OptionsData count.
     */
    uint32_t optionsCount;
    /**
     * Options data the application set to drm framework.
     */
    DRM_CharBufferPair optionsData[0];
} DRM_MediaKeyRequestInfo;

/**
 * @brief Media key request info.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_MediaKeyRequest {
    /**
     * Media key request type.
     */
    DRM_MediaKeyRequestType type;
    /**
     * Media key request data sent to media key server.
     */
    DRM_Uint8Buffer data;
    /**
     * Media key server URL.
     */
    DRM_CharBuffer defaultUrl;
} DRM_MediaKeyRequest;

/**
 * @brief Statistics of MediaKeySystem.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_Statistics {
    /* Statistics count. */
    uint32_t statisticsCount;
    /* Statistics info. */
    DRM_CharBufferPair info[0];
} DRM_Statistics;

/**
 * @brief MediaKeyIds array.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_MediakeyIdArray {
    /* MediaKeyId count. */
    uint32_t mediaKeyIdCount;
    /* MediaKeyIds. */
    DRM_Uint8Buffer mediaKeyIds[0];
} DRM_MediakeyIdArray;

/**
 * @brief Media key info.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_KeysInfo {
    /* Keys count. */
    uint32_t keysCount;
    /* Keys info. */
    DRM_Uint8CharBufferPair keysInfo[0];
} DRM_KeysInfo;

/**
 * @brief MediaKeydescription
 * @since 11
 * @version 1.0
 */
typedef struct DRM_MediaKeyDescription {
    /* MediaKeycount. */
    uint32_t mediaKeyCount;
    /* MediaKeyinfo. */
    DRM_CharBufferPair description[0];
} DRM_MediaKeyDescription;

/**
 * @brief Drm system uuid.
 * @since 11
 * @version 1.0
 */
#define DRM_UUID_LEN 16

/**
 * @brief PSSH info by uuid.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_PsshInfo {
    /**
     * Uuid.
     */
    char uuid[DRM_UUID_LEN];
    /**
     * Unsigned char PSSH len.
     */
    uint32_t dataLen;
    /**
     * Unsigned char PSSH data.
     */
    unsigned char *data;
} DRM_PsshInfo;

/**
 * @brief MediaKeySystemInfo used for player to get media key system info from media source.
 * @since 11
 * @version 1.0
 */
typedef struct DRM_MediaKeySystemInfo {
    /* PSSH count. */
    uint32_t psshCount;
    /* PSSH info. */
    DRM_PsshInfo psshInfo[0];
} DRM_MediaKeySystemInfo;

typedef void (*DRM_MediaKeySystemInfoCallback)(DRM_MediaKeySystemInfo* mediaKeySystemInfo);

/**
 * @brief Media key system struct.
 * @since 11
 * @version 1.0
 */
typedef struct MediaKeySystem MediaKeySystem;

/**
 * @brief Media key session struct.
 * @since 11
 * @version 1.0
 */
typedef struct MediaKeySession MediaKeySession;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_DRM_COMMON_H