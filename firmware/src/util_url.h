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

#ifndef _UTIL_URL_H
#define _UTIL_URL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Get all parts of the following sequence:
//   scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
//
// path_suffix contains all of [/path][?query][#fragment]
bool urlParseGetParts(const char* url,
                      char* scheme, const size_t max_scheme,
                      char* user, const size_t max_user,
                      char* password, const size_t max_password,
                      char* host, const size_t max_host,
                      uint16_t* port,
                      char* path, const size_t max_path,
                      char* query, const size_t max_query,
                      char* fragment, const size_t max_fragment,
                      char* path_suffix, const size_t max_path_suffix);

#endif  // _UTIL_URL_H
