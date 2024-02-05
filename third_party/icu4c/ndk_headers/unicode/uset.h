// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002mar07
*   created by: Markus W. Scherer
*
*   C version of UnicodeSet.
*/


/**
 * \file
 * \brief C API: Unicode Set
 *
 * <p>This is a C wrapper around the C++ UnicodeSet class.</p>
 */

#ifndef __USET_H__
#define __USET_H__

#include "unicode/utypes.h"
#include "unicode/uchar.h"

#if U_SHOW_CPLUSPLUS_API
#include "unicode/localpointer.h"
#endif   // U_SHOW_CPLUSPLUS_API

#ifndef USET_DEFINED

#ifndef U_IN_DOXYGEN
#define USET_DEFINED
#endif
/**
 * USet is the C API type corresponding to C++ class UnicodeSet.
 * Use the uset_* API to manipulate.  Create with
 * uset_open*, and destroy with uset_close.
 * @stable ICU 2.4
 */
typedef struct USet USet;
#endif

/**
 * Bitmask values to be passed to uset_openPatternOptions() or
 * uset_applyPattern() taking an option parameter.
 * @stable ICU 2.4
 */
enum {
    /**
     * Ignore white space within patterns unless quoted or escaped.
     * @stable ICU 2.4
     */
    USET_IGNORE_SPACE = 1,  

    /**
     * Enable case insensitive matching.  E.g., "[ab]" with this flag
     * will match 'a', 'A', 'b', and 'B'.  "[^ab]" with this flag will
     * match all except 'a', 'A', 'b', and 'B'. This performs a full
     * closure over case mappings, e.g. U+017F for s.
     *
     * The resulting set is a superset of the input for the code points but
     * not for the strings.
     * It performs a case mapping closure of the code points and adds
     * full case folding strings for the code points, and reduces strings of
     * the original set to their full case folding equivalents.
     *
     * This is designed for case-insensitive matches, for example
     * in regular expressions. The full code point case closure allows checking of
     * an input character directly against the closure set.
     * Strings are matched by comparing the case-folded form from the closure
     * set with an incremental case folding of the string in question.
     *
     * The closure set will also contain single code points if the original
     * set contained case-equivalent strings (like U+00DF for "ss" or "Ss" etc.).
     * This is not necessary (that is, redundant) for the above matching method
     * but results in the same closure sets regardless of whether the original
     * set contained the code point or a string.
     *
     * @stable ICU 2.4
     */
    USET_CASE_INSENSITIVE = 2,  

    /**
     * Enable case insensitive matching.  E.g., "[ab]" with this flag
     * will match 'a', 'A', 'b', and 'B'.  "[^ab]" with this flag will
     * match all except 'a', 'A', 'b', and 'B'. This adds the lower-,
     * title-, and uppercase mappings as well as the case folding
     * of each existing element in the set.
     * @stable ICU 3.2
     */
    USET_ADD_CASE_MAPPINGS = 4
};

/**
 * Argument values for whether span() and similar functions continue while
 * the current character is contained vs. not contained in the set.
 *
 * The functionality is straightforward for sets with only single code points,
 * without strings (which is the common case):
 * - USET_SPAN_CONTAINED and USET_SPAN_SIMPLE work the same.
 * - USET_SPAN_CONTAINED and USET_SPAN_SIMPLE are inverses of USET_SPAN_NOT_CONTAINED.
 * - span() and spanBack() partition any string the same way when
 *   alternating between span(USET_SPAN_NOT_CONTAINED) and
 *   span(either "contained" condition).
 * - Using a complemented (inverted) set and the opposite span conditions
 *   yields the same results.
 *
 * When a set contains multi-code point strings, then these statements may not
 * be true, depending on the strings in the set (for example, whether they
 * overlap with each other) and the string that is processed.
 * For a set with strings:
 * - The complement of the set contains the opposite set of code points,
 *   but the same set of strings.
 *   Therefore, complementing both the set and the span conditions
 *   may yield different results.
 * - When starting spans at different positions in a string
 *   (span(s, ...) vs. span(s+1, ...)) the ends of the spans may be different
 *   because a set string may start before the later position.
 * - span(USET_SPAN_SIMPLE) may be shorter than
 *   span(USET_SPAN_CONTAINED) because it will not recursively try
 *   all possible paths.
 *   For example, with a set which contains the three strings "xy", "xya" and "ax",
 *   span("xyax", USET_SPAN_CONTAINED) will return 4 but
 *   span("xyax", USET_SPAN_SIMPLE) will return 3.
 *   span(USET_SPAN_SIMPLE) will never be longer than
 *   span(USET_SPAN_CONTAINED).
 * - With either "contained" condition, span() and spanBack() may partition
 *   a string in different ways.
 *   For example, with a set which contains the two strings "ab" and "ba",
 *   and when processing the string "aba",
 *   span() will yield contained/not-contained boundaries of { 0, 2, 3 }
 *   while spanBack() will yield boundaries of { 0, 1, 3 }.
 *
 * Note: If it is important to get the same boundaries whether iterating forward
 * or backward through a string, then either only span() should be used and
 * the boundaries cached for backward operation, or an ICU BreakIterator
 * could be used.
 *
 * Note: Unpaired surrogates are treated like surrogate code points.
 * Similarly, set strings match only on code point boundaries,
 * never in the middle of a surrogate pair.
 * Illegal UTF-8 sequences are treated like U+FFFD.
 * When processing UTF-8 strings, malformed set strings
 * (strings with unpaired surrogates which cannot be converted to UTF-8)
 * are ignored.
 *
 * @stable ICU 3.8
 */
