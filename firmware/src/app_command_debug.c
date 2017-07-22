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

#include "app_command_debug.h"

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#define LOG_PREFIX "APP CMD PHY: "
#define DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

#ifdef SYS_DEBUG_ENABLE

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int appCmdDebugUsage(SYS_CMD_DEVICE_NODE* cmd_io, const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    wolfssl <enable|disable>\r\n"
"        Enable or disable debug and logging from WolfSSL.\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static int appCmdDebugWolfSSL(AppData* app_data,
                              SYS_CMD_DEVICE_NODE* cmd_io,
                              int argc, char** argv) {
  (void) app_data;  // Ignored.
  if (argc != 3) {
    return appCmdDebugUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[2], "enable")) {
    APP_HTTPS_Client_SetWolfSSLDebug(true);
    COMMAND_MESSAGE("WolfSSL debugging and logging are enabled.\r\n");
  } else if (STREQ(argv[2], "disable")) {
    APP_HTTPS_Client_SetWolfSSLDebug(false);
    COMMAND_MESSAGE("WolfSSL debugging and logging are disabled.\r\n");
  } else {
    return appCmdDebugUsage(cmd_io, argv[0]);
  }
  return true;
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Public API.

int APP_Command_Debug(AppData* app_data,
                      SYS_CMD_DEVICE_NODE* cmd_io,
                      int argc, char** argv) {
#ifdef SYS_DEBUG_ENABLE
  if (!APP_Command_CheckAvailable(app_data, cmd_io)) {
    return true;
  }
  if (argc == 1) {
    return appCmdDebugUsage(cmd_io, argv[0]);
  }
  if (STREQ(argv[1], "wolfssl")) {
    appCmdDebugWolfSSL(app_data, cmd_io, argc, argv);
    return true;
  } else {
    // For unknown command show usage.
    return appCmdDebugUsage(cmd_io, argv[0]);
  }
#else
  COMMAND_MESSAGE("Application is built without debug support.\r\n");
#endif
  return true;
}
