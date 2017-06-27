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

#include "app_shift_register.h"

#include "utildefines.h"
#include "system_definitions.h"

#define LOG_PREFIX "APP SHIFT REGISTER: "

// Regular print / message.
#define SHIFT_REGISTER_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define SHIFT_REGISTER_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define SHIFT_REGISTER_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define SHIFT_REGISTER_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define SHIFT_REGISTER_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define SHIFT_REGISTER_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

void APP_ShiftRegister_Initialize(
    AppShiftRegisterData* app_shift_register_data) {
  app_shift_register_data->state = APP_SHIFT_REGISTER_STATE_IDLE;
  APP_ShiftRegister_SetEnabled(app_shift_register_data, false);
  SHIFT_DATA_Off();
  SHIFT_RCK_Off();
  SHIFT_SRCK_Off();
}

void APP_ShiftRegister_Tasks(AppShiftRegisterData* app_shift_register_data) {
  switch (app_shift_register_data->state) {
    case APP_SHIFT_REGISTER_STATE_IDLE:
      // Nothing to do.
      break;
  }
}

bool APP_ShiftRegister_IsBusy(AppShiftRegisterData* app_shift_register_data) {
  return (app_shift_register_data->state != APP_SHIFT_REGISTER_STATE_IDLE);
}

void APP_ShiftRegister_SetEnabled(
    AppShiftRegisterData* app_shift_register_data,
    bool is_enabled) {
  // NOTE: Enabled input is inverting.
  SHIFT_EN_StateSet(is_enabled ? 0 : 1);
}
