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

#include "app_command_wifi.h"

#include <stdbool.h>

#include "app.h"
#include "system_definitions.h"
#include "utildefines.h"

#include "driver/wifi/mrf24w/src/drv_wifi_iwpriv.h"

#define LOG_PREFIX "APP CMD WIFI: "

// Regular print / message.
#define WIFI_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define WIFI_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define WIFI_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define WIFI_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define WIFI_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define WIFI_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

static void IwSecurity_Set_None(void) {
  DRV_WIFI_SecurityOpenSet();
}

static void IwSecurity_Set_WEP(DRV_WIFI_WEP_CONTEXT* context, 
                               uint8_t* key,
                               uint16_t len) {
  if (len == 10) {
    context->wepSecurityType = DRV_WIFI_SECURITY_WEP_40;
    context->wepKeyLength = len;
    memcpy(context->wepKey, key, len);
    DRV_WIFI_SecurityWepSet(context);
  } else if (len == 26) {
    context->wepSecurityType = DRV_WIFI_SECURITY_WEP_104;
    context->wepKeyLength = len;
    memcpy(context->wepKey, key, len);
    DRV_WIFI_SecurityWepSet(context);
  } else {
    WIFI_ERROR_PRINT("Invalid WEP key length %d\r\n", len);
  }
}

static void IwSecurity_Set_WPA(DRV_WIFI_WEP_CONTEXT* context, 
                               uint8_t* key,
                               uint16_t len) {
  if ((len >= 8) && (len <= 63)) {
    context->wepSecurityType = DRV_WIFI_SECURITY_WPA_WITH_PASS_PHRASE;
    context->wepKeyLength = len;
    memcpy(context->wepKey, key, len);
    DRV_WIFI_SecurityWepSet(context);
  } else {
    WIFI_ERROR_PRINT("Invalid WPA passphrase length %d\r\n", len);
  }
}

static void IwSecurity_Set_WPA2(DRV_WIFI_WEP_CONTEXT* context, 
                                uint8_t* key,
                                uint16_t len) {
  if ((len >= 8) && (len <= 63)) {
    context->wepSecurityType = DRV_WIFI_SECURITY_WPA2_WITH_PASS_PHRASE;
    context->wepKeyLength = len;
    memcpy(context->wepKey, key, len);
    DRV_WIFI_SecurityWepSet(context);
  } else {
    WIFI_ERROR_PRINT("Invalid WPA2 passphrase length %d\r\n", len);
  }
}

static void IwSecurity_Set_WPSPIN(DRV_WIFI_WEP_CONTEXT* context, 
                                  uint8_t* key,
                                  uint16_t len) {
  if (len == 8) {
    context->wepSecurityType = DRV_WIFI_SECURITY_WPS_PIN;
    context->wepKeyLength = len;
    memcpy(context->wepKey, key, len);
    DRV_WIFI_SecurityWepSet(context);
  } else {
    WIFI_ERROR_PRINT("Invalid PIN length %d\r\n", len);
  }
}

int APP_Command_IwSecurity(struct AppData* app_data,
                           struct SYS_CMD_DEVICE_NODE* cmd_io,
                           int argc, char** argv) {
  DRV_WIFI_WEP_CONTEXT context;
  size_t len;
  if ((argc == 2) && (STREQ(argv[1], "open"))) {
    IwSecurity_Set_None();
  } else if ((3 == argc) && (STREQ(argv[1], "wep40"))) {
    len = strlen(argv[2]);
    if (len != 10) {
      WIFI_ERROR_MESSAGE("WEP40 key length error\r\n");
      return false;
    }
    IwSecurity_Set_WEP(&context, (uint8_t *)argv[2], len);
  } else if ((argc == 3) && (STREQ(argv[1], "wep104"))) {
    len = strlen(argv[2]);
    if (len != 26) {
      WIFI_ERROR_MESSAGE("WEP104 key length error\r\n");
      return false;
    }
    IwSecurity_Set_WEP(&context, (uint8_t *)argv[2], len);
  } else if ((argc == 3) && (STREQ(argv[1], "wpa"))) {
    len = strlen(argv[2]);
    if ((len < 8) || (len > 63)) {
      WIFI_ERROR_MESSAGE("WPA pass phrase length error\r\n");
      return false;
    }
    IwSecurity_Set_WPA(&context, (uint8_t *)argv[2], len);
  } else if ((argc == 3) && (STREQ(argv[1], "wpa2"))) {
    len = strlen(argv[2]);
    if ((len < 8) || (len > 63)) {
      WIFI_ERROR_MESSAGE("WPA2 pass phrase length error\r\n");
      return false;
    }
    IwSecurity_Set_WPA2(&context, (uint8_t *)argv[2], len);
  } else if ((argc == 3) && (STREQ(argv[1], "pin"))) {
    len = strlen(argv[2]);
    if (len != 8) {
      WIFI_ERROR_MESSAGE("WPS PIN length error\r\n");
      return false;
    }
    IwSecurity_Set_WPSPIN(&context, (uint8_t *)argv[3], len);
  } else {
    WIFI_MESSAGE("Unknown security mode or wrong parameters were typed\r\n");
    WIFI_MESSAGE("Possible usages:\r\n");
    WIFI_MESSAGE("- open : iwsecurity open\r\n");
    WIFI_MESSAGE("- wep40 : iwsecurity wep40 <key>\r\n");
    WIFI_MESSAGE("- wep104 : iwsecurity wep104 <key>\r\n");
    WIFI_MESSAGE("- wpa : iwsecurity wpa <passphrase>\r\n");
    WIFI_MESSAGE("- wpa2 : iwsecurity wpa2 <passphrase>\r\n");
    WIFI_MESSAGE("- wps pin : iwsecurity pin <pin>\r\n");
    return false;
  }
  return true;
}
