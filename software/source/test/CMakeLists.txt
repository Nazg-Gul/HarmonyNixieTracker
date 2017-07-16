# Copyright (c) 2017, Sergey Sharybin
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
# Author: Sergey Sharybin (sergey.vfx@gmail.com)

set(INC
  .
  ..
  ../../external/gtest/include
)

if(WIN32)
  list(APPEND INC
    ../../external/glog/src/windows
  )
else()
  list(APPEND INC
    ../../external/glog/src
  )
endif()

set(INC_SYS
)

set(SRC
  internal/test_main.cc
)

set(SRC_HEADERS
  mock_log.h
  test.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})
add_library(test_main "${SRC}" "${INC}" "${INC_SYS}")

if(GTEST_BUILD_FROM_SOURCE)
  add_dependencies(test_main extern_src_googletest)
endif()
