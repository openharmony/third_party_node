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
 * @brief Provides the UI capabilities of the ArkUI on the Native side, such as UI component creation and destruction, tree node operation, attribute setting, and event listening.
 *
 * @since 12
 */

/**
 * @file native_node.h
 *
 * @brief Provides the type definition of the NativeNode interface.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 12
 */

#ifndef ARKUI_NATIVE_NODE_H
#define ARKUI_NATIVE_NODE_H

#include "native_event.h"
#include "native_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NODE_SCOPE_NUM 1000

/**
 * @brief This API is used to provide the ArkUI to create component types on the Native side.
 *
 * @since 12
 */
typedef enum {
    /** Text.*/
    ARKUI_NODE_TEXT = 1,
    /** Picture.*/
    ARKUI_NODE_IMAGE = 4,
    /** Status switch.*/
    ARKUI_NODE_TOGGLE = 5,
    /** Stack containers.*/
    ARKUI_NODE_STACK = MAX_NODE_SCOPE_NUM,
    /** Paging container.*/
    ARKUI_NODE_SWIPER,
    /** Roll the container.*/
    ARKUI_NODE_SCROLL,
    /** List.*/
    ARKUI_NODE_LIST,
    /** List item.*/
    ARKUI_NODE_LIST_ITEM,
    /** List item group.*/
    ARKUI_NODE_LIST_ITEM_GROUP,
} ArkUI_NodeType;

/**
 * @brief Defines the general input parameter structure of the {@link setAttribute} function.
 *
 * @since 12
 */
typedef struct {
    /** Number array.*/
    const ArkUI_NumberValue* value;
    /** Size of a numeric array.*/
    int32_t size;
    /** Character string type.*/
    const char* string;
    /** Object type.*/
    void* object;
} ArkUI_AttributeItem;

/**
 * @brief Defines the set of attribute styles that can be set by the ArkUI on the Native side.
 *
 * @since 12
 */
