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
 * @file native_drm_err.h
 * @brief Defines the Drm errors.
 * @library libnative_drm.z.so
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */

#ifndef NATIVE_DRM_ERR_H
#define NATIVE_DRM_ERR_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DRM error code
 * @syscap SystemCapability.Multimedia.Drm.Core
 * @since 11
 * @version 1.0
 */
typedef enum Drm_ErrCode {
    /**
     * the operation completed successfully.
     */
    DRM_ERR_OK = 0,
    /**
     * no memory.
     */
    DRM_ERR_NO_MEMORY,
    /**
     * opertation not be permitted.
     */
    DRM_ERR_OPERATION_NOT_PERMITTED,
    /**
     * invalid argument.
     */
    DRM_ERR_INVALID_VAL,
    /**
     * IO error.
     */
    DRM_ERR_IO,
    /**
     * network timeout.
     */
    DRM_ERR_TIMEOUT,
    /**
     * unknown error.
     */
    DRM_ERR_UNKNOWN,
    /**
     * drm service died.
     */
    DRM_ERR_SERVICE_DIED,
    /**
     * not support this operation in this state.
     */
    DRM_ERR_INVALID_STATE,
    /**
     * unsupport interface.
     */
    DRM_ERR_UNSUPPORTED,
    /**
     * Meet max MediaKeySystem num limit.
     */
    DRM_ERR_MAX_SYSTEM_NUM_REACHED,
    /**
     * Meet max MediaKeySession num limit.
     */
    DRM_ERR_MAX_SESSION_NUM_REACHED,
    /**
     * extend err start.
     */
    DRM_ERR_EXTEND_START = 100,
} Drm_ErrCode;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_DRM_ERR_H