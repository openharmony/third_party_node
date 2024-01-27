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
 * @file native_type.h
 *
 * @brief Provides the common type definition of the NativeModule.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 12
 */

#ifndef ARKUI_NATIVE_TYPE_H
#define ARKUI_NATIVE_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides the definition of the ArkUI native component instance object.
 *
 * @since 12
 */
struct ArkUI_Node;

/**
 * @brief Define the object pointer of the ArkUI native component instance.
 *
 * @since 12
 */
typedef struct ArkUI_Node* ArkUI_NodeHandle;

/**
 * @brief ArkUI number type definition on the native side.
 *
 * @since 12
 */
typedef union {
    /** Floating-point type.*/
    float f32;
    /** Signed integer.*/
    int32_t i32;
    /** Unsigned integer.*/
    uint32_t u32;
} ArkUI_NumberValue;

/**
 * @brief Defines the layout alignment enumerated values.
 *
 * @since 12
 */
typedef enum {
    /** Start from the top.*/
    ARKUI_ALIGNMENT_TOP_START = 0,
    /** Top centered.*/
    ARKUI_ALIGNMENT_TOP,
    /** Top and tail ends.*/
    ARKUI_ALIGNMENT_TOP_END,
    /** The start end is vertically centered.*/
    ARKUI_ALIGNMENT_START,
    /** Center horizontally and vertically.*/
    ARKUI_ALIGNMENT_CENTER,
    /** The tail end is centered vertically.*/
    ARKUI_ALIGNMENT_END,
    /** Start end at the bottom.*/
    ARKUI_ALIGNMENT_BOTTOM_START,
    /** The bottom is horizontally centered.*/
    ARKUI_ALIGNMENT_BOTTOM,
    /** Bottom end.*/
    ARKUI_ALIGNMENT_BOTTOM_END,
} ArkUI_Alignment;

/**
 * @brief Defines the enumerated values for repeated image placement.
 *
 * @since 12
 */
typedef enum {
    /** The value must not be repeated.*/
    ARKUI_IMAGE_REPEAT_NONE = 0,
    /** Repeat in the X axis direction.*/
    ARKUI_IMAGE_REPEAT_X,
    /** Repeat on the Y axis.*/
    ARKUI_IMAGE_REPEAT_Y,
    /** Repeat on the X and Y axes.*/
    ARKUI_IMAGE_REPEAT_XY,
} ArkUI_ImageRepeat;

/**
 * @brief Defines the enumerated values of the font style.
 *
 * @since 12
 */
typedef enum {
    /** Standard font style.*/
    ARKUI_FONT_STYLE_NORMAL = 0,
    /** Italic font style.*/
    ARKUI_FONT_STYLE_ITALIC
} ArkUI_FontStyle;

/**
 * @brief Defines the enumerated values of the font thickness and word weight.
 *
 * @since 12
 */
typedef enum {
    /** 100 */
    ARKUI_FONT_WEIGHT_W100 = 0,
    /** 200 */
    ARKUI_FONT_WEIGHT_W200,
    /** 300 */
    ARKUI_FONT_WEIGHT_W300,
    /** 400 */
    ARKUI_FONT_WEIGHT_W400,
    /** 500 */
    ARKUI_FONT_WEIGHT_W500,
    /** 600 */
    ARKUI_FONT_WEIGHT_W600,
    /** 700 */
    ARKUI_FONT_WEIGHT_W700,
    /** 800 */
    ARKUI_FONT_WEIGHT_W800,
    /** 900 */
    ARKUI_FONT_WEIGHT_W900,
    /** The font size is thick.*/
    ARKUI_FONT_WEIGHT_BOLD,
    /** The font thickness is normal.*/
    ARKUI_FONT_WEIGHT_NORMAL,
    /** The font is very thick.*/
    ARKUI_FONT_WEIGHT_BOLDER,
    /** The font size is fine.*/
    ARKUI_FONT_WEIGHT_LIGHTER,
    /** The font thickness is moderate.*/
    ARKUI_FONT_WEIGHT_MEDIUM,
    /** The font thickness is normal.*/
    ARKUI_FONT_WEIGHT_REGULAR,
} ArkUI_FontWeight;

