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
#include "util_string.h"

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
"    periodic\r\n"
"        Print status of periodic tasks.\r\n"
"    periodic <enable|disable>\r\n"
"        Enable or disable periodic tasks.\r\n"
"    fetch\r\n"
"        Fetch value from server and show it in the console.\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static bool performNixieCheckAvailable(AppData* app_data) {
  return !APP_Nixie_IsBusy(&app_data->nixie);
}

// ============ Display ============

static AppCommandTaskCallbackResult performNixieDisplay(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      APP_Nixie_Display(&app_data->nixie,
                        app_data->command.nixie._private.display.value);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdNixieDisplay(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  if (argc != 3) {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  const char* value = argv[2];
  // Make sure all possibly unused digits are zeroed.
  memset(app_data->command.nixie._private.display.value,
         0,
         sizeof(app_data->command.nixie._private.display.value));
  // Copy at max of display size digits.
  strncpy(app_data->command.nixie._private.display.value,
          value,
          sizeof(app_data->command.nixie._private.display.value));
  // Reverse array in memory to match layout in nixie module/
  reverse_bytes(app_data->command.nixie._private.display.value,
                sizeof(app_data->command.nixie._private.display.value));
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performNixieDisplay,
                            performNixieCheckAvailable);
  return true;
}

// ============ Periodic ============

static int appCmdNixiePeriodic(AppData* app_data,
                               SYS_CMD_DEVICE_NODE* cmd_io,
                               int argc, char** argv) {
  if (argc == 2) {
    const bool is_enabled = APP_Nixie_PeriodicTasksEnabled(&app_data->nixie);
    COMMAND_PRINT("Nixie periodic tasks %s.\r\n", is_enabled ? "enabled"
                                                             : "disabled");
    return true;
  }
  if (argc != 3) {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "enable")) {
    APP_Nixie_PeriodicTasksSetEnabled(&app_data->nixie, true);
  } else if (STREQ(argv[2], "disable")) {
    APP_Nixie_PeriodicTasksSetEnabled(&app_data->nixie, false);
  } else {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  return true;
}

// ============ Fetch ============

static AppCommandTaskCallbackResult performNixieFetch(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE: {
      APP_Nixie_Fetch(&app_data->nixie,
                      &app_data->command.nixie._private.fetch.is_fetched,
                      app_data->command.nixie._private.fetch.value);
      break;
    }
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if someone else made nixie module busy?
      if (!APP_Nixie_IsBusy(&app_data->nixie)) {
        if (app_data->command.nixie._private.fetch.is_fetched) {
          COMMAND_PRINT("Value from server: " NIXIE_DISPLAY_FORMAT "\r\n",
                        NIXIE_DISPLAY_VALUES(
                            app_data->command.nixie._private.fetch.value));
        } else {
          COMMAND_MESSAGE("Error fetching value from server.\r\n");
        }
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdNixieFetch(AppData* app_data,
                            SYS_CMD_DEVICE_NODE* cmd_io,
                            int argc, char** argv) {
  if (argc != 2) {
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performNixieFetch,
                            performNixieCheckAvailable);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Command_Nixie_Initialize(AppData* app_data) {
  // TODO(sergey): Memset to zero in debug mode?
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
  } else if (STREQ(argv[1], "periodic")) {
    return appCmdNixiePeriodic(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "fetch")) {
    return appCmdNixieFetch(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return appCmdNixieUsage(cmd_io, argv[0]);
  }
  return true;
}
