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
# Global generic CMake settings.

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
endif()

###########################################################################
# Per-compiler configuration.

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -fno-strict-aliasing")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -fno-strict-aliasing")
endif()

if(APPLE)
  # Require newer cmake on osx because of version handling,
  # older cmake cannot handle 2 digit subversion!
  cmake_minimum_required(VERSION 3.0.0)

  if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES x86_64 CACHE STRING "Choose the architecture you want to build Blender for: i386, x86_64 or ppc" FORCE)
  endif()

  if(NOT DEFINED OSX_SYSTEM)
    execute_process(
      COMMAND xcodebuild -version -sdk macosx SDKVersion
      OUTPUT_VARIABLE OSX_SYSTEM
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  endif()

  # Workaround for incorrect cmake xcode lookup for developer previews - XCODE_VERSION does not
  # take xcode-select path into account but would always look  into /Applications/Xcode.app
  # while dev versions are named Xcode<version>-DP<preview_number>
  execute_process(
    COMMAND xcode-select --print-path
    OUTPUT_VARIABLE XCODE_CHECK OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # Truncate to bundlepath in any case
  string(REPLACE "/Contents/Developer" "" XCODE_BUNDLE ${XCODE_CHECK})

  if(${CMAKE_GENERATOR} MATCHES "Xcode")
    # Earlier xcode has no bundled developer dir, no sense in getting xcode path from
    if(${XCODE_VERSION} VERSION_GREATER 4.2)
      # reduce to XCode name without dp extension
      string(SUBSTRING "${XCODE_CHECK}" 14 6 DP_NAME)
      if(${DP_NAME} MATCHES Xcode5)
        set(XCODE_VERSION 5)
      endif()
    endif()

    ##### CMake incompatibility with xcode 4.3 and higher #####
    # CMake fails due looking for xcode in the wrong path, thus will be empty var
    if(${XCODE_VERSION} MATCHES '')
      message(FATAL_ERROR "Xcode 4.3 and higher must be used with cmake 2.8-8 or higher")
    endif()
    ### end CMake incompatibility with xcode 4.3 and higher ###

    if(${XCODE_VERSION} VERSION_EQUAL 4 OR ${XCODE_VERSION} VERSION_GREATER 4 AND ${XCODE_VERSION} VERSION_LESS 4.3)
      # Xcode 4 defaults to the Apple LLVM Compiler.
      # Override the default compiler selection because Blender only compiles with gcc up to xcode 4.2
      set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
        message(STATUS "Setting compiler to: " ${CMAKE_XCODE_ATTRIBUTE_GCC_VERSION})
    endif()
  else()
    # Unix makefile generator does not fill XCODE_VERSION var, so we get it with a command
    execute_process(COMMAND xcodebuild -version OUTPUT_VARIABLE XCODE_VERS_BUILD_NR)
    # Truncate away build-nr.
    string(SUBSTRING "${XCODE_VERS_BUILD_NR}" 6 3 XCODE_VERSION)
    unset(XCODE_VERS_BUILD_NR)
  endif()

  message(STATUS "Detected OS X ${OSX_SYSTEM} and Xcode ${XCODE_VERSION} at ${XCODE_BUNDLE}")

  if(${XCODE_VERSION} VERSION_LESS 4.3)
    # Use guaranteed existing SDK.
    set(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX${OSX_SYSTEM}.sdk CACHE PATH "" FORCE)
  else()
    # NOTE: xcode-select path could be ambigous,
    # cause /Applications/Xcode.app/Contents/Developer or /Applications/Xcode.app would be allowed
    # so i use a selfcomposed bundlepath here.
    set(OSX_SYSROOT_PREFIX ${XCODE_BUNDLE}/Contents/Developer/Platforms/MacOSX.platform)
    message(STATUS "OSX_SYSROOT_PREFIX: " ${OSX_SYSROOT_PREFIX})
    # Use guaranteed existing SDK.
    set(OSX_DEVELOPER_PREFIX /Developer/SDKs/MacOSX${OSX_SYSTEM}.sdk)
    set(CMAKE_OSX_SYSROOT ${OSX_SYSROOT_PREFIX}/${OSX_DEVELOPER_PREFIX} CACHE PATH "" FORCE)
    if(${CMAKE_GENERATOR} MATCHES "Xcode")
      # To silence sdk not found warning, just overrides CMAKE_OSX_SYSROOT.
      set(CMAKE_XCODE_ATTRIBUTE_SDKROOT macosx${OSX_SYSTEM})
    endif()
  endif()

  if(OSX_SYSTEM MATCHES 10.9)
    # Make sure syslibs and headers are looked up in sdk (expecially for 10.9 OpenGL atm.)
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT})
  endif()

  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # 10.6 is our min. target, if you use higher sdk, weak linking happens.
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "" FORCE)
  endif()

  if(NOT ${CMAKE_GENERATOR} MATCHES "Xcode")
    # Force CMAKE_OSX_DEPLOYMENT_TARGET for makefiles, will not work else (CMake bug?)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
    add_definitions("-DMACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}")
  endif()
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "/nologo /Gd /EHsc /MP" CACHE STRING "MSVC MT C++ flags " FORCE)
  set(CMAKE_C_FLAGS   "/nologo /Gd /MP"       CACHE STRING "MSVC MT C++ flags " FORCE)
  if(CMAKE_CL_64)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /RTC1 /MDd /Zi /MP" CACHE STRING "MSVC MT flags " FORCE)
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /RTC1 /MDd /ZI /MP" CACHE STRING "MSVC MT flags " FORCE)
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /MD /MP" CACHE STRING "MSVC MT flags " FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Ob1 /MD /MP" CACHE STRING "MSVC MT flags " FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /MD /Zi /MP" CACHE STRING "MSVC MT flags " FORCE)
  if(CMAKE_CL_64)
    set(CMAKE_C_FLAGS_DEBUG "/Od /RTC1 /MDd /Zi /MP" CACHE STRING "MSVC MT flags " FORCE)
  else()
    set(CMAKE_C_FLAGS_DEBUG "/Od /RTC1 /MDd /ZI /MP" CACHE STRING "MSVC MT flags " FORCE)
  endif()
  set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /MD /MP" CACHE STRING "MSVC MT flags " FORCE)
  set(CMAKE_C_FLAGS_MINSIZEREL "/O1 /Ob1 /MD /MP" CACHE STRING "MSVC MT flags " FORCE)
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /MD /Zi /MP" CACHE STRING "MSVC MT flags " FORCE)
  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /IGNORE:4221")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

###########################################################################
# C++11

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Compiler does not support C++11 which is required")
  endif()
elseif(MSVC)
  # TODO(sergey): Might want to add version check. But hopefully nobody uses MSVC 2010 nowadays.
else()
  message(FATAL_ERROR "Unknown compiler, building will likely fail. Please contact developers")
endif()
