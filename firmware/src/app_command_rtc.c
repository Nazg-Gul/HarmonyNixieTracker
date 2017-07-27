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

#include "app_command_rtc.h"

#include <string.h>

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#define LOG_PREFIX "APP CMD RTC: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

static const char* days_of_week[] = {"Mon", "Tue", "Wed", "Thu",
                                     "Fri", "Sat", "Sun"};

static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdRTCUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    oscillator <start|stop>\r\n"
"        This command will start or stop oscillator of the RTC module.\r\n"
"    oscillator status\r\n"
"        This command will report current status of oscillator.\r\n"
"\r\n"
"    date\n"
"        This command will report current date and time.\r\n"
"\r\n"
"    date <date-time>\r\n"
"        This command will set specified date and time to the RTC.\r\n"
"\r\n"
"    battery <enable|disable>\r\n"
"        Enable or disable backup battery.\r\n"
"\r\n"
"    battery status\r\n"
"        This command will report current status of backup battery.\r\n"
"\r\n"
"    dump\r\n"
"        This command will dump all internal registers.\r\n"
"\r\n"
"    register read <address>\r\n"
"        Read value of the given register.\r\n"
"\r\n"
"    register write <address> <value>\r\n"
"        Write value of the given register.\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static bool performRTCCheckAvailable(AppData* app_data) {
  return !RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle);
}

// ============ OSCILLATOR START/STOP/STATUS ============

static AppCommandTaskCallbackResult performRTCOscillatorStart(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableOscillator(&app_data->rtc.rtc_handle, true);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCOscillatorStop(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableOscillator(&app_data->rtc.rtc_handle, false);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCOscillatorStatus(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_OscillatorStatus(
          &app_data->rtc.rtc_handle,
          &app_data->command.rtc._private.oscillator.status);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        COMMAND_PRINT(
            "Oscillator status: %s\r\n",
            app_data->command.rtc._private.oscillator.status ? "ENABLED"
                                                             : "DISABLED");
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdRTCOscillator(AppData* app_data,
                               SYS_CMD_DEVICE_NODE* cmd_io,
                               int argc, char** argv) {
  if (argc != 3) {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "start")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                               cmd_io,
                               performRTCOscillatorStart,
                               performRTCCheckAvailable);
  } else if (STREQ(argv[2], "stop")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCOscillatorStop,
                              performRTCCheckAvailable);
  } else if (STREQ(argv[2], "status")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCOscillatorStatus,
                              performRTCCheckAvailable);
  } else {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  return true;
}

// ============ DATE SHOW/SET ============

static AppCommandTaskCallbackResult performRTCDateShow(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_ReadDateAndTime(
          &app_data->rtc.rtc_handle,
          &app_data->command.rtc._private.date.date_time);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        RTC_MCP7940N_DateTime* date_time = &app_data->command.rtc._private.date.date_time;
        // TODO(sergey): Need to get rid of manual year offset here.
        COMMAND_PRINT("%s %2d %s %d %2d:%02d:%02d\r\n",
                      days_of_week[date_time->day_of_week],
                      date_time->day,
                      months[date_time->month],
                      date_time->year + 2000,
                      date_time->hours,
                      date_time->minutes,
                      date_time->seconds);
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCDateSet(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_WriteDateAndTime(
          &app_data->rtc.rtc_handle,
          &app_data->command.rtc._private.date.date_time);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static bool dateDecode(char** argv, RTC_MCP7940N_DateTime* date_time) {
  int day_of_week, day, month, year, hour, minute, second, i;
  const char* month_name;
  const char* day_of_week_name;
  // Parse the string.
  day_of_week_name = argv[2];
  day = atoi(argv[3]);
  month_name = argv[4];
  year = atoi(argv[5]);
  // TODO(sergey): Check output code of scanf.
  sscanf(argv[6], "%d:%d:%d", &hour, &minute, &second);
  // Verify day of week.
  day_of_week = -1;
  for (i = 0; i < 7; ++i) {
    if (STREQ(days_of_week[i], day_of_week_name)) {
      day_of_week = i;
      break;
    }
  }
  if (day_of_week < 0 || day_of_week > 6) {
    return false;
  }
  // Verify date.
  if (day < 0 || day > 31) {
    // TODO(sergey): Tighten some check here so we don't allow 31 Feb.
    return false;
  }
  month = -1;
  for (i = 0; i < 12; ++i) {
    if (STREQ(months[i], month_name)) {
      month = i;
      break;
    }
  }
  if (month < 0 || month > 11) {
    return false;
  }
  // TODO(sergey): Avoid hardware-related limit here.
  if (year < 2001 || year > 2399) {
    return false;
  }
  if (hour < 0 || hour > 24) {
    return false;
  }
  if (minute < 0 || minute > 59) {
    return false;
  }
  if (second < 0 || second > 59) {
    return false;
  }
  // Settings are valid, convert them to RTC structure.
  date_time->seconds = second;
  date_time->minutes = minute;
  date_time->hours = hour;
  date_time->day_of_week = day_of_week;
  date_time->day = day;
  date_time->month = month;
  // TODO(sergey): Avoid hardware-related shifts.
  date_time->year = year - 2000;
  return true;
}

static int appCmdRTCDate(AppData* app_data,
                         SYS_CMD_DEVICE_NODE* cmd_io,
                         int argc, char** argv) {
  if (argc == 2) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCDateShow,
                              performRTCCheckAvailable);
  } else if (argc == 7) {
    if (dateDecode(argv, &app_data->command.rtc._private.date.date_time)) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCDateSet,
                              performRTCCheckAvailable);
    } else {
      COMMAND_MESSAGE("Failed to parse date.\r\n");
      COMMAND_MESSAGE("Expected format: ddd DD MMM YYYY HH:MM:SS.\r\n");
    }
  } else {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  return true;
}