typedef enum USetSpanCondition {
    /**
     * Continues a span() while there is no set element at the current position.
     * Increments by one code point at a time.
     * Stops before the first set element (character or string).
     * (For code points only, this is like while contains(current)==false).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of characters that are not in the set,
     * and none of its strings overlap with the span.
     *
     * @stable ICU 3.8
     */
    USET_SPAN_NOT_CONTAINED = 0,
    /**
     * Spans the longest substring that is a concatenation of set elements (characters or strings).
     * (For characters only, this is like while contains(current)==true).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of set elements (characters or strings) that are in the set.
     *
     * If a set contains strings, then the span will be the longest substring for which there
     * exists at least one non-overlapping concatenation of set elements (characters or strings).
     * This is equivalent to a POSIX regular expression for <code>(OR of each set element)*</code>.
     * (Java/ICU/Perl regex stops at the first match of an OR.)
     *
     * @stable ICU 3.8
     */
    USET_SPAN_CONTAINED = 1,
    /**
     * Continues a span() while there is a set element at the current position.
     * Increments by the longest matching element at each position.
     * (For characters only, this is like while contains(current)==true).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of set elements (characters or strings) that are in the set.
     *
     * If a set only contains single characters, then this is the same
     * as USET_SPAN_CONTAINED.
     *
     * If a set contains strings, then the span will be the longest substring
     * with a match at each position with the longest single set element (character or string).
     *
     * Use this span condition together with other longest-match algorithms,
     * such as ICU converters (ucnv_getUnicodeSet()).
     *
     * @stable ICU 3.8
     */
    USET_SPAN_SIMPLE = 2,
#ifndef U_HIDE_DEPRECATED_API
    /**
     * One more than the last span condition.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    USET_SPAN_CONDITION_COUNT
#endif  // U_HIDE_DEPRECATED_API
} USetSpanCondition;

enum {
    /**
     * Capacity of USerializedSet::staticArray.
     * Enough for any single-code point set.
     * Also provides padding for nice sizeof(USerializedSet).
     * @stable ICU 2.4
     */
    USET_SERIALIZED_STATIC_ARRAY_CAPACITY=8
};

/**
 * A serialized form of a Unicode set.  Limited manipulations are
 * possible directly on a serialized set.  See below.
 * @stable ICU 2.4
 */
typedef struct USerializedSet {
    /**
     * The serialized Unicode Set.
     * @stable ICU 2.4
     */
    const uint16_t *array;
    /**
     * The length of the array that contains BMP characters.
     * @stable ICU 2.4
     */
    int32_t bmpLength;
    /**
     * The total length of the array.
     * @stable ICU 2.4
     */
    int32_t length;
    /**
     * A small buffer for the array to reduce memory allocations.
     * @stable ICU 2.4
     */
    uint16_t staticArray[USET_SERIALIZED_STATIC_ARRAY_CAPACITY];
} USerializedSet;

/*********************************************************************
 * USet API
 *********************************************************************/



#if U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUSetPointer
 * "Smart pointer" class, closes a USet via uset_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUSetPointer, USet, uset_close);

U_NAMESPACE_END

#endif



































/*********************************************************************
 * Serialized set API
 *********************************************************************/





#endif