typedef enum {
    /**
     * @brief Width attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: width, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: width, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {1.2};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_WIDTH, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_WIDTH);
     * auto nodeWidth = item->value[0].f32;
     * @endcode
     *
     */
    NODE_WIDTH = 0,
    /**
     * @brief Height attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: height, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: height, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {1.2};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_HEIGHT, &item);clang-tid
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_HEIGHT);
     * auto nodeHeight = item->value[0].f32;
     * @endcode
     *
     */
    NODE_HEIGHT,
    /**
     * @brief Background color attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].u32: indicates the background color. The value is in the 0xargb format. For example, 0xFFFF0000 indicates red.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: indicates the background color. The value is in the 0xargb format. For example, 0xFFFF0000 indicates red.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.u32=0xFFFF0000}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BACKGROUND_COLOR, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_BACKGROUND_COLOR);
     * auto nodeBackgroundColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_BACKGROUND_COLOR,
    /**
     * @brief Background color image attribute. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: image address.\n
     * .value[0]?.i32: optional, repeat parameter, parameter type {@link ArkUI_ImageRepeat}, default value is ARKUI_IMAGE_REPEAT_NO_REPEAT; \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: image address.\n
     * .value[0].i32: repeat parameter, parameter type {@link ArkUI_ImageRepeat}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_IMAGE_REPEAT_NO_REPEAT}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue), "/pages/icon.png"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BACKGROUND_IMAGE, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_BACKGROUND_IMAGE);
     * auto nodeBackgroundImageUrl = item->string;
     * auto nodeBackgroundImageRepeat = item->value[0].i32;
     * @endcode
     *
     */
    NODE_BACKGROUND_IMAGE,
    /**
     * @brief Interspace attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} has two formats:\n
     * 1: The inner spacing values of the upper, lower, left, and right positions are equal.\n
     * .value[0].f32: internal spacing, in vp.\n
     * 2: Specify the inner spacing values of the upper, lower, left, and right positions.\n
     * .value[0].f32: Value of the upper-inside spacing, in vp.\n
     * .value[1].f32: indicates the inner right spacing, in vp.\n
     * .value[2].f32: indicates the spacing between the bottom and the bottom, in vp.\n
     * .value[3].f32: value of the inner-left spacing, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: indicates the upper-inside spacing, in vp.\n
     * .value[1].f32: right inner spacing, in vp.\n
     * .value[2].f32: Value of the spacing between the bottom and the bottom, in vp.\n
     * .value[3].f32: value of the inner-left spacing, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value1[] = {1, 2, 3, 4};
     * ArkUI_AttributeItem item1 = {value1, sizeof(value1)/sizeof(ArkUI_NumberValue)};
     * ArkUI_NumberValue value2[] = {10};
     * ArkUI_AttributeItem item2 = {value2, sizeof(value2)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_PADDING, &item1);
     * nativeNodeApi->setAttribute(nodeHandle, NODE_PADDING, &item2);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_PADDING);
     * auto nodePaddingTop = item->value[0].f32;
     * @endcode
     *
     */
    NODE_PADDING,
    /**
     * @brief Component ID attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: content of ID; \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: content of ID; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "test"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_ID, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_ID);
     * auto nodeId = item->string;
     * @endcode
     *
     */
    NODE_ID,
    /**
     * @brief Use the {@link setAttribute} method to set whether the component can interact with each other. If the value is false, the component does not respond to operations such as clicking the component.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: false: non-interactive; true: interactive.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: 0: non-interactive; 1: interactive.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = false}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_ENABLED, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_ENABLED);
     * auto nodeEnabled = item->value[0].i32;
     * @endcode
     */
    NODE_ENABLED,
    /**
     * @brief Outer spacing attribute, which supports the attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * The attribute setting method parameter {@link ArkUI_AttributeItem} has two formats:\n
     * 1: The outer spacing values of the upper, lower, left, and right positions are equal.\n
     * .value[0].f32: value of the outer spacing, in vp.\n
     * 2: Specify the outer spacing values of the upper, lower, left, and right positions.\n
     * .value[0].f32: indicates the upper and outer spacing, in vp.\n
     * .value[1].f32: indicates the value of the right outer space, in vp.\n
     * .value[2].f32: Value of the spacing between the bottom and the outer, in vp.\n
     * .value[3].f32: value of the outer left spacing, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: indicates the upper and outer spacing, in vp.\n
     * .value[1].f32: indicates the value of the right outer space, in vp.\n
     * .value[2].f32: Value of the spacing between the bottom and the outer, in vp.\n
     * .value[3].f32: indicates the value of the outer-left spacing, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value1[] = {1, 2, 3, 4};
     * ArkUI_AttributeItem item1 = {value1, sizeof(value1)/sizeof(ArkUI_NumberValue)};
     * ArkUI_NumberValue value2[] = {10};
     * ArkUI_AttributeItem item2 = {value2, sizeof(value2)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_MARGIN, &item1);
     * nativeNodeApi->setAttribute(nodeHandle, NODE_MARGIN, &item2);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_MARGIN);
     * auto nodeMarginTop = item->value[0].f32;
     * @endcode
     *
     */
    NODE_MARGIN,
    /**
     * @brief Component translation, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: moving distance on the x axis, in vp. The default value is 0. \n
     * .value[1].f32: moving distance on the y axis, in vp. The default value is 0. \n
     * .value[2].f32: Z-axis moving distance, in vp. The default value is 0. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: moving distance on the x axis, in vp.\n
     * .value[1].f32: moving distance on the y axis, in vp.\n
     * .value[2].f32: Z-axis moving distance, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {100, 20, 0};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_TRANSLATE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TRANSLATE);
     * auto nodeTranslate = item->value[0].f32;
     * @endcode
     *
     */
    NODE_TRANSLATE,
    /**
     * @brief Component scaling, which supports attribute setting, resetting, and obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: scaling coefficient of the x axis. The default value is 1. \n
     * .value[1].f32: scaling factor of the y-axis. The default value is 1. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: scaling coefficient of the x axis.\n
     * .value[1].f32: The scaling factor for the y-axis.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {1.0, 0.5};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCALE, &item);
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_SCALE);
     * auto nodeScale = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SCALE,
    /**
     * @brief Component rotation attribute, which supports attribute setting, resetting, and obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: x-coordinate of the rotation axis vector. The default value is 0. \n
     * .value[1].f32: y-coordinate of the rotation axis vector. The default value is 0. \n
     * .value[2].f32: z coordinate of the rotation axis vector. The default value is 0. \n
     * .value[3].f32: rotation angle. The default value is 0. \n
     * .value[4].f32: line of sight distance, that is, the distance from the viewpoint to the z=0 plane, in vp. The default value is 0. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: x coordinate of the rotation axis vector.\n
     * .value[1].f32: y-coordinate of the rotation axis vector.\n
     * .value[2].f32: z coordinate of the rotation axis vector.\n
     * .value[3].f32: rotation angle; \n
     * .value[4].f32: line of sight distance, that is, the distance from the viewpoint to the z=0 plane, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {0, 0, 1, 300, 0};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_ROTATE, &item);
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_ROTATE);
     * auto nodeRotate = item->value[0].f32;
     * @endcode
     *
     */
    NODE_ROTATE,
    /**
     * @brief Highlight effect of the component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: luminance value. The default value is 1.0. The recommended value range is [0, 2]. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Luminance value.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {1.2};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_BRIGHTNESS, &item);
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_BRIGHTNESS);
     * auto nodeBrightness = item->value[0].f32;
     * @endcode
     *
     */
    NODE_BRIGHTNESS,
    /**
     * @brief Saturation effect of the component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: saturation value. The default value is 1.0. The recommended value range is [0, FLT_MAX]. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Saturation value.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {1.0};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SATURATION, &item);
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_SATURATION);
     * auto nodeSaturate = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SATURATION,
    /**
     * @brief Component content blur effect, which supports attribute setting, resetting, and obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: blur radius. The larger the blur radius, the more blurred the blur. If the parameter is set to 0, the blur is not blurred. The unit is vp. The default value is 0.0.\n
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: blur radius. The larger the blur radius, the more blurred the blur. If the parameter is set to 0, the blur is not blurred. The unit is vp.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {1.0};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_BLUR, "1.0");
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_BLUR);
     * auto nodeBlur = item->value[0].f32;
     * @endcode
     *
     */
    NODE_BLUR,
    /**
     * @brief Linear color gradient effect of the component. Attribute setting, attribute reset, and attribute obtaining are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: character string combination parameter. Four input parameters are separated by semicolons (;). \n
     * Input parameter 1: indicates the gradient color at a certain percentage position. If an invalid color is set, the gradient color is skipped. Color and position (unit: vp) array type, separated by commas.\n
     * Input parameter 2: start angle of the linear gradient. 0 o'clock rotation is the positive angle. The default value is 180.\n
     * Input parameter 3:
     * Indicates the direction of the linear gradient. This parameter does not take effect after the angle is set. Value range: ("left","top","right","bottom","left-top","left-bottom","right-top",\n.
     * "right-bottom", "none", default "bottom"); \n
     * Input parameter 4: coloring for the gradient color. The default value is false.\n
     * For example, "#ffff0000,0.0,#ff0000ff,0.3,#ffffff00,0.5;;left;true".
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     *  * .string: character string combination parameter. Four input parameters are separated by semicolons (;). \n
     * Input parameter 1: indicates the gradient color at a certain percentage position. If an invalid color is set, the system skips the gradient color. Color and position (unit: vp) array type, separated by commas.\n
     * Input parameter 2: start angle of the linear gradient. Rotate clockwise in the 0 o'clock direction as the positive angle; \n
     * Input parameter 3: linear gradient direction. The setting of angle does not take effect.\n
     * Input parameter 4: repeat coloring for the gradient color.\n
     * @code {.cpp}
     * ARKUI_AttributeItem item = {.string = "#ffff0000,0.0,#ff0000ff,0.3,#ffffff00,0.5;;left;true"};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LINEAR_GRADIENT, &item);
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_LINEAR_GRADIENT);
     * auto nodeLinearGradient = item->string;
     * @endcode
     *
     */
    NODE_LINEAR_GRADIENT,
    /**
     * @brief Alignment mode of the component content in the element drawing area. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: alignment mode. The data type is {@link ArkUI_Alignment}. The default value is AKUI_ALIGNMENT_CENTER.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Alignment mode, data type {@link ArkUI_Alignment}.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_ALIGNMENT_CENTER}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_ALIGNMENT, "center");
     * auto item = basicNodeApi->getAttribute(nodeHandle, NODE_ALIGNMENT);
     * auto nodeAlign = item->value[0].i32;
     * @endcode
     *
     */
    NODE_ALIGNMENT,
    /**
     * @brief Transparency attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Transparency value, ranging from 0 to 1.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Transparency numeric value, ranging from 0 to 1.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {0.5};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_OPACITY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_OPACITY);
     * auto nodeOpacity = item->value[0].f32;
     * @endcode
     *
     */
    NODE_OPACITY,
    /**
     * @brief Border width attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * 1: .value[0].f32: Set the width of the four edges in a unified manner.\n
     * 2: .value[0].f32: Sets the border width of the top border.\n
     * .value[1].f32: Sets the border width of the right box.\n
     * .value[2].f32: Sets the border width of the lower border.\n
     * .value[3].f32: Sets the border width of the left border.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Sets the border width of the top border.\n
     * .value[1].f32: Sets the border width of the right box.\n
     * .value[2].f32: Sets the border width of the lower border.\n
     * .value[3].f32: Sets the border width of the left border.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {5};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_WIDTH, &item);
     * ArkUI_NumberValue value[] = {5, 5, 10, 10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_WIDTH, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BORDER_WIDTH);
     * auto nodeBorderWidth = item->value[0].f32;
     * @endcode
     *
     */
    NODE_BORDER_WIDTH,
    /**
     * @brief Border rounded corner attribute, which supports attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * 1: .value[0].f32: Set the border fillets of the four edges.\n
     * 2: .value[0].f32: Set the radius of the upper left corner fillet.\n
     * .value[1].f32: Sets the radius of the upper right corner fillet.\n
     * .value[2].f32: Sets the radius of the lower left corner fillet.\n
     * .value[3].f32: Sets the radius of the lower right corner fillet.\n
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: Sets the radius of the upper left corner fillet.\n
     * .value[1].f32: Sets the radius of the upper right corner fillet.\n
     * .value[2].f32: Sets the radius of the lower left corner fillet.\n
     * .value[3].f32: Sets the radius of the lower right corner fillet.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {5};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_RADIUS, &item);
     * ArkUI_NumberValue value[] = {5, 5, 10, 10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_RADIUS, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BORDER_RADIUS);
     * auto nodeBorderRadius = item->value[0].f32;
     * @endcode
     *
     */
    NODE_BORDER_RADIUS,
    /**
     * @brief Border color attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * 1: .value[0].u32: Set the border color of the four edges, represented by 0xargb, for example, 0xFFFF11FF.\n
     * 2: .value[0].u32: Set the color of the upper border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[1].u32: sets the color of the right border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[2].u32: sets the color of the lower border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[3].u32: Sets the color of the left border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: Sets the upper border color, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[1].u32: sets the color of the right border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[2].u32: sets the color of the lower border, represented by 0xargb, for example, 0xFFFF11FF.\n
     * .value[3].u32: Sets the color of the left border, represented by 0xargb, for example, 0xFFFF11FF.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.u32 = 0xFFFF11FF}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_COLOR, &item);
     * ArkUI_NumberValue value[] = {{.u32 = 0xFFFF11FF}, {.u32 = 0xFFFF11FF}, {.u32 = 0xFFFFFFFF}, {.u32 = 0x000000}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_COLOR, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BORDER_COLOR);
     * auto nodeBorderColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_BORDER_COLOR,
    /**
     * @brief Border line style attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * 1: .value[0].i32: Set the border line style of the four edges in a unified manner. Parameter type {@link ArkUI_BorderStyle}. The default value is AKUI_BORDER_:00-19:00_SOLID.\n
     * 2:.value[0].i32: Sets the upper border line style. Parameter type {@linkArkUI_BorderStyle}. The default value is AKUI_BORDER_:00-20:00_SOLID.\n
     * .value[1].i32: Sets the line style of the right border. Parameter type {@link ArkUI_BorderStyle}. The default value is ARKUI_BORDER_:00-19:00_SOLID.\n
     * .value[2].i32: Sets the lower border line style. Parameter type {@link ArkUI_BorderStyle}. The default value is AKUI_BORDER_:00-20:00_SOLID.\n
     * .value[3].i32: Sets the left border line style. Parameter type {@link ArkUI_BorderStyle}. The default value is ARKUI_BORDER_:00-19:00_SOLID.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Value corresponding to the top border line style.\n
     * .value[1].i32: Value corresponding to the right border line style.\n
     * .value[2].i32: Value corresponding to the lower border line style.\n
     * .value[3].i32: Value corresponding to the left border line style.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_BORDER_STYLE_DOTTED}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_STYLE, &item);
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_BORDER_STYLE_DOTTED}, {.i32 = ARKUI_BORDER_STYLE_SOLID},
     * {.i32 = ARKUI_BORDER_STYLE_SOLID}, {.i32 = ARKUI_BORDER_STYLE_DOTTED}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BORDER_STYLE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BORDER_STYLE);
     * auto nodeBorderStyle = item->value[0].i32;
     * @endcode
     *
     */
    NODE_BORDER_STYLE,
    /**
     * @brief Stack sequence attribute of the component. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Stacking order value.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Stacking order value.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {2};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_Z_INDEX, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_Z_INDEX);
     * auto nodeZIndex = item->value[0].f32;
     * @endcode
     *
     */
    NODE_Z_INDEX,
    /**
     * @brief Whether the component is visible. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: controls whether the current component is displayed or hidden. Parameter type {@link ArkUI_Visibility}. The default value is ARKUI_VISIBGY_VISIBLE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: controls whether the current component is displayed or hidden. Parameter type {@link ArkUI_Visibility}. The default value is ARKUI_VISIBGY_VISIBLE.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32=ARKUI_VISIBILITY_NONE}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_VISIBILITY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_VISIBILITY);
     * auto nodeVisibility = item->value[0].i32;
     * @endcode
     *
     */
    NODE_VISIBILITY,
    /**
     * @brief The API for tailoring and masking the attributes of the component. The APIs for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: specifies whether to crop the parent container according to the edge outline. The value 0 indicates no, and the value 1 indicates no.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: specifies whether to crop the parent container according to the edge outline. The value 0 indicates not to crop the parent container, and the value 1 indicates to crop the parent container.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 0}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_CLIP, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_CLIP);
     * auto nodeClip = item->value[0].i32;
     * @endcode
     *
     */
    NODE_CLIP,
    /**
     * @brief This API is used to tailor and mask the attributes of the component. The APIs for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: shape description, which is optional:\n
     * "rect(10, 10, 10, 10)" is width, height, radiusWidth, and radiusHeight respectively in parentheses.\n
     * "circle(10, 10)": width, height; \n
     * "ellipse(10, 10)" is width and height respectively; \n
     * "path(10, 10, M0 0 L600 0)" is width, height, commands; \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: Shape description:\n
     * "rect(10, 10, 10, 10)" is width, height, radiusWidth, and radiusHeight respectively in parentheses.\n
     * "circle(10, 10)": width, height; \n
     * "ellipse(10, 10)" is width and height respectively; \n
     * "path(10, 10, M0 0 L600 0)" is width, height, and commands respectively; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "rect(10, 10, 10, 10)"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_CLIP_SHAPE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_CLIP);
     * auto nodeClipShape = item->string;
     * @endcode
     *
     */
    NODE_CLIP_SHAPE,
    /**
     * @brief Matrix transformation function, which can translate, rotate, and zoom in or out a chart, and supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .data[0...15].f32:16 floating point numbers.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .data[0...15].f32:16 floating point numbers.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.f32 = 1}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0},
     * {.f32 = 0}, {.f32 = 0}, {.f32 = 1}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TRANSFORM, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TRANSFORM);
     * auto nodeTransform = item[0].value;
     * @endcode
     *
     */
    NODE_TRANSFORM,
    /**
     * @brief Touch test type. Interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: controls the touch test type of the current component. The parameter type is {@link ArkUI_HitTestMode}. The default value is ARKUI_HIT_TEST_MODE_DEFAULT. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: controls the touch test type of the current component. The parameter type is {@link ArkKUI_HitTestMode}. The default value is ARKUI_HIT_TEST_MODE_DEFAULT. \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32=ARKUI_HIT_TEST_MODE_BLOCK}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_HIT_TEST_BEHAVIOR, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_HIT_TEST_BEHAVIOR);
     * auto nodeHitTestBehavior = item->value[0].i32;
     * @endcode
     *
     */
    NODE_HIT_TEST_BEHAVIOR,
    /**
     * @brief Offset of the upper left corner of the element relative to the upper left corner of the parent container. Attribute setting, attribute reset, and attribute obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: x-axis coordinate.\n
     * .value[1].f32: y-axis coordinates.\n
     * \n
     * Format of the return value {@link ArkUI_AttributeItem} of the attribute obtaining method:\n
     * .value[0].f32: x-axis coordinate.\n
     * .value[1].f32: y-axis coordinates.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {50, 50};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_POSITION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_POSITION);
     * auto nodePositionX = item->value[0].f32;
     * auto nodePositionY = item->value[1].f32;
     * @endcode
     *
     */
    NODE_POSITION,
    /**
     * @brief Shadow effect attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Sets the shadow effect of the current component. Parameter type {@link ArkUI_ShadowStyle}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Sets the shadow effect of the current component. Parameter type {@link ArkUI_ShadowStyle}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_SHADOW_STYLE_OUTER_DEFAULT_XS}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SHADOW, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SHADOW);
     * auto nodePositionX = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SHADOW,
    /**
     * @brief Customize the shadow effect, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: character string combination parameter. Six input parameters are separated by semicolons (;). \n
     * Input parameter 1: shadow blur radius.\n
     * Input parameter 2: X-axis offset of the shadow.\n
     * Input parameter 3: Y-axis offset of the shadow.\n
     * Input parameter 4: shadow type.\n
     * Input parameter 5: shadow color.\n
     * Input parameter 6: indicates whether the shadow is filled in.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: character string combination parameter. Six input parameters are separated by semicolons (;). \n
     * Input parameter 1: shadow blur radius.\n
     * Input parameter 2: X-axis offset of the shadow.\n
     * Input parameter 3: Y-axis offset of the shadow.\n
     * Input parameter 4: shadow type.\n
     * Input parameter 5: shadow color.\n
     * Input parameter 6: indicates whether the shadow is filled in.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "5; 10; 10; COLOR; 0xACCCCC; true"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_CUSTOM_SHADOW, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_CUSTOM_SHADOW);
     * auto nodeCustomShadow = item->string;
     * @endcode
     *
     */
    NODE_CUSTOM_SHADOW,
    /**
     * @brief Width and height attributes of the background image. Interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the width of the image, in vp.\n
     * .value[1].f32 indicates the image height, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the width of the image, in vp.\n
     * .value[1].f32 indicates the image height, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue sizeArray[] = {20, 0}
     * ARKUI_AttributeItem item = {.value = sizeArray, .size = 2};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BACKGROUND_IMAGE_SIZE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BACKGROUND_IMAGE_SIZE);
     * auto width = item->value[0].f32;
     * @endcode
     *
     */
    NODE_BACKGROUND_IMAGE_SIZE,
    /**
     * @brief Attributes of the width and height of the background image. The attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the width and height of the background image. The enumerated values are {@link ArkUI_ImageSize}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates the width and height of the background image. The enumerated values are {@link ArkUI_ImageSize}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue imageSizeStyle[] = {{.i32 = static_cast<int32_t>(ArkUI_ImageSize.ARKUI_IMAGE_SIZE_COVER)}}
     * ARKUI_AttributeItem item = {.value = imageSizeStyle, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE);
     * auto blurStyle = item->value[0].i32
     * @endcode
     */
    NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE,
    /**
     * @brief Fuzzy attributes between the background and content. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the fuzzy type. The enumerated value is {@link ArkUI_BlurStyle}.\n
     * .value[1]?.i32 indicates the light/dark color mode, which uses the enumerated value {@link ArkUI_ColorMode}.\n
     * .value[2]?.i32 indicates the color obtaining mode. The enumerated value of {@link ArkUI_AdaptiveColor} is used.\n
     * .value[3]?.f32 indicates the blur effect degree, which is in the range [0.0, 1.0].\n
     * .value[4]?.f32 indicates the grayscale blur start boundary.\n
     * .value[5]?.f32 indicates the grayscale blur end boundary.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates the fuzzy type, which uses the enumerated value {@link ArkUI_BlurStyle}.\n
     * .value[1]?.i32 indicates the light/dark color mode, which uses the enumerated value {@link ArkUI_ColorMode}.\n
     * .value[2]?.i32 indicates the color obtaining mode. The enumerated value of {@link ArkUI_AdaptiveColor} is used.\n
     * .value[3]?.f32 indicates the blur effect degree, which is in the range [0.0, 1.0].\n
     * .value[4]?.f32 indicates the grayscale blur start boundary.\n
     * .value[5]?.f32 indicates the grayscale blur end boundary.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue blurStyle[] = {{.i32 = static_cast<int32_t>(ArkUI_BlurStyle.ARKUI_BLUR_STYLE_THICK)}}
     * ARKUI_AttributeItem item = {.value = blurStyle, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BACKGROUND_BLUR_STYLE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BACKGROUND_BLUR_STYLE);
     * auto blurStyle = item->value[0].i32
     * @endcode
     *
     */
    NODE_BACKGROUND_BLUR_STYLE,
    /**
     * @brief Center point attribute of the graph transformation. Interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0]?.f32 indicates the X-axis coordinate value of the center point, in vp.\n
     * .value[1]?.f32 indicates the Y-axis coordinate of the central point, in vp.\n
     * .value[2]?.f32 indicates the Z-axis coordinate of the center point, in vp \n
     * .value[3]?.f32 indicates the percentage position of the X-axis coordinate of the center point. For example, 0.2 indicates the position of 20%. This property overrides value[0].f32.\n
     * .value[4]?.f32 indicates the percentage position of the Y axis coordinate of the center point, for example, 0.2 indicates the position of 20 percent. This property overrides value[1].f32.\n
     * .value[5]?.f32 indicates the percentage position of the Z coordinate of the center point, for example, 0.2 indicates the position of 20 percent. This property overrides value[2].f32.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0]?.f32 indicates the X-axis coordinate of the center point, in vp.\n
     * .value[1]?.f32 indicates the Y-axis coordinate of the central point, in vp.\n
     * .value[2]?.f32 indicates the Z-axis coordinate of the central point, in vp \n
     * Note: If the coordinate percentage position is set, the attribute obtaining method returns the calculated vp value.
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue centerPointArray[] = {20}
     * ARKUI_AttributeItem item = {.value = centerPointArray, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TRANSITION_CENTER, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TRANSITION_CENTER);
     * auto centerX = item->value[0].f32
     * @endcode
     */
    NODE_TRANSFORM_CENTER,
    /**
     * @brief Transparency effect attribute during transition. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the transparency value of the start and end points.\n
     * .value[1].i32 indicates the animation duration, in ms.\n
     * .value[2].i32 indicates the type of the animation curve. The enumerated value of {@link ArkUI_AnimationCurve} is used.\n
     * .value[3]?.i32 indicates the animation delay, in ms.\n
     * .value[4]?.i32 indicates the number of times the animation is played.\n
     * .value[5]?.i32 indicates the animation playing mode. The enumerated value of {@link ArkUI_AnimationPlayMode} is used.\n
     * .value[6]?.f32 indicates the animation playback speed.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the transparency value of the start and end points.\n
     * .value[1].i32 indicates the animation duration, in ms.\n
     * .value[2].i32 indicates the type of the animation curve. The enumerated value of {@link ArkUI_AnimationCurve} is used.\n
     * .value[3]?.i32 indicates the animation delay, in ms.\n
     * .value[4]?.i32 indicates the number of times the animation is played.\n
     * .value[5]?.i32 indicates the animation playing mode. The enumerated value of {@link ArkUI_AnimationPlayMode} is used.\n
     * .value[6]?.f32 indicates the animation playback speed.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue opacityTransition[] = {20, {.i32 = 3000},
     * {.i32 = static_cast<int32_t>(ArkUI_AnimationCurve.ARKUI_CURVE_EASE_IN_OUT)}}
     * ARKUI_AttributeItem item = {.value = opacityTransition, .size = 3};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_OPACITY_TRANSITION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_OPACITY_TRANSITION);
     * auto opacity = item->value[0].f32
     * @endcode
     */
    NODE_OPACITY_TRANSITION,
    /**
     * @brief Rotation effect attributes during transition. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 represents the lateral rotation component.\n
     * .value[1].f32 represents the vertical rotation component.\n
     * .value[2].f32 represents a vertical rotation component.\n
     * .value[3].f32 indicates the angle.\n
     * .value[4].f32 for line of sight.\n
     * .value[5].i32 indicates the animation duration, in ms.\n
     * .value[6].i32 indicates the type of the animation curve. The enumerated value {@link ArkUI_AnimationCurve} is used.\n
     * .value[7]?.i32 indicates the animation delay, in milliseconds.\n
     * .value[8]?.i32 indicates the number of times the animation is played.\n
     * .value[9]? .i32 indicates the animation playback mode, and the enumerated value {@link ArkUI_AnimationPlayMode} is used.\n
     * .value[10]?.f32 indicates the animation playback speed.\n
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32 represents the lateral rotation component.\n
     * .value[1].f32 represents the vertical rotation component.\n
     * .value[2].f32 represents a vertical rotation component.\n
     * .value[3].f32 indicates the angle.\n
     * .value[4].f32 for line of sight.\n
     * .value[5].i32 indicates the animation duration, in ms.\n
     * .value[6].i32 indicates the type of the animation curve, which is the enumerated value of {@link ArkUI_AnimationCurve}.\n
     * .value[7]?.i32 indicates the animation delay, in ms.\n
     * .value[8]?.i32 indicates the number of times the animation is played.\n
     * .value[9]? .i32 indicates the animation mode, and the enumerated value {@link ArkUI_AnimationPlayMode} is used.\n
     * .value[10]?.f32 indicates the animation playback speed.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue rotateTransition[] = {0.0f, 0.0f, 1.0f, 180, 0, {.i32 = 3000},
     * {.i32 = static_cast<int32>(ArkUI_AnimationCurve.ARKUI_CURVE_SHARP)}}
     * ARKUI_AttributeItem item = {.value = rotateTransition, .size = 7};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_ROTATE_TRANSITION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_ROTATE_TRANSITION);
     * auto rotateX = item->value[0].f32
     * @endcode
     */
    NODE_ROTATE_TRANSITION,
    /**
     * @brief Zoom effect attributes during transition. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 Landscape magnification.\n
     * .value[1].f32 Vertical magnification.\n
     * .value[2].f32 Vertical magnification.\n
     * .value[3].i32 indicates the animation duration, in ms.\n
     * .value[4].i32 indicates the type of the animation curve, which is the enumerated value of {@link ArkUI_AnimationCurve}.\n
     * .value[5]?.i32 indicates the animation delay, in ms.\n
     * .value[6]?.i32 indicates the number of times the animation is played.\n
     * .value[7]? .i32 indicates the animation playback mode, and the enumerated value {@link ArkUI_AnimationPlayMode} is used.\n
     * .value[8]?.f32 indicates the animation playback speed.\n
     * \n
     * Return value of the attribute obtaining method {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 Landscape magnification.\n
     * .value[1].f32 Vertical magnification.\n
     * .value[2].f32 Vertical magnification.\n
     * .value[3].i32 indicates the animation duration, in ms.\n
     * .value[4].i32 indicates the type of the animation curve, which is the enumerated value of {@link ArkUI_AnimationCurve}.\n
     * .value[5]?.i32 indicates the animation delay, in ms.\n
     * .value[6]?.i32 indicates the number of times the animation is played.\n
     * .value[7]? .i32 indicates the animation playback mode, which is the enumerated value of {@link ArkUI_AnimationPlayMode}.\n
     * .value[8]?.f32 indicates the animation playback speed.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue scaleTransition[] = {0.0f, 0.0f, 0.0f, {.i32 = 3000},
     * {.i32 = static_cast<int32>(ArkUI_AnimationCurve.ARKUI_CURVE_SHARP)}}
     * ARKUI_AttributeItem item = {.value = scaleTransition, .size = 5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SCALE_TRANSITION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SCALE_TRANSITION);
     * auto scaleX = item->value[0].f32
     * @endcode
     */
    NODE_SCALE_TRANSITION,
    /**
     * @brief Panning effect attribute during transition. The interface supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * value[0].f32 indicates the horizontal translation distance, in vp.\n
     * value[1].f32 indicates the vertical translation distance, in vp \n
     * value[2].f32 indicates the vertical translation distance, in vp \n
     * value[3].i32 indicates the animation duration, in milliseconds.\n
     * value[4].i32 indicates the type of the animation curve. The enumerated value {@link ArkUI_AnimationCurve} is used.\n
     * value[5]?.i32 indicates the animation delay, in ms.\n
     * value[6]?.i32 indicates the number of times the animation is played.\n
     * value[7]?.i32 indicates the animation playback mode, which is the enumerated value of {@link ArkUI_AnimationPlayMode}.\n
     * value[8]?.f32 indicates the animation playback speed.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * value[0].f32 indicates the horizontal translation distance, in vp.\n
     * value[1].f32 indicates the vertical translation distance, in vp.\n
     * value[2].f32 indicates the vertical translation distance, in vp.\n
     * value[3].i32 indicates the animation duration, in milliseconds.\n
     * value[4].i32 indicates the type of the animation curve, and the enumerated value {@link ArkUI_AnimationCurve} is used.\n
     * value[5]?.i32 indicates the animation delay duration, in ms.\n
     * value[6]?.i32 indicates the number of times the animation is played.\n
     * value[7]?.i32 indicates the animation playback mode, and the enumerated value {@link ArkUI_AnimationPlayMode} is used.\n
     * value[8]?.f32 indicates the animation playback speed.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue translateTransition[] = {0.0f, 0.0f, 0.0f,
     * {.i32 = 3000}, {.i32 = static_cast<int32>(ArkUI_AnimationCurve.ARKUI_CURVE_SHARP)}}
     * ARKUI_AttributeItem item = {.value = translateTransition, .size = 5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TRANSLATE_TRANSITION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TRANSLATE_TRANSITION);
     * auto translateX = item->value[0].f32
     * @endcode
     */
    NODE_TRANSLATE_TRANSITION,

    /**
     * @brief Focus attribute, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].i32: The parameter type is 1 or 0.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: The parameter type is 1 or 0.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FOCUSABLE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_FOCUSABLE);
     * auto value = item->value[0].i32;
     * @endcode
     *
     */
    NODE_FOCUSABLE,

    /**
     * @brief Default focus attribute, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * value[0].i32: The parameter type is 1 or 0.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * value[0].i32: The parameter type is 1 or 0.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_DEFAULT_FOCUS, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_DEFAULT_FOCUS);
     * auto value = item->value[0].i32;
     * @endcode
     *
     */
    NODE_DEFAULT_FOCUS,

    /**
     * @brief Touch hot zone attributes, which support attribute setting, attribute reset, and attribute obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .data[0].f32: x-axis coordinate of the touch point relative to the upper left corner of the component, in vp.\n
     * .data[1].f32: y-axis coordinate of the touch point relative to the upper left corner of the component, in vp.\n
     * .data[2].f32: Width of the touch hot zone, in%.\n
     * .data[3].f32: Height of the touch hot zone, in%.\n
     * .data[4...].f32: Multiple gesture response areas can be set, and the sequence is the same as that described above.
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .data[0].f32: x-axis coordinate of the touch point relative to the upper left corner of the component, in vp.\n
     * .data[1].f32: y-axis coordinate of the touch point relative to the upper left corner of the component, in vp.\n
     * .data[2].f32: Width of the touch hot zone, in%.\n
     * .data[3].f32: Height of the touch hot zone, in%.\n
     * .data[4...].f32: Multiple gesture response areas can be set, and the sequence is the same as the preceding sequence.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {0, 0, 100, 100};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_RESPONSE_REGION, &item);
     *
     * ArkUI_NumberValue value[] = {0, 0, 100, 100, 0, 0, 100, 100};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_RESPONSE_REGION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_RESPONSE_REGION);
     * auto x = item->value[0].f32;
     * auto y = item->value[1].f32;
     * auto width = item->value[2].f32;
     * auto height = item->value[3].f32;
     * @endcode
     *
     */
    NODE_RESPONSE_REGION,
    /**
     * @brief Mask text attribute, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .string mask text; \n
     * .value[0]?.i32: indicates the position of the floating layer relative to the component. The parameter type is {@link ArkUI_Alignment}. This parameter is optional,
     * Default value is AKUI_ALIGNMENT_TOP_START.\n
     * .value[1]?.i32: (Optional) Indicates the offset X of the floating layer based on the upper left corner of the floating layer. The unit is vp.\n
     * .value[2]?.i32: (Optional) Indicates the offset Y of the floating layer based on the upper left corner of the floating layer. The unit is vp.
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .string mask text; \n
     * .value[0]?.i32: indicates the position of the floating layer relative to the component. The parameter type is {@link ArkUI_Alignment}. This parameter is optional,
     * Default value is AKUI_ALIGNMENT_TOP_START.\n
     * .value[1]?.i32: indicates the offset X of the floating layer based on the upper left corner of the floating layer. The unit is vp.\n
     * .value[2]?.i32: indicates the offset Y of the floating layer based on the upper left corner of the floating layer. The unit is vp.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_ALIGNMENT_TOP_START}, 1.2, 0.3};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue), "test"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_OVERLAY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_OVERLAY);
     * auto text = item->string;
     * @endcode
     *
     */
    NODE_OVERLAY,

    /**
     * @brief Angle gradient effect, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: character string combination parameter. Six input parameters are separated by semicolons:\n
     * Input parameter 1: indicates the center point of the angle gradient, that is, the coordinates relative to the upper left corner of the current component, separated by commas.\n
     * Input parameter 2: start point of the angle gradient. The default value is 0. \n
     * Input parameter 3: end point of the angle gradient. The default value is 0. \n
     * Input parameter 4: rotation angle of the angle gradient. The default value is 0. \n
     * Input parameter 5: specifies the gradient color at a certain percentage position. If an invalid color is set, the gradient color is skipped.\n
     * Input parameter 6: coloring for the gradient color. The default value is false.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: character string combination parameter. Six input parameters are separated by semicolons:\n
     * Input parameter 1: indicates the center point of the angle gradient, that is, the coordinates relative to the upper left corner of the current component. Separate the coordinates with commas (,). \n
     * Input parameter 2: start point of the angle gradient. The default value is 0. \n
     * Input parameter 3: end point of the angle gradient. The default value is 0. \n
     * Input parameter 4: rotation angle of the angle gradient. The default value is 0. \n
     * Input parameter 5: specifies the gradient color at a certain percentage position. If an invalid color is set, the gradient color is skipped.\n
     * Input parameter 6: coloring for the gradient color. The default value is false.\n
     *
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "5,10;60;180;60;#ffff0000,0.0,#ff0000ff,0.3,#ffffff00,0.5;true"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWEEP_GRADIENT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWEEP_GRADIENT);
     * auto nodeCustomShadow = item->string;
     * @endcode
     *
     */
    NODE_SWEEP_GRADIENT,
    /**
     * @brief Angle gradient effect, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string: character string combination parameter. Four input parameters are separated by semicolons (;). \n
     * Input parameter 1: indicates the center point of the radial gradient, that is, the coordinates relative to the upper left corner of the current component, separated by commas.\n
     * Input parameter 2: radius of the radial gradient. The default value is 0. \n
     * Input parameter 3: Specify the gradient color at a certain percentage position. If an invalid color is set, skip this step.\n
     * Input parameter 4: repeat coloring for the gradient color.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: character string combination parameter. Four input parameters are separated by semicolons (;). \n
     * Input parameter 1: indicates the center point of the radial gradient, that is, the coordinates relative to the upper left corner of the current component, separated by commas.\n
     * Input parameter 2: radius of the radial gradient. The default value is 0. \n
     * Input parameter 3: Specifies the gradient color at a certain percentage position. If an invalid color is set, skip the gradient color.\n
     * Input parameter 4: repeat coloring for the gradient color.\n
     *
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "5,10;50;#ffff0000,0.0,#ff0000ff,0.3,#ffffff00,0.5;true"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_RADIAL_GRADIENT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_RADIAL_GRADIENT);
     * auto nodeCustomShadow = item->string;
     * @endcode
     *
     */
    NODE_RADIAL_GRADIENT,
    /**
     * @brief The mask of a specified shape is added to the component. The interface for setting and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .data[0]?.u32: shape filling color.\n
     * .data[1]?.u32: stroke color of a shape.\n
     * .data[2]?.f32: stroke width of a shape.\n
     * .string: shape description, which is optional:\n
     * "progressMask(10, 10, #ff0000ff)" is the current value of the progress mask, the maximum value of the progress mask, and the color of the progress mask.\n
     * "rect(10, 10, 10, 10)" is width, height, radiusWidth, and radiusHeight respectively in parentheses.\n
     * "circle(10, 10)": width, height; \n
     * "ellipse(10, 10)" is width and height respectively; \n
     * "path(10, 10, M0 0 L600 0)" is width, height, commands; \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .data[0].u32: shape filling color.\n
     * .data[1].u32: stroke color of a shape.\n
     * .data[2].f32: stroke width of a shape, in vp.\n
     * .string: Shape description:\n
     * "progressMask(10, 10, #ff0000ff)" is the current value of the progress mask, the maximum value of the progress mask, and the color of the progress mask.\n
     * "rect(10, 10, 10, 10)" is width, height, radiusWidth, and radiusHeight respectively in parentheses.\n
     * "circle(10, 10)": width, height; \n
     * "ellipse(10, 10)" is width and height respectively; \n
     * "path(10, 10, M0 0 L600 0)" is width, height, and commands respectively; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "rect(10, 10, 10, 10)"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_MASK, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_MASK);
     * auto nodeClipShape = item->string;
     * @endcode
     *
     */
    NODE_MASK,
    /**
     * @brief Mix the current control background and subnode content. Interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: specifies the hybrid mode type of the current component. The parameter type is {@link ArkUI_BlendMode}. The default value is AKUI_BLEND_MODE_NONE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: specifies the hybrid mode type of the current component. The parameter type is {@link ArkUI_BlendMode}. The default value is AKUI_BLEND_MODE_NONE.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32=ARKUI_BLEND_MODE_NONE}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_BLEND_MODE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_BLEND_MODE);
     * auto nodeHitTestBehavior = item->value[0].i32;
     * @endcode
     *
     */
    NODE_BLEND_MODE,
    /**
     * @brief Sets the layout in the axis direction of a container element. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Set the layout type in the axis direction of the container element.\n
     * Parameter type {@link ArkUI_Direction}. The default value is ARKUI_DIRECTION_AUTO.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Set the layout type in the axis direction of the container element.\n
     * Parameter type {@link ArkUI_Direction}. The default value is ARKUI_DIRECTION_AUTO.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32=ARKUI_DIRECTION_RTL}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_DIRECTION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_DIRECTION);
     * auto nodeHitTestBehavior = item->value[0].i32;
     * @endcode
     *
     */
    NODE_DIRECTION,
    /**
     * @brief Constraint size attributes. During component layout, the size range is restricted. Attribute setting, attribute reset, and attribute obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: minimum width, in vp.\n
     * .value[1].f32: maximum width, in vp.\n
     * .value[2].f32: Minimum height, in vp.\n
     * .value[3].f32: maximum height, in vp.\n
     * \n
     * Return value of the method for obtaining attributes {@link ArkUI_AttributeItem} Format:\n
     * .value[0].f32: minimum width, in vp; \n
     * .value[1].f32: maximum width, in vp.\n
     * .value[2].f32: Minimum height, in vp.\n
     * .value[3].f32: maximum height, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {0, 5, 0, 5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_CONSTRAINT_SIZE, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_CONSTRAINT_SIZE);
     * auto nodeMinWidth = item->value[0].f32;
     * auto nodeMaxWidth = item->value[1].f32;
     * auto nodeMinHeight = item->value[2].f32;
     * auto nodeMaxHeight = item->value[3].f32;
     * @endcode
     *
     */
    NODE_CONSTRAINT_SIZE,
    /**
     * @brief Grayscale effect attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: grayscale conversion ratio, ranging from 0 to 1. For example, 0.5 indicates that the grayscale processing is 50%.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: grayscale conversion ratio, ranging from 0 to 1.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {0.5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_GRAY_SCALE, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_GRAY_SCALE);
     * auto nodeGrayScale = item->value[0].f32;
     * @endcode
     */
    NODE_GRAY_SCALE,
    /**
     * @brief Inverts the image scale attributes of the input image. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: image inversion ratio. The value ranges from 0 to 1. For example, 0.5 indicates that the image inversion ratio is 50%.\n
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: image inversion ratio, ranging from 0 to 1.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {0.5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_INVERT, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_INVERT);
     * auto nodeInvert = item->value[0].f32;
     * @endcode
     */
    NODE_INVERT,
    /**
     * @brief Converts an image to a dark brown proportion. The APIs for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Convert the image to the dark brown ratio. The ratio ranges from 0 to 1. For example, 0.5 indicates that the dark brown ratio is 50%.\n
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: Convert the image to the sepia ratio, ranging from 0 to 1.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {0.5};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SEPIA, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_SEPIA);
     * auto nodeSepia = item->value[0].f32;
     * @endcode
     */
    NODE_SEPIA,
    /**
     * @brief Contrast attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: contrast. If the value is 1, the original image is used. A larger value indicates a higher contrast. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Contrast; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {10};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_CONTRAST, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_CONTRAST);
     * auto nodeContrast = item->value[0].f32;
     * @endcode
     */
    NODE_CONTRAST,
    /**
     * @brief Foreground color attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format. Two input parameter formats are supported: \n
     * 1: .value[0].u32: color value, 0xargb type, for example, 0xFFFF0000 indicates red.\n
     * 2: .value[0].i32: color value enumeration {@link ArkUI_ColoringStrategy}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: color value, 0xargb type; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {{.u32=0xFFFF0000}};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FOREGROUND_COLOR, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_FOREGROUND_COLOR);
     * auto nodeForegroundColor = item->value[0].u32;
     * @endcode
     */
    NODE_FOREGROUND_COLOR,
    /**
     * @brief Additional offset attributes of a component's sub-element relative to the component itself. Attribute setting, resetting, and obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the offset in the x-axis direction, in vp.\n
     * .value[1].f32 indicates the offset in the y-axis direction, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the offset in the x-axis direction, in vp.\n
     * .value[1].f32 indicates the offset in the y-axis direction, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue offsetArray[] = {20, 0}
     * ARKUI_AttributeItem item = {.value = offsetArray, .size = 2};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_OFFSET, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_OFFSET);
     * auto offsetX = item->value[0].f32
     * @endcode
     *
     */
    NODE_OFFSET,
    /**
     * @brief Anchor attribute of a subelement of a component during location positioning. Attribute setting, attribute reset, and attribute obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the x-coordinate value of the anchor point, in vp.\n
     * .value[1].f32 indicates the y-coordinate value of the anchor point, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the x-coordinate value of the anchor point, in vp.\n
     * .value[1].f32 indicates the y coordinate value of the anchor point, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue pointArray[] = {20, 0}
     * ARKUI_AttributeItem item = {.value = pointArray, .size = 2};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_MARK_ANCHOR, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_MARK_ANCHOR);
     * auto pointX = item->value[0].f32
     * @endcode
     *
     */
    NODE_MARK_ANCHOR,

    /**
     * @brief The text component sets the text content attributes. The interface supports the attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string indicates the text content.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string indicates the text content.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_AttributeItem item = {.string = "https://www.example.com/xxx.png"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_CONTENT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_CONTENT);
     * auto content = item->string
     * @endcode
     */
    NODE_TEXT_CONTENT = MAX_NODE_SCOPE_NUM * ARKUI_NODE_TEXT,
    /**
     * @brief Component font color attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].u32: indicates the font color. The value is in the 0xargb format. For example, 0xFFFF0000 indicates red.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: font color value, in 0xargb format.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.u32=0xFFFF0000}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FONT_COLOR, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_FONT_COLOR);
     * auto nodeFontColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_FONT_COLOR,
    /**
     * @brief Component font size attribute, which supports attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: indicates the font size, in fp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: indicates the font size, in fp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FONT_SIZE, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_FONT_SIZE);
     * auto nodeFontSize = item->value[0].f32;
     * @endcode
     *
     */
    NODE_FONT_SIZE,
    /**
     * @brief Component font style attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: font style {@link ArkUI_FontStyle}. The default value is AKUI_FONT_terrace_NORMAL.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: font style {@link ArkUI_FontStyle}; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_FONT_STYLE_NORMAL}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FONT_STYLE, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_FONT_STYLE);
     * auto nodeFontStyle = item->value[0].i32;
     * @endcode
     *
     */
    NODE_FONT_STYLE,
    /**
     * @brief Component font thickness attribute, which supports attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: font thickness style {@link ArkUI_FontWeight}. The default value is RKUI_FONT_WEIGHT_NORMAL.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: font weight {@link ArkUI_FontWeight}; \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_FONT_WEIGHT_NORMAL}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FONT_WEIGHT, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_FONT_WEIGHT);
     * auto nodeFontWeight = item->value[0].i32;
     * @endcode
     *
     */
    NODE_FONT_WEIGHT,
    /**
     * @brief Text line height attribute, which supports attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the value of lineHeight, in fp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the value of lineHeight, in fp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue lineHeight[] = {20}
     * ARKUI_AttributeItem item = {.value = lineHeight, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_LINE_HEIGHT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_LINE_HEIGHT);
     * auto pointX = item->value[0].f32
     * @endcode
     */
    NODE_TEXT_LINE_HEIGHT,
    /**
     * @brief Sets the style and color attributes of the text decoration line. The interface supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Text decoration line style {@link ArkUI_TextDecorationType}. The default value is ARKUI_TEXT_DECORATION_TYPE_NONE. \n
     * .value[1]?.u32: color of the decoration line. The value is in the format of 0xargb. For example, 0xFFFF0000 indicates red.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Text decoration line style {@link ArkUI_TextDecorationType}; \n
     * .value[1].u32: color of the decoration line, in the 0xargb format.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_TEXT_DECORATION_TYPE_NONE}, {.u32=0xFFFF0000}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_DECORATION, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TEXT_DECORATION);
     * auto nodeDecorationStyle = item->value[0].i32;
     * auto nodeDecorationColor = item->value[1].u32;
     * @endcode
     *
     */
    NODE_TEXT_DECORATION,
    /**
     * @brief The attributes of text are uppercase and lowercase. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates that the text is case-sensitive.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates that the text is case-sensitive.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue textCase[] = {{.i32 = static_cast<int32>(ArkUI_TextCase.ARKUI_TEXT_CASE_LOWER)}}
     * ARKUI_AttributeItem item = {.value = textCase, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_CASE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_CASE);
     * auto textCase = item->value[0].i32
     * @endcode
     *
     */
    NODE_TEXT_CASE,
    /**
     * @brief Character spacing attribute of the text. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32 indicates the character spacing, in fp \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32 indicates the character spacing, in fp \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue letterSpacing[] = {20}
     * ARKUI_AttributeItem item = {.value = letterSpacing, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_LETTER_SPACING, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_LETTER_SPACING);
     * auto letterSpacing = item->value[0].f32
     * @endcode
     *
     */
    NODE_TEXT_LETTER_SPACING,
    /**
     * @brief Attribute of the maximum number of text lines. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the maximum number of lines.\n
     * \n
     * Format of the return value {@link ArkUI_AttributeItem} of the attribute obtaining method:\n
     * .value[0].i32 indicates the maximum number of lines.\n
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue maxLine[] = {{.i32 = 2}};
     * ARKUI_AttributeItem item = {.value = maxLine, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_MAX_LINES, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_MAX_LINES);
     * auto maxLines = item->value[0].i32
     * @endcode
     */
    NODE_TEXT_MAX_LINES,
    /**
     * @brief Text horizontal alignment mode, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: indicates the horizontal alignment mode of the text. The value is the enumerated value of {@link ArkUI_TextAlign}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: indicates the horizontal alignment mode of the text. The value is the enumerated value of {@link ArkUI_TextAlign}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue alignMent[] = {{.i32 = static_cast<int_32>(ArkUI_TextAlign.ARKUI_TEXT_ALIGN_CENTER)}};
     * ARKUI_AttributeItem item = {.value = alignMent, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_ALIGN, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_ALIGN);
     * auto alignMent = item->value[0].i32
     * @endcode
     */
    NODE_TEXT_ALIGN,
    /**
     * @brief Display mode attribute when the text length is too long. The attribute setting, resetting, and obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Indicates the display mode when the text is too long.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Indicates the display mode when the text is too long.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue textOverFlow[] = {{.i32 = static_cast<int_32>(ArkUI_TextOverFlow.ARKUI_TEXT_OVERFLOW_CLIP)}
     *};
    * ARKUI_AttributeItem item = {.value = textOverFlow, .size = 1};
    * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_OVERFLOW, &item);
    * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_OVERFLOW);
    * auto textOverFlow = item->value[0].i32
    * @endcode
    */
    NODE_TEXT_OVERFLOW,
    /**
     * @brief Text Font list attribute, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .string: character string. Separate multiple characters with commas (,).
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string: character string. Separate multiple characters with commas (,).
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_AttributeItem item = {.string = "HarmonyOS Sans"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_FONT_FAMILY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_FONT_FAMILY);
     * auto font = item->string;
     * @endcode
     *
     */
    NODE_FONT_FAMILY,
    /**
     * @brief Text copy and paste attributes, which support the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: copy and paste mode {@link ArkUI_CopyOptions}. The default value is AKUI_COPY_OPTIONS_NONE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: copy and paste mode {@link ArkUI_CopyOptions}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_COPY_OPTIONS_NONE}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_COPY_OPTION, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TEXT_COPY_OPTION);
     * auto nodeTextCopyOption = item->value[0].i32;
     * @endcode
     *
     */
    NODE_TEXT_COPY_OPTION,
    /**
     * @brief Offset attribute of the text baseline, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: offset value, in fp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: offset value, in fp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_BASELINE_OFFSET, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TEXT_BASELINE_OFFSET);
     * auto nodeTextBaselineOffset = item->value[0].f32;
     * @endcode
     *
     */
    NODE_TEXT_BASELINE_OFFSET,
    /**
     * @brief Text shadow effect attribute. The attribute setting, resetting, and obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: shadow blur radius, in vp.\n
     * .value[1].i32: shadow type {@link ArkUI_ShadowType}. The default value is AKUI_SHADOW_TYPE_COLOR.\n
     * .value[2].u32: shadow color, in the format of 0xargb. For example, 0xFFFF0000 indicates red.\n
     * .value[3].f32: X-axis offset of the shadow, in vp.\n
     * .value[4].f32: offset of the Y axis of the shadow, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: shadow blur radius, in vp.\n
     * .value[1].i32: shadow type {@link ArkUI_ShadowType}; \n
     * .value[2].u32: shadow color, in 0xargb format.\n
     * .value[3].f32: X-axis offset of the shadow, in vp.\n
     * .value[4].f32: offset on the Y axis of the shadow, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {10, {.i32=ARKUI_SHADOW_TYPE_COLOR}, {.u32=0xFFFF0000}, 10, 10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_TEXT_SHADOW, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TEXT_TEXT_SHADOW);
     * auto nodeTextShadowRadius = item->value[0].f32;
     * auto nodeTextShadowType = item->value[1].i32;
     * auto nodeTextShadowColor = item->value[2].u32;
     * auto nodeTextShadowOffsetX = item->value[3].f32;
     * auto nodeTextShadowOffsetY = item->value[4].f32;
     * @endcode
     *
     */
    NODE_TEXT_TEXT_SHADOW,

    /**
     * @brief Text Minimum display font size, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: minimum font size, in FP.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: minimum font size, in FP.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {20};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_MIN_FONT_SIZE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_MIN_FONT_SIZE);
     * auto value = item->value[0].f32;
     * @endcode
     *
     */
    NODE_TEXT_MIN_FONT_SIZE,

    /**
     * @brief Text Maximum font size to be displayed. Attributes can be set, reset, and obtained.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: maximum font size of the text, in FP.
     * \n
     * Return value of the method for obtaining attributes {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: maximum font size of the text, in FP.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {20};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_MAX_FONT_SIZE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_MAX_FONT_SIZE);
     * auto value = item->value[0].f32;
     * @endcode
     *
     */
    NODE_TEXT_MAX_FONT_SIZE,

    /**
     * @brief Text, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .string?: list of optional value fonts. Use multiple fonts separated by commas.\n
     * .value[0].f32: Text size unit FP.\n
     * .value[1]?.i32: specifies the font thickness of the text. The parameter type is {@link ArkUI_FontWeight}. This parameter is optional.
     * Default value is AKUI_FONT_WEIGHT_NORMAL.\n
     * .value[2]?.i32: Optional. Font style. Parameter type {@link ArkUI_FontStyle}.
     * The default value is ARKUI_TEXT_FONT_STYLE_NORMAL.
     * \n
     * Format of the return value of the attribute obtaining method {@link ArkUI_AttributeItem}:\n
     * .string?: list of optional value fonts. Use multiple fonts separated by commas.\n
     * .value[0].f32: text size unit FP.\n
     * .value[1]?.i32: specifies the font thickness of the text. The parameter type is {@link ArkUI_FontWeight}. This parameter is optional.
     * Default value is RKUI_FONT_WEIGHT_NORMAL.\n
     * .value[2]?.i32: Optional. Font style. Parameter type {@link ArkUI_FontStyle}.
     * The default value is ARKUI_TEXT_FONT_STYLE_NORMAL.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {16, {.i32 = ARKUI_FONT_STYLE_NORMAL},
     * {.i32 = ARKUI_TEXT_FONT_STYLE_NORMAL}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_FONT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_FONT);
     * auto size = item->value[0].f32;
     * @endcode
     *
     */
    NODE_TEXT_FONT,

    /**
     * @brief Text Height adaptation mode, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].i32: parameter type {@link ArkUI_TextHeightAdaptivePolicy}.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: parameter type {@link ArkUI_TextHeightAdaptivePolicy}.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_TEXT_HEIGHT_ADAPTIVE_POLICY_MAX_LINES_FIRST}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY);
     * auto size = item->value[0].i32;
     * @endcode
     *
     */
    NODE_TEXT_HEIGHT_ADAPTIVE_POLICY,

    /**
     * @brief Indent attribute of the first line of the text. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Indicates the first line indented value.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Indicates the first line indented value.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue textIndent[] = {20}
     * ARKUI_AttributeItem item = {.value = textIndent, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TEXT_INDENT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_TEXT_INDENT);
     * auto indentValue = item->value[0].f32
     * @endcode
     */
    NODE_TEXT_INDENT,

    /**
     * @brief This interface is used to set image address attributes for the image component. The interface supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string indicates the address of the image component.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string indicates the address of the image component.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_AttributeItem item = {.string = "https://www.example.com/xxx.png"};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_SRC, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_SRC);
     * auto imageSrc = item->string
     * @endcode
     */
    NODE_IMAGE_SRC = MAX_NODE_SCOPE_NUM * ARKUI_NODE_IMAGE,
    /**
     * @brief Image filling effect attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the image filling effect, which is the enumerated value {@link ArkUI_ObjectFit}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates the image filling effect, which is the enumerated value {@link ArkUI_ObjectFit}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_NumberValue objectFitValue[] = {.i32 = {static_cast<int>(ArkUI_ObjectFit.ARKUI_OBJECT_FIT_FILL)}};
     * ARKUI_AttributeItem item = {.value = objectFitValue, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_OBJECT_FIT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_OBJECT_FIT);
     * auto objectFit = item->value[0].i32
     * @endcode
     */
    NODE_IMAGE_OBJECT_FIT,
    /**
     * @brief Image interpolation effect attributes. The interfaces for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the interpolation effect. The enumerated value is {@link ArkUI_ImageInterpolation}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates the interpolation effect. The enumerated value is {@link ArkUI_ImageInterpolation}.\n
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_NumberValue interpolationValue[] = {.i32 = {
     * static_cast<int>(ArkUI_ImageInterpolation.ARKUI_INTERPOLATION_LOW)}};
     * ARKUI_AttributeItem item = {.value = interpolationValue, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_INTERPOLATION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_INTERPOLATION);
     * auto interpolation = item->value[0].i32
     * @endcode
     */
    NODE_IMAGE_INTERPOLATION,
    /**
     * @brief Image repetition style attribute, which supports attribute setting, attribute reset, and attribute obtaining interfaces.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 indicates the image repetition style. The enumerated value is {@link ArkUI_ImageRepeat}.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 indicates the image repetition style. The enumerated value is {@link ArkUI_ImageRepeat}.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_NumberValue repeatValue[] = {.i32 = {static_cast<int>(ArkUI_ImageRepeat.ARKUI_IMAGE_REPEAT_X)}};
     * ARKUI_AttributeItem item = {.value = repeatValue, .size = 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_OBJECT_REPEAT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_OBJECT_REPEAT);
     * auto repeat = item->value[0].i32
     * @endcode
     */
    NODE_IMAGE_OBJECT_REPEAT,
    /**
     * @brief Image filter effect attribute. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 ~ .value[19].i32 represents the filter matrix array\n
     * .size indicates the size of the filter array 5*4 \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 ~ .value[19].i32 represents the filter matrix array\n
     * .size indicates the size of the filter array 5*4 \n
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_NumberValue filterValue[] = {.i32 = {1}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32
     * = {1}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32 = {0}, .i32 = {1}, .i32 = {0}, .i32 = {0}, .i32 =
     * {0}, .i32 = {0}, .i32 = {0}, .i32 = {1}, .i32 = {0}};
     * ARKUI_AttributeItem item = {.value = filterValue, .size = sizeof(filterValue)/ sizeof(ARKUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_COLOR_FILTER, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_COLOR_FILTER);
     * auto colorFilter = item->value
     * @endcode
     */
    NODE_IMAGE_COLOR_FILTER,
    /**
     * @brief The automatic scaling attribute of the image source supports the interface for setting, resetting, and obtaining the attribute.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32 Indicates whether a Boolean value is scaled.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32 Indicates whether a Boolean value is scaled.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_NumberValue resizeValue[] = {.i32 = true};
     * ARKUI_AttributeItem item = {.value = resizeValue, .size = 1}}
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_AUTO_RESIZE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_AUTO_RESIZE);
     * auto autoResize = item->value[0].i32
     * @endcode
     */
    NODE_IMAGE_AUTO_RESIZE,
    /**
     * @brief Placeholder map address attribute, which supports the interface for setting, resetting, and obtaining attributes.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .string indicates the address of the image component bitholder.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .string indicates the address of the image component bitholder.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ARKUI_AttributeItem item = {.string = "/pages/loading.png"}
     * nativeNodeApi->setAttribute(nodeHandle, NODE_IMAGE_ALT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_IMAGE_ALT);
     * auto altStr = item->string
     * @endcode
     */
    NODE_IMAGE_ALT,

    /**
     * @brief Background color attribute of the enabled component. The attribute setting, resetting, and obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].u32: indicates the background color. The value is in the 0xargb format. For example, 0xFFFF0000 indicates red.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: background color, in 0xargb format.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.u32=0xFFFF0000}};
     * ArkUI_AttributeItem item = {value, 1};
     * basicNodeApi->setAttribute(nodeHandle, NODE_TOGGLE_SELECTED_COLOR, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TOGGLE_SELECTED_COLOR);
     * auto nodeToggleSelectedColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_TOGGLE_SELECTED_COLOR = MAX_NODE_SCOPE_NUM * ARKUI_NODE_TOGGLE,
    /**
     * @brief Color attribute of the round slider of the Switch type. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].u32: color value of the circular slider. The value is in the 0xargb format. For example, 0xFFFF0000 indicates red.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: color value of the circular slider, in 0xargb format.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi - reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.u32=0xFFFF0000}};
     * ArkUI_AttributeItem item = {value, 1};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_TOGGLE_SWITCH_POINT_COLOR, &item);
     * auto item = nativeNodeApi=>getAttribute(nodeHandle, NODE_TOGGLE_SWITCH_POINT_COLOR);
     * auto nodeSwitchPointColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_TOGGLE_SWITCH_POINT_COLOR,

    /**
     * @brief Alignment mode of a subcomponent in a container. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: alignment mode. The data type is {@link ArkUI_Alignment}. The default value is AKUI_ALIGNMENT_CENTER.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Alignment mode, data type {@link ArkUI_Alignment}.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_ALIGNMENT_CENTER}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_STACK_ALIGN_CONTENT, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_STACK_ALIGN_CONTENT);
     * auto nodeStackAlignContent = item->value[0].i32;
     * @endcode
     */
    NODE_STACK_ALIGN_CONTENT = MAX_NODE_SCOPE_NUM * ARKUI_NODE_STACK,
    /**
     * @brief Status of the scroll bar of a scrolling component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: scroll bar status, data type {@link
     * ArkUI_ScrollBarDisplayMode}, default ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: scroll bar status, data type {@link ArkUI_ScrollBarDisplayMode}.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_BAR_DISPLAY_MODE, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_BAR_DISPLAY_MODE);
     * auto nodeScrollBarDisplayMode = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_BAR_DISPLAY_MODE = MAX_NODE_SCOPE_NUM * ARKUI_NODE_SCROLL,
    /**
     * @brief Width of the scroll bar of a scrolling component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: scroll bar width, in vp. The default value is 4. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: scroll bar width, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {20};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_BAR_WIDTH, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_BAR_WIDTH);
     * auto nodeScrollBarWidth = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SCROLL_BAR_WIDTH,
    /**
     * @brief Scroll bar color of a scrolling component. Attribute setting, attribute reset, and attribute obtaining are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .data[0].u32: scroll bar color, 0xargb type.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .data[0].u32: scroll bar color, 0xargb type.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.u32 = 0xFFFFFFFF}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_BAR_COLOR, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_BAR_COLOR);
     * auto nodeScrollBarColor = item->value[0].u32;
     * @endcode
     *
     */
    NODE_SCROLL_BAR_COLOR,
    /**
     * @brief Scrolling direction of the Scroll component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: scrolling direction, data type {@link ArkUI_Axis}, default value AKUI_AXIS_VERSICAL.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: scrolling direction, data type {@link ArkUI_Axis}.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_AXIS_VERTICAL}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_SCROLL_DIRECTION, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_SCROLL_DIRECTION);
     * auto nodeScrollBarDirection = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_SCROLL_DIRECTION,
    /**
     * @brief Edge sliding effect of a scrolling component. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: edge sliding effect. Parameter type: {@link ArkUI_EdgeEffect}. Default value: ARKUI_EDGE_EFFECT_NONE.\n
     * .value[1]?.i32: Specifies whether to enable the sliding effect when the size of the component content is smaller than that of the component itself. The value can be 1, and the value can be 0. The default value is 1.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: edge sliding effect. Parameter type: {@link ArkUI_EdgeEffect}.\n
     * .value[1]?.i32: Specifies whether to enable the sliding effect when the widget content size is smaller than the widget itself. The options are as follows: 1 If yes, 0 If no.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_EDGE_EFFECT_NONE}, {.i32 = 1}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_EDGE_EFFECT, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_EDGE_EFFECT);
     * auto nodeScrollEdgeEffect = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_EDGE_EFFECT,
    /**
     * @brief Indicates whether the scrolling component supports the scrolling gesture. If this parameter is set to false, the scrolling function cannot be performed by finger or mouse, but the scrolling interface of the controller is not affected.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: whether to support scrolling gestures. The default value is true.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: whether to support scrolling gestures.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = true}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_ENABLE_SCROLL_INTERACTION);
     * auto nodeScrollEnableScroll = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_ENABLE_SCROLL_INTERACTION,
    /**
     * @brief Rolling friction coefficient of rolling components, which takes effect when the rolling area is manually moved. It only affects the inertial rolling process and indirectly affects the chain effect during inertial rolling.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: friction coefficient. The default value is 0.6 for non-wearable devices and 0.9 for wearable devices.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: friction coefficient.
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {0.6};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_FRICTION, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_FRICTION);
     * auto nodeScrollFriction = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SCROLL_FRICTION,
    /**
     * @brief Limit scrolling mode of the scrolling component, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: mode for the Scroll component to limit the scrolling. The data type is {@link ArkUI_ScrollSnapAlign}. The default value is ARKUI_SCROLL_SNAP_ALIGN_NONE. \n
     * .value[1].i32: In the limit scroll mode of the Scroll component, if this property is set to false, the Scroll component slides freely between the start point and the first limit point. The default value is true. This parameter is valid only when there are multiple limit points.\n
     * .value[2].i32: In the limit scroll mode of the Scroll component, if this property is set to false, the Scroll component slides freely between the last limit point and the end. The default value is true. This parameter is valid only when there are multiple limit points.\n
     * .value[3...].f32: limit point of the Scroll component during scrolling. The limit point is the offset by which the Scroll component can slide and dock. You can have one or more.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: mode for the Scroll component to limit the scrolling. The data type is {@link ArkUI_ScrollSnapAlign}. \n
     * .value[1].i32: In the limit scroll mode of the Scroll component, if this property is set to false, the Scroll component slides freely between the start and the first limit point.\n
     * .value[2].i32: In the limit scroll mode of the Scroll component, if this property is set to false, the scroll can slide freely between the last limit point and the end.\n
     * .value[3...].f32: limit point of the Scroll component during scrolling. The limit point is the offset of the Scroll component that can slide and dock.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {
     * {.i32=ARKUI_SCROLL_SNAP_ALIGN_NONE}, {.i32=true}, {.i32=true},
     * {.f32=0}, {.f32=500}, {.f32=1000}, {.f32=1500}
     *};
    * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_SNAP, &item);
    * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_SNAP);
    * auto nodeScrollSnap = item->value[0].i32;
    * @endcode
    *
    */
    NODE_SCROLL_SNAP,

    /**
     * @brief Nested scrolling option of the scrolling component, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0]?.i32: nested scrolling when the scrollable component scrolls to the end, parameter type {@link ArkUI_ScrollNestedMode}.\n
     * .value[1]?.i32: nested scrolling when the scrollable component scrolls to the start end. The parameter type is {@link ArkUI_ScrollNestedMode}.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0]?.i32: nested scrolling when the scrollable component scrolls to the end, parameter type {@link ArkUI_ScrollNestedMode}.\n
     * .value[1]?.i32: nested scrolling when a scrollable component scrolls to the start end. The parameter type is {@link ArkUI_ScrollNestedMode}.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY},
     * {.i32 = ARKUI_SCROLL_NESTED_OPTIONS_SELF_ONLY}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SCROLL_NESTED_SCROLL, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SCROLL_NESTED_SCROLL);
     * auto first = item->value[0].i32;
     * auto second = item->value[1].i32;
     * @endcode
     *
     */
    NODE_SCROLL_NESTED_SCROLL,
    /**
     * @brief Scroll to the specified position. The attribute setting, reset, and obtaining are supported.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].f32: horizontal sliding offset, in vp.\n
     * .value[1].f32: vertical sliding offset, in vp.\n
     * .value[2]?.i32: indicates whether to enable the default spring effect. The default value is 0.
     * .value[3]?.i32: scrolling duration, in milliseconds.\n
     * .value[4]?.i32: optional, scrolling curve, parameter type {@link ArkUI_AnimationCurve}. The default value is RKUI_CURVE_EASE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: horizontal sliding offset, in vp.\n
     * .value[1].f32: vertical sliding offset, in vp.\n
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {10, 100, {.i32 = 1000}, {.i32 = ARKUI_CURVE_EASE},
     * {.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SCROLL_OFFSET, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SCROLL_OFFSET);
     * auto x = item->value[0].f32;
     * auto y = item->value[1].f32;
     * @endcode
     *
     */
    NODE_SCROLL_OFFSET,

    /**
     * @brief Scroll Scroll to the container edge, which supports attribute setting, resetting, and obtaining.
     *
     * Parameter format of the attribute setting method {@link ArkUI_AttributeItem}:\n
     * .value[0].i32: container edge. The parameter type is {@link ArkUI_ScrollEdge}.
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: container edge. The parameter type is {@link ArkUI_ScrollEdge}.
     *
     * @code {.c}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_SCROLL_EDGE_TOP}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SCROLL_EDGE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SCROLL_EDGE);
     * auto value = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_EDGE,

    /**
     * @brief Indicates whether to support sliding page turning, attribute setting, attribute reset, and attribute obtaining.
     *
     * If both the enablePaging and limit scrollSnap parameters are set, scrollSnap takes effect first, and enablePaging does not take effect.\n
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: indicates whether to support page turning by page. The default value is false.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: indicates whether to support page turning.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = true}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SCROLL_ENABLE_PAGING, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_SCROLL_ENABLE_PAGING);
     * auto nodeScrollEnablePaging = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SCROLL_ENABLE_PAGING,

    /**
     * @brief List direction, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Indicates the arrangement direction of the List component. The data type is {@link ArkUI_Axis}. The default value is AKUI_AXIS_VERSTIC.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: arrangement direction of the List component, data type {@link ArkUI_Axis}.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_AXIS_VERTICAL}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LIST_DIRECTION, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_LIST_DIRECTION);
     * auto nodeListDirection = item->value[0].i32;
     * @endcode
     *
     */
    NODE_LIST_DIRECTION = MAX_NODE_SCOPE_NUM * ARKUI_NODE_LIST,
    /**
     * @brief Set whether the header and footer in ListItemGroup need to be installed on the top or bottom.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: used together with the ListItemGroup component to set whether the header and footer in the ListItemGroup need to be installed on the top or bottom. Data type {@link ArkUI_StickyStyle}. Default value
     * AKUI_STICKY_ ??_:00-20:00_NONE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: used together with the ListItemGroup component to set whether the header and footer in the ListItemGroup need to be installed on the top or bottom. Data type {@link ArkUI_StickyStyle}.
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_STICKY_STYLE_NONE}};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LIST_STICKY, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_LIST_STICKY);
     * auto nodeListSticky = item->value[0].i32;
     * @endcode
     *
     */
    NODE_LIST_STICKY,

    /**
     * @brief List item spacing, which supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: interval in the direction of the main axis of the subassembly. Default value: 0.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Interval in the direction of the main axis of the subassembly.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {10};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LIST_SPACE, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_LIST_SPACE);
     * auto nodeListSpace = item->value[0].f32;
     * @endcode
     *
     */
    NODE_LIST_SPACE,

    /**
     * @brief Whether to enable the loop function of the swiper. The interface supports the attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: specifies whether to enable the loop. The value 0 indicates no loop, and the value 1 indicates loop. The default value is 1. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: specifies whether to enable the loop. The value 0 indicates no loop, and the value 1 indicates loop. The default value is 1. \n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.i32 = 0}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_LOOP, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_LOOP);
     * auto nodeSwiperLoop = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_LOOP = MAX_NODE_SCOPE_NUM * ARKUI_NODE_SWIPER,
    /**
     * @brief Indicates whether to automatically play the subcomponent of the swiper. The interface supports attribute setting, attribute resetting, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: controls whether to play the subcomponent automatically. The value 0 indicates not to play the subcomponent automatically, and the value 1 indicates to play the subcomponent automatically. The default value is 0. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: controls whether to play the subcomponent automatically. The value 0 indicates not to play the subcomponent automatically, and the value 1 indicates to play the subcomponent automatically. The default value is 0. \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_AUTO_PLAY, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_AUTO_PLAY);
     * auto nodeSwiperLoop = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_AUTO_PLAY,
    /**
     * @brief Whether to display the navigation point indicator on the swiper. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Indicates whether to display the navigation point indicator. The value 0 indicates that the navigation point indicator is not displayed, and the value 1 indicates that the navigation point indicator is displayed. The default value is 1. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Indicates whether to display the navigation point indicator. The value 0 indicates that the navigation point indicator is not displayed, and the value 1 indicates that the navigation point indicator is displayed. The default value is 1. \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 0}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_SHOW_INDICATOR, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_SHOW_INDICATOR);
     * auto nodeSwiperShowIndicator = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_SHOW_INDICATOR,
    /**
     * @brief Set the interval for automatically playing the Swiper. The interface supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Interval for playing autoplay, in milliseconds.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Interval for playing autoplay, in milliseconds.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {3000};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_INTERVAL, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_INTERVAL);
     * auto nodeSwiperInterval = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SWIPER_INTERVAL,
    /**
     * @brief This interface is used to set whether the Swiper is swiped vertically. The interface supports attribute setting, attribute reset, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: indicates whether the vertical sliding mode is used. The value 0 indicates horizontal sliding and the value 1 indicates vertical sliding. The default value is 0. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: indicates whether the vertical sliding mode is used. The value 0 indicates horizontal sliding and the value 1 indicates vertical sliding. The default value is 0. \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_VERTICAL, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_VERTICAL);
     * auto nodeSwiperVertical = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_VERTICAL,

    /**
     * @brief Set the duration for switching the animation of the Swiper subcomponent. The interface supports attribute setting, attribute resetting, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: animation duration for switching subcomponents, in milliseconds. The default value is 400.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: animation duration for switching subcomponents, in milliseconds. The default value is 400.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {1000};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_DURATION, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_DURATION);
     * auto nodeSwiperVertical = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SWIPER_DURATION,

    /**
     * @brief Sets the Swiper animation curve. The APIs for setting, resetting, and obtaining attributes are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Sets the animation curve parameters. The parameter type is {@link ArkUI_AnimationCurve}. The default value is ARKUI_CURVE_LINEAR.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Sets the animation curve parameters. The parameter type is {@link ArkUI_AnimationCurve}. The default value is ARKUI_CURVE_LINEAR.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_CURVE_SHARP}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_CURVE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_CURVE);
     * auto nodeSwiperVertical = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_CURVE,

    /**
     * @brief Sets the gap between the Swiper subcomponents. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].f32: Value of the gap between subcomponents.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].f32: Value of the gap between subcomponents.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {10};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_ITEM_SPACE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_ITEM_SPACE);
     * auto nodeSwiperVertical = item->value[0].f32;
     * @endcode
     *
     */
    NODE_SWIPER_ITEM_SPACE,

    /**
     * @brief Set the index value of the subcomponents displayed in the container of the swiper. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Index value of the subcomponent.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Index value of the subcomponent.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{i32 = 3}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_INDEX, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_ITEM_SPACE);
     * auto nodeSwiperIndex = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_INDEX,

    /**
     * @brief Set the number of elements to be displayed on a page of the swiper. The interface supports attribute setting, attribute resetting, and attribute obtaining.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: Index value of the subcomponent.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: Index value of the subcomponent.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{i32 = 3}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_DISPLAY_COUNT, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_ITEM_SPACE);
     * auto nodeSwiperDisplayCount = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_DISPLAY_COUNT,

    /**
     * @brief Disables the sliding switchover function of the swiper. The interface for setting, resetting, and obtaining attributes is supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: indicates whether to disable the sliding switch function of the component. The value 0 indicates that the sliding switch function is disabled, and the value 1 indicates that the sliding switch function is disabled. The default value is 0. \n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: indicates whether to disable the sliding switch function of the component. The value 0 indicates that the sliding switch function is disabled, and the value 1 indicates that the sliding switch function is disabled. The default value is 0. \n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = 1}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_SWIPER_DISABLE_SWIPE, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_DISABLE_SWIPE);
     * auto nodeSwiperDisplayCount = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_DISABLE_SWIPE,

    /**
     * @brief This interface is used to set whether the Swiper displays the navigation point arrow. Attribute setting, attribute reset, and attribute obtaining interfaces are supported.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].i32: indicates whether to display the navigation point arrow. Parameter type: {@link ArkUI_SwiperDisplayArrow}, \n
     * The default is ARKUI_SWIPER_ARROW_FALSE.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].i32: indicates whether to display the navigation point arrow. Parameter type: {@link ArkUI_SwiperDisplayArrow}, \n
     * The default is ARKUI_SWIPER_ARROW_FALSE.\n
     *
     * @code {.cpp}
     * ArkUI_NativeNodeAPI_1* nativeNodeApi =
     * reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
     * ArkUI_NumberValue value[] = {{.i32 = ARKUI_SWIPER_ARROW_SHOW_ON_HOVER}};
     * ArkUI_AttributeItem item = {value, sizeof(value)/sizeof(ArkUI_NumberValue)};
     * nativeNodeApi->setAttribute(nodeHandle, NODE_SWIPER_SHOW_DISPLAY_ARROW, &item);
     * auto item = nativeNodeApi->getAttribute(nodeHandle, NODE_SWIPER_SHOW_DISPLAY_ARROW);
     * auto nodeSwiperDisplayCount = item->value[0].i32;
     * @endcode
     *
     */
    NODE_SWIPER_SHOW_DISPLAY_ARROW,

    /**
     * @brief ListItemGroup header component.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .object: Use the {@link ArkUI_NodeHandle} object as the ListItemGroup header component.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .object: Use the {@link ArkUI_NodeHandle} object as the ListItemGroup header component.\n
     *
     * @code {.cpp}
     * auto header = nodeAPI->createNode(ARKUI_NODE_TEXT);
     * ARKUI_AttributeItem item = {.object = header};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LIST_ITEM_GROUP_SET_HEADER, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_LIST_ITEM_GROUP_SET_HEADER);
     * auto nodeListItemGroupSetHeader = item->object;
     * @endcode
     */
    NODE_LIST_ITEM_GROUP_SET_HEADER = MAX_NODE_SCOPE_NUM * ARKUI_NODE_LIST_ITEM_GROUP,
    /**
     * @brief ListItemGroup tail component.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .object: Use the {@link ArkUI_NodeHandle} object as the ListItemGroup tail component.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .object: Use the {@link ArkUI_NodeHandle} object as the ListItemGroup tail component.\n
     *
     * @code {.cpp}
     * auto footer = nodeAPI->createNode(ARKUI_NODE_TEXT);
     * ARKUI_AttributeItem item = {.object = footer};
     * basicNodeApi->setAttribute(nodeHandle, NODE_LIST_ITEM_GROUP_SET_FOOTER, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_LIST_ITEM_GROUP_SET_FOOTER);
     * auto nodeListItemGroupSetFooter = item->value[0].object;
     * @endcode
     */
    NODE_LIST_ITEM_GROUP_SET_FOOTER,
    /**
     * @brief ListItemGroup separator style. By default, there is no separator. Attributes can be set, reset, and obtained.
     *
     * Attribute setting method parameter {@link ArkUI_AttributeItem} format:\n
     * .value[0].u32: color, 0xargb type; \n
     * .value[1].f32: width of the split line, in vp.\n
     * .value[2].f32: distance between the split line and the start end of the side of the list, in vp.\n
     * .value[3].f32: distance between the split line and the end of the list, in vp.\n
     * \n
     * Return value {@link ArkUI_AttributeItem} format of the attribute obtaining method:\n
     * .value[0].u32: color, 0xargb type; \n
     * .value[1].f32: width of the split line, in vp.\n
     * .value[2].f32: distance between the split line and the start end of the side of the list, in vp.\n
     * .value[3].f32: distance between the split line and the end of the list, in vp.\n
     *
     * @code {.cpp}
     * ArkUI_NumberValue value[] = {{.u32 = 0xFFFFFFFF}, 1, 0, 0};
     * ARKUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
     * basicNodeApi->setAttribute(nodeHandle, NODE_XCOMPONENT_SURFACE_SIZE, &item);
     * auto item = basicNodeApi=>getAttribute(nodeHandle, NODE_XCOMPONENT_SURFACE_SIZE);
     * auto nodeXcomponentSurfaceSize = item->value[0].i32;
     * @endcode
     */
    NODE_LIST_ITEM_GROUP_SET_DIVIDER,
} ArkUI_NodeAttributeType;

