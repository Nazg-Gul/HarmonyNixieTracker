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

#ifndef _APP_COMMAND_TASK_H
#define _APP_COMMAND_TASK_H

#include <stdbool.h>

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef enum {
  // Callback is being invoked for the first time after resource is available.
  APP_COMMAND_TASK_MODE_CALLBACK_INVOKE,
  // Callback is called after callback was invoked, to update status of async
  // running task.
  APP_COMMAND_TASK_MODE_CALLBACK_UPDATE,
} AppCommandTaskCallbackMode;

typedef enum {
  // Task is still running.
  APP_COMMAND_TASK_RESULT_RUNNING,
  // Task is finished.
  APP_COMMAND_TASK_RESULT_FINISHED,
} AppCommandTaskCallbackResult;

// Callback implementing actual logic of the task.
typedef AppCommandTaskCallbackResult (*AppCommandTaskCallback)(
    struct AppData* app_data,
    struct SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode);

// Callback which is run to check whether resource is available.
typedef bool (*AppCommandTaskCheckAvailableCallback)(struct AppData* app_data);

typedef enum {
  // None of the tasks to be performed.
  APP_COMMAND_TASK_STATE_NONE,
  // Wait for the resource to become available for the commands.
  APP_COMMAND_TASK_STATE_WAIT_AVAILABLE,
  // There is a task running in background.
  APP_COMMAND_TASK_STATE_RUNNING,
} AppCommandTaskState;

typedef struct AppCommandTaskData {
  // Current state of background task state machine.
  AppCommandTaskState state;
  // Callback which is executed once resource is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets state to WAIT_AVAILABLE,
  //   additionally it sets which callback needs to be called.
  // - State machine waits for resource handle to become free.
  // - State machine calls the specified callback.
  AppCommandTaskCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;
  // Callback to check whether resource is available.
  AppCommandTaskCheckAvailableCallback callback_check_available;
} AppCommandTaskData;

// Initialize task routines.
void APP_Command_Task_Initialize(AppCommandTaskData *app_command_task_data);

// Check whether command task scheduler is busy with some stuff.
bool APP_Command_Task_IsBusy(AppCommandTaskData *app_command_task_data);

// Schedule new task.
void APP_Command_Task_Schedule(
    AppCommandTaskData *app_command_task_data,
    struct SYS_CMD_DEVICE_NODE* callback_cmd_io,
    AppCommandTaskCallback callback,
    AppCommandTaskCheckAvailableCallback callback_check_available);

// Run state machine tasks.
void APP_Command_Task_Tasks(AppCommandTaskData *app_command_task_data,
                            struct AppData* app_data);

#endif  // _APP_COMMAND_TASK_H
