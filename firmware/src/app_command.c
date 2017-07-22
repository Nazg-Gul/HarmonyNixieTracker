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

#include "app_command.h"

#include "app.h"
#include "system_definitions.h"

// TODO(sergey): Find a way to avoid this global thing.
static AppData* g_app_data;

static int cmdFetch(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdFlash(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdIwsecurity(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdNixie(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdNTP(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdPhy(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdPower(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdRTC(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);
static int cmdShiftRegister(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv);

static const SYS_CMD_DESCRIPTOR commands[] = {
  {"fetch", cmdFetch, ": fetch HTTP(S) page"},
  {"flash", cmdFlash, ": Serial flash configuration"},
  // TODO(sergey): This should in theory be handled by iwconfig, but it is not.
  // So we work this around for particular Harmony version and device we use.
  {"iwsecurity", cmdIwsecurity, ": WiFi security configuration"},
  {"nixie", cmdNixie, ": Nixie tube display configuration"},
  {"ntp", cmdNTP, ": NTP client configuration"},
  {"phy", cmdPhy, ": Ethrenat PHY configuration"},
  {"power", cmdPower, ": Power supply configuration"},
  {"rtc", cmdRTC, ": Real Time Clock configuration"},
  {"shift_register", cmdShiftRegister, ": Shift register manipulation"},
};

static int cmdFetch(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_Fetch(g_app_data, cmd_io, argc, argv);
}

static int cmdFlash(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_Flash(g_app_data, cmd_io, argc, argv);
}

static int cmdIwsecurity(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_IwSecurity(g_app_data, cmd_io, argc, argv);
}

static int cmdNixie(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_Nixie(g_app_data, cmd_io, argc, argv);
}

static int cmdNTP(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_NTP(g_app_data, cmd_io, argc, argv);
}

static int cmdPower(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_Power(g_app_data, cmd_io, argc, argv);
}

static int cmdPhy(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_Phy(g_app_data, cmd_io, argc, argv);
}

static int cmdRTC(SYS_CMD_DEVICE_NODE* cmd_io, int argc, char** argv) {
  return APP_Command_RTC(g_app_data, cmd_io, argc, argv);
}

static int cmdShiftRegister(SYS_CMD_DEVICE_NODE* cmd_io,
                            int argc, char** argv) {
  return APP_Command_ShiftRegister(g_app_data, cmd_io, argc, argv);
}

void APP_Command_Initialize(AppData* app_data) {
  const int num_commands = sizeof(commands) / sizeof(*commands);
  if (SYS_CMD_ADDGRP(commands, num_commands, "app", ": app commands") == -1) {
    SYS_CONSOLE_MESSAGE("APP: Error initializing command processor\r\n");
  }
  g_app_data = app_data;
  app_data->command.state = APP_COMMAND_STATE_NONE;
  APP_Command_Fetch_Initialize(app_data);
  APP_Command_Flash_Initialize(app_data);
  APP_Command_Nixie_Initialize(app_data);
  APP_Command_RTC_Initialize(app_data);
  APP_Command_ShiftRegister_Initialize(app_data);
}

void APP_Command_Tasks(AppData* app_data) {
  switch (app_data->command.state) {
    case APP_COMMAND_STATE_NONE:
      // Nothing to do.
      break;
    case APP_COMMAND_STATE_FETCH:
      APP_Command_Fetch_Tasks(app_data);
      break;
    case APP_COMMAND_STATE_FLASH:
      APP_Command_Flash_Tasks(app_data);
      break;
    case APP_COMMAND_STATE_NIXIE:
      APP_Command_Nixie_Tasks(app_data);
      break;
    case APP_COMMAND_STATE_RTC:
      APP_Command_RTC_Tasks(app_data);
      break;
    case APP_COMMAND_STATE_SHIFT_REGISTER:
      APP_Command_ShiftRegister_Tasks(app_data);
      break;
  }
}

bool APP_Command_IsBusy(AppData* app_data) {
  return app_data->command.state != APP_COMMAND_STATE_NONE;
}

bool APP_Command_CheckAvailable(AppData* app_data,
                                struct SYS_CMD_DEVICE_NODE* cmd_io) {
  if (APP_Command_IsBusy(app_data)) {
    COMMAND_MESSAGE("Command processor is busy, try again later.\r\n");
    return false;
  }
  return true;
}
