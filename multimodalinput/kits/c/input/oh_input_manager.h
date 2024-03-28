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

#ifndef OH_INPUT_MANAGER_H
#define OH_INPUT_MANAGER_H

/**
 * @addtogroup input
 * @{
 *
 * @brief Provides the C interface in the multi-modal input domain.
 *
 * @since 12
 */

/**
 * @file oh_input_manager.h
 *
 * @brief Provides capabilities such as event injection and key status query.
 *
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @library liboh_input.so
 * @since 12
 */

#include <stdint.h>

#include "oh_key_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerated values of key event action.
 *
 * @since 12
 */
enum Input_KeyStateAction {
    /** Default */
    KEY_DEFAULT = -1,
    /** Pressing of a key */
    KEY_PRESSED = 0,
    /** Release of a key */
    KEY_RELEASED = 1,
    /** Key switch enabled */
    KEY_SWITCH_ON = 2,
    /** Key switch disabled */
    KEY_SWITCH_OFF = 3
};

/**
 * @brief 按键事件类型
 *
 * @since 12
 */
typedef enum {
    /** 按键取消 */
    KEY_ACTION_CANCEL = 0,
    /** 按键按下 */
    KEY_ACTION_DOWN = 1,
    /** 按键抬起 */
    KEY_ACTION_UP = 2,
} Input_KeyEventAction;

/**
 * @brief Enumerated values of mouse event action.
 *
 * @since 12
 */
typedef enum {
    /** Cancel. */
    MOUSE_ACTION_CANCEL = 0,
    /** Moving of the mouse pointer. */
    MOUSE_ACTION_MOVE = 1,
    /** Pressing down of the mouse. */
    MOUSE_ACTION_BUTTON_DOWN = 2,
    /** Lifting of the mouse button. */
    MOUSE_ACTION_BUTTON_UP = 3,
    /** Beginning of the mouse axis event */
    MOUSE_ACTION_AXIS_BEGIN = 4,
    /** Updating of the mouse axis event */
    MOUSE_ACTION_AXIS_UPDATE = 5,
    /** End of the mouse axis event */
    MOUSE_ACTION_AXIS_END = 6,
} Input_MouseEventAction;

/**
 * @brief Mouse axis types.
 *
 * @since 12
 */
enum InputEvent_MouseAxis {
    /** Vertical scroll axis */
    MOUSE_AXIS_SCROLL_VERTICAL = 0,
    /** Horizontal scroll axis */
    MOUSE_AXIS_SCROLL_HORIZONTAL = 1,
};

/**
 * @brief Enumerated values of mouse event button.
 *
 * @since 12
 */
typedef enum {
    /** Left button on the mouse. */
    MOUSE_BUTTON_LEFT = 0,
    /** Middle button on the mouse. */
    MOUSE_BUTTON_MIDDLE = 1,
    /** Right button on the mouse. */
    MOUSE_BUTTON_RIGHT = 2,
    /** Forward button on the mouse. */
    MOUSE_BUTTON_FORWARD = 3,
    /** Back button on the mouse. */
    MOUSE_BUTTON_BACK = 4,
} Input_MouseEventButton;

/**
 * @brief Enumerated values of touch event action.
 *
 * @since 12
 */
typedef enum {
    /** Touch cancelled. */
    TOUCH_ACTION_CANCEL = 0,
    /** Touch pressed. */
    TOUCH_ACTION_DOWN = 1,
    /** Touch moved. */
    TOUCH_ACTION_MOVE = 2,
    /** Touch lifted. */
    TOUCH_ACTION_UP = 3,
} Input_TouchEventAction;

/**
 * @brief Defines key information, which identifies a key pressing behavior. For example, the Ctrl key information contains the key value and key type.
 *
 * @since 12
 */
struct Input_KeyState;

/**
 * @brief The key event to be injected.
 *
 * @since 12
 */
struct Input_KeyEvent;

/**
 * @brief The mouse event to be injected.
 *
 * @since 12
 */
struct Input_MouseEvent;

/**
 * @brief The touch event to be injected.
 *
 * @since 12
 */
struct Input_TouchEvent;

/**
 * @brief Enumerates the error codes.
 *
 * @since 12
 */
typedef enum {
    /** Success */
    INPUT_SUCCESS = 0,
    /** Permission verification failed */
    INPUT_PERMISSION_DENIED = 201,
    /** Non-system application */
    INPUT_NOT_SYSTEM_APPLICATION = 202,
    /** Parameter check failed */
    INPUT_PARAMETER_ERROR = 401
} Input_Result;

