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

typedef enum {
  APP_COMMAND_RTC_MODE_CALLBACK_INVOKE,
  APP_COMMAND_RTC_MODE_CALLBACK_UPDATE,
} AppCommandRTCCallbackMode;

typedef void (*AppCommandRTCCallback)(struct AppData* app_data,
                                      struct SYS_CMD_DEVICE_NODE* cmd_io,
                                      AppCommandRTCCallbackMode mode);

typedef enum {
  // None of the RTC tasks to be performed.
  APP_COMMAND_RTC_STATE_NONE,
  // Wait for RTC handle to become available for the commands.
  APP_COMMAND_RTC_STATE_WAIT_AVAILABLE,
  // RTC command is running a background task.
  APP_COMMAND_RTC_STATE_RUNNING,
} AppCommandRTCState;

typedef struct AppCommandRTCData {
  // Current state of RTC command handling.
  AppCommandRTCState state;
  // Callback which is executed once RTC handle is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets RTC data state to WAIT_AVAILABLE,
  //   additionally it sels which callback needs to be called.
  // - RTC state machine waits for RTC handle to be free.
  // - RTC state machine calles the specified callback.
  AppCommandRTCCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;
  // Temporary storage buffers for async RTC communication.
  bool status;
  RTC_MCP7940N_DateTime date_time;
} AppCommandRTCData;

// Initialize RTC related command processor state and data.
void APP_Command_RTC_Initialize(struct AppData* app_data);

// Perform RTC command processor tasks.
void APP_Command_RTC_Tasks(struct AppData* app_data);

// Handle `rtc` command line command.
int APP_Command_RTC(struct AppData* app_data,
                    struct SYS_CMD_DEVICE_NODE* cmd_io,
                    int argc, char** argv);

#endif  // _APP_COMMAND_RTC_H
