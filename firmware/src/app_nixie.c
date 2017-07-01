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

#include "app_nixie.h"

#include <string.h>

#include "system_definitions.h"
#include "utildefines.h"

#include "app_shift_register.h"

#define NIXIE_DISPLAY_FORMAT "%c%c%c%c"
#define NIXIE_DISPLAY_VALUES(value)  \
  value[3] ? value[3] : ' ',         \
  value[2] ? value[2] : ' ',         \
  value[1] ? value[1] : ' ',         \
  value[0] ? value[0] : ' '

#define LOG_PREFIX "APP NIXIE: "

// Regular print / message.
#define NIXIE_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define NIXIE_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define NIXIE_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Nixie tube specific routines.
//
// TODO(sergey): Might be interesting to move those to public API.

static int8_t IN12A_SymbolToCathodeIndex(char symbol) {
  if (symbol == '0') {
    return 2;
  } else  if (symbol > '0' && symbol <= '9') {
    return 11 - (symbol - '0');
  }
  return -1;
}

static int8_t IN12B_SymbolToCathodeIndex(char symbol) {
  if (symbol == '0') {
    return 2;
  } else  if (symbol > '0' && symbol <= '9') {
    return 11 - (symbol - '0');
  } else if (symbol == ',') {
    return 12;
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

const char* nixieTypeStringify(NixieType type) {
  switch (type) {
    case NIXIE_TYPE_IN12A:
      return "IN-12A";
    case NIXIE_TYPE_IN12B:
      return "IN-12B";
  }
  SYS_ASSERT(0, "\r\nUnhandled nixie type.\r\n");
  return "unknown";
}

// Returns -1 if there is no cathode for the requested symbol.
static int8_t nixieSymbolToCathodeIndex(NixieType type, char symbol) {
  switch (type) {
    case NIXIE_TYPE_IN12A:
      return IN12A_SymbolToCathodeIndex(symbol);
    case NIXIE_TYPE_IN12B:
      return IN12B_SymbolToCathodeIndex(symbol);
  }
  SYS_ASSERT(0, "\r\nUnhandled nixie type.\r\n");
  return -1;
}

// Decode display value into cathode indices.
static void decodeDisplayValue(AppNixieData* app_nixie_data) {
  int8_t i;
  for (i = 0; i < app_nixie_data->num_nixies; ++i) {
    app_nixie_data->cathodes[i] = nixieSymbolToCathodeIndex(
        app_nixie_data->nixie_types[i],
        app_nixie_data->display_value[i]);
  }
  app_nixie_data->state = APP_NIXIE_STATE_ENCODE_SHIFT_REGISTER;
#ifdef SYS_CMD_REMAP_SYS_DEBUG_MESSAGE
  {
    NIXIE_DEBUG_MESSAGE("Cathode indices:");
    for (i = 0; i < app_nixie_data->num_nixies; ++i) {
      SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, " %d", app_nixie_data->cathodes[i]);
    }
    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\n");
  }
#endif
}

// Encode requested cathode indices to sift register states, taking actual
// wiring into account.
static void encodeShiftRegister(AppNixieData* app_nixie_data) {
  int8_t i;
  // Reset all the registers.
  memset(app_nixie_data->register_shift_state,
         0,
         sizeof(app_nixie_data->register_shift_state));
  // Now iterate over all requested cathodes and set corresponding bits of
  // the shift register.
  for (i = 0; i < app_nixie_data->num_nixies; ++i) {
    const int8_t cathode = app_nixie_data->cathodes[i];
    if (cathode == -1) {
      // TODO(sergey): Need to set corresponding enabled input of shift
      // register to OFF, but it's not possible with current hardware version.
      continue;
    }
    const int8_t byte = app_nixie_data->cathode_mapping[i][cathode].byte;
    const int8_t bit = app_nixie_data->cathode_mapping[i][cathode].bit;
    SYS_ASSERT(byte < app_nixie_data->num_shift_registers,
               "\r\nInvalid shift register index");
    SYS_ASSERT(bit <= 8, "\r\nInvalid shift register bit");
    app_nixie_data->register_shift_state[byte] |= (1 << bit);
  }
  app_nixie_data->state = APP_NIXIE_STATE_WRITE_SHIFT_REGISTER;
#ifdef SYS_CMD_REMAP_SYS_DEBUG_MESSAGE
  {
    NIXIE_DEBUG_MESSAGE("Shift registers:");
    for (i = 0; i < app_nixie_data->num_shift_registers; ++i) {
      SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, " %x",
                      app_nixie_data->register_shift_state[i]);
    }
    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\n");
  }
#endif
}

