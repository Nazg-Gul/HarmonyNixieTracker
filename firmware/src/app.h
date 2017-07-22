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

#ifndef _APP_H
#define _APP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"

// TODO(sergey): Think how we can reduce header hell dependency here.
#include "app_command.h"
#include "app_flash.h"
#include "app_https_client.h"
#include "app_network.h"
#include "app_nixie.h"
#include "app_power.h"
#include "app_rtc.h"
#include "app_shift_register.h"
#include "app_usb_hid.h"

typedef enum {
  // Show greetings message in the console.
  APP_STATE_GREETINGS,
  // Run all runtime services.
  APP_STATE_RUN_SERVICES,
  // Unrecoverable error, can't continue
  APP_STATE_ERROR,
} AppState;

typedef struct AppData {
  struct SystemObjects* system_objects;

  // Current state of the global state machine.
  AppState state;

  // Descriptors of all peripherials.
  AppFlashData flash;
  AppHTTPSClientData https_client;
  AppNetworkData network;
  AppNixieData nixie;
  AppRTCData rtc;
  AppShiftRegisterData shift_register;
  AppUSBHIDData usb_hid;

  // Internal state machine of sub-routines.
  AppCommandData command;
} AppData;

// Stubs for default Harmony code.
#define APP_Initialize()
#define APP_Tasks()

// Initialize all application specific data.
void APP_Initialize_Real(AppData* app_data,
                         struct SystemObjects* system_objects);

// Perform all application tasks.
void APP_Tasks_Real(AppData* app_data);

#endif  // _APP_H
