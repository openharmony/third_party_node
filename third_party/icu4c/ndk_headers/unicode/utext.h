// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2004-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  utext.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004oct06
*   created by: Markus W. Scherer
*/

#ifndef __UTEXT_H__
#define __UTEXT_H__

/**
 * \file
 * \brief C API: Abstract Unicode Text API
 *
 * The Text Access API provides a means to allow text that is stored in alternative
 * formats to work with ICU services.  ICU normally operates on text that is
 * stored in UTF-16 format, in (UChar *) arrays for the C APIs or as type
 * UnicodeString for C++ APIs.
 *
 * ICU Text Access allows other formats, such as UTF-8 or non-contiguous
 * UTF-16 strings, to be placed in a UText wrapper and then passed to ICU services.
 *
 * There are three general classes of usage for UText:
 *
 *     Application Level Use.  This is the simplest usage - applications would
 *     use one of the utext_open() functions on their input text, and pass
 *     the resulting UText to the desired ICU service.
 *
 *     Second is usage in ICU Services, such as break iteration, that will need to
 *     operate on input presented to them as a UText.  These implementations
 *     will need to use the iteration and related UText functions to gain
 *     access to the actual text.
 *
 *     The third class of UText users are "text providers."  These are the
 *     UText implementations for the various text storage formats.  An application
 *     or system with a unique text storage format can implement a set of
 *     UText provider functions for that format, which will then allow
 *     ICU services to operate on that format.
 *
 *
 * <em>Iterating over text</em>
 *
 * Here is sample code for a forward iteration over the contents of a UText
 *
 * \code
 *    UChar32  c;
 *    UText    *ut = whatever();
 *
 *    for (c=utext_next32From(ut, 0); c>=0; c=utext_next32(ut)) {
 *       // do whatever with the codepoint c here.
 *    }
 * \endcode
 *
 * And here is similar code to iterate in the reverse direction, from the end
 * of the text towards the beginning.
 *
 * \code
 *    UChar32  c;
 *    UText    *ut = whatever();
 *    int      textLength = utext_nativeLength(ut);
 *    for (c=utext_previous32From(ut, textLength); c>=0; c=utext_previous32(ut)) {
 *       // do whatever with the codepoint c here.
 *    }
 * \endcode
 *
 * <em>Characters and Indexing</em>
 *
 * Indexing into text by UText functions is nearly always in terms of the native
 * indexing of the underlying text storage.  The storage format could be UTF-8
 * or UTF-32, for example.  When coding to the UText access API, no assumptions
 * can be made regarding the size of characters, or how far an index
 * may move when iterating between characters.
 *
 * All indices supplied to UText functions are pinned to the length of the
 * text.  An out-of-bounds index is not considered to be an error, but is
 * adjusted to be in the range  0 <= index <= length of input text.
 *
 *
 * When an index position is returned from a UText function, it will be
 * a native index to the underlying text.  In the case of multi-unit characters,
 * it will  always refer to the first position of the character,
 * never to the interior.  This is essentially the same thing as saying that
 * a returned index will always point to a boundary between characters.
 *
 * When a native index is supplied to a UText function, all indices that
 * refer to any part of a multi-unit character representation are considered
 * to be equivalent.  In the case of multi-unit characters, an incoming index
 * will be logically normalized to refer to the start of the character.
 * 
 * It is possible to test whether a native index is on a code point boundary
 * by doing a utext_setNativeIndex() followed by a utext_getNativeIndex().
 * If the index is returned unchanged, it was on a code point boundary.  If
 * an adjusted index is returned, the original index referred to the
 * interior of a character.
 *
 * <em>Conventions for calling UText functions</em>
 *
 * Most UText access functions have as their first parameter a (UText *) pointer,
 * which specifies the UText to be used.  Unless otherwise noted, the
 * pointer must refer to a valid, open UText.  Attempting to
 * use a closed UText or passing a NULL pointer is a programming error and
 * will produce undefined results or NULL pointer exceptions.
 * 
 * The UText_Open family of functions can either open an existing (closed)
 * UText, or heap allocate a new UText.  Here is sample code for creating
 * a stack-allocated UText.
 *
 * \code
 *    char     *s = whatever();  // A utf-8 string 
 *    U_ErrorCode status = U_ZERO_ERROR;
 *    UText    ut = UTEXT_INITIALIZER;
 *    utext_openUTF8(ut, s, -1, &status);
 *    if (U_FAILURE(status)) {
 *        // error handling
 *    } else {
 *        // work with the UText
 *    }
 * \endcode
 *
 * Any existing UText passed to an open function _must_ have been initialized, 
 * either by the UTEXT_INITIALIZER, or by having been originally heap-allocated
 * by an open function.  Passing NULL will cause the open function to
 * heap-allocate and fully initialize a new UText.
 *
 */