/**
 * @brief Defines the enumerated values of the horizontal alignment style of the font.
 *
 * @since 12
 */
typedef enum {
    /** Align the header horizontally.*/
    ARKUI_TEXT_ALIGNMENT_START = 0,
    /** Align the center horizontally.*/
    ARKUI_TEXT_ALIGNMENT_CENTER,
    /** Align the tail horizontally.*/
    ARKUI_TEXT_ALIGNMENT_END,
    /** Align the two ends.*/
    ARKUI_TEXT_ALIGNMENT_JUSTIFY,
} ArkUI_TextAlignment;

typedef enum {
    /** Do not use decorative lines.*/
    ARKUI_TEXT_DECORATION_TYPE_NONE = 0,
    /** Text underline decoration.*/
    ARKUI_TEXT_DECORATION_TYPE_UNDERLINE,
    /** The text is marked with a dash.*/
    ARKUI_TEXT_DECORATION_TYPE_OVERLINE,
    /** Decorative line that passes through the text.*/
    ARKUI_TEXT_DECORATION_TYPE_LINE_THROUGH,
} ArkUI_TextDecorationType;

typedef enum {
    /** Retain the original case.*/
    ARKUI_TEXT_CASE_NORMAL = 0,
    /** The text is in lowercase.*/
    ARKUI_TEXT_CASE_LOWER,
    /** The text is in uppercase.*/
    ARKUI_TEXT_CASE_UPPER,
} ArkUI_TextCase;

typedef enum {
    /** Replication is not supported.*/
    ARKUI_COPY_OPTIONS_NONE = 0,
    /** Intra-application replication is supported.*/
    ARKUI_COPY_OPTIONS_IN_APP,
    /** Intra-device replication is supported.*/
    ARKUI_COPY_OPTIONS_LOCAL_DEVICE,
    /** Cross-device replication is supported.*/
    ARKUI_COPY_OPTIONS_CROSS_DEVICE,
} ArkUI_CopyOptions;

typedef enum {
    /** Color.*/
    ARKUI_SHADOW_TYPE_COLOR = 0,
    /** Blur.*/
    ARKUI_SHADOW_TYPE_BLUR
} ArkUI_ShadowType;

/**
 * @brief Defines the enumerated values of the edge sliding effect.
*
 * @since 12
 */
typedef enum {
    /** Elastic physical dynamic effect. After sliding to the edge, you can continue to slide for a certain distance according to the initial speed or through touch events. After releasing your hand, you can bounce back.*/
    ARKUI_EDGE_EFFECT_SPRING = 0,
    /** Shadow effect. When the camera slides to the edge, a circular shadow is displayed.*/
    ARKUI_EDGE_EFFECT_FADE,
    /** No effect after sliding to the edge.*/
    ARKUI_EDGE_EFFECT_NONE,
} ArkUI_EdgeEffect;

/**
 * @brief Defines the end alignment effect of list items.
 *
 * @since 12
 */
typedef enum {
    /** By default, there is no project scrolling alignment effect.*/
    ARKUI_SCROLL_SNAP_ALIGN_NONE = 0,
    /** The first item in the view is aligned at the beginning of the list.*/
    ARKUI_SCROLL_SNAP_ALIGN_START,
    /** Intermediate items in the view will be aligned in the center of the list.*/
    ARKUI_SCROLL_SNAP_ALIGN_CENTER,
    /** The last item in the view is aligned at the end of the list.*/
    ARKUI_SCROLL_SNAP_ALIGN_END,
} ArkUI_ScrollSnapAlign;

/**
 * @brief Defines the enumerated values of the scroll bar status.
 *
 * @since 12
 */
typedef enum {
    /** Not displayed.*/
    ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF = 0,
    /** On-demand display (displayed when touched and disappears after 2s)*/
    ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO,
    /** Resident display.*/
    ARKUI_SCROLL_BAR_DISPLAY_MODE_ON,
} ArkUI_ScrollBarDisplayMode;
/**
 * @brief Defines the scrolling direction and list component arrangement direction.
 *
 * @since 12
 */
