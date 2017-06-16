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

#include "app_command_flash.h"
#include "app_command_rtc.h"

#include <stdbool.h>

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef enum {
  // Command processor has nothing to do.
  APP_COMMAND_STATE_NONE,
  // Per-command processor state.
  APP_COMMAND_STATE_FLASH,
  APP_COMMAND_STATE_RTC,
} AppCommandState;

typedef struct AppCommandData {
  AppCommandState state;
  // Per-command data for the state machine.
  AppCommandFlashData flash;
  AppCommandRTCData rtc;
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
