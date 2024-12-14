#ifndef TARGET_OHOS
#error "PLATFORM OHOS is required"
#endif

#ifndef JSVM_PLATFORM_OHOS_H
#define JSVM_PLATFORM_OHOS_H
#include <string>
#include <unordered_map>

namespace platform {
namespace ohos {
enum ThreadRole : int64_t {
    USER_INTERACT = 0,
    IMPORTANT_DISPLAY = 2,
};

void ReportKeyThread(ThreadRole role);

void SetSecurityMode();

void WriteHisysevent();
} // namespace ohos
} // namespace platform
#endif