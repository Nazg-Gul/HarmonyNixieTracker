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

#ifndef _APP_SHIFT_REGISTER_H
#define _APP_SHIFT_REGISTER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Maximum number of bytes to be sent to shift registers.
#define SHIFT_REGISTER_MAX_DATA 6

typedef enum {
  // No tasks to be performed.
  APP_SHIFT_REGISTER_STATE_IDLE,
  // Begin transmittance. Will start with pulling ~G high.
  APP_SHIFT_REGISTER_STATE_TRANSMIT_BEGIN,
  // Shift register state machine is in transmittance state.
  APP_SHIFT_REGISTER_STATE_TRANSMIT_STEP,
  // End transmittance. Will toggle RCK and pull ~G down.
  APP_SHIFT_REGISTER_STATE_TRANSMIT_FINISH,
} AppShiftRegisterState;

typedef struct AppShiftRegisterData {
  AppShiftRegisterState state;
  // Per-task storage.
  union {
    struct {
      // Data to be transfered.
      uint8_t data[SHIFT_REGISTER_MAX_DATA];
      size_t num_bytes;
      // Current pointer in the array.
      size_t current_byte;
      uint8_t current_bit;
      // To keep timings compatible with any length of the tranmission line
      // we don't change status too often. For example, we pull SRCK high on
      // first iteration of state machine and pull it down on the next one.
      uint8_t counter;
    } send;
  } _private;
} AppShiftRegisterData;

// Initialize shift register related application routines.
void APP_ShiftRegister_Initialize(
    AppShiftRegisterData* app_shift_register_data);

// Perform all shift register related tasks.
void APP_ShiftRegister_Tasks(AppShiftRegisterData* app_shift_register_data);

// Check whether shift register module is busy with any tasks.
bool APP_ShiftRegister_IsBusy(AppShiftRegisterData* app_shift_register_data);

// Set enabled state of the outputs.
void APP_ShiftRegister_SetEnabled(
    AppShiftRegisterData* app_shift_register_data,
    bool is_enabled);

// Send data to shift registers. Assumes all shift registers are daisy-chained.
// Starts with least significant bit of data[0].
void APP_ShiftRegister_SendData(
    AppShiftRegisterData* app_shift_register_data,
    uint8_t* data,
    size_t num_bytes);

#endif  // _APP_SHIFT_REGISTER_H
