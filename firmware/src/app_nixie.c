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
#include "util_math.h"
#include "util_string.h"

#include "app_network.h"
#include "app_shift_register.h"

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

// Interval for periodic tasks in seconds.
#define PERIODIC_INTERVAL_FAST    5
#define PERIODIC_INTERVAL_NORMAL  30

////////////////////////////////////////////////////////////////////////////////
// Nixie tube specific routines.
//
// TODO(sergey): Might be interesting to move those to public API.

static int8_t IN12A_SymbolToCathodeIndex(char symbol) {
  if (symbol == '0') {
    return 2;
  } else  if (symbol > '0' && symbol <= '9') {
    return 12 - (symbol - '0');
  }
  return -1;
}

static int8_t IN12B_SymbolToCathodeIndex(char symbol) {
  if (symbol == '0') {
    return 2;
  } else  if (symbol > '0' && symbol <= '9') {
    return 12 - (symbol - '0');
  } else if (symbol == ',') {
    return 12;
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

////////////////////////////////////////
// submit HTTP(S) request.

// Get value from buffer pointing to the beginning of the value.
static void parseValueFromBuffer(AppNixieData* app_nixie_data,
                                 const char* buffer,
                                 const size_t buffer_len) {
  size_t index = 0;
  size_t num_bytes = min_zz(buffer_len, app_nixie_data->num_nixies);
  // Copy value from the buffer.
  while (index < num_bytes) {
    const char ch = buffer[index];
    if (ch >= '0' && ch <= '9') {
      app_nixie_data->display_value[app_nixie_data->num_nixies - index - 1] = ch;
    } else {
      break;
    }
    ++index;
  }
  // Zero out all unused digits.
  // TODO(sergey): Shift to the right, and set MSB to 0?
  while (index < sizeof(app_nixie_data->display_value)) {
    app_nixie_data->display_value[app_nixie_data->num_nixies - index++ - 1] = '\0';
  }
  NIXIE_DEBUG_PRINT("Parsed value " NIXIE_DISPLAY_FORMAT "\r\n",
                    NIXIE_DISPLAY_VALUES(app_nixie_data->display_value));
  app_nixie_data->is_value_parsed = true;
}

static void bufferReceivedCallback(const uint8_t* buffer,
                                   uint16_t num_bytes,
                                   void* user_data) {
  AppNixieData* app_nixie_data = (AppNixieData*)user_data;
  const char* found;
  if (app_nixie_data->is_value_parsed) {
    // Value is already parsed, no need to waste time trying to find token
    // and such here now.
    return;
  }
  // Number of bytes required to be received from server starting at the
  // token.
  const size_t required_len = app_nixie_data->token_len +
                              app_nixie_data->num_nixies;
  // Maximum number of bytes we can append to the existing cyclic buffer.
  const size_t append_suffix_len =
      min_zz(num_bytes, app_nixie_data->max_cyclic_buffer_len -
                        app_nixie_data->cyclic_buffer_len);
  // Append bytes to the existing cyclic buffer.
  memcpy(app_nixie_data->cyclic_buffer + app_nixie_data->cyclic_buffer_len,
         buffer,
         append_suffix_len);
  app_nixie_data->cyclic_buffer_len += append_suffix_len;
  // Check whether token is in the cyclic buffer with all the possible data
  // added from the new buffer.
  found = strstr_len(app_nixie_data->cyclic_buffer,
                     app_nixie_data->token,
                     app_nixie_data->cyclic_buffer_len);
  if (found != NULL) {
    // If token is in the cyclic buffer, we always make it starting at the
    // beginning of the buffer.
    const size_t prefix_len = found - app_nixie_data->cyclic_buffer;
    if (prefix_len != 0) {
      memmove(app_nixie_data->cyclic_buffer,
              found,
              app_nixie_data->cyclic_buffer_len - prefix_len);
      app_nixie_data->cyclic_buffer_len -= prefix_len;
      // Fill in freed bytes with extra data from received buffer.
      if (num_bytes != append_suffix_len) {
        const size_t extra_suffix_len = min_zz(prefix_len,
                                               num_bytes - append_suffix_len);
        memcpy(
            app_nixie_data->cyclic_buffer + app_nixie_data->cyclic_buffer_len,
               buffer + append_suffix_len,
               extra_suffix_len);
        app_nixie_data->cyclic_buffer_len += extra_suffix_len;
      }
    }
    // Check whether we've got enough bytes from server to not cut value in
    // the middle.
    if (app_nixie_data->cyclic_buffer_len < required_len) {
      // Wait for extra bytes from server.
      //
      // NOTE: At this point we've used as much data of the buffer as we could
      // possibly do.
      return;
    }
    // Parse value from the buffer.
    parseValueFromBuffer(
        app_nixie_data,
        app_nixie_data->cyclic_buffer + app_nixie_data->token_len,
        app_nixie_data->cyclic_buffer_len - app_nixie_data->token_len);
    return;
  }
  // Check whether token is in the new buffer.
  found = strstr_len((char*)buffer, app_nixie_data->token, num_bytes);
  if (found == NULL) {
    // Discards the previously appended suffix to avoid possible data
    // duplication.
    app_nixie_data->cyclic_buffer_len -= append_suffix_len;
    // If token can't be found in the buffer, we chop some suffix from if and
    // store in the cyclic buffer for the further investigation when new data
    // comes in.
    //
    // NOTE: We keep max of token_len + num_nixies bytes in the cyclic buffer.
    const size_t max_store_bytes = app_nixie_data->token_len +
                                   app_nixie_data->num_nixies;
    // Number of bytes to be copied from tail of the new buffer.
    const size_t num_new_bytes = min_zz(num_bytes, max_store_bytes);
    // Number of bytes to be kept from the existing cyclic buffer.
    const size_t num_old_bytes = min_zz(app_nixie_data->cyclic_buffer_len,
                                        max_store_bytes - num_new_bytes);
    if (num_old_bytes != 0) {
      memmove(
          app_nixie_data->cyclic_buffer,
          app_nixie_data->cyclic_buffer + app_nixie_data->cyclic_buffer_len
                                        - num_old_bytes,
          num_old_bytes);
    }
    memcpy(app_nixie_data->cyclic_buffer + num_old_bytes,
           buffer + num_bytes - num_new_bytes,
           num_new_bytes);
    app_nixie_data->cyclic_buffer_len = num_new_bytes + num_old_bytes;
  } else {
    const size_t prefix_len = found - (char*)buffer;
    if (num_bytes - prefix_len >= required_len) {
      // We have enough bytes in input buffer to get the whole value.
      parseValueFromBuffer(
          app_nixie_data,
          (char*)buffer + prefix_len + app_nixie_data->token_len,
          num_bytes - app_nixie_data->token_len - prefix_len);
    } else {
      // Need to wait for more bytes from server.
      memcpy(app_nixie_data->cyclic_buffer,
             found,
             num_bytes - prefix_len);
      app_nixie_data->cyclic_buffer_len = num_bytes - prefix_len;
    }
  }
}

static void requestHandledCallback(void* user_data) {
  AppNixieData* app_nixie_data = (AppNixieData*)user_data;
  NIXIE_DEBUG_PRINT("HTTP(S) transaction finished.\r\n");
  if (!app_nixie_data->is_value_parsed) {
    const char* found = strstr_len(app_nixie_data->cyclic_buffer,
                                   app_nixie_data->token,
                                   app_nixie_data->cyclic_buffer_len);
    if (found != NULL) {
      parseValueFromBuffer(
          app_nixie_data,
          app_nixie_data->cyclic_buffer + app_nixie_data->token_len,
          app_nixie_data->cyclic_buffer_len - app_nixie_data->token_len);
    }
  }
  if (app_nixie_data->is_value_parsed) {
    app_nixie_data->state = APP_NIXIE_STATE_SHUFFLE_SERVER_VALUE;
  } else {
    NIXIE_ERROR_PRINT("Value was not found in the server response.\r\n");
    app_nixie_data->state = APP_NIXIE_STATE_IDLE;
  }
}

static void errorCallback(void* user_data) {
  AppNixieData* app_nixie_data = (AppNixieData*)user_data;
  NIXIE_ERROR_MESSAGE("Error occurred during HTTP(S) transaction.\r\n");
  app_nixie_data->state = APP_NIXIE_STATE_ERROR;
}

static void waitHttpsClientAndSendRequest(AppNixieData* app_nixie_data) {
  if (APP_HTTPS_Client_IsBusy(app_nixie_data->app_https_client_data)) {
    return;
  }
  // Reset some values form previous run.
  app_nixie_data->is_value_parsed = false;
  app_nixie_data->cyclic_buffer_len = 0;
  // Prepare callbacks for HTTP(S) module.
  AppHttpsClientCallbacks callbacks;
  callbacks.buffer_received = bufferReceivedCallback;
  callbacks.request_handled = requestHandledCallback;
  callbacks.error = errorCallback;
  callbacks.user_data = app_nixie_data;
  // NOTE: It is important to submit request now, because HTTP(s) client might
  // become busy at the next state machine iteration.
  if (!APP_HTTPS_Client_Request(app_nixie_data->app_https_client_data,
                                app_nixie_data->request_url,
                                &callbacks)) {
    // TODO(sergey): Provide some more details?
    NIXIE_ERROR_PRINT("Error submitting HTTP(S) request to %s.\r\n",
                      app_nixie_data->request_url);
    app_nixie_data->state = APP_NIXIE_STATE_ERROR;
    return;
  }
  NIXIE_DEBUG_PRINT("Submitted HTTP(S) request to %s.\r\n",
                    app_nixie_data->request_url);
  app_nixie_data->state = APP_NIXIE_STATE_WAIT_HTTPS_RESPONSE;
}

static void shuffleServerValueDigits(AppNixieData* app_nixie_data) {
  int a;
  int num_leading_null = 0;
  for (a = 0; a < app_nixie_data->num_nixies; ++a) {
    if (app_nixie_data->display_value[a] != '\0') {
      break;
    }
    ++num_leading_null;
  }
  if (num_leading_null == 0) {
    NIXIE_DEBUG_MESSAGE("Leaving value unchanged.\r\n");
  } else {
    memmove(app_nixie_data->display_value,
            app_nixie_data->display_value + num_leading_null,
            app_nixie_data->num_nixies - num_leading_null);
    for (a = 0; a < num_leading_null; ++a) {
      app_nixie_data->display_value[app_nixie_data->num_nixies - a - 1] = '0';
    }
    NIXIE_DEBUG_PRINT("Value after shuffle " NIXIE_DISPLAY_FORMAT "\r\n",
                      NIXIE_DISPLAY_VALUES(app_nixie_data->display_value));
  }
  if (app_nixie_data->display_value_out != NULL) {
    app_nixie_data->state = APP_NIXIE_STATE_IDLE;
    memcpy(app_nixie_data->display_value_out,
           app_nixie_data->display_value,
           sizeof(app_nixie_data->display_value));
    *app_nixie_data->is_fetched_out = true;
    // Get rid of pointers.
    app_nixie_data->display_value_out = NULL;
    app_nixie_data->is_fetched_out = NULL;
  } else {
    app_nixie_data->state = APP_NIXIE_STATE_BEGIN_DISPLAY_SEQUENCE;
    NIXIE_MESSAGE("Sending HTTP request.\r\n");
    NIXIE_PRINT("Will display " NIXIE_DISPLAY_FORMAT "\r\n",
                NIXIE_DISPLAY_VALUES(app_nixie_data->display_value));
  }
}

////////////////////////////////////////
// Display requested value.

#ifdef SYS_CMD_REMAP_SYS_DEBUG_MESSAGE
static const char* nixieTypeStringify(NixieType type) {
  switch (type) {
    case NIXIE_TYPE_IN12A:
      return "IN-12A";
    case NIXIE_TYPE_IN12B:
      return "IN-12B";
  }
  SYS_ASSERT(0, "\r\nUnhandled nixie type.\r\n");
  return "unknown";
}
#endif

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
    const int8_t nixie_index = app_nixie_data->num_nixies - i - 1;
    app_nixie_data->cathodes[i] = nixieSymbolToCathodeIndex(
        app_nixie_data->nixie_types[nixie_index],
        app_nixie_data->display_value[nixie_index]);
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
    const int num_byte = app_nixie_data->num_shift_registers - byte - 1;
    app_nixie_data->register_shift_state[num_byte] |= (1 << bit);
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
                             app_nixie_data->register_shift_state,
                             app_nixie_data->num_shift_registers);
  // TODO(sergey): Shall we wait for communication to be over before going idle?
  // TODO(sergey): Shall we enable shift registers here?
  app_nixie_data->state = APP_NIXIE_STATE_IDLE;
}