/**
 * @brief Queries the key state.
 *
 * @param keyState Key state.
 * @HTTP4O4 Returns {@Link Input_Result#INPUT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@Link Input_Result} otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
Input_Result OH_Input_GetKeyState(struct Input_KeyState* keyState);

/**
 * @brief Creates a key status enumeration object.
 *
 * @return Returns an {@link Input_KeyState} pointer object if the operation is successful.
 * returns a null pointer otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
struct Input_KeyState* OH_Input_CreateKeyState();

/**
 * @brief Destroys a key status enumeration object.
 *
 * @param keyState Key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_DestroyKeyState(struct Input_KeyState** keyState);

/**
 * @brief Sets the key value of a key status enumeration object.
 *
 * @param keyState Key status enumeration object.
 * @param keyCode Key value of the key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyCode(struct Input_KeyState* keyState, int32_t keyCode);

/**
 * @brief Obtains the key value of a key status enumeration object.
 *
 * @param keyState Key status enumeration object.
 * @return Key value of the key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyCode(struct Input_KeyState* keyState);

/**
 * @brief Sets whether the key specific to a key status enumeration object is pressed.
 *
 * @param keyState Key status enumeration object.
 * @param keyAction Whether the key is pressed.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyPressed(struct Input_KeyState* keyState, int32_t keyAction);

/**
 * @brief Checks whether the key specific to a key status enumeration object is pressed.
 *
 * @param keyState Key status enumeration object.
 * @return Key pressing status of the key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyPressed(struct Input_KeyState* keyState);

/**
 * @brief Sets the key switch of the key status enumeration object.
 *
 * @param keyState Key status enumeration object.
 * @param keySwitch Key switch of the key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeySwitch(struct Input_KeyState* keyState, int32_t keySwitch);

/**
 * @brief Obtains the key switch of the key status enumeration object.
 *
 * @param keyState Key status enumeration object.
 * @return Key switch of the key status enumeration object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeySwitch(struct Input_KeyState* keyState);

/**
 * @brief Inject system keys.
 *
 * @param keyEvent - the key event to be injected.
 * @return 0 - Success.
 *         201 - Missing permissions.
 *         401 - Parameter error.
 * @permission systemapi hide for inner use
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @since 12
 */
int32_t OH_Input_InjectKeyEvent(const struct Input_KeyEvent* keyEvent);

/**
 * @brief Creates a key event object.
 *
 * @return Returns an {@link Input_KeyEvent} pointer object if the operation is successful.
 * returns a null pointer otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
struct Input_KeyEvent* OH_Input_CreateKeyEvent();

/**
 * @brief Destroys a key event object.
 *
 * @param keyEvent event Key event object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_DestroyKeyEvent(struct Input_KeyEvent** keyEvent);

/**
 * @brief 设置按键事件类型
 *
 * @param keyEvent event Key event object.
 * @param action 按键事件类型.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyEventAction(struct Input_KeyEvent* keyEvent, int32_t action);

/**
 * @brief 获取按键事件类型
 *
 * @param keyEvent event Key event object.
 * @return 按键事件类型
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyEventAction(const struct Input_KeyEvent* keyEvent);

/**
 * @brief Sets the key value for a key event.
 *
 * @param keyEvent event Key event object.
 * @param keyCode keyCode Key code.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetKeyEventKeyCode(struct Input_KeyEvent* keyEvent, int32_t keyCode);

/**
 * @brief Obtains the key value of a key event.
 *
 * @param keyEvent event Key event object.
 * @return Key code.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetKeyEventKeyCode(const struct Input_KeyEvent* keyEvent);

/**
 * @brief Inject mouse event.
 *
 * @param mouseEvent - the mouse event to be injected.
 * @return 0 - Success.
 *         201 - Missing permissions.
 *         401 - Parameter error.
 * @permission systemapi hide for inner use
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @since 12
 */
