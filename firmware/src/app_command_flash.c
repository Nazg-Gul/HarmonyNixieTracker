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

#include "app_command_flash.h"

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#define LOG_PREFIX "APP CMD Flash: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdFlashUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    sectors\r\n"
"        Query information about number of total and free sectors.\r\n"
"\r\n"
"    format\r\n"
"        Format the whole flash drive.\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static bool performFlashCheckAvailable(AppData* app_data) {
  return !APP_Flash_IsBusy(&app_data->flash);
}

// ============ SECTORS ============

static AppCommandTaskCallbackResult performFlashSectors(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE: {
      uint32_t total_sectors, free_sectors;
      if (APP_Flash_DriveSectorGet(&total_sectors, &free_sectors)) {
        COMMAND_PRINT("Total sectors: %d, free sectors: %d\r\n",
                      total_sectors, free_sectors);
      } else {
        COMMAND_MESSAGE("Error querying sector information from flash.\r\n");
      }
      break;
    }
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdFlashSectors(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  if (argc != 2) {
    return appCmdFlashUsage(cmd_io, argv[0]);
  }
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performFlashSectors,
                            performFlashCheckAvailable);
  return true;
}

// ============ FORMAT ============

static AppCommandTaskCallbackResult performFlashFormat(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE: {
      if (APP_Flash_DriveFormat()) {
        COMMAND_MESSAGE("Drive is formatted.\r\n");
      } else {
        COMMAND_MESSAGE("Error formatting the drive.\r\n");
      }
      break;
    }
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdFlashFormat(AppData* app_data,
                             SYS_CMD_DEVICE_NODE* cmd_io,
                             int argc, char** argv) {
  if (argc != 2) {
    return appCmdFlashUsage(cmd_io, argv[0]);
  }
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performFlashFormat,
                            performFlashCheckAvailable);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

int APP_Command_Flash(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdFlashUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "sectors")) {
    return appCmdFlashSectors(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "format")) {
    return appCmdFlashFormat(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return appCmdFlashUsage(cmd_io, argv[0]);
  }
  return true;
}
