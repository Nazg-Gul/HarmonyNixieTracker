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

#include "util_url.h"

#include <stdlib.h>

#include "utildefines.h"

#include "util_math.h"
#include "util_string.h"

// Parse URL part prior to :// (aKa, scheme).
static bool urlParse_scheme(const char* url_part,  size_t len, 
                            char* scheme, size_t max_scheme,
                            uint16_t* port) {
  safe_strncpy_len(scheme, url_part, len, max_scheme);
  if (port == NULL) {
    // Early output, no port is needed to know, we can skip some CPU ticks.
    return true;
  }
  // Special case to get default port based on scheme.
  // TODO(sergey): Make it case-insensitive comparison.
  if (STREQ_LEN(url_part, "https", 5)) {
    *port = 443;
  } else if (STREQ_LEN(url_part, "http", 4)) {
    *port = 80;
  } else {
    // TODO(sergey): Handle ftp protocol, and maybe some others?
  }
  return true;
}

// Parses the following sequence:
//   user[:password]
static bool urlParse_userAndPassword(
    const char* url_part, size_t len,
    char* user, const size_t max_user,
    char* password, const size_t max_password) {
  if (user == NULL && password == NULL) {
    // Early output, no output is needed.
    return true;
  }
  const char* colon = strchr_len(url_part, ':', len);
  if (colon == NULL) {
    // Copy the whole URL part to user name, keep password empty.
    safe_strncpy_len(user, url_part, len, max_user);
    safe_strncpy_len(password, "\0", 1, max_password);
    return true;
  }
  // Copy both user and password.
  safe_strncpy_len(user, url_part, colon - url_part, max_user);
  safe_strncpy_len(password,
                   colon + 1,
                   len - (colon - url_part + 1), max_password);
  return true;
}

// Parses the following sequence:
//   host[:port]
static bool urlParse_hostAndPort(const char* url_part, size_t len,
                                 char* host, const size_t max_host,
                                 uint16_t* port ) {
  if (host == NULL && port == NULL) {
    // Early output, no output is needed.
    return true;
  }
  const char* colon = strchr_len(url_part, ':', len);
  if (colon == NULL) {
    // Only host is specified, keep port at default value.
    safe_strncpy_len(host, url_part, len, max_host);
    return true;
  }
  safe_strncpy_len(host, url_part, colon - url_part, max_host);
  if (port != NULL) {
    // TODO(sergey): Watch out for NULL terminator in some compilers?
    *port = atoi(colon + 1);
  }
  return true;
}

// Parses the following sequence:
//   [user[:password]@]host[:port]
static bool urlParse_userPasswordHostAndPort(
    const char* url_part, size_t len,
    char* user, const size_t max_user,
    char* password, const size_t max_password,
    char* host, const size_t max_host,
    uint16_t* port) {
  const char* at = strchr_len(url_part, '@', len);
  if (at != NULL) {
    const size_t user_password_len = at - url_part;
    if (!urlParse_userAndPassword(url_part, user_password_len,
                                  user, max_user,
                                  password, max_password)) {
      return false;
    }
    // More parsing point past the @ symbol.
    url_part = at + 1;
    len -= user_password_len + 1;
  } else {
    safe_strncpy_len(user, "\0", 1, max_user);
    safe_strncpy_len(password, "\0", 1, max_password);
  }
  if (!urlParse_hostAndPort(url_part, len,
                            host, max_host,
                            port)) {
    return false;
  }
  return true;
}

