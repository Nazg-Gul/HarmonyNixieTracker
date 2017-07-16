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

###########################################################################
# Precompiled libraries tips and hints, for find_package().

include(precompiled_libs)

###########################################################################
# Configure Threads

find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
  message(STATUS "Using pthreads threads")
  set(THREADS_DEFINES -DTHREADS_MODEL_PTHREADS)
  set(THREADS_LIBS ${CMAKE_THREAD_LIBS_INIT})
elseif(CMAKE_USE_WIN32_THREADS_INIT)
  message(STATUS "Using Win32 threads")
  set(THREADS_DEFINES -DTHREADS_MODEL_WIN32)
  set(THREADS_LIBS ${CMAKE_THREAD_LIBS_INIT})
else()
  message(FATAL_ERROR "System doesn't have compatible threads!")
endif()

###########################################################################
# Configure system-wide libraries

#########################
# Gflags

if(WITH_SYSTEM_GFLAGS)
  find_package(Gflags)
  if(NOT GFLAGS_FOUND)
    message(WARNING "System wide Gflags not found, falling back to bundled one")
    set(WITH_SYSTEM_GFLAGS OFF)
  endif()
endif()

if(NOT WITH_SYSTEM_GFLAGS)
  set(GFLAGS_DEFINES
    -DGFLAGS_DLL_DEFINE_FLAG=
    -DGFLAGS_DLL_DECLARE_FLAG=
    -DGFLAGS_DLL_DECL=
  )
  set(GFLAGS_NAMESPACE "gflags")
  set(GFLAGS_LIBRARIES extern_gflags)
  set(GFLAGS_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/gflags/src")
endif()

#########################
# Glog

if(WITH_SYSTEM_GLOG)
  find_package(Glog)
  if(NOT GLOG_FOUND)
    message(WARNING "System wide Glog not found, falling back to bundled one")
    set(WITH_SYSTEM_GLOG OFF)
  endif()
endif()

if(NOT WITH_SYSTEM_GFLAGS)
  set(GLOG_DEFINES
    -DGOOGLE_GLOG_DLL_DECL=
  )
  set(GLOG_LIBRARIES extern_glog)
  if(NOT WIN32)
    set(GLOG_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/glog/src")
  else()
    set(GLOG_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/glog/src/windows")
  endif()
endif()

# TODO(sergey): This is a workaround for buggy behavior of GTEST_IS_NULL_LITERAL_()
# when using Gcc-5
if(CMAKE_COMPILER_IS_GNUCC AND "${CMAKE_C_COMPILER_VERSION}" VERSION_LESS "6.0")
  list(APPEND GLOG_DEFINES
    -DGTEST_ELLIPSIS_NEEDS_POD_
  )
endif()

#########################
# Gtest

if(WITH_SYSTEM_GTEST)
  find_package(Gtest)
  if(NOT GTEST_FOUND)
    message(WARNING "System wide Gtest not found, falling back to bundled one")
    set(WITH_SYSTEM_GTEST OFF)
  else()
    if(GTEST_BUILD_FROM_SOURCE)
      include(ExternalProject)
      set(GTEST_CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      )
      ExternalProject_Add(extern_src_googletest
        SOURCE_DIR "${GTEST_SOURCE_DIRECTORY}"
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/build/gtest
        INSTALL_COMMAND ""
        CMAKE_ARGS "${GTEST_CMAKE_ARGS}"
      )
      ExternalProject_Get_Property(extern_src_googletest binary_dir)
      set(GTEST_LIBRARIES ${binary_dir}/libgtest.a)
      unset(binary_dir)
      unset(GTEST_CMAKE_ARGS)
    endif()
  endif()
endif()

if(NOT WITH_SYSTEM_GTEST)
  set(GTEST_LIBRARIES extern_gtest)
  set(GTEST_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/gtest/include")
endif()

#########################
# Gmock

if(WITH_SYSTEM_GMOCK)
  find_package(Gmock)
  if(NOT GMOCK_FOUND)
    message(WARNING "System wide Gmock not found, falling back to bundled one")
    set(WITH_SYSTEM_GMOCK OFF)
  endif()
endif()

if(NOT WITH_SYSTEM_GMOCK)
  set(GMOCK_LIBRARIES extern_gmock)
  set(GMOCK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/gmock/include")
endif()