// ============ BATTERY ENABLE/DISABLE/STATUS ============

static AppCommandTaskCallbackResult performRTCBatteryEnable(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableBatteryBackup(&app_data->rtc.rtc_handle, true);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return  APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCBatteryDisable(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableBatteryBackup(&app_data->rtc.rtc_handle, false);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCBatteryStatus(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_BatteryBackupStatus(
          &app_data->rtc.rtc_handle,
          &app_data->command.rtc._private.battery.status);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        COMMAND_PRINT(
            "Battery status: %s\r\n",
            app_data->command.rtc._private.battery.status ? "ENABLED"
                                                          : "DISABLED");
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdRTCBattery(AppData* app_data,
                            SYS_CMD_DEVICE_NODE* cmd_io,
                            int argc, char** argv) {
  if (argc != 3) {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "enable")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCBatteryEnable,
                              performRTCCheckAvailable);
  } else if (STREQ(argv[2], "disable")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCBatteryDisable,
                              performRTCCheckAvailable);
  } else if (STREQ(argv[2], "status")) {
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCBatteryStatus,
                              performRTCCheckAvailable);
  } else {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  return true;
}

// ============ DUMP ============

static void printRegisters(SYS_CMD_DEVICE_NODE* cmd_io,
                            uint8_t* registers,
                            uint8_t num_registers) {
  uint8_t current_register = 0;
  while (current_register < num_registers) {
    uint8_t i;
    COMMAND_PRINT("%02x |", current_register);
    for (i = 0; i < 8 && current_register < num_registers; ++i) {
      COMMAND_PRINT(" %02x", registers[current_register]);
      current_register++;
    }
    COMMAND_MESSAGE("\r\n");
  }
}

static AppCommandTaskCallbackResult performRTCDump(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_ReadNumRegisters(
          &app_data->rtc.rtc_handle,
          app_data->command.rtc._private.dump.registers_storage,
          RTC_MCP7940N_NUM_REGISTERS);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        printRegisters(cmd_io,
                        app_data->command.rtc._private.dump.registers_storage,
                        RTC_MCP7940N_NUM_REGISTERS);
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdRTCDump(AppData* app_data,
                         SYS_CMD_DEVICE_NODE* cmd_io,
                         int argc, char** argv) {
  if (argc != 2) {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  APP_Command_Task_Schedule(&app_data->command.task,
                            cmd_io,
                            performRTCDump,
                            performRTCCheckAvailable);
  return true;
}

// ============ REGISTER READ/WRITE ============

static AppCommandTaskCallbackResult performRTCRegisterRead(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_ReadRegister(
          &app_data->rtc.rtc_handle,
          app_data->command.rtc._private.reg.register_address,
          &app_data->command.rtc._private.reg.register_value);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        COMMAND_PRINT("Register value: 0x%02x.\r\n",
                      app_data->command.rtc._private.reg.register_value);
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static AppCommandTaskCallbackResult performRTCRegisterWrite(
    AppData* app_data,
    SYS_CMD_DEVICE_NODE* cmd_io,
    AppCommandTaskCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_TASK_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_WriteRegister(
          &app_data->rtc.rtc_handle,
          app_data->command.rtc._private.reg.register_address,
          app_data->command.rtc._private.reg.register_value);
      break;
    case APP_COMMAND_TASK_MODE_CALLBACK_UPDATE:
      // TODO(sergey): What if other routine will start using RTC
      // before this check?
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        return APP_COMMAND_TASK_RESULT_FINISHED;
      }
      break;
  }
  return APP_COMMAND_TASK_RESULT_RUNNING;
}

static int appCmdRTCRegister(AppData* app_data,
                             SYS_CMD_DEVICE_NODE* cmd_io,
                             int argc, char** argv) {
  if (argc < 4) {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  if (argc == 4 && STREQ(argv[2], "read")) {
    app_data->command.rtc._private.reg.register_address = atoi(argv[3]);
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCRegisterRead,
                              performRTCCheckAvailable);
  } else if (argc == 5 && STREQ(argv[2], "write")) {
    app_data->command.rtc._private.reg.register_address = atoi(argv[3]);
    app_data->command.rtc._private.reg.register_value = atoi(argv[4]);
    APP_Command_Task_Schedule(&app_data->command.task,
                              cmd_io,
                              performRTCRegisterWrite,
                              performRTCCheckAvailable);
  } else {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  return true;
}

void APP_Command_RTC_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.rtc, 0, sizeof(app_data->command.rtc));
}

int APP_Command_RTC(AppData* app_data,
                    SYS_CMD_DEVICE_NODE* cmd_io,
                    int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "oscillator")) {
    return appCmdRTCOscillator(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "date")) {
    return appCmdRTCDate(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "battery")) {
    return appCmdRTCBattery(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "dump")) {
    return appCmdRTCDump(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "register")) {
    return appCmdRTCRegister(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return appCmdRTCUsage(cmd_io, argv[0]);
  }
  return true;
}
