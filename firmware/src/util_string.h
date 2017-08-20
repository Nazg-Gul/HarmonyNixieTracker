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

#ifndef _UTIL_STRING_H
#define _UTIL_STRING_H

#include <stdarg.h>
#include <stddef.h>

// Similar to strstr but only considers given number of characters from
// haystack.
char* strchr_len(const char* haystack,
                 const char needle,
                 const size_t haystack_length);

// Similar to strstr but only considers given number of characters from
// haystack.
char* strstr_len(const char* haystack,
                 const char* needle,
                 const size_t haystack_len);


// Find first occurrence of any character from needles in haystack.
//
// TODO(sergey): Wasn't there similar built-in function.
char* strchr_any_len(const char* haystack,
                     const char* needles,
                     const size_t haystack_length);

// Safe version of strncpy which does extra checks on source and destination
// not being NULL and length not being a zero.
char* safe_strncpy(char* dst, const char* src, const size_t max_copy);

// Same as above, but forces length of the source.
char* safe_strncpy_len(char* dst,
                       const char* src,
                       const size_t len,
                       const size_t max_copy);

// Similar to strlen but stops searching for null-terminator after len bytes.
size_t safe_strnlen(const char* s, const size_t max_len);

// Safe and portable versions of snprintf(). Ensures NULL-terminator.
size_t safe_vsnprintf(char* buffer, 
                      size_t size, 
                      const char* format,
                      va_list arg);
size_t safe_snprintf(char* dst, size_t size, const char* format, ...);

// Reverse the given null-terminated string in place.
void string_inplace_reverse(char *string);

// Reverse array of bytes in memory.
void reverse_bytes(void* mem, size_t size);

#endif  // _UTIL_STRING_H