#define MAX_COMPONENT_EVENT_ARG_NUM 12
/**
 * @brief Defines the parameter type of the component callback event.
 *
 * @since 12
 */
typedef struct {
    /** Data array object.*/
    ArkUI_NumberValue data[MAX_COMPONENT_EVENT_ARG_NUM];
} ArkUI_NodeComponentEvent;

/**
 * @brief Defines the type of the character string parameter used by the component callback event.
 *
 * @since 12
 */
typedef struct {
    /** String data.*/
const char* pStr;
} ArkUI_StringAsyncEvent;

/**
 * @brief Provides the definitions of the event types supported by the NativeNode component.
 *
 * @since 12
 */
typedef enum {
    /**
     * @brief Gesture event type.
     *
     * When an event callback occurs, the union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_TouchEvent}.
     */
    NODE_TOUCH_EVENT = 0,

    /**
     * @brief Mounting event.
     *
     * Condition for triggering this event: This callback is triggered when a component is mounted to the display.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * {@link ArkUI_NodeComponentEvent} does not contain parameters.
     */
    NODE_EVENT_ON_APPEAR,

    /**
     * @brief Component area change event
     *
     * Condition for triggering this event: This callback is triggered when the component area changes.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains 12 parameters.\n
     * <b>ArkUI_NodeComponent.data[0].f32</b> indicates the width of the target element in the old area. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[1].f32</b> indicates the height of the target element in the old area. The type is number and the unit is vp.\n
     * <b>ArkUI_NodeComponent.data[2].f32</b> indicates the x-axis coordinate of the position of the upper left corner of the target element in the old area relative to the upper left corner of the parent element. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[3].f32</b> indicates the y-axis coordinate of the position of the upper left corner of the target element in the old area relative to the upper left corner of the parent element. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[4].f32</b> indicates the x-axis coordinate of the position of the upper left corner of the target element in the old area relative to the upper left corner of the page. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[5].f32</b> indicates the y-axis coordinate of the position of the upper left corner of the target element in the old area relative to the upper left corner of the page. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[6].f32</b> indicates the width of the target element in the new area. The type is number and the unit is vp.\n
     * <b>ArkUI_NodeComponent.data[7].f32</b> indicates the height of the target element in the new area. The type is number and the unit is vp.\n
     * <b>ArkUI_NodeComponent.data[8].f32</b> indicates the x-axis coordinate of the position of the upper left corner of the new Area target element relative to the upper left corner of the parent element. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[9].f32</b> indicates the Y-axis coordinate of the position of the upper left corner of the target element in the new Area relative to the upper left corner of the parent element. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[10].f32</b> indicates the x-axis coordinate of the position of the target element in the upper left corner of the new Area target element relative to the upper left corner of the page. The type is number, in vp.\n
     * <b>ArkUI_NodeComponent.data[11].f32</b> indicates the y-axis coordinate of the position of the target element in the upper left corner of the new Area target element relative to the upper left corner of the page. The type is number, in vp.\n
     */
    NODE_EVENT_ON_AREA_CHANGE,
    /**
     * @brief Focus event.
     *
     * Condition for triggering this event: This callback is triggered when the component is in focus.\n
     * When the event callback occurred, the union type in the {@link ArkUI_NodeEvent} object of the event parameter was {@link ArkUI_NodeComponentEvent}.\n
     * {@link ArkUI_NodeComponentEvent} does not contain parameters.
     */
    NODE_ON_FOCUS,
    /**
     * @brief Focus event.
     *
     * Condition for triggering this event: This callback is triggered when the component is in focus.\n
     * When the event callback occurred, the union type in the {@link ArkUI_NodeEvent} object of the event parameter was {@link ArkUI_NodeComponentEvent}.\n
     * {@link ArkUI_NodeComponentEvent} does not contain parameters.
     */
    NODE_ON_BLUR,
    /**
     * @brief Click event type.
     *
     * When an event callback occurs, the union type in the {@link ArkUI_NodeEvent} object of the event parameter {@link ArkUI_TouchEvent} is {@link ArkUI_TouchEvent}, which contains the touch point information in the hand-lifting phase.
     */
    NODE_ON_CLICK,

    /**
     * @brief Image loading success event.
     *
     * This event is triggered when the image data is successfully loaded and decoded.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurred.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains nine parameters:\n
     * <b>ArkUI_NodeComponent.data[0].i32</b> indicates the loading status. The value 0 indicates that the data is loaded successfully, and the value 1 indicates that the decoding succeeds.\n
     * <b>ArkUI_NodeComponent.data[1].f32</b> indicates the width of the image, in px.\n
     * <b>ArkUI_NodeComponent.data[2].f32</b> indicates the image height, in px.\n
     * <b>ArkUI_NodeComponent.data[3].f32</b> indicates the width of the current component, in px.\n
     * <b>ArkUI_NodeComponent.data[4].f32</b> indicates the height of the current component, in px.\n
     * <b>ArkUI_NodeComponent.data[5].f32</b>Position of the image drawing area relative to the X axis of the component, in px.\n
     * <b>ArkUI_NodeComponent.data[6].f32</b>Position of the drawing area relative to the Y axis of the component, in px.\n
     * <b>ArkUI_NodeComponent.data[7].f32</b> Width of the drawing area, in px.\n
     * <b>ArkUI_NodeComponent.data[8].f32</b>: image drawing area height, in px.\n
     */
    NODE_IMAGE_ON_COMPLETE = MAX_NODE_SCOPE_NUM * ARKUI_NODE_IMAGE,
    /**
     * @brief Image loading failure event.
     *
     * Condition for triggering this event: This callback is triggered when the image loading is abnormal.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains 1 parameter:\n
     * <b>ArkUI_NodeComponent.data[0].i32</b>Error code information:\n
     * 401: The image path parameter is abnormal. The image data cannot be obtained.\n
     * 103101: Picture format not supported.\n
     */
    NODE_IMAGE_ON_ERROR,

    /**
     * @brief Image loading failure event.
     *
     * This event is triggered when the SVG picture animation that drives the effect ends.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * <b>::ArkUI_NodeComponentEvent</b> does not contain parameters.
     *
     */
    NODE_IMAGE_ON_SVG_PLAY_FINISH,

    /**
     * @brief This event is triggered when the switch status changes.
     *
     * This event is triggered when the switch status changes.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains one parameter:\n
     * <b>ArkUI_NodeComponent.data[0].i32</b>: status of the current switch. The value 1 indicates that the switch is enabled, and the value 0 indicates that the switch is disabled.
     *
     */
    NODE_TOGGLE_ON_CHANGE = MAX_NODE_SCOPE_NUM * ARKUI_NODE_TOGGLE,
    /**
     * @brief Defines the scrolling event enumeration values of the AKUI_NODE_SCROLL scrolling component.
     *
     * Trigger conditions for this event:\n
     * 1. When the scrolling component triggers scrolling, other input settings for triggering scrolling, such as keyboard and mouse operations, are supported.\n
     * 2. Invoke the scrolling controller API.\n
     * 3. Out-of-bounds rebound.\n
     * When the event callback occurred, the union type in the {@link ArkUI_NodeEvent} object of the event parameter was {@link ArkUI_NodeComponentEvent}.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains 2 parameters.\n
     * <b>ArkUI_NodeComponent.data[0].f32</b> indicates the X-axis increment from the last event trigger.\n
     * <b>ArkUI_NodeComponent.data[1].f32</b> indicates the Y-axis increment from the last event trigger.\n
     */
    NODE_SCROLL_EVENT_ON_SCROLL = MAX_NODE_SCOPE_NUM * ARKUI_NODE_SCROLL,
    /**
     * @brief Defines the enumerated value of the scroll frame start event of the RKUI_NODE_SC ROLL scrolling component.
     *
     * Trigger conditions for this event:\n
     * 1. The scrolling component triggers scrolling, including other input settings that trigger scrolling, such as keyboard and mouse operations.\n
     * 2. Not triggered when the controller interface is invoked.\n
     * 3. Out-of-bounds rebound is not triggered.\n
     * The union type in the {@link ArkUI_NodeEvent} object was {@link ArkUI_NodeComponentEvent} when the event callback occurred.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains 2 parameters.\n
     * <b>ArkUI_NodeComponent.data[0].f32</b> indicates the amount of scrolling that will occur.\n
     * <b>ArkUI_NodeComponent.data[1].i32</b> indicates the current scrolling status.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains one return value.\n
     * <b>ArkUI_NodeComponent.data[0].f32</b>In the event processing function, the actual scrolling amount can be calculated based on the application scenario and stored in data[0].f32. Scrolling is performed based on the actual scrolling amount of the returned value.\n
     */
    NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN,
    /**
     * @brief Defines the enumerated values of the scrolling start event of the AKUI_NODE_SCROLL component.
     *
     * Trigger conditions for this event:\n
     * 1. Triggered when the scrolling component starts to scroll. Other input settings for triggering scrolling, such as keyboard and mouse operations, are supported.\n
     * 2. Start after calling the scrolling controller API, with transition effect.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurred.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains no parameters.\n
     */
    NODE_SCROLL_EVENT_ON_SCROLL_START,
    /**
     * @brief Defines the enumerated values of the scrolling stop event of the AKUI_NODE_SC ROLL component.
     *
     * Trigger conditions for this event:\n
     * 1. The scrolling component stops after triggering scrolling. Other input settings for triggering scrolling, such as keyboard and mouse operations, are supported.\n
     * 2. After the API of the scrolling controller is invoked, stop with the transition effect.\n
     * The union type in the {@link ArkUI_NodeEvent} object is {@link ArkUI_NodeComponentEvent} when the event callback occurs.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains no parameters.\n
     */
    NODE_SCROLL_EVENT_ON_SCROLL_STOP,
    /**
     * @brief Defines the enumerated values of the scrolling edge event of the ARKUI_NODE_SCROLL component.
     *
     * Trigger conditions for this event:\n
     * 1. The scrolling component is triggered when the scrolling component scrolls to the edge. Other input settings for triggering scrolling, such as keyboard and mouse operations, are supported.\n
     * 2. Invoke the scrolling controller API.\n
     * 3. Out-of-bounds rebound.\n
     * When the event callback occurred, the union type in the {@link ArkUI_NodeEvent} object of the event parameter was {@link ArkUI_NodeComponentEvent}.\n
     * <b>::ArkUI_NodeComponentEvent</b> contains 1 parameter.\n
     * <b>ArkUI_NodeComponent.data[0].i32</b> indicates the upper, lower, left, and right edges that are touched.\n
     */
    NODE_SCROLL_EVENT_ON_SCROLL_EDGE,
} ArkUI_NodeEventType;

