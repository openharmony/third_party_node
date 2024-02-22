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

#ifndef C_INCLUDE_DRAWING_FONT_H
#define C_INCLUDE_DRAWING_FONT_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 11
 * @version 1.0
 */

/**
 * @file drawing_font.h
 *
 * @brief Declares functions related to the <b>font</b> object in the drawing module.
 *
 * @since 11
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Font</b> object created.
 * @since 11
 * @version 1.0
 */
OH_Drawing_Font* OH_Drawing_FontCreate(void);

/**
 * @brief Sets an <b>OH_Drawing_Typeface</b> object for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_Typeface Indicates the pointer to an <b>OH_Drawing_Typeface</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTypeface(OH_Drawing_Font*, OH_Drawing_Typeface*);

/**
 * @brief Gets an <b>OH_Drawing_Typeface</b> object from the <b>OH_Drawing_Typeface</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @return OH_Drawing_Typeface Indicates the pointer to an <b>OH_Drawing_Typeface</b> object.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_FontGetTypeface(OH_Drawing_Font*);

/**
 * @brief Sets text size for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param textSize Indicates the text size.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTextSize(OH_Drawing_Font*, float textSize);

/**
 * @brief Calculate number of glyphs represented by text.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param text Indicates the character storage encoded with text encoding.
 * @param byteLength Indicates the text length in bytes.
 * @param encoding Indicates the text encoding.
 * @since 12
 * @version 1.0
 */
int OH_Drawing_FontCountText(OH_Drawing_Font*, const void* text, size_t byteLength,
    OH_Drawing_TextEncoding encoding);

/**
 * @brief Enables or disables linearly scalable font for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param isLinearText Indicates whether to enable linearly scalable font.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetLinearText(OH_Drawing_Font*, bool isLinearText);

/**
 * @brief Sets text skew on x-axis for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param skewX Indicates the additional shear on x-axis relative to y-axis.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetTextSkewX(OH_Drawing_Font*, float skewX);

/**
 * @brief Enables or disables to increase stroke width to approximate bold fonts for an <b>OH_Drawing_Font</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param isFakeBoldText Indicates whether to enable to increase stroke width.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontSetFakeBoldText(OH_Drawing_Font*, bool isFakeBoldText);

/**
 * @brief Destroys an <b>OH_Drawing_Font</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @since 11
 * @version 1.0
 */
void OH_Drawing_FontDestroy(OH_Drawing_Font*);

/**
 * @brief Defines a run, supplies storage for the metrics of an SkFont.
 *
 * @since 12
 * @version 1.0
 */
typedef struct OH_Drawing_Font_Metrics {
    /**
     * @brief Indicating which metrics are valid
     * @since_2013 12
     */
    uint32_t flags;
    /**
     * @brief storage for top in font metrics
     * @since_2013 12
     */
    float top;
    /**
     * @brief storage for ascent in font metrics
     * @since_2013 12
     */
    float ascent;
    /**
     * @brief storage for descent in font metrics
     * @since_2013 12
     */
    float descent;
    /**
     * @brief storage for bottom in font metrics
     * @since_2013 12
     */
    float bottom;
    /**
     * @brief storage for leading in font metrics
     * @since_2013 12
     */
    float leading;
    /**
     * @brief Average character width, zero if unknown
     * @since_2013 12
     */
    float avgCharWidth;
    /**
     * @brief Maximum character width, zero if unknown
     * @since_2013 12
     */
    float maxCharWidth;
    /**
     * @brief Greatest extent to left of origin of any glyph bounding box, typically negative; deprecated with variable fonts
     * @since_2013 12
     */
    float xMin;
    /**
     * @brief Greatest extent to right of origin of any glyph bounding box, typically positive; deprecated with variable fonts
     * @since_2013 12
     */
    float xMax;
    /**
     * @brief Height of lower-case letter, zero if unknown, typically negative
     * @since_2013 12
     */
    float xHeight;
    /**
     * @brief Height of an upper-case letter, zero if unknown, typically negative
     * @since_2013 12
     */
    float capHeight;
    /**
     * @brief Underline thickness
     * @since_2013 12
     */
    float underlineThickness;
    /**
     * @brief Distance from baseline to top of stroke, typically positive
     * @since_2013 12
     */
    float underlinePosition;
    /**
     * @brief Strikeout thickness
     * @since_2013 12
     */
    float strikeoutThickness;
    /**
     * @brief Distance from baseline to bottom of stroke, typically negative
     * @since_2013 12
     */
    float strikeoutPosition;
} OH_Drawing_Font_Metrics;

/**
 * @brief Obtains the metrics of a font.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Font Indicates the pointer to an <b>OH_Drawing_Font</b> object.
 * @param OH_Drawing_Font_Metrics Indicates the pointer to an <b>OH_Drawing_Font_Metrics</b> object.
 * @return Returns a float variable that recommended spacing between lines.
 * @since 12
 * @version 1.0
 */
float OH_Drawing_FontGetMetrics(OH_Drawing_Font*, OH_Drawing_Font_Metrics*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
