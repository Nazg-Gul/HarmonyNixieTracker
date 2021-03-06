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

#ifndef _APP_COMMAND_H
#define _APP_COMMAND_H

// Simple task scheduler implementation.
#include "app_command_task.h"

#include "app_command_debug.h"
#include "app_command_fetch.h"
#include "app_command_flash.h"
#include "app_command_nixie.h"
#include "app_command_ntp.h"
#include "app_command_rtc.h"
#include "app_command_shift_register.h"
#include "app_command_phy.h"
#include "app_command_power.h"
#include "app_command_wifi.h"

#include <stdbool.h>

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef struct AppCommandData {
  // Simple task scheduler.
  AppCommandTaskData task;
  // Per-command data for the state machine.
  AppCommandFetchData fetch;
  AppCommandNixieData nixie;
  AppCommandRTCData rtc;
  AppCommandShiftRegisterData shift_register;
} AppCommandData;

void APP_Command_Initialize(struct AppData* app_data);

// Perform all periodic command related tasks.
void APP_Command_Tasks(struct AppData* app_data);

// Internal helpers.

#define COMMAND_PRINT(format, ...) \
    (*cmd_io->pCmdApi->print)(cmd_io->cmdIoParam, format, ##__VA_ARGS__)

#define COMMAND_MESSAGE(message) \
    (*cmd_io->pCmdApi->msg)(cmd_io->cmdIoParam, message)

// Check whether command processor is busy with some command.
bool APP_Command_IsBusy(struct AppData* app_data);

// Check availability of command processor, if it's busy will
// print message about this.
bool APP_Command_CheckAvailable(struct AppData* app_data,
                                struct SYS_CMD_DEVICE_NODE* cmd_io);

#endif  // _APP_COMMAND_H
