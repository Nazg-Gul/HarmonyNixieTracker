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

#include "app_command_fetch.h"

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#include "util_string.h"

#define LOG_PREFIX "APP CMD FETCH: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdFetchUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s <url>\r\n", argv0);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static void bufferReceivedCallback(const uint8_t* buffer,
                                  uint16_t num_bytes,
                                  void* user_data) {
  AppData* app_data = (AppData*)user_data;
  // TOFO(sergey): This doesn't look nice.
  SYS_CMD_DEVICE_NODE* cmd_io = app_data->command.task.callback_cmd_io;
  uint16_t i;
  // TODO(sergey): This is because of some nasty defines in stdio which is
  // indirectly included via system_definitions.h -> tpcpip.h.
  // TODO(sergey): Using putc() doesn't work here, always gives error about
  // console being busy.
//#ifdef putc
//#  undef putc
//#endif
  for (i = 0; i < num_bytes; ++i) {
    // cmd_io->pCmdApi->putc(cmd_io->cmdIoParam, 'x');
    COMMAND_PRINT("%c", buffer[i]);
  }
}

static void requestHandledCallback(void* user_data) {
  AppData* app_data = (AppData*)user_data;
  app_data->command.fetch.request_active = false;
}

static void errorCallback(void* user_data) {
  AppData* app_data = (AppData*)user_data;
  app_data->command.fetch.request_active = false;
}

static AppCommandTaskCallbackResult performFetch(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE: {
      AppHttpsClientCallbacks callbacks = {NULL};
      callbacks.buffer_received = bufferReceivedCallback;
      callbacks.request_handled = requestHandledCallback;
      callbacks.error = errorCallback;
      callbacks.user_data = app_data;
      app_data->command.fetch.request_active = true;
      if (!APP_HTTPS_Client_Request(&app_data->https_client,
                                    app_data->command.fetch.url,
                                    &callbacks)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
    }
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      if (!app_data->command.fetch.request_active) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static bool performFetchCheckAvailable(AppData* app_data) {
  return !APP_HTTPS_Client_IsBusy(&app_data->https_client);
}

static int appCmdFetch(AppData* app_data,
                       SYS_CMD_DEVICE_NODE* cmd_io,
                       int argc, char** argv) {
  if (argc != 2) {
    return appCmdFetchUsage(cmd_io, argv[0]);
  }
  safe_strncpy(app_data->command.fetch.url,
               argv[1],
               sizeof(app_data->command.fetch.url));
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performFetch,
                            performFetchCheckAvailable);
  return true;
}

void APP_Command_Fetch_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.fetch, 0, sizeof(app_data->command.fetch));
}

int APP_Command_Fetch(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdFetchUsage(cmd_io, argv[0]);
  }
  appCmdFetch(app_data, cmd_io, argc, argv);
  return true;
}
