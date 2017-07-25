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

#ifndef _TPCIP_TCPIP_STUB_H
#define _TPCIP_TCPIP_STUB_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef union {
  uint32_t Val;
  uint16_t w[2];
  uint8_t  v[4];
} IPV4_ADDR;

typedef union {
  uint8_t  v[16];
  uint16_t w[8];
  uint32_t d[4];
} IPV6_ADDR;

typedef struct {
  IPV4_ADDR v4Add;
  IPV6_ADDR v6Add;
} IP_MULTI_ADDRESS;

typedef void* NET_PRES_SKT_HANDLE_T;

#endif  // _TPCIP_TCPIP_STUB_H_
