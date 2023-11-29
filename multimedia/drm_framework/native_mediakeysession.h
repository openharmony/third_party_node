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
 * @file native_mediakeysession.h
 * @brief Defines the Drm MediaKeySession APIs. Provide following function:
 * generate media key request, process media key response, event listening,
 * get content protection level, check media key status, remove media key etc..
 * @library libnative_drm.z.so
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */

#ifndef OHOS_DRM_NATIVE_MEDIA_KEY_SESSION_H
#define OHOS_DRM_NATIVE_MEDIA_KEY_SESSION_H

#include <stdint.h>
#include <stdio.h>
#include "native_drm_err.h"
#include "native_drm_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Call back will be invoked when event triggers.
 * @param eventType Event type.
 * @param eventInfo Event info gotten from media key system.
 * @return Drm_ErrCode.
 * @since 11
 * @version 1.0
 */
typedef  Drm_ErrCode (*MediaKeySession_EventCallback)(DRM_ListenerType eventType, DRM_Uint8CharBufferPair *eventInfo);

/**
 * @brief Call back will be invoked when key changes.
 * @param keysInfo Key info gotten from media key system.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
typedef  Drm_ErrCode (*MediaKeySession_KeyChangeCallback)(DRM_KeysInfo *keysInfo, bool newKeysAvailable);

/**
 * @brief MediaKeySession_Callback struct, used to listen event like key expired and key change etc..
 * @since 11
 * @version 1.0
 */
typedef struct MediaKeySession_Callback {
    /**
     * Normal event callback like key expired etc..
     */
    MediaKeySession_EventCallback eventCallback;
    /**
     * Key change callback for keys change event.
     */
    MediaKeySession_KeyChangeCallback keyChangeCallback;
} MediaKeySession_Callback;

/**
 * @brief Generate media key request.
 * @param mediaKeySession Media key session instance.
 * @param info Media key request info.
 * @param mediaKeyRequest Media key request.
 * @return Drm_ErrCode.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_GenerateMediaKeyRequest(MediaKeySession *mediaKeySession,
    DRM_MediaKeyRequestInfo *info, DRM_MediaKeyRequest **mediaKeyRequest);

/**
 * @brief Process media key response.
 * @param mediaKeySession Media key session instance.
 * @param response Media Key resposne.
 * @param mediaKeyId Media key identifier.
 * @param mediaKeyIdLen Media key identifier len.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_ProcessMediaKeyResponse(MediaKeySession *keySession,
    DRM_Uint8Buffer *response, unsigned char **mediaKeyId, int32_t *mediaKeyIdLen);

/**
 * @brief Check media key status.
 * @param mediaKeySession Media key session instance.
 * @param mediaKeyDescription Media key status description.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_CheckMediaKeyStatus(MediaKeySession *mediaKeySessoin,
    DRM_MediaKeyDescription **mediaKeyDescription);

/**
 * @brief Clear media keys of the current session .
 * @param mediaKeySession Media key session instance.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_ClearMediaKeys(MediaKeySession *mediaKeySessoin);

/**
 * @brief Generate offline media key release request.
 * @param mediaKeySession Media key session instance.
 * @param mediaKeyId Media key identifier.
 * @param releaseRequest Media Key release request.
 * @param releaseRequestLen Media Key release request len.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_GenerateOfflineReleaseRequest(MediaKeySession *mediaKeySessoin,
    DRM_Uint8Buffer *mediaKeyId, unsigned char **releaseRequest, int32_t *releaseRequestLen);

/**
 * @brief Process offline media key release response.
 * @param mediaKeySession Media key session instance.
 * @param mediaKeyId Media key identifier.
 * @param releaseReponse Media Key resposne.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_ProcessOfflineReleaseResponse(MediaKeySession *mediaKeySessoin,
    DRM_Uint8Buffer *mediaKeyId, DRM_Uint8Buffer *releaseReponse);

/**
 * @brief Restore offline media keys by ID.
 * @param mediaKeySession Media key session instance.
 * @param mediaKeyId Media key identifier.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_RestoreOfflineMediaKeys(MediaKeySession *mediaKeySessoin,
    DRM_Uint8Buffer *mediaKeyId);

/**
 * @brief Get content protection level of the session.
 * @param mediaKeySession Media key session instance.
 * @param contentProtectionLevel Content protection level.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_GetContentProtectionLevel(MediaKeySession *mediaKeySessoin,
    DRM_ContentProtectionLevel *contentProtectionLevel);

/**
 * @brief Whether the encrypted content require a secure decoder or not.
 * @param mediaKeySession Media key session instance.
 * @param mimeType The media type.
 * @param status Whether secure decoder is required.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_RequireSecureDecoderModule(MediaKeySession *mediaKeySessoin,
    const char *mimeType, bool *status);

/**
 * @brief Set media key session event callback.
 * @param mediaKeySession Media key session instance.
 * @param callback Callback to be set to the media key session.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_SetMediaKeySessionCallback(MediaKeySession *mediaKeySessoin,
    MediaKeySession_Callback *callback);

/**
 * @brief Release the resource before the session gonna be unused.
 * @param mediaKeySession Media key session instance.
 * @return DRM_ERR_INVALID_VAL when the params checked failure, return DRM_ERR_OK when function called successfully.
 * @since 11
 * @version 1.0
 */
Drm_ErrCode OH_MediaKeySession_Destroy(MediaKeySession *mediaKeySessoin);

#ifdef __cplusplus
}
#endif

#endif // OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H