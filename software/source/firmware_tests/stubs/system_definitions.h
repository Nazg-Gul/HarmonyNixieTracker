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

#ifndef _SYS_DEFINITIONS_STUB_H_
#define _SYS_DEFINITIONS_STUB_H_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

enum SysErrorType {
  SYS_ERROR_DEBUG,
};

#if 0
#  define SYS_DEBUG_PRINT(severity, format, ...) printf(format, ##__VA_ARGS__)
#  define SYS_DEBUG_MESSAGE(severity, message)   printf("%s\n", message)

#  define SYS_CONSOLE_PRINT(format, ...)  printf(format, ##__VA_ARGS__)
#  define SYS_CONSOLE_MESSAGE(message)    printf("%s\n", message)

#  define SYS_ASSERT(expression, message) assert((expression))
#  define SYS_MESSAGE(message)            printf("%s\n", message)
#else
#  define SYS_DEBUG_PRINT(severity, format, ...)
#  define SYS_DEBUG_MESSAGE(severity, message)

#  define SYS_CONSOLE_PRINT(format, ...)
#  define SYS_CONSOLE_MESSAGE(message)

#  define SYS_ASSERT(expression, message)
#  define SYS_MESSAGE(message)
#endif

#endif
