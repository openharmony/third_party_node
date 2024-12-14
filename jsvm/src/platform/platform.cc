#include "platform/platform.h"

#include <stdarg.h>

#include "unistd.h"

namespace platform {
void OS::Abort()
{
    std::abort();
}

uint64_t OS::GetUid()
{
    return static_cast<uint64_t>(getuid());
}

uint64_t OS::GetPid()
{
    return static_cast<uint64_t>(getppid());
}

uint64_t OS::GetTid()
{
    return static_cast<uint64_t>(gettid());
}

void OS::PrintString(OS::LogLevel level, const char* str)
{
    (void)level;
    printf("%s", str);
}

void VPrint(const char* format, va_list args)
{
    vprintf(format, args);
}

void VPrintError(const char* format, va_list args)
{
    vfprintf(stderr, format, args);
}

void OS::Print(OS::LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    if (static_cast<uint64_t>(level) > static_cast<uint64_t>(OS::LogLevel::LOG_WARN)) {
        VPrintError(format, args);
    } else {
        VPrint(format, args);
    }
    va_end(args);
}


} // namespace platform