/**
 * @brief Defines the common structure type of a component event.
 *
 * @since 12
 */
typedef struct {
    /**
     * @brief Event type.
     *
     * @see ArkUI_NodeEventType
     */
    int32_t kind;

    /**
     * @brief Event ID.
     *
     * The event ID is transferred as a parameter when the {@link registerNodeEvent} function is invoked and can be applied to the same event entry function {@link
     * registerNodeEventReceiver} dispatching logic.
     */
    int32_t eventId;
    /** Component object that triggers the component.*/
    ArkUI_NodeHandle node;
    union {
        /** Callback parameter of the touch event type.*/
        ArkUI_NodeTouchEvent touchEvent;
        /** The common component event uses the callback parameter of the numeric type.*/
        ArkUI_NodeComponentEvent componentEvent;
        /** The common component event uses the callback parameter of the string type.*/
        ArkUI_StringAsyncEvent stringEvent;
    };
} ArkUI_NodeEvent;

/**
 * @brief The custom component invokes <b>::markDirty</b>, which is the type of the passable hide ID.
 *
 * @since 12
 */
typedef enum {
    /**
     * @brief Resize.
     *
     * When the flag type is triggered, re-layout is also triggered by default.
     */
    NODE_NEED_MEASURE = 1,

    /** Relay the position.*/
    NODE_NEED_LAYOUT,
    /** Repaint.*/
    NODE_NEED_RENDER,
} ArkUI_NodeDirtyFlag;

