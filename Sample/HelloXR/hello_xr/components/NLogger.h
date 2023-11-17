// Copyright (c) 2017-2020 The Khronos Group Inc
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <exception>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
#include <sstream>

#include <time.h>
#include <string.h>

#include <jni.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <sys/system_properties.h>

namespace vr
{
    enum class Level { Verbose, Info, Warning, Error };

    class NLog {
    public:
        static void SetLevel(Level minSeverity);
        static void Write(Level severity, const std::string& msg);

    public:

    };

    static inline std::string Fmt(const char* fmt, ...)
    {
        va_list vl;
        va_start(vl, fmt);
        int size = std::vsnprintf(nullptr, 0, fmt, vl);
        va_end(vl);

        if (size != -1) {
            std::unique_ptr<char[]> buffer(new char[size + 1]);

            va_start(vl, fmt);
            size = std::vsnprintf(buffer.get(), size + 1, fmt, vl);
            va_end(vl);
            if (size != -1) {
                return std::string(buffer.get(), size);
            }
        }
        return "Format error !";
    }

}
