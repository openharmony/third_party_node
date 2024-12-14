#ifndef JSVM_PLATFORM_H
#define JSVM_PLATFORM_H
#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef TARGET_OHOS
#include "platform/platform_ohos.h"
#define OHOS_API_CALL(api_call) api_call
#else
#define OHOS_API_CALL(api_call)
#endif

namespace platform {
class OS {
public:
    [[noreturn]] static void Abort();
    static uint64_t GetUid();
    static uint64_t GetPid();
    static uint64_t GetTid();

    enum class LogLevel : uint64_t { LOG_DEBUG = 0, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

    static void Print(LogLevel level, const char* format, ...) __attribute__((format(printf, 2, 3)));
    static void PrintString(LogLevel level, const char* str);
};

class RunJsTrace {
public:
    RunJsTrace(bool RunJs);
    RunJsTrace(const char* name);

    ~RunJsTrace();

private:
    bool RunJs;
};
} // namespace platform

#endif