////////////////////////////////////////
// Periodic tasks.

typedef enum PeriodicTime {
  PERIODIC_TIME_FAST,
  PERIODIC_TIME_NORMAL,
} PeriodicTime;

static void schedulePeriodicTask(AppNixieData* app_nixie_data,
                                 PeriodicTime time) {
  uint64_t interval;
  switch (time) {
    case PERIODIC_TIME_FAST:
      interval = PERIODIC_INTERVAL_FAST;
      break;
    case PERIODIC_TIME_NORMAL:
      interval = PERIODIC_INTERVAL_NORMAL;
      break;
  }
  app_nixie_data->periodic_next_time =
      SYS_TMR_SystemCountGet() + SYS_TMR_SystemCountFrequencyGet() * interval;
}

static void performPeriodicTasks(AppNixieData* app_nixie_data) {
  if (!app_nixie_data->periodic_tasks_enabled) {
    // Periodic tasks are not enabled, so we shouldn't be doing anything here.
    return;
  }
  if (SYS_TMR_SystemCountGet() < app_nixie_data->periodic_next_time) {
    // The time for next periodic tasks did not come yet.
    return;
  }
  if (APP_Nixie_IsBusy(app_nixie_data)) {
    NIXIE_DEBUG_MESSAGE("Application is busy, waiting to become available.\r\n");
    return;
  }
  if (!APP_Network_hasUsableInterface()) {
    return;
  }
  NIXIE_DEBUG_MESSAGE("Performing periodic tasks.\r\n");
  NIXIE_MESSAGE("Sending HTTP request.\r\n");
  app_nixie_data->state = APP_NIXIE_STATE_BEGIN_HTTP_REQUEST;
  app_nixie_data->task_from_periodic = true;
  // Schedule next periodic task.
  schedulePeriodicTask(app_nixie_data, PERIODIC_TIME_NORMAL);
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Nixie_Initialize(AppNixieData* app_nixie_data,
                          AppHTTPSClientData* app_https_client_data,
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
  app_nixie_data->app_https_client_data = app_https_client_data;
  app_nixie_data->app_shift_register_data = app_shift_register_data;
  app_nixie_data->display_value_out = NULL;

  // Set up periodic tasks to fire up as soon as possible.
  app_nixie_data->periodic_tasks_enabled = true;
  app_nixie_data->periodic_next_time = 0;
  app_nixie_data->task_from_periodic = false;

  // ======== Nixie display information =======
  // Fill in nixies information.
  // TODO(sergey): Make it some sort of runtime configuration?
  // TODO(sergey): Make it a proper wiring diagram here.
  NIXIE_REGISTER_BEGIN(app_nixie_data);
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);  /* J2 */
      // NIXIE_CATHODE('.', 12, 3, 2);  /* 2 */
      NIXIE_CATHODE('0', 2,  3, 3);  /* 1 */
      NIXIE_CATHODE('9', 3,  3, 1);  /* 4 */
      NIXIE_CATHODE('8', 4,  3, 4);  /* 3 */
      NIXIE_CATHODE('7', 5,  3, 0);  /* 6 */
      NIXIE_CATHODE('6', 6,  3, 5);  /* 5 */
      NIXIE_CATHODE('5', 7,  2, 7);  /* 8 */
      NIXIE_CATHODE('4', 8,  3, 6);  /* 7 */
      NIXIE_CATHODE('3', 9,  2, 6);  /* 10 */
      NIXIE_CATHODE('2', 10, 3, 7);  /* 9 */
      NIXIE_CATHODE('1', 11, 2, 5);  /* 12 */
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);  /* J3 */
      // NIXIE_CATHODE('.', 12, 1, 7);  /* 2 */
      NIXIE_CATHODE('0', 2,  2, 0);  /* 1 */
      NIXIE_CATHODE('9', 3,  1, 6);  /* 4 */
      NIXIE_CATHODE('8', 4,  2, 1);  /* 3 */
      NIXIE_CATHODE('7', 5,  1, 5);  /* 6 */
      NIXIE_CATHODE('6', 6,  2, 2);  /* 5 */
      NIXIE_CATHODE('5', 7,  1, 4);  /* 8 */
      NIXIE_CATHODE('4', 8,  2, 3);  /* 7 */
      NIXIE_CATHODE('3', 9,  1, 0);  /* 10 */
      NIXIE_CATHODE('2', 10, 2, 4);  /* 9 */
      NIXIE_CATHODE('1', 11, 1, 1);  /* 12 */
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);  /* J4 */
      // NIXIE_CATHODE('.', 12, 0, 4);  /* 2 */
      NIXIE_CATHODE('0', 2,  1, 2);  /* 1 */
      NIXIE_CATHODE('9', 3,  0, 0);  /* 4 */
      NIXIE_CATHODE('8', 4,  1, 3);  /* 3 */
      NIXIE_CATHODE('7', 5,  0, 1);  /* 6 */
      NIXIE_CATHODE('6', 6,  0, 7);  /* 5 */
      NIXIE_CATHODE('5', 7,  0, 2);  /* 8 */
      NIXIE_CATHODE('4', 8,  0, 6);  /* 7 */
      NIXIE_CATHODE('3', 9,  0, 3);  /* 10 */
      NIXIE_CATHODE('2', 10, 0, 5);  /* 9 */
      NIXIE_CATHODE('1', 11, 5, 3);  /* 12 */
    NIXIE_TUBE_END();
    NIXIE_TUBE_BEGIN(NIXIE_TYPE_IN12A);  /* J5 */
      // NIXIE_CATHODE('.', 12, 4, 7);  /* 2 */
      NIXIE_CATHODE('0', 2,  4, 5);  /* 1 */
      NIXIE_CATHODE('9', 3,  4, 6);  /* 4 */
      NIXIE_CATHODE('8', 4,  4, 4);  /* 3 */
      NIXIE_CATHODE('7', 5,  5, 7);  /* 6 */
      NIXIE_CATHODE('6', 6,  5, 0);  /* 5 */
      NIXIE_CATHODE('5', 7,  5, 6);  /* 8 */
      NIXIE_CATHODE('4', 8,  5, 1);  /* 7 */
      NIXIE_CATHODE('3', 9,  5, 5);  /* 10 */
      NIXIE_CATHODE('2', 10, 5, 2);  /* 9 */
      NIXIE_CATHODE('1', 11, 5, 4);  /* 12 */
    NIXIE_TUBE_END();
  NIXIE_REGISTER_END();

  // ======== HTTP(S) server information.

  // TODO(sergey)L Make it some sort of stored configuration.
  safe_strncpy(app_nixie_data->request_url,
               "https://developer.blender.org/maniphest/project/2/type/Bug/query/open/",  // NOLINT
               sizeof(app_nixie_data->request_url));
  safe_strncpy(app_nixie_data->token,
               ">Open Tasks (",
               sizeof(app_nixie_data->token));
  app_nixie_data->token_len = strlen(app_nixie_data->token);
  app_nixie_data->max_cyclic_buffer_len =
    2 * (app_nixie_data->token_len + app_nixie_data->num_nixies);

  // ======== Support components information ========
  app_nixie_data->num_shift_registers = 6;
  // Cleanup shift registers from previous run.
  memset(app_nixie_data->register_shift_state,
         0,
         sizeof(app_nixie_data->register_shift_state));
  APP_ShiftRegister_SendData(app_shift_register_data,
                             app_nixie_data->register_shift_state,
                             app_nixie_data->num_shift_registers);

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
      app_nixie_data->task_from_periodic = false;
      performPeriodicTasks(app_nixie_data);
      break;

    case APP_NIXIE_STATE_ERROR:
      // TODO(sergey): Check whether it was a recoverable error.
      app_nixie_data->state = APP_NIXIE_STATE_IDLE;
      // If error happened from periodic, schedule next update as soon as
      // possible.
      if (app_nixie_data->task_from_periodic) {
          schedulePeriodicTask(app_nixie_data, PERIODIC_TIME_FAST);
      }
      break;

    case APP_NIXIE_STATE_BEGIN_HTTP_REQUEST:
      app_nixie_data->state = APP_NIXIE_STATE_WAIT_HTTPS_CLIENT;
      break;
    case APP_NIXIE_STATE_WAIT_HTTPS_CLIENT:
      waitHttpsClientAndSendRequest(app_nixie_data);
      break;
    case APP_NIXIE_STATE_WAIT_HTTPS_RESPONSE:
      // NOTE: Nothing to do, all interaction is done via HTTP(S) callbacks.
      break;
    case APP_NIXIE_STATE_SHUFFLE_SERVER_VALUE:
      shuffleServerValueDigits(app_nixie_data);
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
  NIXIE_DEBUG_PRINT("Requested to display " NIXIE_DISPLAY_FORMAT "\r\n",
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

bool APP_Nixie_Fetch(AppNixieData* app_nixie_data,
                     bool* is_fetched,
                     char value[MAX_NIXIE_TUBES]) {
  *is_fetched = false;
  if (APP_Nixie_IsBusy(app_nixie_data)) {
    return false;
  }
  NIXIE_DEBUG_MESSAGE("Requested to fetch value.\r\n");
  app_nixie_data->state = APP_NIXIE_STATE_BEGIN_HTTP_REQUEST;
  app_nixie_data->is_fetched_out = is_fetched;
  app_nixie_data->display_value_out = value;
  return true;
}

bool APP_Nixie_PeriodicTasksEnabled(AppNixieData* app_nixie_data) {
  return app_nixie_data->periodic_tasks_enabled;
}

void APP_Nixie_PeriodicTasksSetEnabled(AppNixieData* app_nixie_data,
                                       bool enabled) {
  app_nixie_data->periodic_tasks_enabled = enabled;
}
