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

#include "test/test.h"

#include <string>

extern "C" {
#include "util_url.h"
}

namespace NixieTracker {

using std::string;

namespace {

void defaultPortTest(const string& url, uint16_t expected_port) {
  uint16_t actual_port;
  urlParseGetParts(url.c_str(),
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   &actual_port,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0);
    EXPECT_EQ(actual_port, expected_port);
}

}  // namespace

TEST(urlParseGetParts, EmptyStringAllNULL) {
  urlParseGetParts("",
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0);

}

TEST(urlParseGetParts, ProperURLNULL) {
  urlParseGetParts("https://example.org/",
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0,
                   NULL, 0);

}

TEST(urlParseGetParts, DefaultPorts) {
  defaultPortTest("http://example.org/", 80);
  defaultPortTest("https://example.org/", 443);
}

}  // namespace NixieTracker
