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

#ifndef SRC_JSVM_UTIL_H_
#define SRC_JSVM_UTIL_H_

#include <array>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// jsvm header
#include "jsvm_dfx.h"
#include "jsvm_log.h"
#include "jsvm_version.h"
#include "platform/platform.h"

// v8 header
#include "v8-inspector.h"
#include "v8-platform.h"
#include "v8-profiler.h"
#include "v8.h"

// Use FORCE_INLINE on functions that have a debug-category-enabled check first
// and then ideally only a single function call following it, to maintain
// performance for the common case (no debugging used).
#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline))
#define COLD_NOINLINE __attribute__((cold, noinline))
#else
#define FORCE_INLINE
#define COLD_NOINLINE
#endif

#ifdef __GNUC__
#define LIKELY(expr) __builtin_expect(!!(expr), 1)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#define LIKELY(expr) expr
#define UNLIKELY(expr) expr
#endif

namespace jsvm {
template<typename T, size_t N>
constexpr size_t ArraySize(const T (&)[N])
{
    return N;
}

[[noreturn]] inline void OnFatalError(const char* location, const char* message)
{
    LOG(Fatal) << "JSVM Fatal Error Position : " << (location ? location : "Unkown");
    LOG(Fatal) << "JSVM Fatal Error Message : " << (message ? message : "Unkown");
    platform::OS::Abort();
}
} // namespace jsvm

namespace v8impl {
template<typename T>
using Persistent = v8::Global<T>;

// Convert a v8::PersistentBase, e.g. v8::Global, to a Local, with an extra
// optimization for strong persistent handles.
class PersistentToLocal {
public:
    // If persistent.IsWeak() == false, then do not call persistent.Reset()
    // while the returned Local<T> is still in scope, it will destroy the
    // reference to the object.
    template<class TypeName>
    static inline v8::Local<TypeName> Default(v8::Isolate* isolate, const v8::PersistentBase<TypeName>& persistent)
    {
        if (persistent.IsWeak()) {
            return PersistentToLocal::Weak(isolate, persistent);
        } else {
            return PersistentToLocal::Strong(persistent);
        }
    }

    // Unchecked conversion from a non-weak Persistent<T> to Local<T>,
    // use with care!
    //
    // Do not call persistent.Reset() while the returned Local<T> is still in
    // scope, it will destroy the reference to the object.
    template<class TypeName>
    static inline v8::Local<TypeName> Strong(const v8::PersistentBase<TypeName>& persistent)
    {
        DCHECK(!persistent.IsWeak());
        return *reinterpret_cast<v8::Local<TypeName>*>(const_cast<v8::PersistentBase<TypeName>*>(&persistent));
    }

    template<class TypeName>
    static inline v8::Local<TypeName> Weak(v8::Isolate* isolate, const v8::PersistentBase<TypeName>& persistent)
    {
        return v8::Local<TypeName>::New(isolate, persistent);
    }
};
} // namespace v8impl
#endif