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

#ifndef _APP_COMMAND_SHIFT_REGISTER_H
#define _APP_COMMAND_SHIFT_REGISTER_H

#include <stdbool.h>

#include "app_https_client.h"

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef enum {
  // Callback is being invoked for the first time after shift register client
  // becames available.
  APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_INVOKE,
  // Callback is called after callback was invoked, to update status of async
  // running task.
  APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_UPDATE,
} AppCommandShiftRegisterCallbackMode;

typedef void (*AppCommandShiftRegisterCallback)(
    struct AppData* app_data,
    struct SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandShiftRegisterCallbackMode mode);

typedef enum {
  // None of the shift register tasks to be performed.
  APP_COMMAND_SHIFT_REGISTER_STATE_NONE,
  // Wait for shift register handle to become available for the commands.
  APP_COMMAND_SHIFT_REGISTER_STATE_WAIT_AVAILABLE,
  // Shift register command is running a background task.
  APP_COMMAND_SHIFT_REGISTER_STATE_RUNNING,
} AppCommandShiftRegisterState;

typedef struct AppCommandShiftRegisterData {
  // Current state of shift register command handling.
  AppCommandShiftRegisterState state;
  // Callback which is executed once shift register handle is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets shift register data state to
  //   WAIT_AVAILABLE,
  //   additionally it sets which callback needs to be called.
  // - Shift register state machine waits for shift register client
  //   handle to be free.
  // - Shift register state machine calls the specified callback.
  AppCommandShiftRegisterCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;
} AppCommandShiftRegisterData;

// Initialize shift register related command processor state and data.
void APP_Command_ShiftRegister_Initialize(struct AppData* app_data);

// Perform shift register command processor tasks.
void APP_Command_ShiftRegister_Tasks(struct AppData* app_data);

// Handle `shift_register` command line command.
int APP_Command_ShiftRegister(struct AppData* app_data,
                              struct SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv);

#endif  // _APP_COMMAND_SHIFT_REGISTER_H
