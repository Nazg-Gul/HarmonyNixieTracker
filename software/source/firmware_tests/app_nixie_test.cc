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
#include <vector>

extern "C" {
#include "app_https_client.h"
#include "app_nixie.h"
#include "app_shift_register.h"
}

extern "C" {

bool APP_HTTPS_Client_Request(AppHTTPSClientData* app_https_client_data,
                              const char /*url*/[MAX_URL],
                              const AppHttpsClientCallbacks* callbacks) {
  app_https_client_data->callbacks = *callbacks;
  return true;
}

bool APP_HTTPS_Client_IsBusy(AppHTTPSClientData* /*app_https_client_data*/) {
  return false;
}

void APP_ShiftRegister_SendData(
    AppShiftRegisterData* /*app_shift_register_data*/,
    uint8_t* /*data*/,
    size_t /*num_bytes*/) {
}

bool APP_ShiftRegister_IsBusy(
    AppShiftRegisterData* /*app_shift_register_data*/) {
  return false;
}

}  // extern "C"

namespace NixieTracker {

using std::vector;
using std::string;

namespace {

class FragmentedSender {
 public:
  explicit FragmentedSender(const AppHttpsClientCallbacks& callbacks)
    : callbacks_(callbacks) {
  }

  void sendData(const vector<string>& chunks) {
    for (const string& chunk : chunks) {
      callbacks_.buffer_received(reinterpret_cast<const uint8_t*>(chunk.data()),
                                 chunk.size(),
                                 callbacks_.user_data);
    }
    callbacks_.request_handled(callbacks_.user_data);
  }

 protected:
  AppHttpsClientCallbacks callbacks_;
};

void pokeAppNixieWithReceivedData(AppNixieData* app_nixie_data,
                                  const vector<string>& data_chunks) {
  AppHTTPSClientData app_https_client_data = {(AppHTTPSClientIPMode)0};
  AppShiftRegisterData app_shift_register_data = {(AppShiftRegisterState)0};
  APP_Nixie_Initialize(app_nixie_data,
                       &app_https_client_data,
                       &app_shift_register_data);
  app_nixie_data->state = APP_NIXIE_STATE_BEGIN_HTTP_REQUEST;
  // Make sure state machine is ready for data.
  while (app_nixie_data->state != APP_NIXIE_STATE_WAIT_HTTPS_RESPONSE) {
    APP_Nixie_Tasks(app_nixie_data);
  }
  // Send all the chunks, one by one.
  FragmentedSender sender(app_https_client_data.callbacks);
  sender.sendData(data_chunks);
  // Wait for the state machine to do all tasks related on data post-receive.
  while (app_nixie_data->state != APP_NIXIE_STATE_BEGIN_DISPLAY_SEQUENCE &&
         app_nixie_data->state != APP_NIXIE_STATE_ERROR &&
         app_nixie_data->state != APP_NIXIE_STATE_IDLE) {
    APP_Nixie_Tasks(app_nixie_data);
  }
}

string displayValueAsString(const AppNixieData& app_nixie_data) {
  return string(app_nixie_data.display_value, app_nixie_data.num_nixies);
}

void expectDisplayValue(const AppNixieData& app_nixie_data,
                        const char* expected_value) {
  string actual_value = displayValueAsString(app_nixie_data);
  std::reverse(actual_value.begin(), actual_value.end());
  EXPECT_EQ(actual_value, expected_value);
}

}  // namespace

TEST(AppNixie, CyclicBufferRefill) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"012345678901234567890123",
       ">Open Tasks (1234)xxxxxxxxxxxxxxxxxxxxx",
       "xxxxxxxxxxxxxxxxxxxxxxx"
       });
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "1234");
}

TEST(AppNixie, CyclicBufferSingleSmallBuffer) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data, {"0123456789"});
  EXPECT_EQ(memcmp(app_nixie_data.cyclic_buffer, "0123456789", 10), 0);
}

TEST(AppNixie, CyclicBufferSingleHugeBuffer) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data,
    {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"});
  EXPECT_EQ(memcmp(app_nixie_data.cyclic_buffer, "TUVWXYZ0123456789", 17), 0);
}

TEST(AppNixie, CyclicBufferSMultipleSmallBuffers) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data,
    {"abcdefghijk",
     "lmnopqrstuv",
     "wxyzABCDEFG",
     "HIJKLMNOPQR",
     "STUVWXYZ012",
     "3456789"});
  EXPECT_EQ(memcmp(app_nixie_data.cyclic_buffer, "TUVWXYZ0123456789", 17), 0);
}

TEST(AppNixie, CyclicBufferMultipleHugeBuffer) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data,
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
     "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
     "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz",
     "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"});
  EXPECT_EQ(memcmp(app_nixie_data.cyclic_buffer, "TUVWXYZ0123456789", 17), 0);
}

TEST(AppNixie, SingleBufferWithoutValueAndNoToken) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data, {"Hello, World!"});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleHugeBufferWithoutValueAndNoToken) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleBufferWithoutValueAndWithTokenAtTheEnd) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data, {">Open Tasks "});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleHugeBufferWithoutValueAndWithTokenAtTheEnd) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       ">Open Tasks "});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleHugeBufferWithoutValueAndWithTokenAtTheBeginning) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {">Open Tasks "
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleHugeBufferWithoutValueAndWithTokenInTheMiddle) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       ">Open Tasks "
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"});
  EXPECT_FALSE(app_nixie_data.is_value_parsed);
}

TEST(AppNixie, SingleBufferWithValue) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data, {">Open Tasks (123)<"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, SingleHugeBufferWithValue) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       ">Open Tasks (123)<"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, SingleBufferWithValueAtTheEnd) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data, {">Open Tasks (123"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, MultipleBuffersWithValue) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data,
                               {">Open ", "Tasks ", "(123)<", "tail"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, MultipleBuffersWithValueAtTheEnd) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(&app_nixie_data,
                               {">Open ", "Tasks ", "(123"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, MultipleBuffersWithValueAndHugePrefix) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
       ">Open ", "Tasks ", "(123)<", "tail"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, MultipleBuffersWithValueAndTokenInHugePrefix) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       ">Open Tasks (",
       "123)<", "tail"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

TEST(AppNixie, MultipleBuffersWithValueAndPartialTokenInHugePrefix) {
  AppNixieData app_nixie_data = {NULL};
  pokeAppNixieWithReceivedData(
      &app_nixie_data,
      {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
       ">Open ",
       "Tasks ", "(123)<", "tail"});
  EXPECT_TRUE(app_nixie_data.is_value_parsed);
  expectDisplayValue(app_nixie_data, "0123");
}

}  // namespace NixieTracker
