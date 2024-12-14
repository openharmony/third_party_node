#ifndef JSVM_VERSION_H
#define JSVM_VERSION_H

// JSVM version
#define JSVM_MAJOR_VERSION 1
#define JSVM_MINOR_VERSION 0
#define JSVM_PATCH_VERSION 0

#ifndef DEBUG
#define JSVM_COMPILE_STATUS "Debug"
#else
#define JSVM_COMPILE_STATUS "Release"
#endif

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define JSVM_VERSION_STRING                                                            \
    "v" STRINGIFY(JSVM_MAJOR_VERSION) "." STRINGIFY(JSVM_MINOR_VERSION) "." STRINGIFY( \
        JSVM_PATCH_VERSION) "(" JSVM_COMPILE_STATUS ")"

// JSVM_API_VERSION
#define JSVM_API_VERSION 9

#endif