static void writeShiftRegister(AppNixieData* app_nixie_data) {
  if (APP_ShiftRegister_IsBusy(app_nixie_data->app_shift_register_data)) {
    return;
  }
  APP_ShiftRegister_SendData(app_nixie_data->app_shift_register_data,
                             (uint8_t*)app_nixie_data->register_shift_state,
                             app_nixie_data->num_shift_registers);
  // TODO(sergey): Shall we wait for communication to be over before going idle?
  // TODO(sergey): Shall we enable shift registers here?
  app_nixie_data->state = APP_NIXIE_STATE_IDLE;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Nixie_Initialize(AppNixieData* app_nixie_data,
                          AppShiftRegisterData* app_shift_register_data) {
#define NIXIE_REGISTER_BEGIN(app_nixie_data)                           \
  do {                                                                 \
    AppNixieData* data = app_nixie_data;                               \
    data->num_nixies = 0;                                              \
    (void) 0
#define NIXIE_TUBE_BEGIN(type)                                         \
  do {                                                                 \
      data->nixie_types[data->num_nixies] = type;                      \
      NIXIE_DEBUG_PRINT("Adding %s to display.\r\n",                   \
                        nixieTypeStringify(type));                     \
      (void) 0
#define NIXIE_CATHODE(symbol, cathode_index, shift_byte, shift_bit)    \
  do {                                                                 \
    data->cathode_mapping[data->num_nixies][cathode_index].byte = shift_byte;  \
    data->cathode_mapping[data->num_nixies][cathode_index].bit = shift_bit;    \
  } while (false)
#define NIXIE_TUBE_END()                                               \
    ++data->num_nixies;                                                \
  } while (false)
#define NIXIE_REGISTER_END()                                           \
    NIXIE_DEBUG_PRINT("Registered display of %d tubes.\r\n",           \
                      data->num_nixies);                               \
  } while (false)

  app_nixie_data->state = APP_NIXIE_STATE_IDLE;
  app_nixie_data->app_shift_register_data = app_shift_register_data;

  // ======== Nixie display information =======
  // Fill in nixies information.
  // TODO(sergey): Make it some sort of runtime configuration?
  // TODO(sergey): Make it a proper wiring diagram here.
  NIXIE_REGISTER_BEGIN(app_nixie_data);
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);
      NIXIE_CATHODE('0', 2,  0, 0);
      NIXIE_CATHODE('9', 3,  0, 0);
      NIXIE_CATHODE('8', 4,  0, 0);
      NIXIE_CATHODE('7', 5,  0, 0);
      NIXIE_CATHODE('6', 6,  0, 0);
      NIXIE_CATHODE('5', 7,  0, 0);
      NIXIE_CATHODE('4', 8,  0, 0);
      NIXIE_CATHODE('3', 9,  0, 0);
      NIXIE_CATHODE('2', 10, 0, 0);
      NIXIE_CATHODE('1', 11, 0, 0);
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);
      NIXIE_CATHODE('0', 2,  0, 0);
      NIXIE_CATHODE('9', 3,  0, 0);
      NIXIE_CATHODE('8', 4,  0, 0);
      NIXIE_CATHODE('7', 5,  0, 0);
      NIXIE_CATHODE('6', 6,  0, 0);
      NIXIE_CATHODE('5', 7,  0, 0);
      NIXIE_CATHODE('4', 8,  0, 0);
      NIXIE_CATHODE('3', 9,  0, 0);
      NIXIE_CATHODE('2', 10, 0, 0);
      NIXIE_CATHODE('1', 11, 0, 0);
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);
      NIXIE_CATHODE('0', 2,  0, 0);
      NIXIE_CATHODE('9', 3,  0, 0);
      NIXIE_CATHODE('8', 4,  0, 0);
      NIXIE_CATHODE('7', 5,  0, 0);
      NIXIE_CATHODE('6', 6,  0, 0);
      NIXIE_CATHODE('5', 7,  0, 0);
      NIXIE_CATHODE('4', 8,  0, 0);
      NIXIE_CATHODE('3', 9,  0, 0);
      NIXIE_CATHODE('2', 10, 0, 0);
      NIXIE_CATHODE('1', 11, 0, 0);
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);
      NIXIE_CATHODE('0', 2,  0, 0);
      NIXIE_CATHODE('9', 3,  0, 0);
      NIXIE_CATHODE('8', 4,  0, 0);
      NIXIE_CATHODE('7', 5,  0, 0);
      NIXIE_CATHODE('6', 6,  0, 0);
      NIXIE_CATHODE('5', 7,  0, 0);
      NIXIE_CATHODE('4', 8,  0, 0);
      NIXIE_CATHODE('3', 9,  0, 0);
      NIXIE_CATHODE('2', 10, 0, 0);
      NIXIE_CATHODE('1', 11, 0, 0);
    NIXIE_TUBE_END();
  NIXIE_REGISTER_END();
  // ======== Support components information ========
  app_nixie_data->num_shift_registers = 6;
  // Everything is done.
  SYS_MESSAGE("Nixie tubes subsystem initialized.\r\n");

