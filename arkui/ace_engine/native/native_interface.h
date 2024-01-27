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
 * @brief Provide UI capabilities of the ArkUI on the Native side, such as creating and destroying UI components, operating tree nodes, setting attributes, and listening on events.
 *
 * @since 12
 */

/**
 * @file native_interface.h
 *
 * @brief Provide the unified entry function of the NativeModule interface.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 12
 */

#ifndef ARKUI_NATIVE_INTERFACE_H
#define ARKUI_NATIVE_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the native interface type of any version.
 *
 * @since 12
 */
typedef struct {
/**
 * @brief Define the version information about the Native interface set.
 *
 * Different from the NDK version, the version field of the NativeNode interface indicates the version information of the structure.
 */
int32_t version;
} ArkUI_AnyNativeAPI;

/**
 * @brief Define the native interface set type.
 *
 * @since 12
 */
typedef enum {
    /** Indicates the type of the interface related to the UI component. */
    ARKUI_NATIVE_NODE,
} ArkUI_NativeAPIVariantKind;

/**
 * @brief Obtain the Native interface set of a specified version.
 *
 * @param type indicates the class of the Native interface set provided by the ArkUI, for example, the UI component interface class ARKUI_NATIVE_NODE.
 * @param version Version of the native interface structure, which is obtained through the suffix defined by the structure, for example, ArkUI_NativeNodeAPI_1 of the UI component structure of version 1.
 * @return ArkUI_AnyNativeAPI* Return the abstract object of the Native interface that carries the version.
 * @code {.cpp}
 * #include<arkui/native_interface.h>
 * #include<arkui/native_node.h>
 *
 * auto anyNativeAPI = OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1);
 * if (anyNativeAPI->version == 1) {
 *     auto basicNodeApi = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(anyNativeAPI);
 * }
 * @endcode
 *
 * @since 12
*/
ArkUI_AnyNativeAPI* OH_ArkUI_GetNativeAPI(ArkUI_NativeAPIVariantKind type, int32_t version);

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_INTERFACE_H
/** @} */
