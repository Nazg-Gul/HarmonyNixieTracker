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

string glueURL(const string& scheme,
               const string& user,
               const string& password,
               const string& host,
               uint16_t port,
               const string& path,
               const string& query,
               const string& fragment) {
  string url = "";
  url += scheme + "://";
  if (user != "") {
    url += user;
    if (password != "") {
      url += ":" + password;
    }
    url += "@";
  }
  url += host;
  if (port != 0) {
    url += ":" + std::to_string(port);
  }
  if (path != "") {
    url += "/" + path;
  }
  if (query != "") {
    url += "?" + query;
  }
  if (fragment != "") {
    url += "#" + fragment;
  }
  return url;
}

void runURLParseTest(const string& expected_scheme,
                     const string& expected_user,
                     const string& expected_password,
                     const string& expected_host,
                     uint16_t expected_port,
                     const string& expected_path,
                     const string& expected_query,
                     const string& expected_fragment) {
  const string url = glueURL(expected_scheme,
                             expected_user,
                             expected_password,
                             expected_host,
                             expected_port,
                             expected_path,
                             expected_query,
                             expected_fragment);
  static const int N = 1024;
  char actual_scheme[N];
  char actual_user[N];
  char actual_password[N];
  char actual_host[N];
  uint16_t actual_port;
  char actual_path[N];
  char actual_query[N];
  char actual_fragment[N];
  char actual_path_suffix[N];
  EXPECT_TRUE(urlParseGetParts(url.c_str(),
                               actual_scheme, sizeof(actual_scheme),
                               actual_user, sizeof(actual_user),
                               actual_password, sizeof(actual_password),
                               actual_host, sizeof(actual_host),
                               &actual_port,
                               actual_path, sizeof(actual_path),
                               actual_query, sizeof(actual_query),
                               actual_fragment, sizeof(actual_fragment),
                               actual_path_suffix, sizeof(actual_path_suffix)));
  EXPECT_EQ(actual_scheme, expected_scheme);
  EXPECT_EQ(actual_user, expected_user);
  EXPECT_EQ(actual_password, expected_password);
  EXPECT_EQ(actual_host, expected_host);
  EXPECT_EQ(actual_port, expected_port);
  // NOTE: The logic in urlParseGetParts is such that / is a part of path.
  if (expected_path != "") {
    EXPECT_EQ(actual_path, "/" + expected_path);
  } else {
    EXPECT_EQ(actual_path, string("/"));
  }
  EXPECT_EQ(actual_query, expected_query);
  EXPECT_EQ(actual_fragment, expected_fragment);
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

TEST(urlParseGetParts, AllComponents) {
  runURLParseTest("scheme",
                  "user",
                  "password",
                  "host",
                  1234,
                  "path",
                  "query",
                  "fragment");
}

TEST(urlParseGetParts, NoPassword) {
  runURLParseTest("scheme",
                  "user",
                  "",
                  "host",
                  1234,
                  "path",
                  "query",
                  "fragment");
}

TEST(urlParseGetParts, NoUser) {
  runURLParseTest("scheme",
                  "",
                  "",
                  "host",
                  1234,
                  "path",
                  "query",
                  "fragment");
}

TEST(urlParseGetParts, NoFragment) {
  runURLParseTest("scheme",
                  "",
                  "",
                  "host",
                  1234,
                  "path",
                  "query",
                  "");
}

TEST(urlParseGetParts, NoQuery) {
  runURLParseTest("scheme",
                  "",
                  "",
                  "host",
                  1234,
                  "path",
                  "",
                  "");
}

TEST(urlParseGetParts, NoPath) {
  runURLParseTest("scheme",
                  "",
                  "",
                  "host",
                  1234,
                  "",
                  "",
                  "");
}

}  // namespace NixieTracker