int32_t OH_Input_InjectMouseEvent(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Creates a mouse event object.
 *
 * @return Returns an {@link Input_MouseEvent} pointer object if the operation is successful.
 * returns a null pointer otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
struct Input_MouseEvent* OH_Input_CreateMouseEvent();

/**
 * @brief Destroys a mouse event object.
 *
 * @param mouseEvent Mouse event object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_DestroyMouseEvent(struct Input_MouseEvent** mouseEvent);

/**
 * @brief Sets the action for a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @param action Mouse action.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventAction(struct Input_MouseEvent* mouseEvent, int32_t action);

/**
 * @brief Obtains the action of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return Mouse action.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetMouseEventAction(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Sets the X coordinate for a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @param screenX  X coordinate on the screen.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventScreenX(struct Input_MouseEvent* mouseEvent, int32_t screenX);

/**
 * @brief Obtains the X coordinate of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return X coordinate on the screen.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetMouseEventScreenX(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Sets the Y coordinate for a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @param screenY Y coordinate on the screen.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventScreenY(struct Input_MouseEvent* mouseEvent, int32_t screenY);

/**
 * @brief Obtains the Y coordinate of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return Y coordinate on the screen.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetMouseEventScreenY(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Sets the button for a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @param button Mouse button.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventButton(struct Input_MouseEvent* mouseEvent, int32_t button);

/**
 * @brief Obtains the button of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return Mouse button.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetMouseEventButton(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Sets the axis type for mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @param axisType Axis type, for example, X axis or Y axis.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventAxisType(struct Input_MouseEvent* mouseEvent, int32_t axisType);

/**
 * @brief Obtains the axis type of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return Axis type.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetMouseEventAxisType(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Sets the axis value for a mouse axis event.
 *
 * @param mouseEvent Mouse event object.
 * @param axisType Axis value. A positive value means scrolling forward, and a negative number means scrolling backward.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetMouseEventAxisValue(struct Input_MouseEvent* mouseEvent, float axisValue);

/**
 * @brief Obtains the axis value of a mouse event.
 *
 * @param mouseEvent Mouse event object.
 * @return Axis value.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
float OH_Input_GetMouseEventAxisValue(const struct Input_MouseEvent* mouseEvent);

/**
 * @brief Inject touch event.
 *
 * @param touchEvent - the touch event to be injected.
 * @return 0 - Success.
 *         201 - Missing permissions.
 *         401 - Parameter error.
 * @permission systemapi hide for inner use
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @since 12
 */
int32_t OH_Input_InjectTouchEvent(const struct Input_TouchEvent* touchEvent);

/**
 * @brief Creates a touch event object.
 *
 * @return Returns an {@link Input_TouchEvent} pointer object if the operation is successful.
 * returns a null pointer otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
struct Input_TouchEvent* OH_Input_CreateTouchEvent();

/**
 * @brief Destroys a touch event object.
 *
 * @param touchEvent Touch event object.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_DestroyTouchEvent(struct Input_TouchEvent** touchEvent);

/**
 * @brief Sets the action for a touch event.
 *
 * @param touchEvent Touch event object.
 * @param action Touch action.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetTouchEventAction(struct Input_TouchEvent* touchEvent, int32_t action);

/**
 * @brief Obtains the action of a touch event.
 *
 * @param touchEvent Touch event object.
 * @return Touch action.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetTouchEventAction(const struct Input_TouchEvent* touchEvent);

/**
 * @brief Sets the finger ID for the touch event.
 *
 * @param touchEvent Touch event object.
 * @param id Finger ID.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetTouchEventFingerId(struct Input_TouchEvent* touchEvent, int32_t id);

/**
 * @brief Obtains the finger ID of a touch event.
 *
 * @param touchEvent Touch event object.
 * @return Finger ID.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetTouchEventFingerId(const struct Input_TouchEvent* touchEvent);

/**
 * @brief Sets the X coordinate for a touch event.
 *
 * @param touchEvent Touch event object.
 * @param screenX X coordinate.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetTouchEventDisplayX(struct Input_TouchEvent* touchEvent, int32_t screenX);

/**
 * @brief Obtains the X coordinate of a touch event.
 *
 * @param touchEvent Touch event object.
 * @return X coordinate.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetTouchEventDisplayX(const struct Input_TouchEvent* touchEvent);

/**
 * @brief Sets the Y coordinate for a touch event.
 *
 * @param touchEvent Touch event object.
 * @param screenY Y coordinate.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetTouchEventDisplayY(struct Input_TouchEvent* touchEvent, int32_t screenY);

/**
 * @brief Obtains the Y coordinate of a touch event.
 *
 * @param touchEvent Touch event object.
 * @return Y coordinate.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
int32_t OH_Input_GetTouchEventDisplayY(const struct Input_TouchEvent* touchEvent);

/**
 * @brief Sets the finger pressure for a touch event.
 *
 * @param touchEvent Touch event object.
 * @param pressure Finger pressure.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_SetTouchEventPressure(struct Input_TouchEvent* touchEvent, double pressure);

/**
 * @brief Obtains the finger pressure of a touch event.
 *
 * @param touchEvent Touch event object.
 * @return Finger pressure.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
double OH_Input_GetTouchEventPressure(const struct Input_TouchEvent* touchEvent);

/**
 * @brief 停止注入，取消授权
 *
 * @HTTP4O4 Returns {@Link Input_Result#INPUT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@Link Input_Result} otherwise.
 * @syscap SystemCapability.MultimodalInput.Input.Core
 * @since 12
 */
void OH_Input_StopInjection();

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* OH_INPUT_MANAGER_H */
