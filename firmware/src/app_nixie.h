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
#include <stddef.h>
#include <stdint.h>

#include "app_https_client.h"

struct AppHTTPSClientData;
struct AppShiftRegisterData;

// Maximum number of nixie tubes in the display.
#define MAX_NIXIE_TUBES 4
// Maximum number of shift registers guarding all the tubes.
#define NUM_NIXIE_SHIFT_REGISTERS 6
// Maximum number of cathodes in supported nixie tube type.
#define MAX_NIXIE_CATHODE 12
// Maximal length of token used for parsing HTML page.
#define MAX_NIXIE_TOKEN 64

typedef enum {
  NIXIE_TYPE_IN12A,
  NIXIE_TYPE_IN12B,
} NixieType;

typedef enum {
  // None of nixie type related tasks is to be performed.
  APP_NIXIE_STATE_IDLE,
  // Error was encountered.
  APP_NIXIE_STATE_ERROR,

  // Request value from server.
  APP_NIXIE_STATE_BEGIN_HTTP_REQUEST,
  // Wait HTTP(S) client to become available.
  // Will immediately send request to server,
  APP_NIXIE_STATE_WAIT_HTTPS_CLIENT,
  // Wait for the response from server.
  APP_NIXIE_STATE_WAIT_HTTPS_RESPONSE,
  // Shuffle digits in value to replace trailing '\0' with leading '0'.
  APP_NIXIE_STATE_SHUFFLE_SERVER_VALUE,

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
  struct AppHTTPSClientData* app_https_client_data;
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

  // ======== HTTP(S) request to server.
  char request_url[MAX_URL];
  // Token which comes prior to the "interesting" value in the HTML page.
  char token[MAX_NIXIE_TOKEN];
  size_t token_len;
  // Will be set to truth when proper value is parsed from the incoming buffers.
  bool is_value_parsed;
  // This buffer contains tail of the previously received data from server.
  // We are storing `MAX_NIXIE_TOKEN + MAX_NIXIE_TUBES` actual bytes there, rest
  // of the memory is reserved for run-time glueing of previous buffer with the
  // new buffer for token lookup.
  char cyclic_buffer[2 * (MAX_NIXIE_TOKEN + MAX_NIXIE_TUBES)];
  // Number of bytes in the cyclic buffer.
  size_t cyclic_buffer_len;
  // Maximum size of the cyclic size.
  // We only store actually required number of bytes, so we don't waste extra
  // time on substring search and memory shift.
  size_t max_cyclic_buffer_len;

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
                          struct AppHTTPSClientData* app_https_client_data,
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
