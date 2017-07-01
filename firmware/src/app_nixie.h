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

#ifndef _APP_NIXIE_H
#define _APP_NIXIE_H

#include <stdbool.h>
#include <stdint.h>

struct AppShiftRegisterData;

// Maximum number of nixie tubes in the display.
#define MAX_NIXIE_TUBES 4
// Maximum number of shift registers guarding all the tubes.
#define NUM_NIXIE_SHIFT_REGISTERS 6
// Maximum number of cathodes in supported nixie tube type.
#define MAX_NIXIE_CATHODE 12

typedef enum {
  NIXIE_TYPE_IN12A,
  NIXIE_TYPE_IN12B,
} NixieType;

typedef enum {
  // None of nixie type related tasks is to be performed.
  APP_NIXIE_STATE_IDLE,

  // Display sequence gets value from AppNixieData::display_value and does all
  // the tasks needed to get that value shown on nixies.
  APP_NIXIE_STATE_BEGIN_DISPLAY_SEQUENCE,
  // Decode AppNixieData::display_value into a cathode numbers, stored in
  // AppNixieData::cathodes.
  APP_NIXIE_STATE_DECODE_DISPLAY_VALUE,
  // Encode decoded cathode indices to shift register bits.
  APP_NIXIE_STATE_ENCODE_SHIFT_REGISTER,
  // Push prepared shift register data to actual shift registers.
  APP_NIXIE_STATE_WRITE_SHIFT_REGISTER,
} AppNixieState;

// Correspondence between cathode and shift register bit.
//
// Shift register bit is denoted by the byte (aka, shift register index) and
// bit index within that byte,
typedef struct NixieCathodeBit {
  int8_t byte;
  int8_t bit;
} NixieCathodeBit;

typedef struct AppNixieData {
  struct AppShiftRegisterData* app_shift_register_data;

  AppNixieState state;

  // ======== Static information about display ========
  // Number of nixie tubes in the display.
  int8_t num_nixies;
  // Information about nixie tubes used for display.
  // nixies[0] is the least significant digit (nixie is on the right).
  NixieType nixie_types[MAX_NIXIE_TUBES];
  // Mapping of cathodes to shift register bits, for each of the tubes in the
  // display.
  NixieCathodeBit cathode_mapping[MAX_NIXIE_TUBES][MAX_NIXIE_CATHODE];
  // Number of shift registers guarding the display.
  int8_t num_shift_registers;

  // ======== Display routines ========
  // Value requested to be displayed.
  char display_value[MAX_NIXIE_TUBES];
  // Per-nixie digit number of cathode to glow.
  // Cathode index of -1 means no cathode is to glow.
  //
  // NOTE: We consider cathode index is the same as pin number.
  int8_t cathodes[MAX_NIXIE_TUBES];
  // State of shift registers corresponding to the requested cathodes.
  // TODO(sergey): Make it more obvious name, and thing of naming conflict with
  // `app_*_data` names, since this array is kind of a data.
  int8_t register_shift_state[NUM_NIXIE_SHIFT_REGISTERS];
} AppNixieData;

// Initialize nixie types and state machine.
void APP_Nixie_Initialize(AppNixieData* app_nixie_data,
                          struct AppShiftRegisterData* app_shift_register_data);

// Perform periodic tasks related on nixie types.
void APP_Nixie_Tasks(AppNixieData* app_nixie_data);

// Check whether nixie module is busy with any tasks.
bool APP_Nixie_IsBusy(AppNixieData* app_nixie_data);

// Show given string on display.
//
// Returns truth on success.
bool APP_Nixie_Display(AppNixieData* app_nixie_data,
                       const char value[MAX_NIXIE_TUBES]);

#endif  // _APP_NIXIE_H