typedef enum {
    /** Only vertical scrolling is supported.*/
    ARKUI_AXIS_VERTICAL = 0,
    /** Only horizontal scrolling is supported.*/
    ARKUI_AXIS_HORIZONTAL,
} ArkUI_Axis;

/**
 * @brief Defines the enumerated values of whether the list is ceiling-mounted and bottom-mounted.
 *
 * @since 12
 */
typedef enum {
    /** The header of ListItemGroup does not absorb the top, and the footer does not absorb the bottom.*/
    ARKUI_STICKY_STYLE_NONE = 0,
    /** The header of the ListItemGroup is ceiling-mounted, and the footer is not bottom-mounted.*/
    ARKUI_STICKY_STYLE_HEADER,
    /** The footer of ListItemGroup is bottomed, and the header is not topped.*/
    ARKUI_STICKY_STYLE_FOOTER,
    /** The footer and header of ListItemGroup are installed at the bottom and the top.*/
    ARKUI_STICKY_STYLE_BOTH,
} ArkUI_StickyStyle;


/**
 * @brief Enumerated value of the border line style.
 *
 * @since 12
 */
typedef enum {
    /** Displayed as a solid line.*/
    ARKUI_BORDER_STYLE_SOLID = 0,
    /** Displayed as a series of short square dashed lines.*/
    ARKUI_BORDER_STYLE_DASHED,
    /** Displayed as a series of dots.*/
    ARKUI_BORDER_STYLE_DOTTED,
} ArkUI_BorderStyle;

/**
 * @brief Touch test control enumerated value.
 *
 * @since 12
 */
typedef enum {
    /** Default touch test effect.*/
    ARKUI_HIT_TEST_MODE_DEFAULT = 0,
    /** Self-response touch test.*/
    ARKUI_HIT_TEST_MODE_BLOCK,
    /** Both itself and child nodes respond to the touch test.*/
    ARKUI_HIT_TEST_MODE_TRANSPARENT,
    /** Does not respond to the touch test.*/
    ARKUI_HIT_TEST_MODE_NONE
} ArkUI_HitTestMode;

/**
 * @brief Shadow effect enumerated value.
 *
 * @since 12
 */
typedef enum {
    /** Super small shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_DEFAULT_XS = 0,
    /** Small shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_DEFAULT_SM,
    /** Medium shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_DEFAULT_MD,
    /** Big shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_DEFAULT_LG,
    /** Floating small shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_FLOATING_SM,
    /** Floating medium shadow.*/
    ARKUI_SHADOW_STYLE_OUTER_FLOATING_MD,
} ArkUI_ShadowStyle;

/**
 * @brief Enumerated value of the animation curve.
 *
 * @since 12
 */
typedef enum {
    /** The animation speed is the same from start to finish.*/
    ARKUI_CURVE_LINEAR = 0,
    /** The animation starts at a low speed, then speeds up, and slows down before ending.*/
    ARKUI_CURVE_EASE,
    /** The animation starts at low speed.*/
    ARKUI_CURVE_EASE_IN,
    /** The animation ends at low speed.*/
    ARKUI_CURVE_EASE_OUT,
    /** The animation starts and ends at a low speed.*/
    ARKUI_CURVE_EASE_IN_OUT,
    /** Animation standard curve.*/
    ARKUI_CURVE_FAST_OUT_SLOW_IN,
    /** Animate the deceleration curve.*/
    ARKUI_CURVE_LINEAR_OUT_SLOW_IN,
    /** Animation acceleration curve.*/
    ARKUI_CURVE_FAST_OUT_LINEAR_IN,
    /** Animate the sharp curve.*/
    ARKUI_CURVE_EXTREME_DECELERATION,
    /** Animate the sharp curve.*/
    ARKUI_CURVE_SHARP,
    /** Animated rhythm curve.*/
    ARKUI_CURVE_RHYTHM,
    /** Smooth the animation curve.*/
    ARKUI_CURVE_SMOOTH,
    /** Animated damping curve.*/
    ARKUI_CURVE_FRICTION,
} ArkUI_AnimationCurve;

