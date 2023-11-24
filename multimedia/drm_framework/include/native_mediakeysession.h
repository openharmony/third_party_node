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
 * @file native_mediakeysession.h
 *
 * @brief Defines the Drm MediaKeySession APIs.
 *
 * @library libdrm_framework.z.so
 * @since 11
 * @version 1.0
 */

#ifndef OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H
#define OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include "native_drm_common.h"
#include "native_drm_base.h"
#include "native_drm_err.h"

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct OH_MediaKeySession OH_MediaKeySession;

typedef  OH_DrmErrCode (*OH_MediaKeySessionEventCallback)(OH_DRM_CharBufferPair *eventInfo);
typedef  OH_DrmErrCode (*OH_MediaKeySessionKeyChangeCallback)(OH_DRM_CharBufferPair *OH_DRM_KeysInfo);
typedef struct OH_MediaKeySessionCallback {
    OH_MediaKeySessionEventCallback eventCallback;
    OH_MediaKeySessionKeyChangeCallback keyChangeCallback;
} OH_MediaKeySessionCallback;
/**
 * @brief Generate license request.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param mediaKeySession Media key session instance.
 * if the function returns DRM_ERR_OK.
 * @param info License request info.
 * @param request Out parameter. License request.
 * @returns OH_DrmErrCode refers to OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySession_GenerateLicenseRequest(OH_MediaKeySession *keySession,
    OH_DRM_LicenseRequestInfo *info, OH_DRM_Uint8Buffer *licenseRequest);

/**
 * @brief Process license request.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param mediaKeySession Media key session instance.
 * @param response License resposne.
 * @param responseLen The length of license resposne.
 * @param licenseId Specifies which license corresponded.
 * @param licenseIdLen The length of license id.
 * @returns OH_DrmErrCode refers to OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySession_ProcessLicenseResponse(OH_MediaKeySession *keySession,
    OH_DRM_Uint8Buffer *response, OH_DRM_Uint8Buffer *licenseId);

/**
 * @brief Check license status.
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @param mediaKeySession Media key session instance.
 * @param response License resposne.
 * @param responseLen The length of license resposne.
 * @param licenseId Specifies which license corresponded.
 * @param licenseIdLen The length of license id.
 * @returns OH_DrmErrCode refers to OH_DrmErrCode.
 * @since 11
 * @version 1.0
 */
OH_DrmErrCode OH_MediaKeySession_CheckLicenseStatus(OH_MediaKeySession *mediaKeySessoin,
    OH_DRM_LicenseDescription *licenseDescription);

OH_DrmErrCode OH_MediaKeySession_RemoveLicense(OH_MediaKeySession *mediaKeySessoin);

OH_DrmErrCode OH_MediaKeySession_GenerateOfflineReleaseRequest(OH_MediaKeySession *mediaKeySessoin,
    OH_DRM_Uint8Buffer *licenseId, OH_DRM_Uint8Buffer *releaseRequest);
OH_DrmErrCode OH_MediaKeySession_ProcessOfflineReleaseResponse(OH_MediaKeySession *mediaKeySessoin,
    OH_DRM_Uint8Buffer *licenseId, OH_DRM_Uint8Buffer *releaseReponse);

OH_DrmErrCode OH_MediaKeySession_RestoreOfflineLicense(OH_MediaKeySession *mediaKeySessoin,
    OH_DRM_Uint8Buffer *licenseId);
OH_DrmErrCode OH_MediaKeySession_GetSecurityLevel(OH_MediaKeySession *mediaKeySessoin,
    OH_DRM_ContentProtectionLevel *contentProtectionLevel);

OH_DrmErrCode OH_MediaKeySession_RequireSecureDecoderModule(OH_MediaKeySession *mediaKeySessoin,
    const char *mimeType, bool *status);

OH_DrmErrCode OH_MediaKeySession_SetMediaKeySessionCallback(OH_MediaKeySession *mediaKeySessoin,
    OH_MediaKeySessionCallback *callback);

OH_DrmErrCode OH_MediaKeySession_Destroy(OH_MediaKeySession *mediaKeySessoin);

#ifdef __cplusplus
}
#endif

#endif // OHOS_DRM_NATIVE_MEDIA_KEY_SYSTEM_H