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


typedef  OH_DrmErrCode (*OH_MediaKeySystemCallback)(OH_DRM_CharBufferPair *eventInfo);
/**
 * @brief Query if media key system is supported.
 * @param name Used to point a Digital Right Management solution.
 * @return Supported or not in boolean.
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported(const char *name);
/**
 * @brief Query if media key system is supported.
 * @param name Used to point a Digital Right Management solution.
 * @param mimeType Used to specifies the media type.
 * @return Supported or not in boolean.
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported2(const char *name, const char *mimeType);
/**
 * @brief Query if media key system is supported.
 * @param name Used to point a Digital Right Management solution.
 * @param mimeType Used to specifies the media type.
 * @param contentProtectionLevel Used to specifies the ContentProtectionLevel.
 * @return Supported or not in boolean.
 * @since 11
 * @version 1.0
 */
bool OH_MediaKeySystem_IsSupported3(const char *name, const char *mimeType,
    OH_DRM_ContentProtectionLevel contentProtectionLevel);

/**
 * @brief Creates a media key system instance from the name.
 * @param name Secifies which drm system will be created by name.
 * @param mediaKeySystem Media key system instance.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_Create(const char *name, OH_MediaKeySystem **mediaKeySystem);
/**
 * @brief Set media key system configuration value by name.
 * @param mediaKeySystem Media key system instance.
 * @param configName Configuratoin name string.
 * @param value Configuratoin vaule string to be set.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_SetConfigurationString(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, const char *value);
/**
 * @brief Get media key system configuration value by name.
 * @param mediaKeySystem Media key system instance.
 * @param configName Configuratoin name string.
 * @param value Configuratoin vaule string to be get.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetConfigurationString(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, char **value, int32_t *valueLen);
/**
 * @brief Set media key system configuration value by name.
 * @param mediaKeySystem Media key system instance.
 * @param configName Configuratoin name string.
 * @param value Configuratoin vaule in byte array to be set.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_SetConfigurationByteArray(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, OH_DRM_Uint8Buffer *value);
/**
 * @brief Get media key system configuration value by name.
 * @param mediaKeySystem Media key system instance.
 * @param configName Configuratoin name string.
 * @param value Configuratoin vaule in byte array to be get.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetConfigurationByteArray(OH_MediaKeySystem *mediaKeySystem,
    const char *configName, unsigned char **value, int32_t *valueLen);
/**
 * @brief Get media key system statistics info.
 * @param mediaKeySystem Media key system instance.
 * @param statistics Statistic info gotten.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetStatistics(OH_MediaKeySystem *mediaKeySystem, OH_DRM_Statistics **statistics);
/**
 * @brief Get the max content protection level media key system supported.
 * @param mediaKeySystem Media key system instance.
 * @param contentProtectionLevel Content protection level.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetMaxContentProtectionLevel(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_ContentProtectionLevel *contentProtectionLevel);
/**
 * @brief Set media key system event callback.
 * @param mediaKeySystem Media key system instance.
 * @param callback Callback to be set to the media key system.
 * @return Returns OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_SetMediaKeySystemCallback(OH_MediaKeySystem *mediaKeySystem,
    OH_MediaKeySystemCallback callback);

/**
 * @brief Create a media key session instance.
 * @param mediaKeySystem Media key system instance which will create the media key session.
 * @param level Specifies the content protection level.
 * @param mediaKeySession Media key session instance.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_CreateMediaKeySession(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_ContentProtectionLevel *level, OH_MediaKeySession **mediaKeySession);

/**
 * @brief Generate a media key system provision request.
 * @param mediaKeySystem Media key system instance.
 * @param request Provision request data sent to provision server.
 * @param defaultUrl Provision server URL.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GenerateKeySystemRequest(OH_MediaKeySystem *mediaKeySystem, unsigned char **request,
    int32_t *requestLen, char **defaultUrl, int32_t *defaultUrlLen);

/**
 * @brief Process a media key system provision response.
 * @param mediaKeySystem Media key system instance.
 * @param response The provision reponse will be processed.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_ProcessKeySystemResponse(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *response);

/**
 * @brief Get offline media key ids .
 * @param mediaKeySystem Media key system instance.
 * @param mediaKeyIds Media key ids of all offline media keys.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetOfflineMediaKeyIds(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_MediaKeyIdArray **mediaKeyIds, int32_t *mediaKeyIdsLen);

/**
 * @brief Get offline media key status.
 * @param mediaKeySystem Media key system instance.
 * @param mediaKeyId Media key identifier.
 * @param status The media key status gotten.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetOfflineMediaKeyStatus(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *mediaKeyId, OH_DRM_OfflineMediaKeyStatus *status);

/**
 * @brief Clear an offline media key by id.
 * @param mediaKeySystem Media key system instance.
 * @param mediaKeyId Media key identifier.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_ClearOfflineMediaKeys(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_Uint8Buffer *mediaKeyId);

/**
 * @brief Get certificate status of media key system.
 * @param mediaKeySystem Media key system instance.
 * @param certStatus Status will be gotten.
 * @return OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_GetCertificateStatus(OH_MediaKeySystem *mediaKeySystem,
    OH_DRM_CertificateStatus *certStatus);

/**
 * @brief Destroy a media key system instance.
 * @param mediaKeySystem Secifies which media key system instance will be destroyed.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySystem_Destroy(OH_MediaKeySystem *mediaKeySystem);


#ifdef __cplusplus
}
#endif

#endif // OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H