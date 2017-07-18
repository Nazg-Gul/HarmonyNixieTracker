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
#include "util_string.h"
}

namespace NixieTracker {

using std::string;

////////////////////////////////////////////////////////////////////////////////
// strchr_len

TEST(strchr_len, EmptyHaystack) {
  EXPECT_NULL(strchr_len("", '@', 0));
}

TEST(strchr_len, LongHaystackNoNeedle) {
  EXPECT_NULL(strchr_len("qwertyuiop", '@', 10));
}

TEST(strchr_len, LongHaystackNoNeedleTrunkcated) {
  EXPECT_NULL(strchr_len("qwertyuiop", '@', 5));
}

TEST(strchr_len, LongHaystackWithNeedle) {
  const char* haystack = "qwertyuiop@123456";
  EXPECT_EQ(strchr_len(haystack, '@', 17), haystack + 10);
}

TEST(strchr_len, LongHaystackWithNeedleTrukcated) {
  const char* haystack = "qwertyuiop@123456";
  EXPECT_EQ(strchr_len(haystack, '@', 12), haystack + 10);
}

TEST(strchr_len, LongHaystackWithNeedleTrukcatedBeforeNeedle) {
  EXPECT_NULL(strchr_len("qwertyuiop@123456", '@', 9));
}

////////////////////////////////////////////////////////////////////////////////
// strstr_len

TEST(strstr_len, EmptyHaystack) {
  EXPECT_NULL(strstr_len("", "@", 0));
}

TEST(strstr_len, EmptyHaystackEmptyNeedle) {
  const char* haystack = "";
  EXPECT_EQ(strstr_len(haystack, "", 0), haystack);
}

TEST(strstr_len, HaystackEmptyNeedle) {
  const char* haystack = "qwerty";
  EXPECT_EQ(strstr_len(haystack, "", 6), haystack);
}

TEST(strstr_len, HaystackShorterThanNeedle) {
  EXPECT_NULL(strstr_len("qwerty", "qwerty1234", 6));
}

TEST(strstr_len, HaystackWithNeedle) {
  const char* haystack = "qwertyTOKEN123456";
  EXPECT_EQ(strstr_len(haystack, "TOKEN", 17), haystack + 6);
}

TEST(strstr_len, LongHaystackWithTrunkatedNeedle) {
  EXPECT_NULL(strstr_len("qwertyTOKEN123456", "TOKEN", 10));
}

////////////////////////////////////////////////////////////////////////////////
// strchr_any_len

TEST(strchr_any_len, EmptyHaystack) {
  EXPECT_NULL(strchr_any_len("", "@", 0));
}

TEST(strchr_any_len, LongHaystackNoNeedle) {
  EXPECT_NULL(strchr_any_len("qwertyuiop", "@", 10));
}

TEST(strchr_any_len, LongHaystackNoNeedleTrunkcated) {
  EXPECT_NULL(strchr_any_len("qwertyuiop", "@", 5));
}

TEST(strchr_any_len, LongHaystackWithNeedle) {
  const char* haystack = "qwertyuiop@123456";
  EXPECT_EQ(strchr_any_len(haystack, "@", 17), haystack + 10);
}

TEST(strchr_any_len, LongHaystackWithNeedleTrukcated) {
  const char* haystack = "qwertyuiop@123456";
  EXPECT_EQ(strchr_any_len(haystack, "@", 12), haystack + 10);
}

TEST(strchr_any_len, LongHaystackWithNeedleTrukcatedBeforeNeedle) {
  EXPECT_NULL(strchr_any_len("qwertyuiop@123456", "@", 9));
}

TEST(strchr_any_len, LongHaystackWithMultipleNeedle) {
  const char* haystack = "qwertyuiop@123456";
  EXPECT_EQ(strchr_any_len(haystack, "123456", 17), haystack + 11);
}

TEST(strchr_any_len, LongHaystackWithMultipleNeedleTrukcatedBeforeNeedle) {
  EXPECT_NULL(strchr_any_len("qwertyuiop@123456", "123456", 9));
}

////////////////////////////////////////////////////////////////////////////////
// safe_strncpy

TEST(safe_strncpy, EmptySource) {
  char dst[32];
  safe_strncpy(dst, "", sizeof(dst));
  EXPECT_EQ(string(dst), "");
}

TEST(safe_strncpy, EmptySourceZeroBytes) {
  char dst[32];
  EXPECT_NULL(safe_strncpy(dst, "", 0));
}

TEST(safe_strncpy, EmptySourceSingleByte) {
  char dst[32];
  safe_strncpy(dst, "", 1);
  EXPECT_EQ(string(dst), "");
}

TEST(safe_strncpy, SourceShorterThanDestination) {
  char dst[32];
  safe_strncpy(dst, "source", sizeof(dst));
  EXPECT_EQ(string(dst), "source");
}

TEST(safe_strncpy, SourceLongerThanDestination) {
  char dst[3];
  safe_strncpy(dst, "source", sizeof(dst));
  EXPECT_EQ(string(dst), "so");
}

TEST(safe_strncpy, SuffixIsKept) {
  char dst[32] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0\0";
  safe_strncpy(dst, "source", sizeof(dst));
  EXPECT_EQ(string(dst), "source");
  EXPECT_EQ(memcmp(dst, "source\0HIJKLMNOPQRSTUVWXYZ\0\0\0\0\0", sizeof(dst)), 0);
}

////////////////////////////////////////////////////////////////////////////////
// safe_strncpy_len

TEST(safe_strncpy_len, EmptySource) {
  char dst[32];
  safe_strncpy_len(dst, "", 0, sizeof(dst));
  EXPECT_EQ(string(dst), "");
}

TEST(safe_strncpy_len, EmptySourceZeroBytes) {
  char dst[32];
  EXPECT_NULL(safe_strncpy_len(dst, "", 0, 0));
}

TEST(safe_strncpy_len, EmptySourceSingleByte) {
  char dst[32];
  safe_strncpy_len(dst, "", 0, 1);
  EXPECT_EQ(string(dst), "");
}

TEST(safe_strncpy_len, SourceShorterThanDestination) {
  char dst[32];
  safe_strncpy_len(dst, "source", 6, sizeof(dst));
  EXPECT_EQ(string(dst), "source");
}

TEST(safe_strncpy_len, SourceLongerThanDestination) {
  char dst[3];
  safe_strncpy_len(dst, "source", 6, sizeof(dst));
  EXPECT_EQ(string(dst), "so");
}

TEST(safe_strncpy_len, SuffixIsKept) {
  char dst[32] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0\0";
  safe_strncpy_len(dst, "source", 6, sizeof(dst));
  EXPECT_EQ(string(dst), "source");
  EXPECT_EQ(memcmp(dst, "source\0HIJKLMNOPQRSTUVWXYZ\0\0\0\0\0", sizeof(dst)), 0);
}

TEST(safe_strncpy_len, TruncateSource) {
  char dst[32];
  safe_strncpy_len(dst, "source", 3, sizeof(dst));
  EXPECT_EQ(string(dst), "sou");
}

////////////////////////////////////////////////////////////////////////////////
// strnlen

TEST(safe_strnlen, Empty) {
  EXPECT_EQ(safe_strnlen("", 0), 0);
}

TEST(safe_strnlen, Long) {
  EXPECT_EQ(safe_strnlen("1234567890", 10), 10);
}

TEST(safe_strnlen, LongWithLargeSize) {
  EXPECT_EQ(safe_strnlen("1234567890", 20), 10);
}

TEST(safe_strnlen, LongWithSmallSize) {
  EXPECT_EQ(safe_strnlen("1234567890", 4), 4);
}

////////////////////////////////////////////////////////////////////////////////
// safe_snprintf

TEST(safe_snprintf, EmptyFormat) {
  char dst[32];
  safe_snprintf(dst, sizeof(dst), "");
  EXPECT_EQ(string(dst), "");
}

TEST(safe_snprintf, FormatLongerThanDestination) {
  char dst[8];
  safe_snprintf(dst, sizeof(dst), "1234567890qwertyuiop");
  EXPECT_EQ(string(dst), "1234567");
}

}  // namespace NixieTracker
