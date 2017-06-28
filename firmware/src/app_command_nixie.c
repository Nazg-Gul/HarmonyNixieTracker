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

#include "app_command_nixie.h"

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#define LOG_PREFIX "APP CMD NIXIE: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdNixieUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    display <value>\r\n"
"        Display given number on nixie display.\r\n"
    );
  return true;
}

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

static void appCmdNixieSetCallback(AppData* app_data,
                                   SYS_CMD_DEVICE_NODE* cmd_io,
                                   AppCommandNixieCallback callback) {
  app_data->command.nixie.callback = callback;
  app_data->command.nixie.callback_cmd_io = cmd_io;
}

static void appCmdNixieStartTask(AppData* app_data,
                                 SYS_CMD_DEVICE_NODE* cmd_io,
                                 AppCommandNixieCallback callback) {
  SYS_ASSERT(app_data->command.nixie.state == APP_COMMAND_NIXIE_STATE_NONE,
             "\r\nAttempt to start task while another one is in process\r\n");
  app_data->command.state = APP_COMMAND_STATE_NIXIE;
  app_data->command.nixie.state = APP_COMMAND_NIXIE_STATE_WAIT_AVAILABLE;
  appCmdNixieSetCallback(app_data, cmd_io, callback);
}

static void appCmdNixieFinishTask(AppData* app_data) {
  SYS_ASSERT(app_data->command.nixie.state != APP_COMMAND_NIXIE_STATE_NONE,
             "\r\nAttempt to finish non-existing task\r\n");
  app_data->command.state = APP_COMMAND_STATE_NONE;
  app_data->command.nixie.state = APP_COMMAND_NIXIE_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  appCmdNixieSetCallback(app_data, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

// ============ Display ============

static void performDisplay(AppData* app_data,
                           SYS_CMD_DEVICE_NODE* cmd_io,
                           AppCommandNixieCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_NIXIE_MODE_CALLBACK_INVOKE: {
      // TODO(sergey): Needs implementation.
      break;
    }
    case APP_COMMAND_NIXIE_MODE_CALLBACK_UPDATE:
      appCmdNixieFinishTask(app_data);
      break;
  }
}

static int appCmdNixieDisplay(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  if (argc != 3) {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  app_data->command.nixie._private.display.value = atoi(argv[2]);
  appCmdNixieStartTask(app_data, cmd_io, &performDisplay);
  return true;
}
////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Command_Nixie_Initialize(AppData* app_data) {
  app_data->command.nixie.state = APP_COMMAND_NIXIE_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  appCmdNixieSetCallback(app_data, NULL, NULL);
}

void APP_Command_Nixie_Tasks(AppData* app_data) {
  switch (app_data->command.nixie.state) {
    case APP_COMMAND_NIXIE_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_NIXIE_STATE_WAIT_AVAILABLE:
      if (!APP_Nixie_IsBusy(&app_data->nixie)) {
        DEBUG_MESSAGE("Nixie is free, invoking command.\r\n");
        app_data->command.nixie.state = APP_COMMAND_NIXIE_STATE_RUNNING;
        app_data->command.nixie.callback(
            app_data,
            app_data->command.nixie.callback_cmd_io,
            APP_COMMAND_NIXIE_MODE_CALLBACK_INVOKE);
      } else {
        DEBUG_MESSAGE("Nixie is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_NIXIE_STATE_RUNNING:
      app_data->command.nixie.callback(app_data,
                                       app_data->command.nixie.callback_cmd_io,
                                       APP_COMMAND_NIXIE_MODE_CALLBACK_UPDATE);
      break;
  }
}

int APP_Command_Nixie(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "display")) {
    return appCmdNixieDisplay(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  return true;
}
