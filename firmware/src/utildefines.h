// Copyright (c) 2017, Sergey Sharybin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Sergey Sharybin (sergey.vfx@gmail.com)

#ifndef _UTILDEFINES_H
#define _UTILDEFINES_H

#include <string.h>

#define STREQ(a, b) (strcmp((a), (b)) == 0)

////////////////////////////////////////////////////////////////////////////////
// Helper macros for fancier state printing to the console

#define APP_PRINT_SEVERITY(severity, prefix, format, ...) \
  SYS_CONSOLE_PRINT(severity prefix format, ##__VA_ARGS__)
#define APP_MESSAGE_SEVERITY(severity, prefix, message) \
  SYS_CONSOLE_MESSAGE(severity prefix message)

// Regular print / message
#define APP_PRINT(prefix, format, ...) \
  APP_PRINT_SEVERITY("", prefix, format, ##__VA_ARGS__)
#define APP_MESSAGE(prefix, message) APP_MESSAGE_SEVERITY("", prefix, message)

// Warning print / message
#define APP_WARNING_PRINT(prefix, format, ...) \
  APP_PRINT_SEVERITY("[WARNING] ", prefix, format, ##__VA_ARGS__)
#define APP_WARNING_MESSAGE(prefix, message) \
  APP_MESSAGE_SEVERITY("[WARNING] ", prefix, message)

// Error print / message
#define APP_ERROR_PRINT(prefix, format, ...) \
  APP_PRINT_SEVERITY("[ERROR] ", prefix, format, ##__VA_ARGS__)
#define APP_ERROR_MESSAGE(prefix, message) \
  APP_MESSAGE_SEVERITY("[ERROR] ", prefix, message)

// Debug print / message
#define APP_DEBUG_PRINT(prefix, format, ...) \
  SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "[DEBUG] "  prefix format, ##__VA_ARGS__)
#define APP_DEBUG_MESSAGE(prefix, message) \
  SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "[DEBUG] " prefix message)

#endif  // _UTILDEFINES_H
