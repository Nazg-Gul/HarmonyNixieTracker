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

static int app_command_flash_usage(SYS_CMD_DEVICE_NODE* cmd_io,
                                   const char* argv0) {
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

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

static void app_command_flash_set_callback(AppData* app_data,
                                           SYS_CMD_DEVICE_NODE* cmd_io,
                                           AppCommandFlashCallback callback) {
  app_data->command.flash.callback = callback;
  app_data->command.flash.callback_cmd_io = cmd_io;
}

static void app_command_flash_start_task(AppData* app_data,
                                         SYS_CMD_DEVICE_NODE* cmd_io,
                                         AppCommandFlashCallback callback) {
  SYS_ASSERT(app_data->command.flash.state == APP_COMMAND_FLASH_STATE_NONE,
             "\r\nAttempt to start task while another one is in process\r\n");
  app_data->command.state = APP_COMMAND_STATE_FLASH;
  app_data->command.flash.state = APP_COMMAND_FLASH_STATE_WAIT_AVAILABLE;
  app_command_flash_set_callback(app_data, cmd_io, callback);
}

static void app_command_flash_finish_task(AppData* app_data) {
  SYS_ASSERT(app_data->command.flash.state != APP_COMMAND_FLASH_STATE_NONE,
             "\r\nAttempt to finish non-existing task\r\n");
  app_data->command.state = APP_COMMAND_STATE_NONE;
  app_data->command.flash.state = APP_COMMAND_FLASH_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.flash.callback = NULL;
  app_data->command.flash.callback_cmd_io = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

// ============ SECTORS ============

static void flash_sectors(AppData* app_data,
                          SYS_CMD_DEVICE_NODE* cmd_io,
                          AppCommandFlashCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_FLASH_MODE_CALLBACK_INVOKE: {
      uint32_t total_sectors, free_sectors;
      if (APP_Flash_DriveSectorGet(&total_sectors, &free_sectors)) {
        COMMAND_PRINT("Total sectors: %d, free sectors: %d\r\n",
                      total_sectors, free_sectors);
      } else {
        COMMAND_MESSAGE("Error querying sector information from flash.\r\n");
      }
      break;
    }
    case APP_COMMAND_FLASH_MODE_CALLBACK_UPDATE:
      app_command_flash_finish_task(app_data);
      break;
  }
}

static int app_command_flash_sectors(AppData* app_data,
                                     SYS_CMD_DEVICE_NODE* cmd_io,
                                     int argc, char** argv) {
  if (argc != 2) {
    return app_command_flash_usage(cmd_io, argv[0]);
  }
  app_command_flash_start_task(app_data, cmd_io, &flash_sectors);
  return true;
}

// ============ FORMAT ============

static void flash_format(AppData* app_data,
                         SYS_CMD_DEVICE_NODE* cmd_io,
                         AppCommandFlashCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_FLASH_MODE_CALLBACK_INVOKE: {
      if (APP_Flash_DriveFormat()) {
        COMMAND_MESSAGE("Drive is formatted.\r\n");
      } else {
        COMMAND_MESSAGE("Error formatting the drive.\r\n");
      }
      break;
    }
    case APP_COMMAND_FLASH_MODE_CALLBACK_UPDATE:
      app_command_flash_finish_task(app_data);
      break;
  }
}

static int app_command_flash_format(AppData* app_data,
                                    SYS_CMD_DEVICE_NODE* cmd_io,
                                    int argc, char** argv) {
  if (argc != 2) {
    return app_command_flash_usage(cmd_io, argv[0]);
  }
  app_command_flash_start_task(app_data, cmd_io, &flash_format);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_Command_Flash_Initialize(AppData* app_data) {
  app_data->command.flash.state = APP_COMMAND_FLASH_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.flash.callback = NULL;
  app_data->command.flash.callback_cmd_io = NULL;
}

void APP_Command_Flash_Tasks(AppData* app_data) {
  switch (app_data->command.flash.state) {
    case APP_COMMAND_FLASH_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_FLASH_STATE_WAIT_AVAILABLE:
      if (!APP_Flash_IsBusy(&app_data->flash)) {
        if (APP_Flash_IsError(&app_data->flash)) {
          DEBUG_MESSAGE("Flash is in error state, can't perform operation.\r\n");
          app_command_flash_finish_task(app_data);
        } else {
          DEBUG_MESSAGE("Flash is free, invoking command.\r\n");
          app_data->command.flash.state = APP_COMMAND_FLASH_STATE_RUNNING;
          app_data->command.flash.callback(
              app_data,
              app_data->command.flash.callback_cmd_io,
              APP_COMMAND_FLASH_MODE_CALLBACK_INVOKE);
        }
      } else {
        DEBUG_MESSAGE("Flash is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_FLASH_STATE_RUNNING:
      app_data->command.flash.callback(app_data,
                                       app_data->command.flash.callback_cmd_io,
                                       APP_COMMAND_FLASH_MODE_CALLBACK_UPDATE);
      break;
  }
}

int APP_Command_Flash(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return app_command_flash_usage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "sectors")) {
    return app_command_flash_sectors(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "format")) {
    return app_command_flash_format(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return app_command_flash_usage(cmd_io, argv[0]);
  }
  return true;
}