#include "unicode/utypes.h"
#include "unicode/uchar.h"
#if U_SHOW_CPLUSPLUS_API
#include "unicode/localpointer.h"
#include "unicode/rep.h"
#include "unicode/unistr.h"
#include "unicode/chariter.h"
#endif


U_CDECL_BEGIN

struct UText;
typedef struct UText UText; /**< C typedef for struct UText. @stable ICU 3.6 */


/***************************************************************************************
 *
 *   C Functions for creating UText wrappers around various kinds of text strings.
 *
 ****************************************************************************************/




#if U_SHOW_CPLUSPLUS_API




#endif


/*****************************************************************************
 *
 *   Functions to work with the text represented by a UText wrapper
 *
 *****************************************************************************/









/**
 * Get the native index of the character preceding the current position.
 * If the iteration position is already at the start of the text, zero
 * is returned.
 * The value returned is the same as that obtained from the following sequence,
 * but without the side effect of changing the iteration position.
 *   
 * \code
 *    UText  *ut = whatever;
 *      ...
 *    utext_previous(ut)
 *    utext_getNativeIndex(ut);
 * \endcode
 *
 * This function is most useful during forwards iteration, where it will get the
 *   native index of the character most recently returned from utext_next().
 *
 * @param ut the text to be accessed
 * @return the native index of the character preceding the current index position,
 *         or zero if the current position is at the start of the text.
 * @stable ICU 3.6
 */
U_CAPI int64_t U_EXPORT2
utext_getPreviousNativeIndex(UText *ut); 


/************************************************************************************
 *
 *  #define inline versions of selected performance-critical text access functions
 *          Caution:  do not use auto increment++ or decrement-- expressions
 *                    as parameters to these macros.
 *
 *          For most use, where there is no extreme performance constraint, the
 *          normal, non-inline functions are a better choice.  The resulting code
 *          will be smaller, and, if the need ever arises, easier to debug.
 *
 *          These are implemented as #defines rather than real functions
 *          because there is no fully portable way to do inline functions in plain C.
 *
 ************************************************************************************/

#ifndef U_HIDE_INTERNAL_API
/**
 * inline version of utext_current32(), for performance-critical situations.
 *
 * Get the code point at the current iteration position of the UText.
 * Returns U_SENTINEL (-1) if the position is at the end of the
 * text.
 *
 * @internal ICU 4.4 technology preview
 */
#define UTEXT_CURRENT32(ut)  \
    ((ut)->chunkOffset < (ut)->chunkLength && ((ut)->chunkContents)[(ut)->chunkOffset]<0xd800 ? \
    ((ut)->chunkContents)[((ut)->chunkOffset)] : utext_current32(ut))
#endif  /* U_HIDE_INTERNAL_API */

/**
 * inline version of utext_next32(), for performance-critical situations.
 *
 * Get the code point at the current iteration position of the UText, and
 * advance the position to the first index following the character.
 * This is a post-increment operation.
 * Returns U_SENTINEL (-1) if the position is at the end of the
 * text.
 *
 * @stable ICU 3.4
 */