/**
 * @brief Defines whether text can be copied and pasted.
 *
 * @since 12
 */
typedef enum {
    /** Replication is not supported.*/
    ARKUI_TEXT_COPY_OPTIONS_NONE = 0,
    /** Intra-application replication is supported.*/
    ARKUI_TEXT_COPY_OPTIONS_IN_APP,
    /** Intra-device replication is supported.*/
    ARKUI_TEXT_COPY_OPTIONS_LOCAL_DEVICE,
    /** Cross-device replication is supported.*/
    ARKUI_TEXT_COPY_OPTIONS_CROSS_DEVICE,
} ArkUI_TextCopyOptions;

/**
 * @brief Defines the mode in which the text adapts to the height.
 *
 * @since 12
 */
typedef enum {
    /** Set the text height adaptation mode to MaxLines first.*/
    ARKUI_TEXT_HEIGHT_ADAPTIVE_POLICY_MAX_LINES_FIRST = 0,
    /** Set the adaptive mode of the text height to zoom out the font first.*/
    ARKUI_TEXT_HEIGHT_ADAPTIVE_POLICY_MIN_FONT_SIZE_FIRST,
    /** Set the text height adaptation mode to layout constraint (height) first.*/
    ARKUI_TEXT_HEIGHT_ADAPTIVE_POLICY_LAYOUT_CONSTRAINT_FIRST,
} ArkUI_TextHeightAdaptivePolicy;

/**
 * @brief Define nested scrolling options.
 *
 * @since 12
 */
typedef enum {
    /** Only scrolls by itself and does not associate with the parent component.*/
    ARKUI_SCROLL_NESTED_MODE_SELF_ONLY = 0,
    /** Scroll itself first. After the parent component scrolls to the edge, the parent component scrolls. After the parent component scrolls to the edge
    If the parent component has the edge effect, the parent component triggers the edge effect. Otherwise, the child component triggers the edge effect.*/
    ARKUI_SCROLL_NESTED_MODE_SELF_FIRST,
    /** The parent component scrolls first. After the parent component scrolls to the edge, it scrolls itself.
    After the body scrolls to the edge, if the edge effect exists, the edge effect of the body is triggered. Otherwise, the edge effect of the parent component is triggered.  */
    ARKUI_SCROLL_NESTED_MODE_PARENT_FIRST,
    /** Scrolling itself and parent components at the same time, after both themselves and parent components reach the edge
    If the component has the edge effect, the component triggers the edge effect. Otherwise, the parent component triggers the edge effect.*/
    ARKUI_SCROLL_NESTED_MODE_PARALLEL,
} ArkUI_ScrollNestedMode;

/**
 * @brief Define the edge position to which the scrolling is performed.
 *
 * @since 12
 */
typedef enum {
    /** Upside edge in the vertical direction.*/
    ARKUI_SCROLL_EDGE_TOP = 0,
    /** Vertical center position.*/
    ARKUI_SCROLL_EDGE_CENTER,
    /** Lower vertical edge.*/
    ARKUI_SCROLL_EDGE_BOTTOM,
    /** Baseline position of the text in the cross axis direction.*/
    ARKUI_SCROLL_EDGE_BASELINE,
    /** Start position in the horizontal direction.*/
    ARKUI_SCROLL_EDGE_START,
    /** Center the position in the horizontal direction.*/
    ARKUI_SCROLL_EDGE_MIDDLE,
    /** End position in the horizontal direction.*/
    ARKUI_SCROLL_EDGE_END,
} ArkUI_ScrollEdge;

/**
 * @brief Enumerated value of the mixed mode.
 *
 * @since 12
 */
