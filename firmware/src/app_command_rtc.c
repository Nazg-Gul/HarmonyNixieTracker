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

// TODO(sergey): Use message of command processor?
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

static const char* days_of_week[] = {"Mon", "Tue", "Wed", "Thu",
                                     "Fri", "Sat", "Sun"};

static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static int app_command_rtc_usage(SYS_CMD_DEVICE_NODE* cmd_io,
                                 const char* argv0) {
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
    );
  return true;
}

static void app_command_rtc_set_callback(AppData* app_data,
                                         SYS_CMD_DEVICE_NODE* cmd_io,
                                         AppCommandRTCCallback callback) {
  app_data->command.rtc.callback = callback;
  app_data->command.rtc.callback_cmd_io = cmd_io;
}

static void app_command_rtc_start_task(AppData* app_data,
                                       SYS_CMD_DEVICE_NODE* cmd_io,
                                       AppCommandRTCCallback callback) {
  SYS_ASSERT(app_data->command.rtc.state == APP_COMMAND_RTC_STATE_NONE,
             "\r\nAttempt to start task while another one is in process\r\n");
  app_data->command.state = APP_COMMAND_STATE_RTC;
  app_data->command.rtc.state = APP_COMMAND_RTC_STATE_WAIT_AVAILABLE;
  app_command_rtc_set_callback(app_data, cmd_io, callback);
}

static void app_command_rtc_finish_task(AppData* app_data) {
  SYS_ASSERT(app_data->command.rtc.state != APP_COMMAND_RTC_STATE_NONE,
             "\r\nAttempt to finish non-existing task\r\n");
  app_data->command.state = APP_COMMAND_STATE_NONE;
  app_data->command.rtc.state = APP_COMMAND_RTC_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.rtc.callback = NULL;
  app_data->command.rtc.callback_cmd_io = NULL;
}

