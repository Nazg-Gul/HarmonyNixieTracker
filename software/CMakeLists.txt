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

project(NixieTracker)

###########################################################################
# Policies

cmake_minimum_required(VERSION 2.6)
if(NOT CMAKE_VERSION VERSION_LESS 2.8.4)
  cmake_policy(SET CMP0017 NEW)
  cmake_policy(SET CMP0020 NEW)
endif()

if(NOT EXECUTABLE_OUTPUT_PATH)
  set(IS_FIRST_RUN TRUE)
else()
  set(IS_FIRST_RUN FALSE)
endif()

###########################################################################
# Macros and utilities.

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake"
  "${CMAKE_SOURCE_DIR}/cmake/Modules"
)

include(CMakeParseArguments)
include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)

include(macros)

###########################################################################
# Options.

# General options
option(WITH_TESTS "Build the unit tests" ON)

###########################################################################
# Global configuration.

enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "" FORCE)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "" FORCE)
set(NIXIETRACKER_TESTS_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/tests CACHE INTERNAL "" FORCE)
set(NIXIETRACKER_GENERATORS_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/generators CACHE INTERNAL "" FORCE)

include(configure_build)
include(external_libs)

###########################################################################
# Include directories.

###########################################################################
# Sources.

add_subdirectory(external)
add_subdirectory(source)

###########################################################################
# Configuration.

if(IS_FIRST_RUN)
  function(info_cfg_option _name _setting)
    set(_msg "${_name}")
    string(LENGTH "${_msg}" _len)
    while("18" GREATER "${_len}")
      set(_msg "${_msg} ")
      math(EXPR _len "${_len} + 1")
    endwhile()
    message("  ${_msg} : ${${_setting}}")
  endfunction()

  message("\n======== NixieTracker Configuration ========\n")

  info_cfg_option("Unit tests" WITH_TESTS)

  # Finish
  message("")
endif()
