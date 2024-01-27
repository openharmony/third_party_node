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
 * @brief Provides the UI component capabilities of the ArkUI on the Native side, such as component creation and destruction, tree node operations, attribute setting, and event listening.
 *
 * @since 12
 */

/**
 * @file native_event.h
 *
 * @brief Provides the event type definition set of the ArkUI on the Native side.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 12
 */

#ifndef ARKUI_NATIVE_EVENT
#define ARKUI_NATIVE_EVENT

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Tool type definition of the Touch event.
 *
 * @since 12
 */
typedef enum {
    /** The tool type is not supported. */
    NODE_TOOL_TYPE_UNKNOWN = -1,

    /** Finger */
    NODE_TOOL_TYPE_FINGER = 0,

    /** Pen */
    NODE_TOOL_TYPE_STYLUS = 1,
} ArkUI_NodeToolType;

/**
 * @brief Source type definition of the Touch event.
 *
 * @since 12
 */
typedef enum {
    /** Unsupported source type */
    NODE_SOURCE_TYPE_UNKNOWN = -1,
    /** Touch screen */
    NODE_SOURCE_TYPE_TOUCH_SCREEN = 0,
    /** Stylus */
    NODE_SOURCE_TYPE_STYLUS = 1,
    /** Touchpad */
    NODE_SOURCE_TYPE_TOUCHPAD = 2,
} ArkUI_NodeSourceType;

/**
 * @brief Defines the data structure of the touch point information of the Touch event.
 *
 * @since 12
 */
typedef struct {
    /** Touch event ID. */
    int32_t id;

    /** Timestamp when a finger is pressed, in microseconds (us). */
    int64_t pressedTime;

    /** X coordinate of the screen to which the touch position belongs. */
    int32_t screenX;

    /** Y coordinate of the screen to which the touch position belongs. */
    int32_t screenY;

    /** X coordinate of the touch position in the window. */
    int32_t windowX;

    /** Y coordinate of the touch position in the window. */
    int32_t windowY;

    /** X coordinate of the touch position in the current trigger event component. */
    int32_t nodeX;

    /** Y coordinate of the touch position in the current trigger event component. */
    int32_t nodeY;

    /** Pressure value. The value range is [0.0, 1.0]. 0.0 indicates that the pressure is not supported. */
    double pressure;

    /** Wideness of the touch area. */
    int32_t contactAreaWidth;

    /** Height of the touch area. */
    int32_t contactAreaHeight;

    /** Angle relative to the YZ plane. The value range is [-90, 90], where a positive value indicates the right slant. */
    double tiltX;

    /** Angle relative to the XZ plane. The value range is [-90, 90]. A positive value indicates a downward tilt. */
    double tiltY;

    /** X coordinate of the center point of the tool area. */
    int32_t toolX;

    /** Y coordinate of the center point of the tool area. */
    int32_t toolY;

    /** Width of the contact area of the tool. */
    int32_t toolWidth;

    /** Height of the contact area of the tool. */
    int32_t toolHeight;

    /** Enter the X coordinate of the device. */
    int32_t rawX;

    /** Input the Y coordinate of the device. */
    int32_t rawY;

    /** Tool type. */
    ArkUI_NodeToolType toolType;
} ArkUI_NodeTouchPoint;

/**
 * @brief Defines the enumerated value of the touch event type.
 *
 * @since 12
 */
typedef enum {
    /**  Touch to cancel. */
    NODE_ACTION_CANCEL = 0,
    /** Touch and press. */
    NODE_ACTION_DOWN = 1,
    /** Touch to move. */
    NODE_ACTION_MOVE = 2,
    /**  Touch to lift. */
    NODE_ACTION_UP = 3,
} ArkUI_NodeTouchEventAction;

/**
 * @brief Defines the structure type of historical point information.
 *
 * @since 12
 */
typedef struct {
    /** Screen touch event type. */
    ArkUI_NodeTouchEventAction action;
    /** Timestamp of historical touchscreen events, in microseconds (us). */
    int64_t timeStamp;
    /** Source type of historical touch events. */
    ArkUI_NodeTouchPoint actionTouch;
    /** Source type of historical touch events. */
    ArkUI_NodeSourceType sourceType;
} ArkUI_NodeHistoricalTouchPoint;

/**
 * @brief Defines the structure type of the Touch event.
 *
 * @since 12
 */
typedef struct {
    /** Type of the touch event. */
    ArkUI_NodeTouchEventAction action;

    /** Timestamp of the touch event, in microseconds (us). */
    int64_t timeStamp;

    /**  Information about the touch point of the current touch event. */
    ArkUI_NodeTouchPoint actionTouch;

    /**
     * @brief Return the information about all screen touch points when this event occurs.
     * @param points Pointer object used to receive data.
     * @return Number of screen contact points.
     * @note
     * When this function is invoked, the framework creates a heap memory object of the touch point information array and returns a pointer. Developers need to manually release the memory by calling delete[] after using the function.
     */
    int32_t (*getTouches)(ArkUI_NodeTouchPoint** points);

    /**
     * @brief Return the historical point information in the event. These are the motions that occur between this event and the previous event.
     * @param historicalPoints Pointer object used to receive data.
     * @return Number of historical points.
     * @note
     * When this function is invoked, the framework creates a heap memory object of the historical point data array and returns a pointer. After the function is invoked, you need to manually release the memory by calling delete[].
     */
    int32_t (*getHistoricalPoints)(ArkUI_NodeHistoricalTouchPoint** historicalPoints);

    /** Type of the triggering event source. */
    ArkUI_NodeSourceType sourceType;

    /** Prohibit further pop-up processing of the event to the parent node. */
    bool stopPropagation;

    /** Block the default event processing behavior of the current node and allow events to bubble up further. */
    bool preventDefault;
} ArkUI_NodeTouchEvent;

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_EVENT
/** @} */
