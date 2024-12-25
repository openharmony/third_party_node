/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JSVM_DFX_H
#define JSVM_DFX_H

#include <cassert>

#include "v8.h"

#define ERROR_AND_ABORT(expr)                                                     \
    do {                                                                          \
        /* Make sure that this struct does not end up in inline code, but      */ \
        /* rather in a read-only data section when modifying this code.        */ \
        abort();                                                                  \
    } while (0)

#define UNREACHABLE(...) ERROR_AND_ABORT("Unreachable code reached" __VA_OPT__(": ") __VA_ARGS__)

#define CHECK(expr)                \
    do {                           \
        if (UNLIKELY(!(expr))) {   \
            ERROR_AND_ABORT(expr); \
        }                          \
    } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_NULL(val) CHECK((val) == nullptr)
#define CHECK_NOT_NULL(val) CHECK((val) != nullptr)
#define CHECK_IMPLIES(a, b) CHECK(!(a) || (b))

#ifdef DEBUG
#define DCHECK(expr) CHECK(expr)
#define DCHECK_EQ(a, b) CHECK((a) == (b))
#define DCHECK_GE(a, b) CHECK((a) >= (b))
#define DCHECK_GT(a, b) CHECK((a) > (b))
#define DCHECK_LE(a, b) CHECK((a) <= (b))
#define DCHECK_LT(a, b) CHECK((a) < (b))
#define DCHECK_NE(a, b) CHECK((a) != (b))
#define DCHECK_NULL(val) CHECK((val) == nullptr)
#define DCHECK_NOT_NULL(val) CHECK((val) != nullptr)
#define DCHECK_IMPLIES(a, b) CHECK(!(a) || (b))
#else
#define DCHECK(expr)
#define DCHECK_EQ(a, b)
#define DCHECK_GE(a, b)
#define DCHECK_GT(a, b)
#define DCHECK_LE(a, b)
#define DCHECK_LT(a, b)
#define DCHECK_NE(a, b)
#define DCHECK_NULL(val)
#define DCHECK_NOT_NULL(val)
#define DCHECK_IMPLIES(a, b)
#endif

namespace jsvm {
class DebugSealHandleScope {
public:
    explicit inline DebugSealHandleScope(v8::Isolate* isolate = nullptr)
#ifdef DEBUG
        : sealHandleScope(isolate != nullptr ? isolate : v8::Isolate::GetCurrent())
#endif
    {}

private:
#ifdef DEBUG
    v8::SealHandleScope sealHandleScope;
#endif
};
} // namespace jsvm

#endif