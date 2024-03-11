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

#ifndef NATIVE_INPUT_MANAGER_H
#define NATIVE_INPUT_MANAGER_H

/**
 * @addtogroup OHInput
 * @{
 *
 * @brief Provides the C interface in the multi-modal input domain.
 *
 * @since 12
 */


/**
 * @file native_input_manager.h
 *
 * @brief Provides capabilities such as event injection and key status query.
 *
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @library libohinput.so
 * @since 12
 */

#include <stdint.h>
#include "native_key_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerated values of key event action.
 *
 * @since 12
 */
enum {
    /** Default */
    KEY_DEFAULT = -1,
    /** Pressing of a key */
    KEY_PRESSED = 0,
    /** Release of a key */
    KEY_NOT_PRESSED = 1,
};

/**
 * @brief 按键开关状态，包括使能和去使能，比如caplock键切换大小写。
 *
 * @since 12
 */
enum {
    /** 按键开关使能 */
    KEY_SWITCH_ENABLE = 0,
    /** 按键开关去使能 */
    KEY_SWITCH_DISABLE = 1,
};

/**
 * @brief Defines key information, which identifies a key pressing behavior. For example, the Ctrl key information contains the key value and key type.
 *
 * @since 12
 */
struct Input_KeyState;

/**
 * @brief Enumerates the error codes.
 *
 * @since 12
 */
typedef enum {
    /** Success */
    SUCCESS = 0,
    /** Permission verification failed */
    PERMISSION_REFUSED = 201,
    /** Non-system application */
    NOT_SYSTEM_APPLICATION= 202,
    /** Parameter check failed */
    PARAMETER_ERROR = 401,

} Input_ErrorCode;

/**
 * @brief Queries the key status.
 *
 * @param keyState Key state.
 * @return SUCCESS - Success.
 *         PARAMETER_ERROR - Parameter error.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
Input_ErrorCode OH_Input_GetKeyState(struct Input_KeyState* keyState);

/**
 * @brief 创建按键状态枚举对象.
 *
 * @return Returns an {@link Input_KeyState} pointer object if the operation is successful
 * returns a null pointer otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
struct Input_KeyState* OH_Input_CreateKeyState();

/**
 * @brief 销毁按键状态枚举对象。
 * @param keyState 按键状态枚举对象.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_DestroyKeyState(struct Input_KeyState* keyState);

/**
 * @brief 设置按键状态对象的键值。
 * @param keyState 按键状态枚举对象.
 * @param keyCode 按键键值
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyCode(struct Input_KeyState* keyState, int32_t keyCode);

/**
 * @brief 获取按键状态对象的键值。
 * @param keyState 按键状态枚举对象.
 * @return 返回按键状态对象的键值。
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyCode(struct Input_KeyState* keyState);

/**
 * @brief 设置按键状态对象的按键是否按下。
 * @param keyState 按键状态枚举对象.
 * @param keyAction 按键是否按下
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyPressed(struct Input_KeyState* keyState, int32_t keyAction);

/**
 * @brief 获取按键状态对象的按键是否按下。
 * @param keyState 按键状态枚举对象.
 * @return 返回按键状态对象的按键按下状态。
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyPressed(struct Input_KeyState* keyState);

/**
 * @brief 设置按键状态对象的按键开关。
 * @param keyState 按键状态枚举对象.
 * @param keySwitch 按键开关
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeySwitch(struct Input_KeyState* keyState, int32_t keySwitch);

/**
 * @brief 获取按键状态对象的按键开关。
 * @param keyState 按键状态枚举对象.
 * @return 返回按键状态对象的按键开关。
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeySwitch(struct Input_KeyState* keyState);

#ifdef __cplusplus
}
#endif
#endif // NATIVE_INPUT_MANAGER_H
