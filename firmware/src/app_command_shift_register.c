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

#include "util_math.h"
#include "util_string.h"

#define LOG_PREFIX "APP CMD SHIFT REGISTER: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdShiftRegisterUsage(SYS_CMD_DEVICE_NODE* cmd_io,
                                    const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    <enable|disable>\r\n"
"        Enable or disable shift register outputs.\r\n"
"    send <data>\r\n"
"        Send data to the shift register.\r\n"
"\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static bool performShiftRegisterCheckAvailable(AppData* app_data) {
  return !APP_ShiftRegister_IsBusy(&app_data->shift_register);
}

// ============ enable/disable ============

// TODO(sergey): Consider merigng the following two callbacks.

static AppCommandTaskCallbackResult performShiftRegisterSetEnabled(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      APP_ShiftRegister_SetEnabled(&app_data->shift_register, true);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performShiftRegisterSetDisabled(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      APP_ShiftRegister_SetEnabled(&app_data->shift_register, false);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int commandShiftRegisterSetEnabledOrDisabled(AppData* app_data,
                                                    SYS_CMD_DEVICE_NODE* cmd_io,
                                                    int argc, char** argv) {
  if (argc != 2) {
    return appCmdShiftRegisterUsage(cmd_io, argv[0]);
  } else if (STREQ(argv[1], "enable")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performShiftRegisterSetEnabled,
                              performShiftRegisterCheckAvailable);
  } else if (STREQ(argv[1], "disable")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performShiftRegisterSetDisabled,
                              performShiftRegisterCheckAvailable);
  } else {
     appCmdShiftRegisterUsage(cmd_io, argv[0]);
  }
  return true;
}

// ============ data ============

static AppCommandTaskCallbackResult performShiftRegisterSendData(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      APP_ShiftRegister_SendData(
          &app_data->shift_register,
          app_data->command.shift_register._private.send.data,
          app_data->command.shift_register._private.send.num_bytes);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      return APP_COMMAND_TASK_RESULT_FINISHED;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int commandShiftRegisterSendData(AppData* app_data,
                                        SYS_CMD_DEVICE_NODE* cmd_io,
                                        int argc, char** argv,
                                        bool is_enabled) {
  if (argc < 3) {
    return appCmdShiftRegisterUsage(cmd_io, argv[0]);
  }
  const size_t num_bytes =
    min_zz(argc - 2,
           sizeof(app_data->command.shift_register._private.send.data));
  size_t i;
  for (i = 0; i < num_bytes; ++i) {
    app_data->command.shift_register._private.send.data[i] = atoi(argv[i + 2]);
  }
  app_data->command.shift_register._private.send.num_bytes = num_bytes;
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performShiftRegisterSendData,
                            performShiftRegisterCheckAvailable);
  return true;
}

void APP_Command_ShiftRegister_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.shift_register,
         0,
         sizeof(app_data->command.shift_register));
}

int APP_Command_ShiftRegister(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdShiftRegisterUsage(cmd_io, argv[0]);
  } else if (argc == 2 && STREQ(argv[1], "enable")) {
    commandShiftRegisterSetEnabledOrDisabled(app_data, cmd_io, argc, argv);
  } else if (argc == 2 && STREQ(argv[1], "disable")) {
    commandShiftRegisterSetEnabledOrDisabled(app_data, cmd_io, argc, argv);
  } else if (argc >= 2 && STREQ(argv[1], "send")) {
    commandShiftRegisterSendData(app_data, cmd_io, argc, argv, false);
  } else {
    return appCmdShiftRegisterUsage(cmd_io, argv[0]);
  }
  return true;
}
