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

#ifndef NATIVE_DRM_BASE_H
#define NATIVE_DRM_BASE_H

#include <stdint.h>
#include <stdio.h>
#include <refbase.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief MediaKeySystem struct.
 *
 * @since 11
 * @version 1.0
 */
struct OH_MediaKeySystem : public OHOS::RefBase {
    OH_MediaKeySystem() = default;
    virtual ~OH_MediaKeySystem() = default;
};

/**
 * @brief MediaKeySession struct.
 *
 * @since 11
 * @version 1.0
 */
struct OH_MediaKeySession : public OHOS::RefBase {
    OH_MediaKeySession() = default;
    virtual ~OH_MediaKeySession() = default;
};

typedef struct OH_MediaKeySystem OH_MediaKeySystem;
typedef struct OH_MediaKeySession OH_MediaKeySession;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_DRM_BASE_H