typedef enum {
    /** Overwrite the upper-layer image with the lower-layer image without performing any blending operation.*/
    ARKUI_BLEND_MODE_NONE,
    /** Clear the target pixel covered by the source pixel to be completely transparent.*/
    ARKUI_BLEND_MODE_CLEAR,
    /** r = s. Only the source pixel is displayed.*/
    ARKUI_BLEND_MODE_SRC,
    /** r = d. Only the target pixel is displayed.*/
    ARKUI_BLEND_MODE_DST,
    /** r = s + (1 - sa) * d. Blend the source pixel based on the transparency and overwrite the target pixel.*/
    ARKUI_BLEND_MODE_SRC_OVER,
    /** r = d + (1 - da) * s. Blend the target pixel based on the transparency and overwrite the source pixel.  */
    ARKUI_BLEND_MODE_DST_OVER,
    /** r = s * da. Only the part of the source pixel that overlaps the destination pixel is displayed.  */
    ARKUI_BLEND_MODE_SRC_IN,
    /** r = d * sa. Only the part that overlaps the source pixel in the target pixel is displayed.  */
    ARKUI_BLEND_MODE_DST_IN,
    /** r = s * (1 - da): Only the part that does not overlap the destination pixel in the source pixel is displayed.  */
    ARKUI_BLEND_MODE_SRC_OUT,
    /** r = d * (1 - sa). Only the part that does not overlap the source pixel in the target pixel is displayed.  */
    ARKUI_BLEND_MODE_DST_OUT,
    /** r = s * da + d * (1 - sa): Draw the source pixel where the source pixel overlaps the target pixel, and draw the target pixel where the source pixel does not overlap the target pixel.
     */
    ARKUI_BLEND_MODE_SRC_ATOP,
    /** r = d * sa + s * (1 - da): Draw the target pixel where the source pixel and the target pixel overlap and draw the source pixel where the source pixel and the target pixel do not overlap.
     */
    ARKUI_BLEND_MODE_DST_ATOP,
    /** r = s * (1 - da) + d * (1 - sa). Only the part that does not overlap the source pixel and the target pixel is displayed.  */
    ARKUI_BLEND_MODE_XOR,
    /** r = min(s + d, 1). Add the source pixel value and the target pixel value and use the result as the new pixel value.*/
    ARKUI_BLEND_MODE_PLUS,
    /** r = s * d: Multiplies the source pixel by the target pixel and uses the result as the new pixel value.*/
    ARKUI_BLEND_MODE_MODULATE,
    /** r = s + d - s * d. Add the pixel values of two images and subtract their product to achieve blending. */
    ARKUI_BLEND_MODE_SCREEN,
    /** Determine whether the MULTIPLY or SCREEN blending mode is used based on the target pixel.*/
    ARKUI_BLEND_MODE_OVERLAY,
    /** rc = s + d - max(s * da, d * sa), ra = kSrcOver. When two colors overlap, the darker color overwrites the lighter color. */
    ARKUI_BLEND_MODE_DARKEN,
    /** rc = s + d - min(s * da, d * sa), ra =
    kSrcOver: Compares the pixels in the source image and the target image, and selects the brightest pixel as the final mixing result.*/
    ARKUI_BLEND_MODE_LIGHTEN,
    /** Make the target pixel brighter to reflect the source pixel.*/
    ARKUI_BLEND_MODE_COLOR_DODGE,
    /** Makes the target pixel darker to reflect the source pixel.*/
    ARKUI_BLEND_MODE_COLOR_BURN,
    /** Determine whether the target pixel becomes brighter or darker based on the value of the source pixel. The source pixel determines whether to use the MULTIPLY blending mode or DISCREEN blending mode.*/
    ARKUI_BLEND_MODE_HARD_LIGHT,
    /** Determine whether the LIGHTEN blending mode or DARKEN blending mode is used based on the source pixel.*/
    ARKUI_BLEND_MODE_SOFT_LIGHT,
    /** rc = s + d - 2 * (min(s * da, d * sa)), ra =
    kSrcOver: Compares the source pixel and target pixel, and subtracts the pixel with the lower brightness from the higher brightness to generate a high contrast effect.*/
    ARKUI_BLEND_MODE_DIFFERENCE,
    /** rc = s + d - two(s * d), ra = kSrcOver. Compare the source pixel with the target pixel, and subtract the lower pixel from the higher pixel to produce a soft effect.
     */
    ARKUI_BLEND_MODE_EXCLUSION,
    /** r = s * (1 - da) + d * (1 - sa) + s * d. Multiplies the source image and the target image to obtain a new image.*/
    ARKUI_BLEND_MODE_MULTIPLY,
    /** Retains the brightness and saturation of the source image, but replaces the hue of the source image with the hue of the target image.*/
    ARKUI_BLEND_MODE_HUE,
    /** Retains the luminance and hue of the target pixel, but replaces the saturation of the target pixel with the saturation of the source pixel.*/
    ARKUI_BLEND_MODE_SATURATION,
    /** Retains the saturation and hue of the source pixel, but replaces the luminance of the source pixel with the luminance of the target pixel.*/
    ARKUI_BLEND_MODE_COLOR,
    /** The hue and saturation of the target pixel are retained, but the luminance of the target pixel is replaced by the luminance of the source pixel.*/
    ARKUI_BLEND_MODE_LUMINOSITY,
} ArkUI_BlendMode;