#define UTEXT_NEXT32(ut)  \
    ((ut)->chunkOffset < (ut)->chunkLength && ((ut)->chunkContents)[(ut)->chunkOffset]<0xd800 ? \
    ((ut)->chunkContents)[((ut)->chunkOffset)++] : utext_next32(ut))

/**
 * inline version of utext_previous32(), for performance-critical situations.
 *
 *  Move the iterator position to the character (code point) whose
 *  index precedes the current position, and return that character.
 *  This is a pre-decrement operation.
 *  Returns U_SENTINEL (-1) if the position is at the start of the  text.
 *
 * @stable ICU 3.4
 */
#define UTEXT_PREVIOUS32(ut)  \
    ((ut)->chunkOffset > 0 && \
     (ut)->chunkContents[(ut)->chunkOffset-1] < 0xd800 ? \
          (ut)->chunkContents[--((ut)->chunkOffset)]  :  utext_previous32(ut))

/**
  *  inline version of utext_getNativeIndex(), for performance-critical situations.
  *
  * Get the current iterator position, which can range from 0 to 
  * the length of the text.
  * The position is a native index into the input text, in whatever format it
  * may have (possibly UTF-8 for example), and may not always be the same as
  * the corresponding UChar (UTF-16) index.
  * The returned position will always be aligned to a code point boundary. 
  *
  * @stable ICU 3.6
  */
#define UTEXT_GETNATIVEINDEX(ut)                       \
    ((ut)->chunkOffset <= (ut)->nativeIndexingLimit?   \
        (ut)->chunkNativeStart+(ut)->chunkOffset :     \
        (ut)->pFuncs->mapOffsetToNative(ut))    

/**
  *  inline version of utext_setNativeIndex(), for performance-critical situations.
  *
  * Set the current iteration position to the nearest code point
  * boundary at or preceding the specified index.
  * The index is in the native units of the original input text.
  * If the index is out of range, it will be pinned to be within
  * the range of the input text.
  *
  * @stable ICU 3.8
  */
#define UTEXT_SETNATIVEINDEX(ut, ix) UPRV_BLOCK_MACRO_BEGIN { \
    int64_t __offset = (ix) - (ut)->chunkNativeStart; \
    if (__offset>=0 && __offset<(int64_t)(ut)->nativeIndexingLimit && (ut)->chunkContents[__offset]<0xdc00) { \
        (ut)->chunkOffset=(int32_t)__offset; \
    } else { \
        utext_setNativeIndex((ut), (ix)); \
    } \
} UPRV_BLOCK_MACRO_END



/************************************************************************************
 *
 *   Functions related to writing or modifying the text.
 *   These will work only with modifiable UTexts.  Attempting to
 *   modify a read-only UText will return an error status.
 *
 ************************************************************************************/






/**
 * UText provider properties (bit field indexes).
 *
 * @see UText
 * @stable ICU 3.4
 */
enum {
    /**
     * It is potentially time consuming for the provider to determine the length of the text.
     * @stable ICU 3.4
     */
    UTEXT_PROVIDER_LENGTH_IS_EXPENSIVE = 1,
    /**
     * Text chunks remain valid and usable until the text object is modified or
     * deleted, not just until the next time the access() function is called
     * (which is the default).
     * @stable ICU 3.4
     */
    UTEXT_PROVIDER_STABLE_CHUNKS = 2,
    /**
     * The provider supports modifying the text via the replace() and copy()
     * functions.
     * @see Replaceable
     * @stable ICU 3.4
     */
    UTEXT_PROVIDER_WRITABLE = 3,
    /**
     * There is meta data associated with the text.
     * @see Replaceable::hasMetaData()
     * @stable ICU 3.4
     */ 
    UTEXT_PROVIDER_HAS_META_DATA = 4,
    /**
     * Text provider owns the text storage.
     *  Generally occurs as the result of a deep clone of the UText.
     *  When closing the UText, the associated text must
     *  also be closed/deleted/freed/ whatever is appropriate.
     * @stable ICU 3.6
     */
     UTEXT_PROVIDER_OWNS_TEXT = 5
};

/**
  * Function type declaration for UText.clone().
  *
  *  clone a UText.  Much like opening a UText where the source text is itself
  *  another UText.
  *
  *  A deep clone will copy both the UText data structures and the underlying text.
  *  The original and cloned UText will operate completely independently; modifications
  *  made to the text in one will not effect the other.  Text providers are not
  *  required to support deep clones.  The user of clone() must check the status return
  *  and be prepared to handle failures.
  *
  *  A shallow clone replicates only the UText data structures; it does not make
  *  a copy of the underlying text.  Shallow clones can be used as an efficient way to 
  *  have multiple iterators active in a single text string that is not being
  *  modified.
  *
  *  A shallow clone operation must not fail except for truly exceptional conditions such
  *  as memory allocation failures.
  *
  *  A UText and its clone may be safely concurrently accessed by separate threads.
  *  This is true for both shallow and deep clones.
  *  It is the responsibility of the Text Provider to ensure that this thread safety
  *  constraint is met.

  *
  *  @param dest   A UText struct to be filled in with the result of the clone operation,
  *                or NULL if the clone function should heap-allocate a new UText struct.
  *  @param src    The UText to be cloned.
  *  @param deep   true to request a deep clone, false for a shallow clone.
  *  @param status Errors are returned here.  For deep clones, U_UNSUPPORTED_ERROR
  *                should be returned if the text provider is unable to clone the
  *                original text.
  *  @return       The newly created clone, or NULL if the clone operation failed.
  *
  * @stable ICU 3.4
  */
typedef UText * U_CALLCONV
UTextClone(UText *dest, const UText *src, UBool deep, UErrorCode *status);


/**
 * Function type declaration for UText.nativeLength().
 *
 * @param ut the UText to get the length of.
 * @return the length, in the native units of the original text string.
 * @see UText
 * @stable ICU 3.4
 */
typedef int64_t U_CALLCONV
UTextNativeLength(UText *ut);

/**
 * Function type declaration for UText.access().  Get the description of the text chunk
 *  containing the text at a requested native index.  The UText's iteration
 *  position will be left at the requested index.  If the index is out
 *  of bounds, the iteration position will be left at the start or end
 *  of the string, as appropriate.
 *
 *  Chunks must begin and end on code point boundaries.  A single code point
 *  comprised of multiple storage units must never span a chunk boundary.
 *
 *
 * @param ut          the UText being accessed.
 * @param nativeIndex Requested index of the text to be accessed.
 * @param forward     If true, then the returned chunk must contain text
 *                    starting from the index, so that start<=index<limit.
 *                    If false, then the returned chunk must contain text
 *                    before the index, so that start<index<=limit.
 * @return            True if the requested index could be accessed.  The chunk
 *                    will contain the requested text.
 *                    False value if a chunk cannot be accessed
 *                    (the requested index is out of bounds).
 *
 * @see UText
 * @stable ICU 3.4
 */
typedef UBool U_CALLCONV
UTextAccess(UText *ut, int64_t nativeIndex, UBool forward);

/**
 * Function type declaration for UText.extract().
 *
 * Extract text from a UText into a UChar buffer.  The range of text to be extracted
 * is specified in the native indices of the UText provider.  These may not necessarily
 * be UTF-16 indices.
 * <p>
 * The size (number of 16 bit UChars) in the data to be extracted is returned.  The
 * full amount is returned, even when the specified buffer size is smaller.
 * <p>
 * The extracted string will (if you are a user) / must (if you are a text provider)
 * be NUL-terminated if there is sufficient space in the destination buffer.
 *
 * @param  ut            the UText from which to extract data.
 * @param  nativeStart   the native index of the first character to extract.
 * @param  nativeLimit   the native string index of the position following the last
 *                       character to extract.
 * @param  dest          the UChar (UTF-16) buffer into which the extracted text is placed
 * @param  destCapacity  The size, in UChars, of the destination buffer.  May be zero
 *                       for precomputing the required size.
 * @param  status        receives any error status.
 *                       If U_BUFFER_OVERFLOW_ERROR: Returns number of UChars for
 *                       preflighting.
 * @return Number of UChars in the data.  Does not include a trailing NUL.
 *
 * @stable ICU 3.4
 */
typedef int32_t U_CALLCONV
UTextExtract(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             UChar *dest, int32_t destCapacity,
             UErrorCode *status);

/**
 * Function type declaration for UText.replace().
 *
 * Replace a range of the original text with a replacement text.
 *
 * Leaves the current iteration position at the position following the
 *  newly inserted replacement text.
 *
 * This function need only be implemented on UText types that support writing.
 *
 * When using this function, there should be only a single UText opened onto the
 * underlying native text string.  The function is responsible for updating the
 * text chunk within the UText to reflect the updated iteration position,
 * taking into account any changes to the underlying string's structure caused
 * by the replace operation.
 *
 * @param ut               the UText representing the text to be operated on.
 * @param nativeStart      the index of the start of the region to be replaced
 * @param nativeLimit      the index of the character following the region to be replaced.
 * @param replacementText  pointer to the replacement text
 * @param replacmentLength length of the replacement text in UChars, or -1 if the text is NUL terminated.
 * @param status           receives any error status.  Possible errors include
 *                         U_NO_WRITE_PERMISSION
 *
 * @return The signed number of (native) storage units by which
 *         the length of the text expanded or contracted.
 *
 * @stable ICU 3.4
 */
typedef int32_t U_CALLCONV
UTextReplace(UText *ut,
             int64_t nativeStart, int64_t nativeLimit,
             const UChar *replacementText, int32_t replacmentLength,
             UErrorCode *status);

/**
 * Function type declaration for UText.copy().
 *
 * Copy or move a substring from one position to another within the text,
 * while retaining any metadata associated with the text.
 * This function is used to duplicate or reorder substrings.
 * The destination index must not overlap the source range.
 *
 * The text to be copied or moved is inserted at destIndex;
 * it does not replace or overwrite any existing text.
 *
 * This function need only be implemented for UText types that support writing.
 *
 * When using this function, there should be only a single UText opened onto the
 * underlying native text string.  The function is responsible for updating the
 * text chunk within the UText to reflect the updated iteration position,
 * taking into account any changes to the underlying string's structure caused
 * by the replace operation.
 *
 * @param ut           The UText representing the text to be operated on.
 * @param nativeStart  The index of the start of the region to be copied or moved
 * @param nativeLimit  The index of the character following the region to be replaced.
 * @param nativeDest   The destination index to which the source substring is copied or moved.
 * @param move         If true, then the substring is moved, not copied/duplicated.
 * @param status       receives any error status.  Possible errors include U_NO_WRITE_PERMISSION
 *
 * @stable ICU 3.4
 */
typedef void U_CALLCONV
UTextCopy(UText *ut,
          int64_t nativeStart, int64_t nativeLimit,
          int64_t nativeDest,
          UBool move,
          UErrorCode *status);

/**
 * Function type declaration for UText.mapOffsetToNative().
 * Map from the current UChar offset within the current text chunk to
 *  the corresponding native index in the original source text.
 *
 * This is required only for text providers that do not use native UTF-16 indexes.
 *
 * @param ut     the UText.
 * @return Absolute (native) index corresponding to chunkOffset in the current chunk.
 *         The returned native index should always be to a code point boundary.
 *
 * @stable ICU 3.4
 */
typedef int64_t U_CALLCONV
UTextMapOffsetToNative(const UText *ut);

/**
 * Function type declaration for UText.mapIndexToUTF16().
 * Map from a native index to a UChar offset within a text chunk.
 * Behavior is undefined if the native index does not fall within the
 *   current chunk.
 *
 * This function is required only for text providers that do not use native UTF-16 indexes.
 *
 * @param ut          The UText containing the text chunk.
 * @param nativeIndex Absolute (native) text index, chunk->start<=index<=chunk->limit.
 * @return            Chunk-relative UTF-16 offset corresponding to the specified native
 *                    index.
 *
 * @stable ICU 3.4
 */
typedef int32_t U_CALLCONV
UTextMapNativeIndexToUTF16(const UText *ut, int64_t nativeIndex);


/**
 * Function type declaration for UText.utextClose().
 *
 * A Text Provider close function is only required for provider types that make
 *  allocations in their open function (or other functions) that must be 
 *  cleaned when the UText is closed.
 *
 * The allocation of the UText struct itself and any "extra" storage
 * associated with the UText is handled by the common UText implementation
 * and does not require provider specific cleanup in a close function.
 *
 * Most UText provider implementations do not need to implement this function.
 *
 * @param ut A UText object to be closed.
 *
 * @stable ICU 3.4
 */
typedef void U_CALLCONV
UTextClose(UText *ut);


/**
  *   (public)  Function dispatch table for UText.
  *             Conceptually very much like a C++ Virtual Function Table.
  *             This struct defines the organization of the table.
  *             Each text provider implementation must provide an
  *              actual table that is initialized with the appropriate functions
  *              for the type of text being handled.
  *   @stable ICU 3.6
  */
struct UTextFuncs {
    /**
     *   (public)  Function table size, sizeof(UTextFuncs)
     *             Intended for use should the table grow to accommodate added
     *             functions in the future, to allow tests for older format
     *             function tables that do not contain the extensions.
     *
     *             Fields are placed for optimal alignment on
     *             32/64/128-bit-pointer machines, by normally grouping together
     *             4 32-bit fields,
     *             4 pointers,
     *             2 64-bit fields
     *             in sequence.
     *   @stable ICU 3.6
     */
    int32_t       tableSize;

    /**
      *   (private)  Alignment padding.
      *              Do not use, reserved for use by the UText framework only.
      *   @internal
      */
    int32_t       reserved1, /** @internal */ reserved2, /** @internal */ reserved3;


    /**
     * (public) Function pointer for UTextClone
     *
     * @see UTextClone
     * @stable ICU 3.6
     */
    UTextClone *clone;

    /**
     * (public) function pointer for UTextLength
     * May be expensive to compute!
     *
     * @see UTextLength
     * @stable ICU 3.6
     */
    UTextNativeLength *nativeLength;

    /**
     * (public) Function pointer for UTextAccess.
     *
     * @see UTextAccess
     * @stable ICU 3.6
     */
    UTextAccess *access;

    /**
     * (public) Function pointer for UTextExtract.
     *
     * @see UTextExtract
     * @stable ICU 3.6
     */
    UTextExtract *extract;

    /**
     * (public) Function pointer for UTextReplace.
     *
     * @see UTextReplace
     * @stable ICU 3.6
     */
    UTextReplace *replace;

    /**
     * (public) Function pointer for UTextCopy.
     *
     * @see UTextCopy
     * @stable ICU 3.6
     */
    UTextCopy *copy;

    /**
     * (public) Function pointer for UTextMapOffsetToNative.
     *
     * @see UTextMapOffsetToNative
     * @stable ICU 3.6
     */
    UTextMapOffsetToNative *mapOffsetToNative;

    /**
     * (public) Function pointer for UTextMapNativeIndexToUTF16.
     *
     * @see UTextMapNativeIndexToUTF16
     * @stable ICU 3.6
     */
    UTextMapNativeIndexToUTF16 *mapNativeIndexToUTF16;

    /**
     * (public) Function pointer for UTextClose.
      *
      * @see UTextClose
      * @stable ICU 3.6
      */
    UTextClose  *close;

    /**
      * (private)  Spare function pointer
      * @internal
      */
    UTextClose  *spare1;
    
    /**
      * (private)  Spare function pointer
      * @internal
      */
    UTextClose  *spare2;

    /**
      * (private)  Spare function pointer
      * @internal
      */
    UTextClose  *spare3;

};
/**
 * Function dispatch table for UText
 * @see UTextFuncs
 */
typedef struct UTextFuncs UTextFuncs;

 /**
  *   UText struct.  Provides the interface between the generic UText access code
  *                  and the UText provider code that works on specific kinds of
  *                  text  (UTF-8, noncontiguous UTF-16, whatever.)
  *
  *                  Applications that are using predefined types of text providers
  *                  to pass text data to ICU services will have no need to view the
  *                  internals of the UText structs that they open.
  *
  * @stable ICU 3.6
  */
struct UText {
    /**
     *     (private)  Magic.  Used to help detect when UText functions are handed
     *                        invalid or uninitialized UText structs.
     *                        utext_openXYZ() functions take an initialized,
     *                        but not necessarily open, UText struct as an
     *                        optional fill-in parameter.  This magic field
     *                        is used to check for that initialization.
     *                        Text provider close functions must NOT clear
     *                        the magic field because that would prevent
     *                        reuse of the UText struct.
     * @internal
     */
    uint32_t       magic;


    /**
     *     (private)  Flags for managing the allocation and freeing of
     *                memory associated with this UText.
     * @internal
     */
    int32_t        flags;


    /**
      *  Text provider properties.  This set of flags is maintained by the
      *                             text provider implementation.
      *  @stable ICU 3.4
      */
    int32_t         providerProperties;

    /**
     * (public) sizeOfStruct=sizeof(UText)
     * Allows possible backward compatible extension.
     *
     * @stable ICU 3.4
     */
    int32_t         sizeOfStruct;
    
    /* ------ 16 byte alignment boundary -----------  */
    

    /**
      *  (protected) Native index of the first character position following
      *              the current chunk.
      *  @stable ICU 3.6
      */
    int64_t         chunkNativeLimit;

    /**
     *   (protected)  Size in bytes of the extra space (pExtra).
     *  @stable ICU 3.4
     */
    int32_t        extraSize;

    /**
      *    (protected) The highest chunk offset where native indexing and
      *    chunk (UTF-16) indexing correspond.  For UTF-16 sources, value
      *    will be equal to chunkLength.
      *
      *    @stable ICU 3.6
      */
    int32_t         nativeIndexingLimit;

    /* ---- 16 byte alignment boundary------ */
    
    /**
     *  (protected) Native index of the first character in the text chunk.
     *  @stable ICU 3.6
     */
    int64_t         chunkNativeStart;

    /**
     *  (protected) Current iteration position within the text chunk (UTF-16 buffer).
     *  This is the index to the character that will be returned by utext_next32().
     *  @stable ICU 3.6
     */
    int32_t         chunkOffset;

    /**
     *  (protected) Length the text chunk (UTF-16 buffer), in UChars.
     *  @stable ICU 3.6
     */
    int32_t         chunkLength;

    /* ---- 16  byte alignment boundary-- */
    

    /**
     *  (protected)  pointer to a chunk of text in UTF-16 format.
     *  May refer either to original storage of the source of the text, or
     *  if conversion was required, to a buffer owned by the UText.
     *  @stable ICU 3.6
     */
    const UChar    *chunkContents;

     /**
      * (public)     Pointer to Dispatch table for accessing functions for this UText.
      * @stable ICU 3.6
      */
    const UTextFuncs     *pFuncs;

    /**
     *  (protected)  Pointer to additional space requested by the
     *               text provider during the utext_open operation.
     * @stable ICU 3.4
     */
    void          *pExtra;

