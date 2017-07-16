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

# Set binary executable output directory.
#
# Deals with MSVC's tendency to use separate folders for release and debug
# builds. Handy in this case to have single solution and be able to compile
# both release and debug builds without causing conflicts.
#
# TODO(sergey): Do we need to deal somehow with targets like RelWithDebInfo?
function(NIXIETRACKER_SET_TARGET_RUNTIME_DIRECTORY TARGET_NAME OUTPUT_DIRECTORY)
  if(MSVC)
    set(_output_dir_release "${OUTPUT_DIRECTORY}/Release")
    set(_output_dir_debug "${OUTPUT_DIRECTORY}/Debug")
  else()
    set(_output_dir_release "${OUTPUT_DIRECTORY}")
    set(_output_dir_debug "${OUTPUT_DIRECTORY}")
  endif()
  set_target_properties(
    "${TARGET_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_output_dir_release}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${_output_dir_debug}"
  )
endfunction()

# Set output directory for an executable which is a code-generated and not
# intended to be distributed.
function(NIXIETRACKER_SET_GENERATOR_TARGET_RUNTIME_DIRECTORY TARGET_NAME)
  NIXIETRACKER_SET_TARGET_RUNTIME_DIRECTORY(
    "${TARGET_NAME}"
    "${NIXIETRACKER_GENERATORS_OUTPUT_PATH}"
  )
endfunction()

# Add new test target.
#
# Will use sourve file of ${NAME}_test.cc, compile it and link against
# ${EXTRA_LIBS} and add executable to ctests.
function(NIXIETRACKER_TEST NAME)
  if(WITH_TESTS)
    cmake_parse_arguments(
      TEST
      ""
      "MODULE"
      "LIBRARIES;ARGUMENTS;DIRECTORY"
      ${ARGN}
    )
    set(_target_prefix "")
    if(NOT "${TEST_MODULE}" STREQUAL "")
      set(_target_prefix "${TEST_MODULE}_")
    endif()
    set(_target_name "${_target_prefix}${NAME}_test")
    set(_target_source "${NAME}_test.cc")
    if(NOT "${TEST_DIRECTORY}" STREQUAL "")
      set(_target_source "${TEST_DIRECTORY}/${_target_source}")
    endif()

    add_executable(
      "${_target_name}"
      "${_target_source}"
      ${EXTRA_SRC})
    target_link_libraries(
      "${_target_name}"
      ${TEST_LIBRARIES}  # Extra libs MUST be first.
      test_main
      ${GTEST_LIBRARIES}
      ${GLOG_LIBRARIES}
      ${GFLAGS_LIBRARIES}
      ${GMOCK_LIBRARIES}
      ${THREADS_LIBS})
    set_target_properties("${_target_name}" PROPERTIES FOLDER "Unit Tests")
    add_test(
      "${_target_name}"
      "${NIXIETRACKER_TESTS_OUTPUT_DIR}/${_target_name}"
      ${TEST_ARGUMENTS}
    )
    # Add extra include directories to the target so it finds GLog ad GFlags.
    set(_extra_include_directories
      ${GLOG_INCLUDE_DIRS}
      ${GFLAGS_INCLUDE_DIRS}
      ${GTEST_INCLUDE_DIRS}
      ${GMOCK_INCLUDE_DIRS}
    )
    target_include_directories(
      ${_target_name}
      PRIVATE ${_extra_include_directories}
    )
    # Make sure test is created in it's own dedicated directory.
    NIXIETRACKER_SET_TARGET_RUNTIME_DIRECTORY(
      "${_target_name}"
      "${NIXIETRACKER_TESTS_OUTPUT_DIR}"
    )
  endif()
endfunction()

macro(remove_cc_flag _flag)
  foreach(flag ${ARGV})
    string(REGEX REPLACE ${flag} "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE ${flag} "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE ${flag} "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE ${flag} "" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
    string(REGEX REPLACE ${flag} "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

    string(REGEX REPLACE ${flag} "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE ${flag} "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE ${flag} "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE ${flag} "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    string(REGEX REPLACE ${flag} "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  endforeach()
  unset(flag)
endmacro()

macro(add_cc_flag flag)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
endmacro()

macro(remove_strict_flags)
  if(CMAKE_COMPILER_IS_GNUCC)
    remove_cc_flag(
      "-Wstrict-prototypes"
      "-Wmissing-prototypes"
      "-Wmissing-declarations"
      "-Wmissing-format-attribute"
      "-Wunused-local-typedefs"
      "-Wunused-macros"
      "-Wunused-parameter"
      "-Wwrite-strings"
      "-Wredundant-decls"
      "-Wundef"
      "-Wshadow"
      "-Wdouble-promotion"
      "-Wold-style-definition"
      "-Werror=[^ ]+"
      "-Werror"
    )
    if(CMAKE_COMPILER_IS_GNUCC AND (NOT "${CMAKE_C_COMPILER_VERSION}" VERSION_LESS "7.0"))
        add_cc_flag(
          "-Wno-implicit-fallthrough"
        )
    endif()
    # Negate flags implied by '-Wall'.
    # TODO(sergey): This is a missing bit.
    # add_cc_flag("${CC_REMOVE_STRICT_FLAGS}")
  elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    remove_cc_flag(
      "-Wunused-parameter"
      "-Wunused-variable"
      "-Werror=[^ ]+"
      "-Werror"
    )
    # Negate flags implied by '-Wall'.
    # TODO(sergey): This is a missing bit.
    # add_cc_flag("${CC_REMOVE_STRICT_FLAGS}")
  elseif(MSVC)
    # TODO
  endif()
endmacro()

macro(remove_extra_strict_flags)
  if(CMAKE_COMPILER_IS_GNUCC)
    remove_cc_flag(
      "-Wunused-parameter"
    )
  elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    remove_cc_flag(
      "-Wunused-parameter"
      )
  elseif(MSVC)
    # TODO
  endif()
endmacro()
