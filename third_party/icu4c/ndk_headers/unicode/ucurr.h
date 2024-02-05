// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ifndef _UCURR_H_
#define _UCURR_H_

#include "unicode/utypes.h"
#include "unicode/uenum.h"

/**
 * \file 
 * \brief C API: Encapsulates information about a currency.
 *
 * The ucurr API encapsulates information about a currency, as defined by
 * ISO 4217.  A currency is represented by a 3-character string
 * containing its ISO 4217 code.  This API can return various data
 * necessary the proper display of a currency:
 *
 * <ul><li>A display symbol, for a specific locale
 * <li>The number of fraction digits to display
 * <li>A rounding increment
 * </ul>
 *
 * The <tt>DecimalFormat</tt> class uses these data to display
 * currencies.
 * @author Alan Liu
 * @since ICU 2.2
 */

#if !UCONFIG_NO_FORMATTING

/**
 * Currency Usage used for Decimal Format
 * @stable ICU 54
 */
enum UCurrencyUsage {
    /**
     * a setting to specify currency usage which determines currency digit
     * and rounding for standard usage, for example: "50.00 NT$"
     * used as DEFAULT value
     * @stable ICU 54
     */
    UCURR_USAGE_STANDARD=0,
    /**
     * a setting to specify currency usage which determines currency digit
     * and rounding for cash usage, for example: "50 NT$"
     * @stable ICU 54
     */
    UCURR_USAGE_CASH=1,
#ifndef U_HIDE_DEPRECATED_API
    /**
     * One higher than the last enum UCurrencyUsage constant.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    UCURR_USAGE_COUNT=2
#endif  // U_HIDE_DEPRECATED_API
};
/** Currency Usage used for Decimal Format */
typedef enum UCurrencyUsage UCurrencyUsage; 

/**
 * Selector constants for ucurr_getName().
 *
 * @see ucurr_getName
 * @stable ICU 2.6
 */
typedef enum UCurrNameStyle {
    /**
     * Selector for ucurr_getName indicating a symbolic name for a
     * currency, such as "$" for USD.
     * @stable ICU 2.6
     */
    UCURR_SYMBOL_NAME,

    /**
     * Selector for ucurr_getName indicating the long name for a
     * currency, such as "US Dollar" for USD.
     * @stable ICU 2.6
     */
    UCURR_LONG_NAME,

    /**
     * Selector for getName() indicating the narrow currency symbol.
     * The narrow currency symbol is similar to the regular currency
     * symbol, but it always takes the shortest form: for example,
     * "$" instead of "US$" for USD in en-CA.
     *
     * @stable ICU 61
     */
    UCURR_NARROW_SYMBOL_NAME,

    /**
     * Selector for getName() indicating the formal currency symbol.
     * The formal currency symbol is similar to the regular currency
     * symbol, but it always takes the form used in formal settings
     * such as banking; for example, "NT$" instead of "$" for TWD in zh-TW.
     *
     * @stable ICU 68
     */
    UCURR_FORMAL_SYMBOL_NAME,

    /**
     * Selector for getName() indicating the variant currency symbol.
     * The variant symbol for a currency is an alternative symbol
     * that is not necessarily as widely used as the regular symbol.
     *
     * @stable ICU 68
     */
    UCURR_VARIANT_SYMBOL_NAME
    
} UCurrNameStyle;

#if !UCONFIG_NO_SERVICE
/**
 * @stable ICU 2.6
 */
typedef const void* UCurrRegistryKey;

#endif /* UCONFIG_NO_SERVICE */





/**
 * Selector constants for ucurr_openCurrencies().
 *
 * @see ucurr_openCurrencies
 * @stable ICU 3.2
 */
typedef enum UCurrCurrencyType {
    /**
     * Select all ISO-4217 currency codes.
     * @stable ICU 3.2
     */
    UCURR_ALL = INT32_MAX,
    /**
     * Select only ISO-4217 commonly used currency codes.
     * These currencies can be found in common use, and they usually have
     * bank notes or coins associated with the currency code.
     * This does not include fund codes, precious metals and other
     * various ISO-4217 codes limited to special financial products.
     * @stable ICU 3.2
     */
    UCURR_COMMON = 1,
    /**
     * Select ISO-4217 uncommon currency codes.
     * These codes respresent fund codes, precious metals and other
     * various ISO-4217 codes limited to special financial products.
     * A fund code is a monetary resource associated with a currency.
     * @stable ICU 3.2
     */
    UCURR_UNCOMMON = 2,
    /**
     * Select only deprecated ISO-4217 codes.
     * These codes are no longer in general public use.
     * @stable ICU 3.2
     */
    UCURR_DEPRECATED = 4,
    /**
     * Select only non-deprecated ISO-4217 codes.
     * These codes are in general public use.
     * @stable ICU 3.2
     */
    UCURR_NON_DEPRECATED = 8
} UCurrCurrencyType;

/** 
 * Finds the number of valid currency codes for the
 * given locale and date.
 * @param locale the locale for which to retrieve the
 *               currency count.
 * @param date   the date for which to retrieve the
 *               currency count for the given locale.
 * @param ec     error code
 * @return       the number of currency codes for the
 *               given locale and date.  If 0, currency
 *               codes couldn't be found for the input
 *               values are invalid.
 * @stable ICU 4.0
 */
U_CAPI int32_t U_EXPORT2
ucurr_countCurrencies(const char* locale, 
                 UDate date, 
                 UErrorCode* ec); 

/** 
 * Finds a currency code for the given locale and date 
 * @param locale the locale for which to retrieve a currency code.  
 *               Currency can be specified by the "currency" keyword 
 *               in which case it overrides the default currency code 
 * @param date   the date for which to retrieve a currency code for 
 *               the given locale. 
 * @param index  the index within the available list of currency codes
 *               for the given locale on the given date.
 * @param buff   fill in buffer. Can be NULL for preflighting. 
 * @param buffCapacity capacity of the fill in buffer. Can be 0 for 
 *               preflighting. If it is non-zero, the buff parameter 
 *               must not be NULL. 
 * @param ec     error code 
 * @return       length of the currency string. It should always be 3. 
 *               If 0, currency couldn't be found or the input values are  
 *               invalid.  
 * @stable ICU 4.0 
 */ 
U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(const char* locale, 
                UDate date, 
                int32_t index,
                UChar* buff, 
                int32_t buffCapacity, 
                UErrorCode* ec); 

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif
