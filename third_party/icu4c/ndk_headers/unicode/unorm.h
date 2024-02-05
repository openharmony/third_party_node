// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (c) 1996-2016, International Business Machines Corporation
*               and others. All Rights Reserved.
*******************************************************************************
* File unorm.h
*
* Created by: Vladimir Weinstein 12052000
*
* Modification history :
*
* Date        Name        Description
* 02/01/01    synwee      Added normalization quickcheck enum and method.
*/
#ifndef UNORM_H
#define UNORM_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_NORMALIZATION

#include "unicode/uiter.h"
#include "unicode/unorm2.h"

/**
 * \file
 * \brief C API: Unicode Normalization
 *
 * Old Unicode normalization API.
 *
 * This API has been replaced by the unorm2.h API and is only available
 * for backward compatibility. The functions here simply delegate to the
 * unorm2.h functions, for example unorm2_getInstance() and unorm2_normalize().
 * There is one exception: The new API does not provide a replacement for unorm_compare().
 * Its declaration has been moved to unorm2.h.
 *
 * <code>unorm_normalize</code> transforms Unicode text into an equivalent composed or
 * decomposed form, allowing for easier sorting and searching of text.
 * <code>unorm_normalize</code> supports the standard normalization forms described in
 * <a href="http://www.unicode.org/unicode/reports/tr15/" target="unicode">
 * Unicode Standard Annex #15: Unicode Normalization Forms</a>.
 *
 * Characters with accents or other adornments can be encoded in
 * several different ways in Unicode.  For example, take the character A-acute.
 * In Unicode, this can be encoded as a single character (the
 * "composed" form):
 *
 * \code
 *      00C1    LATIN CAPITAL LETTER A WITH ACUTE
 * \endcode
 *
 * or as two separate characters (the "decomposed" form):
 *
 * \code
 *      0041    LATIN CAPITAL LETTER A
 *      0301    COMBINING ACUTE ACCENT
 * \endcode
 *
 * To a user of your program, however, both of these sequences should be
 * treated as the same "user-level" character "A with acute accent".  When you are searching or
 * comparing text, you must ensure that these two sequences are treated
 * equivalently.  In addition, you must handle characters with more than one
 * accent.  Sometimes the order of a character's combining accents is
 * significant, while in other cases accent sequences in different orders are
 * really equivalent.
 *
 * Similarly, the string "ffi" can be encoded as three separate letters:
 *
 * \code
 *      0066    LATIN SMALL LETTER F
 *      0066    LATIN SMALL LETTER F
 *      0069    LATIN SMALL LETTER I
 * \endcode
 *
 * or as the single character
 *
 * \code
 *      FB03    LATIN SMALL LIGATURE FFI
 * \endcode
 *
 * The ffi ligature is not a distinct semantic character, and strictly speaking
 * it shouldn't be in Unicode at all, but it was included for compatibility
 * with existing character sets that already provided it.  The Unicode standard
 * identifies such characters by giving them "compatibility" decompositions
 * into the corresponding semantic characters.  When sorting and searching, you
 * will often want to use these mappings.
 *
 * <code>unorm_normalize</code> helps solve these problems by transforming text into the
 * canonical composed and decomposed forms as shown in the first example above.
 * In addition, you can have it perform compatibility decompositions so that
 * you can treat compatibility characters the same as their equivalents.
 * Finally, <code>unorm_normalize</code> rearranges accents into the proper canonical
 * order, so that you do not have to worry about accent rearrangement on your
 * own.
 *
 * Form FCD, "Fast C or D", is also designed for collation.
 * It allows to work on strings that are not necessarily normalized
 * with an algorithm (like in collation) that works under "canonical closure", i.e., it treats precomposed
 * characters and their decomposed equivalents the same.
 *
 * It is not a normalization form because it does not provide for uniqueness of representation. Multiple strings
 * may be canonically equivalent (their NFDs are identical) and may all conform to FCD without being identical
 * themselves.
 *
 * The form is defined such that the "raw decomposition", the recursive canonical decomposition of each character,
 * results in a string that is canonically ordered. This means that precomposed characters are allowed for as long
 * as their decompositions do not need canonical reordering.
 *
 * Its advantage for a process like collation is that all NFD and most NFC texts - and many unnormalized texts -
 * already conform to FCD and do not need to be normalized (NFD) for such a process. The FCD quick check will
 * return UNORM_YES for most strings in practice.
 *
 * unorm_normalize(UNORM_FCD) may be implemented with UNORM_NFD.
 *
 * For more details on FCD see the collation design document:
 * https://htmlpreview.github.io/?https://github.com/unicode-org/icu-docs/blob/main/design/collation/ICU_collation_design.htm
 *
 * ICU collation performs either NFD or FCD normalization automatically if normalization
 * is turned on for the collator object.
 * Beyond collation and string search, normalized strings may be useful for string equivalence comparisons,
 * transliteration/transcription, unique representations, etc.
 *
 * The W3C generally recommends to exchange texts in NFC.
 * Note also that most legacy character encodings use only precomposed forms and often do not
 * encode any combining marks by themselves. For conversion to such character encodings the
 * Unicode text needs to be normalized to NFC.
 * For more usage examples, see the Unicode Standard Annex.
 */

// Do not conditionalize the following enum with #ifndef U_HIDE_DEPRECATED_API,
// it is needed for layout of Normalizer object.
#ifndef U_FORCE_HIDE_DEPRECATED_API

/**
 * Constants for normalization modes.
 * @deprecated ICU 56 Use unorm2.h instead.
 */
typedef enum {
  /** No decomposition/composition. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_NONE = 1,
  /** Canonical decomposition. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_NFD = 2,
  /** Compatibility decomposition. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_NFKD = 3,
  /** Canonical decomposition followed by canonical composition. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_NFC = 4,
  /** Default normalization. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_DEFAULT = UNORM_NFC,
  /** Compatibility decomposition followed by canonical composition. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_NFKC =5,
  /** "Fast C or D" form. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_FCD = 6,

  /** One more than the highest normalization mode constant. @deprecated ICU 56 Use unorm2.h instead. */
  UNORM_MODE_COUNT
} UNormalizationMode;

#endif  // U_FORCE_HIDE_DEPRECATED_API

#ifndef U_HIDE_DEPRECATED_API

/**
 * Constants for options flags for normalization.
 * Use 0 for default options,
 * including normalization according to the Unicode version
 * that is currently supported by ICU (see u_getUnicodeVersion).
 * @deprecated ICU 56 Use unorm2.h instead.
 */
enum {
    /**
     * Options bit set value to select Unicode 3.2 normalization
     * (except NormalizationCorrections).
     * At most one Unicode version can be selected at a time.
     * @deprecated ICU 56 Use unorm2.h instead.
     */
    UNORM_UNICODE_3_2=0x20
};

/**
 * Lowest-order bit number of unorm_compare() options bits corresponding to
 * normalization options bits.
 *
 * The options parameter for unorm_compare() uses most bits for
 * itself and for various comparison and folding flags.
 * The most significant bits, however, are shifted down and passed on
 * to the normalization implementation.
 * (That is, from unorm_compare(..., options, ...),
 * options>>UNORM_COMPARE_NORM_OPTIONS_SHIFT will be passed on to the
 * internal normalization functions.)
 *
 * @see unorm_compare
 * @deprecated ICU 56 Use unorm2.h instead.
 */
#define UNORM_COMPARE_NORM_OPTIONS_SHIFT 20





#endif  /* U_HIDE_DEPRECATED_API */
#endif /* #if !UCONFIG_NO_NORMALIZATION */
#endif
