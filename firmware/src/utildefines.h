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

#define APP_ERROR_MESSAGE(prefix, message)  \
    SYS_CONSOLE_MESSAGE(prefix message)

#define APP_DEBUG_MESSAGE(prefix, message)  \
    SYS_DEBUG_MESSAGE(0, "[DEBUG] " prefix message)

#define APP_DEBUG_PRINT(prefix, format, ...)  \
    SYS_DEBUG_PRINT(0, "[DEBUG] " prefix format, ##__VA_ARGS__)

#endif  // _UTILDEFINES_H
