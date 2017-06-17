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

#ifndef _APP_COMMAND_FLASH_H
#define _APP_COMMAND_FLASH_H

struct AppData;
struct SYS_CMD_DEVICE_NODE;

// TODO(sergey): Consider de-duplicating task logic with RTC commands.

typedef enum {
  // Callback is being invoked for the first time after flash module became
  // available.
  APP_COMMAND_FLASH_MODE_CALLBACK_INVOKE,
  // Callback is called after callback was invoked, to update status of async
  // running task.
  APP_COMMAND_FLASH_MODE_CALLBACK_UPDATE,
} AppCommandFlashCallbackMode;

typedef void (*AppCommandFlashCallback)(struct AppData* app_data,
                                        struct SYS_CMD_DEVICE_NODE* cmd_io,
                                        AppCommandFlashCallbackMode mode);

typedef enum {
  // None of the flash tasks to be performed.
  APP_COMMAND_FLASH_STATE_NONE,
  // Wait for flash to become available for the commands.
  APP_COMMAND_FLASH_STATE_WAIT_AVAILABLE,
  // Flash command is running a background task.
  APP_COMMAND_FLASH_STATE_RUNNING,
} AppCommandFlashState;

typedef struct AppCommandFlashData {
  // Current state of flash command handling.
  AppCommandFlashState state;

  // Callback which is executed once flash handle is free.
  //
  // This is a way to avoid too many states of the state machine,
  // and general rule here is:
  //
  // - Command processor callback sets flash data state to WAIT_AVAILABLE,
  //   additionally it sets which callback needs to be called.
  // - Flash state machine waits for flash handle to be free.
  // - Flash state machine calls the specified callback.
  AppCommandFlashCallback callback;
  struct SYS_CMD_DEVICE_NODE* callback_cmd_io;
} AppCommandFlashData;

// Initialize flash related command processor state and data.
void APP_Command_Flash_Initialize(struct AppData* app_data);

// Perform flash command processor tasks.
void APP_Command_Flash_Tasks(struct AppData* app_data);

// Handle `flash` command line command.
int APP_Command_Flash(struct AppData* app_data,
                      struct SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv);

#endif  // _APP_COMMAND_FLASH_H
