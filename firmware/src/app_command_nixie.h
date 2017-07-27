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

#ifndef _APP_COMMAND_NIXIE_H
#define _APP_COMMAND_NIXIE_H

#include <stdint.h>

#include "app_nixie.h"

struct AppData;
struct SYS_CMD_DEVICE_NODE;

typedef struct AppCommandNixieData {
  // Per-task storage.
  union {
    struct {
      // Value to be displayed.
      char value[MAX_NIXIE_TUBES];
    } display;
    struct {
      // Boolean value indicating whether value was received from server.
      bool is_fetched;
      // Fetched value.
      char value[MAX_NIXIE_TUBES];
    } fetch;
  } _private;
} AppCommandNixieData;

// Initialize nixie related command processor state and data.
void APP_Command_Nixie_Initialize(struct AppData* app_data);

// Handle `nixie` command line command.
int APP_Command_Nixie(struct AppData* app_data,
                      struct SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv);

#endif  // _APP_COMMAND_NIXIE_H
