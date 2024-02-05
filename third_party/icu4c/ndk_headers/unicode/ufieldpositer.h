// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*****************************************************************************************
* Copyright (C) 2015-2016, International Business Machines
* Corporation and others. All Rights Reserved.
*****************************************************************************************
*/

#ifndef UFIELDPOSITER_H
#define UFIELDPOSITER_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#if U_SHOW_CPLUSPLUS_API
#include "unicode/localpointer.h"
#endif   // U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C API: UFieldPositionIterator for use with format APIs.
 *
 * Usage:
 * ufieldpositer_open creates an empty (unset) UFieldPositionIterator.
 * This can be passed to format functions such as {@link #udat_formatForFields},
 * which will set it to apply to the fields in a particular formatted string.
 * ufieldpositer_next can then be used to iterate over those fields,
 * providing for each field its type (using values that are specific to the
 * particular format type, such as date or number formats), as well as the
 * start and end positions of the field in the formatted string.
 * A given UFieldPositionIterator can be re-used for different format calls;
 * each such call resets it to apply to that format string.
 * ufieldpositer_close should be called to dispose of the UFieldPositionIterator
 * when it is no longer needed.
 *
 * @see FieldPositionIterator
 */

/**
 * Opaque UFieldPositionIterator object for use in C.
 * @stable ICU 55
 */
struct UFieldPositionIterator;
typedef struct UFieldPositionIterator UFieldPositionIterator;  /**< C typedef for struct UFieldPositionIterator. @stable ICU 55 */


#if U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUFieldPositionIteratorPointer
 * "Smart pointer" class, closes a UFieldPositionIterator via ufieldpositer_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 55
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUFieldPositionIteratorPointer, UFieldPositionIterator, ufieldpositer_close);

U_NAMESPACE_END

#endif

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif
