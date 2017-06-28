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

#include "app_nixie.h"

#include "system_definitions.h"

#define LOG_PREFIX "APP NIXIE: "

// Regular print / message.
#define NIXIE_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define NIXIE_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define NIXIE_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define NIXIE_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

void APP_Nixie_Initialize(AppNixieData* app_nixie_data) {
  SYS_MESSAGE("Nixie tubes subsystem initialized.\r\n");
}

void APP_Nixie_Tasks(AppNixieData* app_nixie_data) {
  switch (app_nixie_data->state) {
    case APP_NIXIE_STATE_IDLE:
      // Nothing to do.
      // TODO(sergey): Check timer, and start fetching new value from server.
      break;
  }
}

bool APP_Nixie_IsBusy(AppNixieData* app_nixie_data) {
  return app_nixie_data->state != APP_NIXIE_STATE_IDLE;
}
