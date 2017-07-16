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

#pragma once

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "gtest/gtest.h"

namespace NixieTracker {

#define EXPECT_FLOAT2_NEAR(expected, actual, tolerance)                 \
  do {                                                                  \
    float2 expected_ = expected;                                        \
    float2 actual_ = actual;                                            \
    EXPECT_NEAR(expected_.x, actual_.x, tolerance);                     \
    EXPECT_NEAR(expected_.y, actual_.y, tolerance);                     \
  } while (false);

#define EXPECT_FLOAT3_NEAR(expected, actual, tolerance)                 \
  do {                                                                  \
    float3 expected_ = expected;                                        \
    float3 actual_ = actual;                                            \
    EXPECT_NEAR(expected_.x, actual_.x, tolerance);                     \
    EXPECT_NEAR(expected_.y, actual_.y, tolerance);                     \
    EXPECT_NEAR(expected_.z, actual_.z, tolerance);                     \
  } while (false);

#define EXPECT_FLOAT4_NEAR(expected, actual, tolerance)                 \
  do {                                                                  \
    float4 expected_ = expected;                                        \
    float4 actual_ = actual;                                            \
    EXPECT_NEAR(expected_.x, actual_.x, tolerance);                     \
    EXPECT_NEAR(expected_.y, actual_.y, tolerance);                     \
    EXPECT_NEAR(expected_.z, actual_.z, tolerance);                     \
    EXPECT_NEAR(expected_.w, actual_.w, tolerance);                     \
  } while (false);

#define EXPECT_TRANSFORM_NEAR(expected, actual, tolerance)              \
  do {                                                                  \
    Transform _tfm_expected = expected;                                 \
    Transform _tfm_actual = actual;                                     \
    EXPECT_FLOAT4_NEAR(_tfm_expected.x, _tfm_actual.x, tolerance);      \
    EXPECT_FLOAT4_NEAR(_tfm_expected.y, _tfm_actual.y, tolerance);      \
    EXPECT_FLOAT4_NEAR(_tfm_expected.z, _tfm_actual.z, tolerance);      \
    EXPECT_FLOAT4_NEAR(_tfm_expected.w, _tfm_actual.w, tolerance);      \
  } while (false);

#define EXPECT_VECTOR_EQ(vec, element_type, ...)                \
  do {                                                          \
    element_type array[] = { __VA_ARGS__ };                     \
    size_t num_array_elements = sizeof(array) / sizeof(*array); \
    EXPECT_EQ(vec.size(), num_array_elements);                  \
    if (vec.size() == num_array_elements) {                     \
      for (size_t i = 0; i < num_array_elements; ++i) {         \
        EXPECT_EQ(vec[i], array[i]);                            \
      }                                                         \
    }                                                           \
  } while (false)

#define EXPECT_NULL(expression) EXPECT_EQ((expression), (void*)NULL)

// Force enable logging.
//
// Mainly used for mock log checks.
void testEnableLogging();

void testDisableExitOnDFatal();
void testEnableExitOnDFatal();

}  // namespace NixieTracker