#undef NIXIE_REGISTER_BEGIN
#undef NIXIE_TUBE_BEGIN
#undef NIXIE_TUBE_END
#undef NIXIE_REGISTER_END
}

void APP_Nixie_Tasks(AppNixieData* app_nixie_data) {
  switch (app_nixie_data->state) {
    case APP_NIXIE_STATE_IDLE:
      // Nothing to do.
      // TODO(sergey): Check timer, and start fetching new value from server.
      break;

    case APP_NIXIE_STATE_BEGIN_DISPLAY_SEQUENCE:
      app_nixie_data->state = APP_NIXIE_STATE_DECODE_DISPLAY_VALUE;
      break;
    case APP_NIXIE_STATE_DECODE_DISPLAY_VALUE:
      decodeDisplayValue(app_nixie_data);
      break;
    case APP_NIXIE_STATE_ENCODE_SHIFT_REGISTER:
      encodeShiftRegister(app_nixie_data);
      break;
    case APP_NIXIE_STATE_WRITE_SHIFT_REGISTER:
      writeShiftRegister(app_nixie_data);
      break;
  }
}

bool APP_Nixie_IsBusy(AppNixieData* app_nixie_data) {
  return app_nixie_data->state != APP_NIXIE_STATE_IDLE;
}

bool APP_Nixie_Display(AppNixieData* app_nixie_data,
                       const char value[MAX_NIXIE_TUBES]) {
  if (APP_Nixie_IsBusy(app_nixie_data)) {
    return false;
  }
  NIXIE_DEBUG_PRINT("Requested to display " NIXIE_DISPLAY_FORMAT,
                    NIXIE_DISPLAY_VALUES(value));
  // Make sure all possibly unused digits are zeroed.
  memset(app_nixie_data->display_value,
         0, 
         sizeof(app_nixie_data->display_value));
  // Copy at max of display size digits.
  strncpy(app_nixie_data->display_value,
          value,
          sizeof(app_nixie_data->display_value));
  app_nixie_data->state = APP_NIXIE_STATE_BEGIN_DISPLAY_SEQUENCE;
  return true;
}