/**
 * @brief Sets the layout enumeration value in the axis direction of the container element.
 *
 * @since 12
 */
typedef enum {
    /** Layout elements from left to right.*/
    ARKUI_DIRECTION_LTR = 0,
    /** Layout elements from right to left.*/
    ARKUI_DIRECTION_RTL,
    /** Use the default layout direction.*/
    ARKUI_DIRECTION_AUTO = 3,
} ArkUI_Direction;

/**
 * @brief Enumerated value of the foreground color.
 *
 * @since 12
 */
typedef enum {
    /** The foreground color is the inverted color of the control background color.*/
    ARKUI_COLOR_STRATEGY_INVERT = 0,
    /** The control background shadow color is the average color of the control background shadow area.*/
    ARKUI_COLOR_STRATEGY_AVERAGE,
    /** The background shadow color of the control is the main color of the background shadow area of the control.*/
    ARKUI_COLOR_STRATEGY_PRIMARY,
} ArkUI_ColorStrategy;

/**
 * @brief Defines the horizontal alignment mode.
 *
 * @since 12
 */
typedef enum {
    /** Align the start end according to the language direction.*/
    ARKUI_HORIZONTAL_ALIGNMENT_START = 0,
    /** Center alignment, which is the default alignment mode.*/
    ARKUI_HORIZONTAL_ALIGNMENT_CENTER,
    /** Align the end according to the language direction.*/
    ARKUI_HORIZONTAL_ALIGNMENT_END,
} ArkUI_HorizontalAlignment;

/**
 * @brief Defines the animation playback mode.
 *
 * @since 12
 */
typedef enum {
    /** The animation is played in the forward direction.*/
    ARKUI_ANIMATION_PLAY_MODE_NORMAL = 0,
    /** Reversely play the animation.*/
    ARKUI_ANIMATION_PLAY_MODE_REVERSE,
    /** The animation is played backwards for odd times (1, 3, 5...) and forwards for even times (2, 4, 6...).*/
    ARKUI_ANIMATION_PLAY_MODE_ALTERNATE_REVERSE,
} ArkUI_AnimationPlayMode;

/**
 * @brief Defines the image width and height.
 *
 * @since 12
 */
typedef enum {
    /** Retain the scale of the original image.*/
    ARKUI_IMAGE_SIZE_AUTO = 0,
    /** Default value. Use the aspect ratio to zoom out or zoom in so that the two sides of the image are greater than or equal to the display boundary.*/
    ARKUI_IMAGE_SIZE_COVER,
    /** Retain the aspect ratio and zoom in or zoom out the image so that the image can be completely displayed within the display boundary.*/
    ARKUI_IMAGE_SIZE_CONTAIN,
} ArkUI_ImageSize;

/**
 * @brief Defines the color obtaining mode.
 *
 * @since 12
 */
