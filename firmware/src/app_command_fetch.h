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

#ifndef _APP_COMMAND_FETCH_H
#define _APP_COMMAND_FETCH_H

#include <stdbool.h>

#include "app_https_client.h"

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef enum {
  // Callback is being invoked for the first time after HTTP(S) client becames
  // available.
  APP_COMMAND_FETCH_MODE_CALLBACK_INVOKE,
  // Callback is called after callback was invoked, to update status of async
  // running task.
  APP_COMMAND_FETCH_MODE_CALLBACK_UPDATE,
} AppCommandFetchCallbackMode;

typedef void (*AppCommandFetchCallback)(struct AppData* app_data,
                                        struct SYS_CMD_DEVICE_NODE* cmd_io,
                                        AppCommandFetchCallbackMode mode);

typedef enum {
  // None of the fetch tasks to be performed.
  APP_COMMAND_FETCH_STATE_NONE,
  // Wait for fetch handle to become available for the commands.
  APP_COMMAND_FETCH_STATE_WAIT_AVAILABLE,
  // Fetch command is running a background task.
  APP_COMMAND_FETCH_STATE_RUNNING,
} AppCommandFetchState;

typedef struct AppCommandFetchData {
  // Current state of fetch command handling.
  AppCommandFetchState state;
  // Callback which is executed once fetch handle is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets fetch data state to WAIT_AVAILABLE,
  //   additionally it sets which callback needs to be called.
  // - Fetch state machine waits for HTTP(S) client handle to be free.
  // - Fetch state machine calls the specified callback.
  AppCommandFetchCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;

  // Requested URL for fetch.
  char url[MAX_URL];

  // Indicates whether request is still being fetched from server.
  bool request_active;
} AppCommandFetchData;

// Initialize fetch related command processor state and data.
void APP_Command_Fetch_Initialize(struct AppData* app_data);

// Perform Fetch command processor tasks.
void APP_Command_Fetch_Tasks(struct AppData* app_data);

// Handle `fetch` command line command.
int APP_Command_Fetch(struct AppData* app_data,
                      struct SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv);

#endif  // _APP_COMMAND_FETCH_H
