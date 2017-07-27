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

// Define this when there is an inverter between uC and shift registers.
#define USE_INVERTER

#ifdef USE_INVERTER
#  define SHIFT_EN_PULL_DOWN()   SHIFT_EN_On()
#  define SHIFT_EN_PULL_UP()     SHIFT_EN_Off()
#  define SHIFT_SRCK_PULL_DOWN() SHIFT_SRCK_On()
#  define SHIFT_SRCK_PULL_UP()   SHIFT_SRCK_Off()
#  define SHIFT_RCK_PULL_DOWN()  SHIFT_RCK_On()
#  define SHIFT_RCK_PULL_UP()    SHIFT_RCK_Off()
#  define SHIFT_DATA_PULL_DOWN() SHIFT_DATA_On()
#  define SHIFT_DATA_PULL_UP()   SHIFT_DATA_Off()
#  define SHIFT_DATA_SET(x)      SHIFT_DATA_StateSet(x ? 0 : 1)
#else
#  define SHIFT_EN_PULL_DOWN()   SHIFT_EN_Off()
#  define SHIFT_EN_PULL_UP()     SHIFT_EN_On()
#  define SHIFT_SRCK_PULL_DOWN() SHIFT_SRCK_Off()
#  define SHIFT_SRCK_PULL_UP()   SHIFT_SRCK_On()
#  define SHIFT_RCK_PULL_DOWN()  SHIFT_RCK_Off()
#  define SHIFT_RCK_PULL_UP()    SHIFT_RCK_On()
#  define SHIFT_RCK_DATA_DOWN()  SHIFT_DATA_Off()
#  define SHIFT_RCK_DATA_UP()    SHIFT_DATA_On()
#  define SHIFT_DATA_SET(x)      SHIFT_DATA_StateSet(x ? 1 : 0)
#endif

static void transmitBegin(AppShiftRegisterData* app_shift_register_data) {
  // Reset current status.
  app_shift_register_data->_private.send.current_byte = 0;
  app_shift_register_data->_private.send.current_bit = 0;
  app_shift_register_data->_private.send.counter = 0;
  // Make sure everything else is down.
  SHIFT_DATA_PULL_DOWN();
  SHIFT_RCK_PULL_DOWN();
  SHIFT_SRCK_PULL_DOWN();
  // We start with pulling ~G high,
  SHIFT_EN_PULL_UP();
  app_shift_register_data->state = APP_SHIFT_REGISTER_STATE_TRANSMIT_STEP;
}

static void transmitStep(AppShiftRegisterData* app_shift_register_data) {
  // NOTE: We are wired via inverter for shifting the level, so we are inverting
  // bits here as well.
  AppShiftRegisterData* data = app_shift_register_data;
  // Handle falling edge and go to the next bit when needed.
  if (data->_private.send.counter == 1) {
    SHIFT_SRCK_PULL_DOWN();
    // For the purposes of debug, we always bring DATA low.
    SHIFT_DATA_PULL_DOWN();
    // Advance to the next bit.
    ++data->_private.send.current_bit;
    data->_private.send.counter = 0;
    return;
  }
  // Move to the next byte when needed.
  if (data->_private.send.current_bit == 8) {
    data->_private.send.current_bit = 0;
    ++data->_private.send.current_byte;
  }
  // Check whether all data was transferred.
  if (data->_private.send.current_byte == data->_private.send.num_bytes) {
    data->state = APP_SHIFT_REGISTER_STATE_TRANSMIT_FINISH;
    return;
  }
  const uint8_t current_byte =
      data->_private.send.data[data->_private.send.current_byte];
  const uint8_t value = current_byte & (1 << (8 - data->_private.send.current_bit));
  // Set new value on the serial output.
  SHIFT_DATA_SET(value);
  // NOTE: Sampling happens on the raising edge.
  // NOTE: Falling edge will happen on the next iteration of the state machine.
  SHIFT_SRCK_PULL_UP();
  ++data->_private.send.counter;
}

static void transmitFinish(AppShiftRegisterData* app_shift_register_data) {
  AppShiftRegisterData* data = app_shift_register_data;
  if (data->_private.send.counter == 1) {
    // Transaction finished.
    SHIFT_RCK_PULL_DOWN();
    SHIFT_EN_PULL_DOWN();
    app_shift_register_data->state = APP_SHIFT_REGISTER_STATE_IDLE;
    return;
  }
  // Toggle RCK to copy data from shift register to storage.
  SHIFT_RCK_PULL_UP();
  ++data->_private.send.counter;
}

void APP_ShiftRegister_Initialize(
    AppShiftRegisterData* app_shift_register_data) {
  app_shift_register_data->state = APP_SHIFT_REGISTER_STATE_IDLE;
  SHIFT_EN_PULL_DOWN();
  SHIFT_DATA_PULL_DOWN();
  SHIFT_RCK_PULL_DOWN();
  SHIFT_SRCK_PULL_DOWN();
}

void APP_ShiftRegister_Tasks(AppShiftRegisterData* app_shift_register_data) {
  switch (app_shift_register_data->state) {
    case APP_SHIFT_REGISTER_STATE_IDLE:
      // Nothing to do.
      break;
    case APP_SHIFT_REGISTER_STATE_TRANSMIT_BEGIN:
      transmitBegin(app_shift_register_data);
      break;
    case APP_SHIFT_REGISTER_STATE_TRANSMIT_STEP:
      transmitStep(app_shift_register_data);
      break;
    case APP_SHIFT_REGISTER_STATE_TRANSMIT_FINISH:
      transmitFinish(app_shift_register_data);
      break;
  }
}

bool APP_ShiftRegister_IsBusy(AppShiftRegisterData* app_shift_register_data) {
  return (app_shift_register_data->state != APP_SHIFT_REGISTER_STATE_IDLE);
}

void APP_ShiftRegister_SetEnabled(
    AppShiftRegisterData* app_shift_register_data,
    bool is_enabled) {
  if (is_enabled) {
    SHIFT_EN_PULL_DOWN();
  } else {
    SHIFT_EN_PULL_UP();
  }
}

void APP_ShiftRegister_SendData(
    AppShiftRegisterData* app_shift_register_data,
    uint8_t* data,
    size_t num_bytes) {
  // TODO(sergey): Check shift registers are ready for data transmit.
  SHIFT_REGISTER_DEBUG_PRINT("Begin transmittance of %d bytes.\r\n", num_bytes);
  memcpy(app_shift_register_data->_private.send.data,
         data,
         sizeof(app_shift_register_data->_private.send.data));
  // TODO(sergey): Clip num_bytes here.
  app_shift_register_data->_private.send.num_bytes = num_bytes;
  // Begin transmittance.
  app_shift_register_data->state = APP_SHIFT_REGISTER_STATE_TRANSMIT_BEGIN;
}
