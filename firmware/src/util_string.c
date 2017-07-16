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

#define _GNU_SOURCE

#include "util_string.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "gcc/memmem.h"
#include "util_math.h"

char* strchr_len(const char* haystack,
                 const char needle,
                 const size_t haystack_length) {
  size_t a;
  for (a = 0; a < haystack_length; ++a, ++haystack) {
    if (*haystack == '\0') {
      return NULL;
    }
    if (*haystack == needle) {
      return (char*)haystack;
    }
  }
  return NULL;
}

char* strstr_len(const char* haystack,
                 const char* needle,
                 const size_t haystack_len) {
  return memmem(haystack, haystack_len, needle, strlen(needle));
}

char* strchr_any_len(const char* haystack,
                     const char* needles,
                     const size_t haystack_length) {
  size_t a;
  for (a = 0; a < haystack_length; ++a, ++haystack) {
    if (*haystack == '\0') {
      return NULL;
    }
    if (strchr(needles, *haystack) != NULL) {
      return (char*)haystack;
    }
  }
  return NULL;
}

char* safe_strncpy(char* dst, const char* src, const size_t max_copy) {
  if (dst == NULL || src == NULL || max_copy == 0) {
    return NULL;
  }
  const size_t src_len = strnlen(src, max_copy - 1);
  memcpy(dst, src, src_len);
  dst[src_len] = '\0';
  return dst;
}

char* safe_strncpy_len(char* dst,
                       const char* src,
                       const size_t len,
                       const size_t max_copy) {
  if (dst == NULL || src == NULL || max_copy == 0) {
    return NULL;
  }
  const size_t max_len = min_zz(len, max_copy - 1);
  memcpy(dst, src, max_len);
  dst[max_len] = '\0';
  return dst;
}

size_t strnlen(const char* s, const size_t max_len) {
  size_t len;
  for (len = 0; len < max_len; ++len, ++s) {
    if (*s == '\0') {
      break;
    }
  }
  return len;
}

size_t safe_vsnprintf(char* buffer,
                      size_t size,
                      const char* format,
                      va_list arg) {
  size_t n = vsnprintf(buffer, size, format, arg);
  if (n != -1 && n < size) {
    buffer[n] = '\0';
  } else {
    buffer[size - 1] = '\0';
  }
  return n;
}

size_t safe_snprintf(char* dst, size_t size, const char* format, ...) {
  size_t n;
  va_list arg;
  va_start(arg, format);
  n = safe_vsnprintf(dst, size, format, arg);
  va_end(arg);
  return n;
}