// Parse the following sequence:
//  [/path][?query][#fragment]
static bool urlParse_pathQueryAndFragment(
    const char* url_part, size_t len,
    char* path, const size_t max_path,
    char* query, const size_t max_query,
    char* fragment, const size_t max_fragment) {
  if (path == NULL && query == NULL && fragment == NULL) {
    // Early output, no output is needed.
    return true;
  }
  // Get fragment.
  const char* token = strchr_len(url_part, '#', len);
  if (token != NULL) {
    safe_strncpy_len(fragment,
                     token + 1,
                     len - (token - url_part) - 1,
                     max_fragment);
    // Move end of the url_path to only include [/path][?query].
    len = token - url_part;
  } else {
    safe_strncpy_len(fragment, "\0", 1, max_fragment);
  }
  // Get query.
  token = strchr_len(url_part, '?', len);
  if (token != NULL) {
    safe_strncpy_len(query, 
                     token + 1, 
                     len - (token - url_part) - 1,
                     max_query);
    // Move end of the url_path to only include [/path].
    len = token - url_part;
  } else {
    safe_strncpy_len(query, "\0", 1, max_query);
  }
  // Get path.
  safe_strncpy_len(path, url_part, len, max_path);
  return true;
}

// Parses the following sequence:
//   [//[user[:password]@]host[:port]][/path][?query][#fragment]
static bool urlParse_pastScheme(const char* url_part, size_t len,
                                char* user, const size_t max_user,
                                char* password, const size_t max_password,
                                char* host, const size_t max_host,
                                uint16_t* port,
                                char* path, const size_t max_path,
                                char* query, const size_t max_query,
                                char* fragment, const size_t max_fragment,
                                char* path_suffix, const size_t max_path_suffix) {
  if (len >= 3 && STREQ_LEN(url_part, "//", 2)) {
    // We need to parse host with possible credentials and port.
    const char* path_start;
    url_part += 2;
    len += 2;
    // Get character index in the string where path begin.
    // NOTE: If path_start is NULL, then the whole string is only credentials.
    //       host and port.
    path_start = strchr_any_len(url_part, "/?#", len);
    const size_t first_part_length = path_start != NULL ? path_start - url_part
                                                        : len;
    if (!urlParse_userPasswordHostAndPort(url_part, first_part_length,
                                          user, max_user,
                                          password, max_password,
                                          host, max_host,
                                          port)) {
      return false;
    }
    // Move parsing point to possible path and following.
    url_part = path_start;
    len = first_part_length;
  }
  // If there is no path component, zero out all remaining outputs.
  if (url_part == NULL) {
    safe_strncpy_len(path, "/", 1, max_path);
    safe_strncpy_len(query, "\0", 1, max_query);
    safe_strncpy_len(fragment, "\0", 1, max_fragment);
    safe_strncpy_len(path_suffix, "/", 1, max_path_suffix);
    return true;
  }
  safe_strncpy_len(path_suffix, url_part, len, max_path_suffix);
  if (!urlParse_pathQueryAndFragment(url_part, len,
                                     path, max_path,
                                     query, max_query,
                                     fragment, max_fragment)) {
    return false;
  }
  return true;
}

// scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
bool urlParseGetParts(const char* url,
                      char* scheme, const size_t max_scheme,
                      char* user, const size_t max_user,
                      char* password, const size_t max_password,
                      char* host, const size_t max_host,
                      uint16_t* port,
                      char* path, const size_t max_path,
                      char* query, const size_t max_query,
                      char* fragment, const size_t max_fragment,
                      char* path_suffix, const size_t max_path_suffix) {
  // Begin parsing with scheme.
  //
  // NOTE: Scheme is a mandatory part of URL for our use.
  const char* scheme_delim = strstr(url, ":");
  if (scheme_delim == NULL) {
    return false;
  }
  // Get "default" port from the protocol.
  const size_t scheme_len = scheme_delim - url;
  if (!urlParse_scheme(url, scheme_len, scheme, max_scheme, port)) {
    return false;
  }
  // This is part of URL without scheme and delimeter, <host>/<path>
  const size_t total_len = strlen(url);
  const char* url_no_scheme = scheme_delim + 1;
  if (!urlParse_pastScheme(url_no_scheme, total_len - scheme_len - 3,
                           user, max_user,
                           password, max_password,
                           host, max_host,
                           port,
                           path, max_path,
                           query, max_query,
                           fragment, max_fragment,
                           path_suffix, max_path_suffix)) {
    return false;
  }
  return true;
}
