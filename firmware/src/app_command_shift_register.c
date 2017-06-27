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

#include "app_command_shift_register.h"

#include "app.h"
#include "app_shift_register.h"

#include "system_definitions.h"
#include "utildefines.h"

#include "util_string.h"

#define LOG_PREFIX "APP CMD SHIFT REGISTER: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

static int app_command_shift_register_usage(SYS_CMD_DEVICE_NODE* cmd_io,
                                            const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    <enable|disable>\r\n"
"        Enable or disable shift register outputs.\r\n"
"\r\n"
    );
  return true;
}

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

static void app_command_shift_register_set_callback(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandShiftRegisterCallback callback) {
  app_data->command.shift_register.callback = callback;
  app_data->command.shift_register.callback_cmd_io = cmd_io;
}

static void app_command_shift_register_start_task(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandShiftRegisterCallback callback) {
  SYS_ASSERT(
      app_data->command.shift_register.state == APP_COMMAND_SHIFT_REGISTER_STATE_NONE,  // NOLINT
      "\r\nAttempt to start task while another one is in process\r\n");
  app_data->command.state = APP_COMMAND_STATE_SHIFT_REGISTER;
  app_data->command.shift_register.state =
      APP_COMMAND_SHIFT_REGISTER_STATE_WAIT_AVAILABLE;
  app_command_shift_register_set_callback(app_data, cmd_io, callback);
}

static void app_command_shift_register_finish_task(AppData* app_data) {
  SYS_ASSERT(
      app_data->command.shift_register.state != APP_COMMAND_SHIFT_REGISTER_STATE_NONE,  // NOLINT
     "\r\nAttempt to finish non-existing task\r\n");
  app_data->command.state = APP_COMMAND_STATE_NONE;
  app_data->command.shift_register.state = APP_COMMAND_SHIFT_REGISTER_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.shift_register.callback = NULL;
  app_data->command.shift_register.callback_cmd_io = NULL;
}

static void setEnabled(AppData* app_data,
                       SYS_CMD_DEVICE_NODE* cmd_io,
                       AppCommandShiftRegisterCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_INVOKE:
      APP_ShiftRegister_SetEnabled(
          &app_data->shift_register,
          app_data->command.shift_register._private.enable.do_enable);
      break;
    case APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_UPDATE:
      app_command_shift_register_finish_task(app_data);
      break;
  }
}

static int commandShiftRegisterSetEnabled(AppData* app_data,
                                          SYS_CMD_DEVICE_NODE* cmd_io,
                                          int argc, char** argv,
                                          bool is_enabled) {
  if (argc != 2) {
    return app_command_shift_register_usage(cmd_io, argv[0]);
  }
  app_data->command.shift_register._private.enable.do_enable = is_enabled;
  app_command_shift_register_start_task(app_data, cmd_io, &setEnabled);
  return true;
}

void APP_Command_ShiftRegister_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.shift_register,
         0,
         sizeof(app_data->command.shift_register));
  app_data->command.shift_register.state = APP_COMMAND_SHIFT_REGISTER_STATE_NONE;  // NOLINT
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.shift_register.callback = NULL;
  app_data->command.shift_register.callback_cmd_io = NULL;
}

void APP_Command_ShiftRegister_Tasks(struct AppData* app_data) {
  switch (app_data->command.shift_register.state) {
    case APP_COMMAND_SHIFT_REGISTER_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_SHIFT_REGISTER_STATE_WAIT_AVAILABLE:
      if (!APP_ShiftRegister_IsBusy(&app_data->shift_register)) {
        DEBUG_MESSAGE("Shift register is free, invoking command.\r\n");
        app_data->command.shift_register.state =
            APP_COMMAND_SHIFT_REGISTER_STATE_RUNNING;
        app_data->command.shift_register.callback(
            app_data,
            app_data->command.shift_register.callback_cmd_io,
            APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_INVOKE);
      } else {
        DEBUG_MESSAGE("Shift register is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_SHIFT_REGISTER_STATE_RUNNING:
      app_data->command.shift_register.callback(
          app_data,
          app_data->command.shift_register.callback_cmd_io,
          APP_COMMAND_SHIFT_REGISTER_MODE_CALLBACK_UPDATE);
      break;
  }
}

int APP_Command_ShiftRegister(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return app_command_shift_register_usage(cmd_io, argv[0]);
  } else if (argc == 2 && STREQ(argv[1], "enable")) {
    commandShiftRegisterSetEnabled(app_data, cmd_io, argc, argv, true);
  } else if (argc == 2 && STREQ(argv[1], "disable")) {
    commandShiftRegisterSetEnabled(app_data, cmd_io, argc, argv, false);
  } else {
    return app_command_shift_register_usage(cmd_io, argv[0]);
  }
  return true;
}
