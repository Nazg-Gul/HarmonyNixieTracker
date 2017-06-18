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

#include "app_command_ntp.h"

#include <stdbool.h>

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#include <tcpip/sntp.h>

#define LOG_PREFIX "APP CMD NTP: "

// Regular print / message.
#define NTP_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NTP_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define NTP_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NTP_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define NTP_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NTP_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

////////////////////////////////////////////////////////////////////////////////
// Internal routines.

static int app_command_ntp_usage(SYS_CMD_DEVICE_NODE* cmd_io,
                                 const char* argv0) {
  COMMAND_PRINT("Usage: %s command arguments ...\r\n", argv0);
  COMMAND_MESSAGE(
"where 'command' is one of the following:\r\n"
"\r\n"
"    seconds <enable|disable>\r\n"
"        Obtains the current time from the SNTP module.\r\n"
    );
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation.

static int app_command_ntp_seconds(AppData* app_data,
                                   SYS_CMD_DEVICE_NODE* cmd_io,
                                   int argc, char** argv) {
  (void) app_data;  /* Ignored. */
  if (argc != 2) {
    return app_command_ntp_usage(cmd_io, argv[0]);
  }
  uint32_t seconds = TCPIP_SNTP_UTCSecondsGet();
  COMMAND_PRINT("UTC seconds: %d\r\n", seconds);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

int APP_Command_NTP(struct AppData* app_data,
                    struct SYS_CMD_DEVICE_NODE* cmd_io,
                    int argc, char** argv) {
  if (argc == 1) {
    return app_command_ntp_usage(cmd_io, argv[0]);
    return true;
  }
  if (STREQ(argv[1], "seconds")) {
    return app_command_ntp_seconds(app_data, cmd_io, argc, argv);
  } else {
    // For unknown command show usage.
    return app_command_ntp_usage(cmd_io, argv[0]);
  }
  return true;
}
