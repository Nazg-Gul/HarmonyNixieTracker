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

#ifndef _APP_COMMAND_NIXIE_H
#define _APP_COMMAND_NIXIE_H

#include <stdint.h>

#include "app_nixie.h"

struct AppData;
struct SYS_CMD_DEVICE_NODE;

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

typedef enum {
  // Callback is being invoked for the first time after nixie module became
  // available.
  APP_COMMAND_NIXIE_MODE_CALLBACK_INVOKE,
  // Callback is called after callback was invoked, to update status of async
  // running task.
  APP_COMMAND_NIXIE_MODE_CALLBACK_UPDATE,
} AppCommandNixieCallbackMode;

typedef void (*AppCommandNixieCallback)(struct AppData* app_data,
                                        struct SYS_CMD_DEVICE_NODE* cmd_io,
                                        AppCommandNixieCallbackMode mode);

typedef enum {
  // None of the nixie tasks to be performed.
  APP_COMMAND_NIXIE_STATE_NONE,
  // Wait for nixie to become available for the commands.
  APP_COMMAND_NIXIE_STATE_WAIT_AVAILABLE,
  // Nixie command is running a background task.
  APP_COMMAND_NIXIE_STATE_RUNNING,
} AppCommandNixieState;

typedef struct AppCommandNixieData {
  // Current state of nixie command handling.
  AppCommandNixieState state;

  // Callback which is executed once nixie handle is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets nixie data state to WAIT_AVAILABLE,
  //   additionally it sets which callback needs to be called.
  // - Nixie state machine waits for nixie handle to be free.
  // - Nixie state machine calls the specified callback.
  AppCommandNixieCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;

  // Per-task storage.
  union {
    struct {
      // Value to be displayed.
      char value[MAX_NIXIE_TUBES];
    } display;
  } _private;
} AppCommandNixieData;

// Initialize nixie related command processor state and data.
void APP_Command_Nixie_Initialize(struct AppData* app_data);

// Perform nixie command processor tasks.
void APP_Command_Nixie_Tasks(struct AppData* app_data);

// Handle `nixie` command line command.
int APP_Command_Nixie(struct AppData* app_data,
                      struct SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv);

#endif  // _APP_COMMAND_NIXIE_H
