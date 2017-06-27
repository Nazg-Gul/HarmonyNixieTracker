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

static int appCmdFetchUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s <url>\r\n", argv0);
  return true;
}

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

static void appCmdFetchSetCallback(AppData* app_data,
                                   SYS_CMD_DEVICE_NODE* cmd_io,
                                   AppCommandFetchCallback callback) {
  app_data->command.fetch.callback = callback;
  app_data->command.fetch.callback_cmd_io = cmd_io;
}

static void appCmdFetchStartTask(AppData* app_data,
                                 SYS_CMD_DEVICE_NODE* cmd_io,
                                 AppCommandFetchCallback callback) {
  SYS_ASSERT(app_data->command.fetch.state == APP_COMMAND_FETCH_STATE_NONE,
             "\r\nAttempt to start task while another one is in process\r\n");
  app_data->command.state = APP_COMMAND_STATE_FETCH;
  app_data->command.fetch.state = APP_COMMAND_FETCH_STATE_WAIT_AVAILABLE;
  appCmdFetchSetCallback(app_data, cmd_io, callback);
}

static void appCmdFetchFinishTask(AppData* app_data) {
  SYS_ASSERT(app_data->command.fetch.state != APP_COMMAND_FETCH_STATE_NONE,
             "\r\nAttempt to finish non-existing task\r\n");
  app_data->command.state = APP_COMMAND_STATE_NONE;
  app_data->command.fetch.state = APP_COMMAND_FETCH_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  appCmdFetchSetCallback(app_data, NULL, NULL);
}

static void bufferReceivedCallback(const uint8_t* buffer,
                                  uint16_t num_bytes,
                                  void* user_data) {
  AppData* app_data = (AppData*)user_data;
  // TOFO(sergey): This doesn't look nice.
  SYS_CMD_DEVICE_NODE* cmd_io = app_data->command.fetch.callback_cmd_io;
  uint16_t i;
  // TODO(sergey): This is because of some nasty defines in stdio which is
  // indirectly included via system_definitions.h -> tpcpip.h.
#ifdef putc
#  undef putc
#endif
  for (i = 0; i < num_bytes; ++i) {
    cmd_io->pCmdApi->putc(cmd_io->cmdIoParam, 'x');
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

static void performFetch(AppData* app_data,
                         SYS_CMD_DEVICE_NODE* cmd_io,
                         AppCommandFetchCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_FETCH_MODE_CALLBACK_INVOKE: {
      AppHttpsClientCallbacks callbacks = {NULL};
      callbacks.buffer_received = bufferReceivedCallback;
      callbacks.request_handled = requestHandledCallback;
      callbacks.error = errorCallback;
      callbacks.user_data = app_data;
      app_data->command.fetch.request_active = true;
      if (!APP_HTTPS_Client_Request(&app_data->https_client,
                                    app_data->command.fetch.url,
                                    &callbacks)) {
        appCmdFetchFinishTask(app_data);
      }
      break;
    }
    case APP_COMMAND_FETCH_MODE_CALLBACK_UPDATE:
      if (!app_data->command.fetch.request_active) {
        appCmdFetchFinishTask(app_data);
      }
      break;
  }
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
  appCmdFetchStartTask(app_data, cmd_io, &performFetch);
  return true;
}

void APP_Command_Fetch_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.fetch, 0, sizeof(app_data->command.fetch));
  app_data->command.fetch.state = APP_COMMAND_FETCH_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  appCmdFetchSetCallback(app_data, NULL, NULL);
}

void APP_Command_Fetch_Tasks(struct AppData* app_data) {
  switch (app_data->command.fetch.state) {
    case APP_COMMAND_FETCH_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_FETCH_STATE_WAIT_AVAILABLE:
      if (!APP_HTTPS_Client_IsBusy(&app_data->https_client)) {
        DEBUG_MESSAGE("HTTP(S) client is free, invoking command.\r\n");
        app_data->command.fetch.state = APP_COMMAND_FETCH_STATE_RUNNING;
        app_data->command.fetch.callback(
            app_data,
            app_data->command.fetch.callback_cmd_io,
            APP_COMMAND_FETCH_MODE_CALLBACK_INVOKE);
      } else {
        DEBUG_MESSAGE("HTTPS(client) is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_FETCH_STATE_RUNNING:
      app_data->command.fetch.callback(
          app_data,
          app_data->command.fetch.callback_cmd_io,
          APP_COMMAND_FETCH_MODE_CALLBACK_UPDATE);
      break;
  }
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
