// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
* Copyright (C) 2013-2014, International Business Machines Corporation and others.
* All Rights Reserved.
********************************************************************************
*
* File UFORMATTABLE.H
*
* Modification History:
*
*   Date        Name        Description
*   2013 Jun 7  srl         New
********************************************************************************
*/

/**
 * \file
 * \brief C API: UFormattable is a thin wrapper for primitive types used for formatting and parsing.
 *
 * This is a C interface to the icu::Formattable class. Static functions on this class convert
 * to and from this interface (via reinterpret_cast).  Note that Formattables (and thus UFormattables)
 * are mutable, and many operations (even getters) may actually modify the internal state. For this
 * reason, UFormattables are not thread safe, and should not be shared between threads.
 *
 * See {@link unum_parseToUFormattable} for example code.
 */

#ifndef UFORMATTABLE_H
#define UFORMATTABLE_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#if U_SHOW_CPLUSPLUS_API
#include "unicode/localpointer.h"
#endif   // U_SHOW_CPLUSPLUS_API

/**
 * Enum designating the type of a UFormattable instance.
 * Practically, this indicates which of the getters would return without conversion
 * or error.
 * @see icu::Formattable::Type
 * @stable ICU 52
 */
typedef enum UFormattableType {
  UFMT_DATE = 0, /**< ufmt_getDate() will return without conversion. @see ufmt_getDate*/
  UFMT_DOUBLE,   /**< ufmt_getDouble() will return without conversion.  @see ufmt_getDouble*/
  UFMT_LONG,     /**< ufmt_getLong() will return without conversion. @see ufmt_getLong */
  UFMT_STRING,   /**< ufmt_getUChars() will return without conversion.  @see ufmt_getUChars*/
  UFMT_ARRAY,    /**< ufmt_countArray() and ufmt_getArray() will return the value.  @see ufmt_getArrayItemByIndex */
  UFMT_INT64,    /**< ufmt_getInt64() will return without conversion. @see ufmt_getInt64 */
  UFMT_OBJECT,   /**< ufmt_getObject() will return without conversion.  @see ufmt_getObject*/
#ifndef U_HIDE_DEPRECATED_API
    /**
     * One more than the highest normal UFormattableType value.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    UFMT_COUNT
#endif  /* U_HIDE_DEPRECATED_API */
} UFormattableType;


/**
 * Opaque type representing various types of data which may be used for formatting
 * and parsing operations.
 * @see icu::Formattable
 * @stable ICU 52
 */
typedef void *UFormattable;

#if U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUFormattablePointer
 * "Smart pointer" class, closes a UFormattable via ufmt_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 52
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUFormattablePointer, UFormattable, ufmt_close);

U_NAMESPACE_END

#endif








#endif

#endif
