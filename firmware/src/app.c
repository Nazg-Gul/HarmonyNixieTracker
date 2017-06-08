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

#include "app.h"

#include "app_command.h"
#include "app_network.h"
#include "app_usb_hid.h"
#include "app_version.h"

static void app_greetings(AppData* app_data) {
  SYS_CONSOLE_MESSAGE("\r\n");
  SYS_CONSOLE_MESSAGE("System initialization finished.\r\n");
  SYS_CONSOLE_MESSAGE("\r\n");
  SYS_CONSOLE_MESSAGE("====================================\r\n");
  SYS_CONSOLE_MESSAGE("***          NixieTracker        ***\r\n");
  SYS_CONSOLE_MESSAGE("====================================\r\n");
  SYS_CONSOLE_MESSAGE("\r\n");
  SYS_CONSOLE_PRINT("Hardware version: %s\r\n", APP_VERSION_HARDWARE);
  SYS_CONSOLE_PRINT("Software version: %s\r\n", APP_VERSION_SOFTWARE);
  SYS_CONSOLE_MESSAGE("\r\n");
  app_data->state = APP_STATE_RUN_SERVICES;
}

void APP_Initialize(AppData* app_data, SYSTEM_OBJECTS* system_objects) {
  app_data->system_objects = system_objects;
  app_data->state = APP_STATE_GREETINGS;
  APP_Command_Initialize(app_data);
  APP_Network_Initialize(&app_data->network, app_data->system_objects);
  APP_USB_HID_Initialize(&app_data->usb_hid);
  APP_RTC_Initialize(&app_data->rtc);
}

void APP_Tasks(AppData* app_data) {
  switch (app_data->state) {
    case APP_STATE_GREETINGS:
      app_greetings(app_data);
      break;
    case APP_STATE_RUN_SERVICES:
      APP_Network_Tasks(&app_data->network);
      APP_USB_HID_Tasks(&app_data->usb_hid);
      APP_RTC_Tasks(&app_data->rtc);
      APP_Command_Tasks(app_data);
      break;
    case APP_STATE_ERROR:
      // TODO(sergey): Do we need to do something here?
      break;
  }
}
