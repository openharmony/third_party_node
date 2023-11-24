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
 *
 * @Syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */

/**
 * @file native_mediakeysystem.h
 *
 * @brief Defines the Drm MediaKeySystem APIs.
 *
 * @library libdrm_framework.z.so
 * @since 11
 * @version 1.0
 */

#ifndef OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H
#define OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include "native_drm_base.h"
#include "native_drm_err.h"
#include "native_drm_common.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct OH_MediaKeySystem OH_MediaKeySystem;
typedef struct OH_MediaKeySession OH_MediaKeySession;

typedef  OH_DrmErrCode (*OH_MediaKeySystemCallback)(OH_DRM_CharBufferPair *eventInfo);
/**
 * @brief Query if media key system is supported by name.
 * @param name Secifies which drm system will be created.
 * @return Returns a Pointer to an OH_MediaKeySystem instance.
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported(const char *name);
/**
 * @brief Query if media key system is supported by name.
 * @param name Secifies which drm system will be created.
 * @return Returns a Pointer to an OH_MediaKeySystem instance.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported2(const char *name, const char *mimeType);
/**
 * @brief Creates a media key system instance from the uuid.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param uuid Secifies which drm system will be created.
 * @return Returns a Pointer to an OH_MediaKeySystem instance.
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported3(const char *name, const char *mimeType, OH_DRM_ContentProtectionLevel contentProtectionLevel);

/**
 * @brief Creates a media key system instance from the uuid.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param uuid Secifies which drm system will be created.
 * @return Returns a Pointer to an OH_MediaKeySystem instance.
 * @since 11
 * @version 1.0
 */
OH_MediaKeySystem *OH_MediaKeySystem_Create(const char *name);

OH_DrmErrCode OH_MediaKeySystem_SetConfigurationString(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, const char *value);
OH_DrmErrCode OH_MediaKeySystem_GetConfigurationString(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, OH_DRM_CharBuffer *value);
OH_DrmErrCode OH_MediaKeySystem_SetConfigurationByteArray(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, OH_DRM_Uint8Buffer *value);
OH_DrmErrCode OH_MediaKeySystem_GetConfigurationByteArray(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, OH_DRM_Uint8Buffer *value);

OH_DrmErrCode OH_MediaKeySystem_GetMetrics(OH_MediaKeySystem *mediaKeySystem, OH_DRM_Metrics *metrics);
OH_DrmErrCode OH_MediaKeySystem_GetMaxSecurityLevel(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_ContentProtectionLevel *contentProtectionLevel);

OH_DrmErrCode OH_MediaKeySystem_SetMediaKeySystemCallback(OH_MediaKeySystem *mediaKeySystem,
        OH_MediaKeySystemCallback callback);

/**
 * @brief Create a media key session instance.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param mediaKeySystem Media key system instance which will create media key session.
 * @param OH_DRM_ContentProtectionLevel Specifies the security level.
 * @param mediaKeySession Out parameter. Media key session instance has been created
 * if the function returns DRM_ERR_OK.
 * @returns OH_DrmErrCode refers to OH_DrmErrCode
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_CreateMediaKeySession(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_ContentProtectionLevel *level, OH_MediaKeySession **mediaKeySession);

OH_DrmErrCode OH_MediaKeySystem_GenerateKeySystemRequest(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *request, OH_DRM_CharBuffer *defaultUrl);
OH_DrmErrCode OH_MediaKeySystem_ProcessKeySystemResponse(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *response);

OH_DrmErrCode OH_MediaKeySystem_GetOfflineLicenseIds(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_LicenseIdArray *licenseIds);
OH_DrmErrCode OH_MediaKeySystem_GetOfflineLicenseStatus(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *licenseId, OH_DRM_OfflineMediaKeyStatus *status);
OH_DrmErrCode OH_MediaKeySystem_RemoveOfflineLicense(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *licenseId);

OH_DrmErrCode OH_MediaKeySystem_GetCertificateStatus(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_CertificateStatus *certStatus);

/**
 * @brief Destroy a media key system instance.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param uuid Secifies which media key system instance will be destroyed.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_Destroy(OH_MediaKeySystem *mediaKeySystem);


#ifdef __cplusplus
}
#endif

#endif // OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H