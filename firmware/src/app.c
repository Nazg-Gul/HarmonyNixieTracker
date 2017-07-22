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
#include "system_objects.h"

static void appGreetings(AppData* app_data) {
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
  // Disable status LEDs.
  STATUS_LED_1_Off();
  STATUS_LED_2_Off();
  STATUS_LED_3_Off();
  STATUS_LED_4_Off();
  app_data->state = APP_STATE_RUN_SERVICES;
}

void APP_Initialize_Real(AppData* app_data, SystemObjects* system_objects) {
  app_data->system_objects = system_objects;
  app_data->state = APP_STATE_GREETINGS;
  APP_Command_Initialize(app_data);
  APP_Network_Initialize(&app_data->network, app_data->system_objects);
  APP_USB_HID_Initialize(&app_data->usb_hid);
  APP_RTC_Initialize(&app_data->rtc);
  APP_Flash_Initialize(&app_data->flash, app_data->system_objects);
  APP_Power_Initialize();
  APP_HTTPS_Client_Initialize(&app_data->https_client);
  APP_ShiftRegister_Initialize(&app_data->shift_register);
  APP_Nixie_Initialize(&app_data->nixie,
                       &app_data->https_client,
                       &app_data->shift_register);
}

void APP_Tasks_Real(AppData* app_data) {
  switch (app_data->state) {
    case APP_STATE_GREETINGS:
      appGreetings(app_data);
      break;
    case APP_STATE_RUN_SERVICES:
      // TODO(sergey): Shall we pixk some subset of tasks per iteration?
      APP_Network_Tasks(&app_data->network);
      APP_USB_HID_Tasks(&app_data->usb_hid);
      APP_RTC_Tasks(&app_data->rtc);
      APP_Flash_Tasks(&app_data->flash);
      APP_HTTPS_Client_Tasks(&app_data->https_client);
      APP_ShiftRegister_Tasks(&app_data->shift_register);
      APP_Nixie_Tasks(&app_data->nixie);
      APP_Command_Tasks(app_data);
      if (!APP_Command_IsBusy(app_data)) {
        SYS_CMD_READY_TO_READ();
      }
      break;
    case APP_STATE_ERROR:
      // TODO(sergey): Do we need to do something here?
      break;
  }
}
