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

#include "app_command_task.h"

#include <stddef.h>

#include "system_definitions.h"
#include "utildefines.h"

#define LOG_PREFIX "APP CMD TASK: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static void appCmdTaskSetCallback(
    AppCommandTaskData* app_command_task_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallback callback,
    AppCommandTaskCheckAvailableCallback callback_check_available) {
  app_command_task_data->callback_cmd_io = cmd_io;
  app_command_task_data->callback = callback;
  app_command_task_data->callback_check_available = callback_check_available;
}

static void checkCallbackResult(AppCommandTaskData *app_command_task_data,
                                AppCommandTaskCallbackResult result) {
  switch (result) {
    case APP_COMMAND_TASK_RESULT_RUNNING:
      app_command_task_data->state = APP_COMMAND_TASK_STATE_RUNNING;
      break;
    case APP_COMMAND_TASK_RESULT_FINISHED:
      app_command_task_data->state = APP_COMMAND_TASK_STATE_NONE;
      appCmdTaskSetCallback(app_command_task_data, NULL, NULL, NULL);
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Command_Task_Initialize(AppCommandTaskData *app_command_task_data) {
  app_command_task_data->state = APP_COMMAND_TASK_STATE_NONE;
  appCmdTaskSetCallback(app_command_task_data, NULL, NULL, NULL);
}

bool APP_Command_Task_IsBusy(AppCommandTaskData *app_command_task_data) {
  return app_command_task_data->state != APP_COMMAND_TASK_STATE_NONE;
}

void APP_Command_Task_Schedule(
    AppCommandTaskData *app_command_task_data,
    struct SYS_CMD_DEVICE_NODE* callback_cmd_io,
    AppCommandTaskCallback callback,
    AppCommandTaskCheckAvailableCallback callback_check_available) {
  SYS_ASSERT(
      app_command_task_data->state == APP_COMMAND_TASK_STATE_NONE,
      "\r\nAttempt to start task while another one is in process\r\n");
  app_command_task_data->state = APP_COMMAND_TASK_STATE_WAIT_AVAILABLE;
  appCmdTaskSetCallback(app_command_task_data,
                        callback_cmd_io,
                        callback,
                        callback_check_available);
}

void APP_Command_Task_Tasks(AppCommandTaskData *app_command_task_data,
                            struct AppData* app_data) {
  switch (app_command_task_data->state) {
    case APP_COMMAND_TASK_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_TASK_STATE_WAIT_AVAILABLE:
      if (app_command_task_data->callback_check_available(app_data)) {
        DEBUG_MESSAGE("Shift register is free, invoking command.\r\n");
        app_command_task_data->state = APP_COMMAND_TASK_STATE_RUNNING;
        AppCommandTaskCallbackResult result =
            app_command_task_data->callback(
                app_data,
                app_command_task_data->callback_cmd_io,
                APP_COMMAND_TASK_MODE_CALLBACK_INVOKE);
        checkCallbackResult(app_command_task_data, result);
      } else {
        DEBUG_MESSAGE("Shift register is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_TASK_STATE_RUNNING: {
      AppCommandTaskCallbackResult result =
          app_command_task_data->callback(
              app_data,
              app_command_task_data->callback_cmd_io,
              APP_COMMAND_TASK_MODE_CALLBACK_UPDATE);
      checkCallbackResult(app_command_task_data, result);
      break;
    }
  }
}