/**
 * @brief ArkUI node-type API set on the Native side.
 *
 * @version 1
 * @since 12
 */
typedef struct {
    /** Structure version.*/
    int32_t version;

    /**
     * @brief Generate the corresponding component based on {@link ArkUI_NodeType} and return the component object pointer.
     *
     * @param type Create a UI component node of a specified type.
     * @return Return the pointer to the created component operation. If the creation fails, NULL is returned.
     */
    ArkUI_NodeHandle (*createNode)(ArkUI_NodeType type);

    /**
     * @brief Destroys the component object pointed to by the component pointer.
     *
     * @param node Component pointer object.
     */
    void (*disposeNode)(ArkUI_NodeHandle node);

    /**
     * @brief Mount the component to a parent node.
     *
     * @param parent Pointer to the parent node.
     * @param child Pointer to the child node.
     * @return 0: success
     *         401 - Function parameter exception.
     */
    int32_t (*addChild)(ArkUI_NodeHandle parent, ArkUI_NodeHandle child);

    /**
     * @brief Remove the component from the parent node.
     *
     * @param parent Pointer to the parent node.
     * @param child Pointer to the child node.
     * @return 0: success
     *         401 - Function parameter exception.
     */
    int32_t (*removeChild)(ArkUI_NodeHandle parent, ArkUI_NodeHandle child);

    /**
     * @brief Mount the component to a parent node after the <b>sibling</b> node.
     *
     * @param parent Pointer to the parent node.
     * @param child Pointer to the child node.
     * @param sibling Pointer to the previous sibling node. If the value is empty, the insertion position is at the beginning.
     * @return 0: success
     *         401 - Function parameter exception.
     */
    int32_t (*insertChildAfter)(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, ArkUI_NodeHandle sibling);

    /**
     * @brief Mount the component to a parent node. The mounting position is before the <b>sibling</b> node.
     *
     * @param parent Pointer to the parent node.
     * @param child Pointer to the child node.
     * @param sibling Pointer to the next sibling node. If the value is empty, the insertion position is at the end.
     * @return 0: success
     *         401 - Function parameter exception.
     */
    int32_t (*insertChildBefore)(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, ArkUI_NodeHandle sibling);

    /**
     * @brief Mount the component to a parent node. The mounting position is specified by <b>position</b>.
     *
     * @param parent Pointer to the parent node.
     * @param child Pointer to the child node.
     * @param postion Insertion Position. If the insertion position is a negative number or does not exist, the insertion position is at the end by default.
     * @return 0: success
     *         401 - Function parameter exception.
     */
    int32_t (*insertChildAt)(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, int32_t position);

    /**
     * @brief Attribute setting function.
     *
     * @param node Node object for which attributes need to be set.
     * @param attribute Type of the attribute to be set.
     * @param item Attribute value to be set.
     * @return 0: success.
     *         401 - Function parameter exception.
     *         106102 - The attribute type is not supported by the current component.
     */
    int32_t (*setAttribute)(ArkUI_NodeHandle node, ArkUI_NodeAttributeType attribute, const ArkUI_AttributeItem* item);

    /**
     * @brief Attribute obtaining function.
     *
     * The pointer returned by this API is the buffer pointer in the ArkUI framework. Developers do not need to call delete to release memory. However, the pointer must be used before the function is called next time. Otherwise, the pointer may be overwritten by other values.
     *
     * @param node Node object whose attributes need to be obtained.
     * @param attribute Type of the attribute to be obtained.
     * @return Attribute value of the current attribute type. If the operation fails, a null pointer is returned.
     */
    const ArkUI_AttributeItem* (*getAttribute)(ArkUI_NodeHandle node, ArkUI_NodeAttributeType attribute);

    /**
     * @brief Reset attribute function.
     *
     * @param node Node object whose attributes need to be reset.
     * @param attribute Type of the attribute to be reset.
     * @return 0: success.
     *         401 - Function parameter exception.
     *         106102 - The attribute type is not supported by the current component.
     */
    int32_t (*resetAttribute)(ArkUI_NodeHandle node, ArkUI_NodeAttributeType attribute);

    /**
     * @brief Register the node event function.
     *
     * @param node Node object to register the event.
     * @param eventType Event type to be registered.
     * @param eventId User-defined event ID, which is carried in the callback parameter <@link ArkUI_NodeEvent> when an event is triggered.
     * @return 0: success.
     *         401 - Function parameter exception.
     *         106102 - This event type is not supported by the current component.
     */
    int32_t (*registerNodeEvent)(ArkUI_NodeHandle node, ArkUI_NodeEventType eventType, int32_t eventId);

    /**
     * @brief Event function for deregistering a node.
     *
     * @param node Node object whose event needs to be deregistered.
     * @param eventType Event type to be deregistered.
     */
    void (*unregisterNodeEvent)(ArkUI_NodeHandle node, ArkUI_NodeEventType eventType);

    /**
     * @brief Register the unified entry function for event callback.
     *
     * The ArkUI framework collects component events generated during the process and invokes the registered eventReceiver function to call back the eventReceiver function to developers.\n
     * If the function is called repeatedly, the previously registered function will be overwritten.
     *
     * @param eventReceiver Unified callback entry function.
     */
    void (*registerNodeEventReceiver)(void (*eventReceiver)(ArkUI_NodeEvent* event));

    /**
     * @brief Unregister event callback unified entry function.
     *
     */
    void (*unregisterNodeEventReceiver)();

    /**
     * @brief Forcibly mark the current node that needs to be recalculated, laid out, or drawn.
     *
     * In the system attribute setting update scenario, the ArkUI framework automatically marks the hidden area and performs calculation, layout, or drawing again. Developers do not need to invoke this function.
     *
     * @param nodeThe node object that needs to be marked in the hidden area.
     * @param dirtyFlag Tibetan area type.
     */
    void (*markDirty)(ArkUI_NodeHandle node, ArkUI_NodeDirtyFlag dirtyFlag);
} ArkUI_NativeNodeAPI_1;

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_NODE_H
/** @}*/
