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

#ifndef _APP_COMMAND_RTC_H
#define _APP_COMMAND_RTC_H

#include <stdbool.h>

#include "rtc_mcp7940n.h"

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef struct AppCommandRTCData {
  // Storage buffers for async RTC communication.
  union {
    // Storage for `oscillator` command.
    struct {
      bool status;
    } oscillator;

    // Storage for `battery` command.
    struct {
      bool status;
    } battery;

    // Storage for `date` command.
    struct {
      // Buffer to store date and time read from the RTC.
      RTC_MCP7940N_DateTime date_time;
    } date;

    // Storage for `dump` command.
    struct {
      // Buffer to read all registers values.
      uint8_t registers_storage[RTC_MCP7940N_NUM_REGISTERS];
    } dump;

    // Storage for `register` command.
    struct {
      // Address of register to be read or written.
      uint8_t register_address;
      // This is a new register value for `register write` command and this is
      // the received register value for `register read` command.
      uint8_t register_value;
    } reg;
  } _private;
} AppCommandRTCData;

// Initialize RTC related command processor state and data.
void APP_Command_RTC_Initialize(struct AppData* app_data);

// Handle `rtc` command line command.
int APP_Command_RTC(struct AppData* app_data,
                    struct SYS_CMD_DEVICE_NODE* cmd_io,
                    int argc, char** argv);

#endif  // _APP_COMMAND_RTC_H
