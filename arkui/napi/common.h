/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_COMMON_H
#define FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_COMMON_H

typedef enum {
    napi_qos_background = 0,
    napi_qos_utility = 1,
    napi_qos_default = 2,
    napi_qos_user_initiated = 3,
} napi_qos_t;

/**
 * @brief Indicates the running mode of the native event loop in an asynchronous native thread.
 *
 * @since 12
 */
typedef enum {
    /**
     * In this mode, the current asynchronous thread will be blocked and events of native event loop will
     * be processed.
     */
    napi_event_mode_default = 0,

    /**
     * In this mode, the current asynchronous thread will not be blocked. If there are events in the event loop,
     * only one event will be processed and then the event loop will stop. If there are no events in the loop,
     * the event loop will stop immediately.
     */
    napi_event_mode_nowait = 1,
} napi_event_mode;

#endif /* FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_NATIVE_API_H */