    /**
     * (protected) Pointer to string or text-containing object or similar.
     * This is the source of the text that this UText is wrapping, in a format
     *  that is known to the text provider functions.
     * @stable ICU 3.4
     */
    const void   *context;

    /* --- 16 byte alignment boundary--- */

    /**
     * (protected) Pointer fields available for use by the text provider.
     * Not used by UText common code.
     * @stable ICU 3.6
     */
    const void     *p; 
    /**
     * (protected) Pointer fields available for use by the text provider.
     * Not used by UText common code.
     * @stable ICU 3.6
     */
    const void     *q;
     /**
     * (protected) Pointer fields available for use by the text provider.
     * Not used by UText common code.
     * @stable ICU 3.6
      */
    const void     *r;

    /**
      *  Private field reserved for future use by the UText framework
      *     itself.  This is not to be touched by the text providers.
      * @internal ICU 3.4
      */
    void           *privP;


    /* --- 16 byte alignment boundary--- */
    

    /**
      * (protected) Integer field reserved for use by the text provider.
      * Not used by the UText framework, or by the client (user) of the UText.
      * @stable ICU 3.4
      */
    int64_t         a;

    /**
      * (protected) Integer field reserved for use by the text provider.
      * Not used by the UText framework, or by the client (user) of the UText.
      * @stable ICU 3.4
      */
    int32_t         b;

    /**
      * (protected) Integer field reserved for use by the text provider.
      * Not used by the UText framework, or by the client (user) of the UText.
      * @stable ICU 3.4
      */
    int32_t         c;

    /*  ---- 16 byte alignment boundary---- */


    /**
      *  Private field reserved for future use by the UText framework
      *     itself.  This is not to be touched by the text providers.
      * @internal ICU 3.4
      */
    int64_t         privA;
    /**
      *  Private field reserved for future use by the UText framework
      *     itself.  This is not to be touched by the text providers.
      * @internal ICU 3.4
      */
    int32_t         privB;
    /**
      *  Private field reserved for future use by the UText framework
      *     itself.  This is not to be touched by the text providers.
      * @internal ICU 3.4
      */
    int32_t         privC;
};


// do not use #ifndef U_HIDE_INTERNAL_API around the following!
/**
  * @internal
  *  Value used to help identify correctly initialized UText structs.
  *  Note:  must be publicly visible so that UTEXT_INITIALIZER can access it.
  */
enum {
    UTEXT_MAGIC = 0x345ad82c
};

/**
 * initializer to be used with local (stack) instances of a UText
 *  struct.  UText structs must be initialized before passing
 *  them to one of the utext_open functions.
 *
 * @stable ICU 3.6
 */
#define UTEXT_INITIALIZER {                                        \
                  UTEXT_MAGIC,          /* magic                */ \
                  0,                    /* flags                */ \
                  0,                    /* providerProps        */ \
                  sizeof(UText),        /* sizeOfStruct         */ \
                  0,                    /* chunkNativeLimit     */ \
                  0,                    /* extraSize            */ \
                  0,                    /* nativeIndexingLimit  */ \
                  0,                    /* chunkNativeStart     */ \
                  0,                    /* chunkOffset          */ \
                  0,                    /* chunkLength          */ \
                  NULL,                 /* chunkContents        */ \
                  NULL,                 /* pFuncs               */ \
                  NULL,                 /* pExtra               */ \
                  NULL,                 /* context              */ \
                  NULL, NULL, NULL,     /* p, q, r              */ \
                  NULL,                 /* privP                */ \
                  0, 0, 0,              /* a, b, c              */ \
                  0, 0, 0               /* privA,B,C,           */ \
                  }


U_CDECL_END


#if U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUTextPointer
 * "Smart pointer" class, closes a UText via utext_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUTextPointer, UText, utext_close);

U_NAMESPACE_END

#endif


#endif
