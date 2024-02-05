

// This file provides preprocessor configuration for libicuuc headers when used in NDK.

#pragma once

#define U_DISABLE_RENAMING 1
#define U_HIDE_DRAFT_API 1
#define U_HIDE_DEPRECATED_API 1
#define U_SHOW_CPLUSPLUS_API 0
#define U_HIDE_INTERNAL_API 1
#define U_HIDE_OBSOLETE_UTF_OLD_H 1

// Set this flag to allow header-only C++ usages when using a C++ compiler
#ifdef __cplusplus
#   ifndef LIBICU_U_SHOW_CPLUSPLUS_API
#       define LIBICU_U_SHOW_CPLUSPLUS_API 1
#   endif
#else
#   undef LIBICU_U_SHOW_CPLUSPLUS_API
#   define LIBICU_U_SHOW_CPLUSPLUS_API 0
#endif