typedef enum {
    /** Do not use the color blur.*/
    ARKUI_ADAPTIVE_COLOR_DEFAULT = 0,
    /** Use the color blur.*/
    ARKUI_ADAPTIVE_COLOR_AVERAGE,
} ArkUI_AdaptiveColor;

/**
 * @brief Defines the dark and light color mode.
 *
 * @since 12
 */
typedef enum {
    /** Follow the dark and light color mode of the system.*/
    ARKUI_COLOR_MODE_SYSTEM = 0,
    /** The light color mode is fixed.*/
    ARKUI_COLOR_MODE_LIGHT,
    /** The dark color mode is fixed.*/
    ARKUI_COLOR_MODE_DARK,
} ArkUI_ColorMode;

/**
 * @brief Defines the background blur style.
 *
 * @since 12
 */
typedef enum {
    /** Light and thin materials are blurred.*/
    ARKUI_BLUR_STYLE_THIN = 0,
    /** The common thickness material is blurred.*/
    ARKUI_BLUR_STYLE_REGULAR,
    /** Thick material blurs.*/
    ARKUI_BLUR_STYLE_THICK,
    /** The depth of field (DOF) is blurred at close distance.*/
    ARKUI_BLUR_STYLE_BACKGROUND_THIN,
    /** The mid-range depth of field is blurred.*/
    ARKUI_BLUR_STYLE_BACKGROUND_REGULAR,
    /** The far-range depth of field is blurred.*/
    ARKUI_BLUR_STYLE_BACKGROUND_THICK,
    /** DFO blurs at the ultra-far distance.*/
    ARKUI_BLUR_STYLE_BACKGROUND_ULTRA_THICK,
    /** Disable blurring.*/
    ARKUI_BLUR_STYLE_NONE,
    /** Component ultra-light and thin material blurs.*/
    ARKUI_BLUR_STYLE_COMPONENT_ULTRA_THIN,
    /** The light and thin material of the component is blurred.*/
    ARKUI_BLUR_STYLE_COMPONENT_THIN,
    /** Common materials of the component are blurred.*/
    ARKUI_BLUR_STYLE_COMPONENT_REGULAR,
    /** The component thickness is blurred.*/
    ARKUI_BLUR_STYLE_COMPONENT_THICK,
    /** The super thick material of the component is blurred.*/
    ARKUI_BLUR_STYLE_COMPONENT_ULTRA_THICK,
} ArkUI_BlurStyle;

/**
 * @brief Defines the image filling effect.
 * ImageSpanAlignment
 * @since 12
 */
typedef enum {
    /** Retain the aspect ratio and zoom in or zoom out the image so that the image can be completely displayed within the display boundary.*/
    ARKUI_OBJECT_FIT_CONTAIN = 0,
    /** Retain the aspect ratio and zoom in or zoom out the image so that both sides of the image are larger than or equal to the display boundary.*/
    ARKUI_OBJECT_FIT_COVER,
    /** Adaptive display.*/
    ARKUI_OBJECT_FIT_AUTO,
    /** The aspect ratio is not retained, so that the image is full of the display boundary.*/
    ARKUI_OBJECT_FIT_FILL,
    /** The aspect ratio is retained, and the image is zoomed out or retained.*/
    ARKUI_OBJECT_FIT_SCALE_DOWN,
    /** Retain the original size.*/
    ARKUI_OBJECT_FIT_NONE,
} ArkUI_ObjectFit;

/**
 * @brief Defines the image interpolation effect.
 *
 * @since 12
 */
typedef enum {
    /** Do not use image interpolation.*/
    ARKUI_IMAGE_INTERPOLATION_NONE = 0,
    /** Low image interpolation.*/
    ARKUI_IMAGE_INTERPOLATION_LOW,
    /** Interpolate the image in.*/
    ARKUI_IMAGE_INTERPOLATION_MEDIUM,
    /** High image interpolation, with the highest interpolation quality.*/
    ARKUI_IMAGE_INTERPOLATION_HIGH,
} ArkUI_ImageInterpolation;

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_TYPE_H
/** @} */