static void oscillator_start(AppData* app_data,
                             SYS_CMD_DEVICE_NODE* cmd_io,
                             AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableOscillator(&app_data->rtc.rtc_handle, true);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static void oscillator_stop(AppData* app_data,
                            SYS_CMD_DEVICE_NODE* cmd_io,
                            AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableOscillator(&app_data->rtc.rtc_handle, false);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static void oscillator_status(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_OscillatorStatus(&app_data->rtc.rtc_handle,
                                    &app_data->command.rtc.status);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        COMMAND_PRINT("Oscillator status: %s\r\n",
                      app_data->command.rtc.status ? "ENABLED" : "DISABLED");
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static int app_command_rtc_oscillator(AppData* app_data,
                                      SYS_CMD_DEVICE_NODE* cmd_io,
                                      int argc, char** argv) {
  if (argc != 3) {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "start")) {
    app_command_rtc_start_task(app_data, cmd_io, &oscillator_start);
  } else if (STREQ(argv[2], "stop")) {
    app_command_rtc_start_task(app_data, cmd_io, &oscillator_stop);
  } else if (STREQ(argv[2], "status")) {
    app_command_rtc_start_task(app_data, cmd_io, &oscillator_status);
  } else {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  return true;
}

static void date_show(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_ReadDateAndTime(&app_data->rtc.rtc_handle,
                                   &app_data->command.rtc.date_time);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        // TODO(sergey): Need to get rid of manual year offset here.
        COMMAND_PRINT("%s %2d %s %d %2d:%02d:%02d\r\n",
                      days_of_week[app_data->command.rtc.date_time.day_of_week],
                      app_data->command.rtc.date_time.day,
                      months[app_data->command.rtc.date_time.month],
                      app_data->command.rtc.date_time.year + 2000,
                      app_data->command.rtc.date_time.hours,
                      app_data->command.rtc.date_time.minutes,
                      app_data->command.rtc.date_time.seconds);
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static void date_set(AppData* app_data,
                     SYS_CMD_DEVICE_NODE* cmd_io,
                     AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_WriteDateAndTime(&app_data->rtc.rtc_handle,
                                    &app_data->command.rtc.date_time);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static bool date_decode(char** argv,
                        RTC_MCP7940N_DateTime* date_time) {
  int day, month, year, hour, minute, second, i;
  const char* month_name;
  // Parse the string.
  day = atoi(argv[2]);
  month_name = argv[3];
  year = atoi(argv[4]);
  // TODO(sergey): Check output code of scanf.
  sscanf(argv[5], "%d:%d:%d", &hour, &minute, &second);
  // Verify date.
  if (day < 0 || day > 31) {
    // TODO(sergey): Tighten some check here so we don't allow 31 Feb.
    return false;
  }
  month = -1;
  for (i = 0; i < 12; ++i) {
    if (STREQ(months[i], month_name)) {
      month = i + 1;
      break;
    }
  }
  if (month < 1 || month > 12) {
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
  date_time->day = day;
  date_time->month = month;
  // TODO(sergey): Avoid hardware-related shifts.
  date_time->year = year - 2000;
  return true;
}

static int app_command_rtc_date(AppData* app_data,
                                SYS_CMD_DEVICE_NODE* cmd_io,
                                int argc, char** argv) {
  if (argc == 2) {
    app_command_rtc_start_task(app_data, cmd_io, &date_show);
  } else if (argc == 6) {
    if (date_decode(argv, &app_data->command.rtc.date_time)) {
      app_command_rtc_start_task(app_data, cmd_io, &date_set);
    } else {
      COMMAND_MESSAGE("Failed to parse date.\r\n");
      COMMAND_MESSAGE("Expected format: DD MMM YYYY HH:MM:SS.\r\n");
    }
  } else {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  return true;
}

static void battery_enable(AppData* app_data,
                           SYS_CMD_DEVICE_NODE* cmd_io,
                           AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableBatteryBackup(&app_data->rtc.rtc_handle, true);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static void battery_disable(AppData* app_data,
                            SYS_CMD_DEVICE_NODE* cmd_io,
                            AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_EnableBatteryBackup(&app_data->rtc.rtc_handle, true);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static void battery_status(AppData* app_data,
                           SYS_CMD_DEVICE_NODE* cmd_io,
                           AppCommandRTCCallbackMode mode) {
  switch (mode) {
    case APP_COMMAND_RTC_MODE_CALLBACK_INVOKE:
      RTC_MCP7940N_BatteryBackupStatus(&app_data->rtc.rtc_handle,
                                       &app_data->command.rtc.status);
      break;
    case APP_COMMAND_RTC_MODE_CALLBACK_UPDATE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        COMMAND_PRINT("Battery status: %s\r\n",
                      app_data->command.rtc.status ? "ENABLED" : "DISABLED");
        app_command_rtc_finish_task(app_data);
      }
      break;
  }
}

static int app_command_rtc_battery(AppData* app_data,
                                   SYS_CMD_DEVICE_NODE* cmd_io,
                                   int argc, char** argv) {
  if (argc != 3) {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "enable")) {
    app_command_rtc_start_task(app_data, cmd_io, &battery_enable);
  } else if (STREQ(argv[2], "disable")) {
    app_command_rtc_start_task(app_data, cmd_io, &battery_disable);
  } else if (STREQ(argv[2], "status")) {
    app_command_rtc_start_task(app_data, cmd_io, &battery_status);
  } else {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  return true;
}

void APP_Command_RTC_Initialize(AppData* app_data) {
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  memset(&app_data->command.rtc, 0, sizeof(app_data->command.rtc));
  app_data->command.rtc.state = APP_COMMAND_RTC_STATE_NONE;
  // TODO(sergey): Ignore for release builds to save CPU ticks?
  app_data->command.rtc.callback = NULL;
  app_data->command.rtc.callback_cmd_io = NULL;
}

void APP_Command_RTC_Tasks(struct AppData* app_data) {
  switch (app_data->command.rtc.state) {
    case APP_COMMAND_RTC_STATE_NONE:
      // Pass, nothing to do.
      break;
    case APP_COMMAND_RTC_STATE_WAIT_AVAILABLE:
      if (!RTC_MCP7940N_IsBusy(&app_data->rtc.rtc_handle)) {
        DEBUG_MESSAGE("RTC is free, invoking command.\r\n");
        app_data->command.rtc.state = APP_COMMAND_RTC_STATE_RUNNING;
        app_data->command.rtc.callback(app_data,
                                       app_data->command.rtc.callback_cmd_io,
                                       APP_COMMAND_RTC_MODE_CALLBACK_INVOKE);
      } else {
        DEBUG_MESSAGE("RTC is busy, waiting.\r\n");
      }
      break;
    case APP_COMMAND_RTC_STATE_RUNNING:
      app_data->command.rtc.callback(app_data,
                                     app_data->command.rtc.callback_cmd_io,
                                     APP_COMMAND_RTC_MODE_CALLBACK_UPDATE);
      break;
  }
}

int APP_Command_RTC(AppData* app_data,
                    SYS_CMD_DEVICE_NODE* cmd_io,
                    int argc, char** argv) {
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "oscillator")) {
    return app_command_rtc_oscillator(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "date")) {
    return app_command_rtc_date(app_data, cmd_io, argc, argv);
  } else if (STREQ(argv[1], "battery")) {
    return app_command_rtc_battery(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return app_command_rtc_usage(cmd_io, argv[0]);
  }
  return true;
}
