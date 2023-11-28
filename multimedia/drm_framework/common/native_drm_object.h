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

#ifndef NATIVE_DRM_OBJECT_H
#define NATIVE_DRM_OBJECT_H

#include <stdint.h>
#include <stdio.h>
#include "native_drm_err.h"
#include "native_drm_base.h"

#include "key_session_impl.h"
#include "media_key_system_impl.h"
#include "media_key_system_factory_impl.h"

struct MediaKeySystemObject : public OH_MediaKeySystem {
    explicit MediaKeySystemObject(const OHOS::sptr<OHOS::DrmStandard::MediaKeySystemImpl> &impl)
        : systemImpl_(impl)
    {
    }
    ~MediaKeySystemObject() = default;

    const OHOS::sptr<OHOS::DrmStandard::MediaKeySystemImpl> systemImpl_ = nullptr;
};

struct MediaKeySessionObject : public OH_MediaKeySession {
    explicit MediaKeySessionObject(const OHOS::sptr<OHOS::DrmStandard::MediaKeySessionImpl> &impl)
        : sessionImpl_(impl)
    {
    }
    ~MediaKeySessionObject() = default;

    const OHOS::sptr<OHOS::DrmStandard::MediaKeySessionImpl> sessionImpl_ = nullptr;
};


#endif // NATIVE_DRM_OBJECT_H