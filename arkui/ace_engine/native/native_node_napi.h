/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 * @addtogroup ArkUI_NativeModule
 * @{
 *
 * @brief Provides UI capabilities of ArkUI on the native side, such as UI component creation and destruction,
 * tree node operations, attribute setting, and event listening.
 *
 * @since 12
 */

/**
 * @file native_node_napi.h
 *
 * @brief Declares APIs for converting <b>FrameNode</b> objects on the ArkTS side to <b>ArkUI_NodeHandle</b> objects on
 * the native side.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 12
 */

#ifndef ARKUI_NATIVE_NODE_NAPI_H
#define ARKUI_NATIVE_NODE_NAPI_H

#include "napi/native_api.h"
#include "native_type.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Obtains a <b>FrameNode</b> object on the ArkTS side and maps it to an <b>ArkUI_NodeHandle</b> object on the
 * native side.
 *
 * @param env Indicates the NAPI environment pointer.
 * @param frameNode Indicates the <b>FrameNode</b> object created on the ArkTS side.
 * @param handle Indicates the pointer to the <b>ArkUI_NodeHandle</b> object.
 * @return Returns 0 if success.
 * Returns 401 if a parameter exception occurs.
 * @since 12
 */
int32_t OH_ArkUI_GetNodeHandleFromNapiValue(napi_env env, napi_value frameNode, ArkUI_NodeHandle* handle);

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_NODE_NAPI_H
/** @} */
