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

#ifndef NATIVE_DRM_COMMON_H
#define NATIVE_DRM_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include "native_drm_err.h"
#include "native_drm_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Content potection level.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_ContentProtectionLevel {
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
     * Content potection all levels supported.
     */
    CONTENT_PROTECTION_LEVEL_HW_ALL,
    /**
     * Content potection level max stub.
     */
    CONTENT_PROTECTION_LEVEL_MAX,
} OH_DRM_ContentProtectionLevel;

/**
 * @brief License type.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_LicenseType {
    /**
     * License type online
     */
    LICENSE_TYPE_ONLINE = 0,
    /**
     * License type offline.
     */
    LICENSE_TYPE_OFFLINE,
} OH_DRM_LicenseType;

/**
 * @brief License request type.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_LicenseRequestType {
    /**
     * License request type unknown.
     */
    REQUEST_TYPE_UNKNOWN = 0,
    /**
     * License request type initial.
     */
    REQUEST_TYPE_INITIAL = 1,
    /**
     * License request type renewal.
     */
    REQUEST_TYPE_RENEWAL = 2,
    /**
     * License request type release.
     */
    REQUEST_TYPE_RELEASE = 3,
    /**
     * License request type none.
     */
    REQUEST_TYPE_NONE = 4,
} OH_DRM_LicenseRequestType;

/**
 * @brief Offline media key status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_OfflineMediaKeyStatus {
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
} OH_DRM_OfflineMediaKeyStatus;

/**
 * @brief Offline media key status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_CertificateStatus {
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
} OH_DRM_CertificateStatus;

/**
 * @brief Offline media key status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum OH_DRM_LicenseStatus {
    /**
     * License status OK.
     */
    LICENSE_STATUS_OK = 0,
    /**
     * License is invalid e.g. not exist.
     */
    LICENSE_STATUS_NOT_EXIST = 1,
} OH_DRM_LicenseStatus;

/**
 * @brief Unsigned char buffer.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_Uint8Buffer {
    /**
     * Unsigned char buffer addr.
     */
    unsigned char *buffer;
    /**
     * Unsigned char buffer len.
     */
    uint32_t bufferLen;
};

/**
 * @brief Char buffer.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_CharBuffer {
    /**
     * Char buffer addr.
     */
    char *buffer;
    /**
     * Char buffer len.
     */
    uint32_t bufferLen;
};

/**
 * @brief Char-char buffer pair.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_CharBufferPair {
    /* Name buffer in chars.*/
    OH_DRM_CharBuffer name;
    /* Value buffer in chars.*/
    OH_DRM_CharBuffer value;
};

/**
 * @brief Unsignedchar-char buffer.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_Uint8CharBufferPair {
    /* Key buffer in Uint8Array.*/
    OH_DRM_Uint8Buffer key;
    /* Value buffer in chars.*/
    OH_DRM_CharBuffer value;
};

/**
 * @brief Defines the handles of models for Neural Network Runtime.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_LicenseRequestInfo {
    /**
     * Offline or online license type.
     */
    OH_DRM_LicenseType type;
    /**
     * Initial data format as PSSH after base64 encoding.
     */
    OH_DRM_Uint8Buffer data;
    /**
     * Media content mime type.
     */
    OH_DRM_CharBuffer mimeType;
    /**
     * OptionsData count.
     */
    uint32_t optionsCount;
    /**
     * Options data the application set to drm framework.
     */
    OH_DRM_CharBufferPair *optionsData;
};

/**
 * @brief Statistics of OH_MediaKeySystem.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_Statistics {
    /* Statistics count.*/
    uint32_t statisticsCount;
    /* Statistics info.*/
    OH_DRM_CharBufferPair *info;
};

/**
 * @brief License Ids.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_LicenseIdArray {
    /* License Id count.*/
    uint32_t licenseIdCount;
    /* License Ids.*/
    OH_DRM_Uint8Buffer *licenseIds;
};

/**
 * @brief Media key info.
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_KeysInfo {
    /* Keys count.*/
    uint32_t keysCount;
    /* Keys info.*/
    OH_DRM_Uint8CharBufferPair *keysInfo;
};

/**
 * @brief License description
 *
 * @since 11
 * @version 1.0
 */
struct OH_DRM_LicenseDescription {
    /* License count.*/
    uint32_t licenseCount;
    /* License info.*/
    OH_DRM_CharBufferPair *description;
};

typedef struct OH_DRM_Uint8Buffer OH_DRM_Uint8Buffer;
typedef struct OH_DRM_CharBuffer OH_DRM_CharBuffer;
typedef struct OH_DRM_CharBufferPair OH_DRM_CharBufferPair;
typedef struct OH_DRM_Uint8CharBufferPair OH_DRM_Uint8CharBufferPair;
typedef struct OH_DRM_LicenseRequestInfo OH_DRM_LicenseRequestInfo;
typedef struct OH_DRM_MetricInfo OH_DRM_MetricInfo;
typedef struct OH_DRM_Metrics OH_DRM_Metrics;
typedef struct OH_DRM_LicenseIdArray OH_DRM_LicenseIdArray;
typedef struct OH_DRM_LicenseDescription OH_DRM_LicenseDescription;
typedef struct OH_DRM_KeysInfo OH_DRM_KeysInfo;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_DRM_COMMON_H