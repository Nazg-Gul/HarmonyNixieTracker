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

set(FIRMWARE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../firmware/src/")

set(INC
  .
  ./stubs/
  ${FIRMWARE_SOURCE_DIR}
)

set(INC_SYS
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(fw_test_util_math ${FIRMWARE_SOURCE_DIR}/util_math.c
                              ${FIRMWARE_SOURCE_DIR}/util_math.h)
add_library(fw_test_util_string ${FIRMWARE_SOURCE_DIR}/util_string.c
                                ${FIRMWARE_SOURCE_DIR}/util_string.h
                                ${FIRMWARE_SOURCE_DIR}/gcc/memmem.c
                                ${FIRMWARE_SOURCE_DIR}/gcc/memmem.h)
target_link_libraries(fw_test_util_string fw_test_util_math)
add_library(fw_test_util_url ${FIRMWARE_SOURCE_DIR}/util_url.c
                             ${FIRMWARE_SOURCE_DIR}/util_url.h)
target_link_libraries(fw_test_util_url fw_test_util_string)

add_library(fw_test_app_nixie ${FIRMWARE_SOURCE_DIR}/app_nixie.c)
target_link_libraries(fw_test_app_nixie "fw_test_util_math;fw_test_util_string")

NIXIETRACKER_TEST(app_nixie   MODULE firmware LIBRARIES fw_test_app_nixie)
NIXIETRACKER_TEST(util_string MODULE firmware LIBRARIES fw_test_util_string)
NIXIETRACKER_TEST(util_url    MODULE firmware LIBRARIES fw_test_util_url)
