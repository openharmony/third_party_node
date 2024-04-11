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

#ifndef ARKUI_NATIVE_ANIMATE_H
#define ARKUI_NATIVE_ANIMATE_H

#include <cstdint>

#include "native_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief 设置动画的期望帧率。
*
* @since 12
*/
typedef struct {
    /** 期望的最小帧率。*/
    uint32_t min;
    /** 期望的最大帧率。*/
    uint32_t max;
    /** 期望的最优帧率。*/
    uint32_t expected;
} ArkUI_ExpectedFrameRateRange;

/**
* @brief 动画播放完成回调类型。
*
* @since 12
*/
typedef struct {
    /** 在动画中定义onFinish回调的类型。*/
    ArkUI_FinishCallbackType type;
    /** 动画播放完成回调。*/
    void (*callback)(void* userData);
    /** 自定义类型。*/
    void* userData;
} ArkUI_AnimateCompleteCallback;

/**
* @brief 设置动画效果相关参数。
*
* @since 12
*/
typedef struct ArkUI_AnimateOption ArkUI_AnimateOption;

/**
 * @brief ArkUI提供的Native侧动画接口集合。
 *
 * @version 1
 * @since 12
 */
typedef struct {
    /**
    * @brief 显式动画接口
    *
    * @note event闭包中要设置的组件属性，必须在其之前设置过。
    *
    * @param option 设置动画效果相关参数。
    * @param update 指定动效的闭包函数，在闭包函数中导致的状态变化系统会自动插入过渡动画。
    * @param complete 设置动画播放完成回调参数。
    * @return 返回错误码，0 - 成功， 401 - 参数错误。
    */
    int32_t (*animateTo)(
        ArkUI_AnimateOption* option, ArkUI_ContextCallback* update, ArkUI_AnimateCompleteCallback* complete);
} ArkUI_NativeAnimateAPI_1;

/**
* @brief 创建动画效果参数。
*
* @return 新的动画效果参数指针。
* @since 12
*/
ArkUI_AnimateOption* OH_ArkUI_AnimateOption_Create();

/**
* @brief 销毁动画效果参数指针。
*
* @since 12
*/
void OH_ArkUI_AnimateOption_Dispose(ArkUI_AnimateOption* option);

/**
* @brief 获取动画持续时间，单位为ms(毫秒)。
*
* @param option 动画效果参数。
* @return 持续时间。
* @since 12
*/
uint32_t OH_ArkUI_AnimateOption_GetDuration(ArkUI_AnimateOption* option);

/**
* @brief 获取动画播放速度。
*
* @param option 动画效果参数。
* @return 动画播放速度。
* @since 12
*/
float OH_ArkUI_AnimateOption_GetTempo(ArkUI_AnimateOption* option);

/**
* @brief 获取动画曲线。
*
* @param option 动画效果参数。
* @return 动画曲线。
* @since 12
*/
ArkUI_AnimationCurve OH_ArkUI_AnimateOption_GetCurve(ArkUI_AnimateOption* option);

/**
* @brief 获取动画延迟播放时间，单位为ms(毫秒)。
*
* @param option 动画效果参数。
* @return 动画延迟播放时间。
* @since 12
*/
int32_t OH_ArkUI_AnimateOption_GetDelay(ArkUI_AnimateOption* option);

/**
* @brief 获取动画播放次数。
*
* @param option 动画效果参数。
* @return 动画播放次数。
* @since 12
*/
int32_t OH_ArkUI_AnimateOption_GetIterations(ArkUI_AnimateOption* option);

/**
* @brief 获取动画播放模式。
*
* @param option 动画效果参数。
* @return 动画播放模式。
* @since 12
*/
ArkUI_AnimationPlayMode OH_ArkUI_AnimateOption_GetPlayMode(ArkUI_AnimateOption* option);

/**
* @brief 获取动画的期望帧率。
*
* @param option 动画效果参数。
* @return 动画的期望帧率。
* @since 12
*/
ArkUI_ExpectedFrameRateRange* OH_ArkUI_AnimateOption_GetExpectedFrameRateRange(ArkUI_AnimateOption* option);

/**
* @brief 设置动画持续时间。
*
* @param option 动画效果参数。
* @param value 持续时间，单位为ms(毫秒)。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetDuration(ArkUI_AnimateOption* option, int32_t value);

/**
* @brief 设置动画播放速度。
*
* @param option 动画效果参数。
* @param value 动画播放速度。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetTempo(ArkUI_AnimateOption* option, float value);

/**
* @brief 设置动画曲线。
*
* @param option 动画效果参数。
* @param value 动画曲线。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetCurve(ArkUI_AnimateOption* option, ArkUI_AnimationCurve value);

/**
* @brief 设置动画延迟播放时间。
*
* @param option 动画效果参数。
* @param value 动画延迟播放时间。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetDelay(ArkUI_AnimateOption* option, int32_t value);

/**
* @brief 设置动画播放次数。
*
* @param option 动画效果参数。
* @param value 动画播放次数。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetIterations(ArkUI_AnimateOption* option, int32_t value);

/**
* @brief 设置动画播放模式。
*
* @param option 动画效果参数。
* @param value 动画播放模式。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetPlayMode(ArkUI_AnimateOption* option, ArkUI_AnimationPlayMode value);

/**
* @brief 设置动画的期望帧率。
*
* @param option 动画效果参数。
* @param value 动画的期望帧率。
* @since 12
*/
void OH_ArkUI_AnimateOption_SetExpectedFrameRateRange(ArkUI_AnimateOption* option, ArkUI_ExpectedFrameRateRange* value);

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_ANIMATE_H