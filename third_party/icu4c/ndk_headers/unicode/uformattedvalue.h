// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __UFORMATTEDVALUE_H__
#define __UFORMATTEDVALUE_H__

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/ufieldpositer.h"

/**
 * \file
 * \brief C API: Abstract operations for localized strings.
 * 
 * This file contains declarations for classes that deal with formatted strings. A number
 * of APIs throughout ICU use these classes for expressing their localized output.
 */


/**
 * All possible field categories in ICU. Every entry in this enum corresponds
 * to another enum that exists in ICU.
 * 
 * In the APIs that take a UFieldCategory, an int32_t type is used. Field
 * categories having any of the top four bits turned on are reserved as
 * private-use for external APIs implementing FormattedValue. This means that
 * categories 2^28 and higher or below zero (with the highest bit turned on)
 * are private-use and will not be used by ICU in the future.
 *
 * @stable ICU 64
 */
typedef enum UFieldCategory {
    /**
     * For an undefined field category.
     * 
     * @stable ICU 64
     */
    UFIELD_CATEGORY_UNDEFINED = 0,

    /**
     * For fields in UDateFormatField (udat.h), from ICU 3.0.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_DATE,

    /**
     * For fields in UNumberFormatFields (unum.h), from ICU 49.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_NUMBER,

    /**
     * For fields in UListFormatterField (ulistformatter.h), from ICU 63.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_LIST,

    /**
     * For fields in URelativeDateTimeFormatterField (ureldatefmt.h), from ICU 64.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_RELATIVE_DATETIME,

    /**
     * Reserved for possible future fields in UDateIntervalFormatField.
     *
     * @internal
     */
    UFIELD_CATEGORY_DATE_INTERVAL,

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    UFIELD_CATEGORY_COUNT,
#endif  /* U_HIDE_INTERNAL_API */

    /**
     * Category for spans in a list.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_LIST_SPAN = 0x1000 + UFIELD_CATEGORY_LIST,

    /**
     * Category for spans in a date interval.
     *
     * @stable ICU 64
     */
    UFIELD_CATEGORY_DATE_INTERVAL_SPAN = 0x1000 + UFIELD_CATEGORY_DATE_INTERVAL,

    /**
     * Category for spans in a number range.
     *
     * @stable ICU 69
     */
    UFIELD_CATEGORY_NUMBER_RANGE_SPAN = 0x1000 + UFIELD_CATEGORY_NUMBER,

} UFieldCategory;


struct UConstrainedFieldPosition;
/**
 * Represents a span of a string containing a given field.
 *
 * This struct differs from UFieldPosition in the following ways:
 *
 *   1. It has information on the field category.
 *   2. It allows you to set constraints to use when iterating over field positions.
 *   3. It is used for the newer FormattedValue APIs.
 *
 * @stable ICU 64
 */
typedef struct UConstrainedFieldPosition UConstrainedFieldPosition;












struct UFormattedValue;
/**
 * An abstract formatted value: a string with associated field attributes.
 * Many formatters format to types compatible with UFormattedValue.
 *
 * @stable ICU 64
 */
typedef struct UFormattedValue UFormattedValue;


#if U_SHOW_CPLUSPLUS_API
U_NAMESPACE_BEGIN

/**
 * \class LocalUConstrainedFieldPositionPointer
 * "Smart pointer" class; closes a UConstrainedFieldPosition via ucfpos_close().
 * For most methods see the LocalPointerBase base class.
 *
 * Usage:
 *
 *     LocalUConstrainedFieldPositionPointer ucfpos(ucfpos_open(ec));
 *     // no need to explicitly call ucfpos_close()
 *
 * @stable ICU 64
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUConstrainedFieldPositionPointer,
    UConstrainedFieldPosition,
    ucfpos_close);

U_NAMESPACE_END
#endif // U_SHOW_CPLUSPLUS_API


#endif /* #if !UCONFIG_NO_FORMATTING */
#endif // __UFORMATTEDVALUE_H__
