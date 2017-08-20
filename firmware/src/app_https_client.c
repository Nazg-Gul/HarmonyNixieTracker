// Based on Microchip Harmony examples with the following license:
//
// Copyright (c) 2013-2015 released Microchip Technology Inc. All rights reserved.
//
// Microchip licenses to you the right to use, modify, copy and distribute
// Software only when embedded on a Microchip microcontroller or digital signal
// controller that is integrated into your product or third party product
// (pursuant to the sublicense terms in the accompanying license agreement).
//
// You should refer to the license agreement accompanying this Software for
// additional information regarding your rights and obligations.
//
// SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
// MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
// IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
// CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
// OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
// INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
// CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
// SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
// (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
//
// Modifications are:
//
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

#include "app_https_client.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <net/pres/net_pres_socketapi.h>
#include <config.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/logging.h>

#include "system_definitions.h"
#include "utildefines.h"

#include "util_string.h"
#include "util_url.h"

#define LOG_PREFIX "APP HTTPS CLIENT: "

// Regular print / message.
#define HTTPS_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define HTTPS_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define HTTPS_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define HTTPS_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define HTTPS_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define HTTPS_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

#if defined(SYS_DEBUG_ENABLE) && defined(DEBUG_WOLFSSL)
#  define WITH_WOLFSSL_DEBUG
#endif

////////////////////////////////////////////////////////////////////////////////
// Global variables.

static bool g_wolfssl_debug = false;

////////////////////////////////////////////////////////////////////////////////
// Internal helpers.

#ifdef WITH_WOLFSSL_DEBUG
// Logging function for WolfSSL.
void wolfssl_logging_cb(const int level, const char* const message) {
  if (!g_wolfssl_debug) {
    return;
  }
  (void) level; // Ignored,
  APP_PRINT("WolfSSL: ", "%s\r\n", message);
}
#endif

static void enterErrorState(AppHTTPSClientData* app_https_client_data) {
  // TODO(sergey): Add some sort of error code.
  app_https_client_data->state = APP_HTTPS_CLIENT_STATE_ERROR;
}

static bool checkNetworkIsAvailable(AppHTTPSClientData* app_https_client_data) {
  (void) app_https_client_data;  /* Ignored. */
  // TODO(sergey): Needs implementation.
  return true;
}

static void waitForNetworkAvailable(AppHTTPSClientData* app_https_client_data) {
  if (checkNetworkIsAvailable(app_https_client_data)) {
    HTTPS_DEBUG_MESSAGE("Network is available.\r\n");
    app_https_client_data->state = APP_HTTPS_CLIENT_STATE_PARSE_REQUEST_URL;
  } else if (SYS_TMR_SystemCountGet() > app_https_client_data->timeout) {
    HTTPS_ERROR_MESSAGE("Timeout waiting for network connection.\r\n");
    enterErrorState(app_https_client_data);
  }
}

// Parse URL into decoupled components.
static void pasreRequestURL(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (!urlParseGetParts(data->request_url,
                        data->scheme, sizeof(data->scheme),
                        NULL, 0,  // User.
                        NULL, 0,  // Password.
                        data->host, sizeof(data->host),
                        &data->port,
                        NULL, 0,  // Path.
                        NULL, 0,  // Query.
                        NULL, 0,  // fragment.
                        data->path, sizeof(data->path))) {  // Path suffix
    HTTPS_ERROR_MESSAGE("Error parsing URL.\r\n");
    enterErrorState(app_https_client_data);
    return;
  }
  app_https_client_data->state = APP_HTTPS_CLIENT_STATE_PROCESS_REQUEST;
}

// Process request by checking what is the way to approach the connection:
// - Do we need DNS lookup?
// - Shall we do IPv4 connection?
// - Shall we do IPv6 connection?
static void processRequest(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  // TODO(sergey): Shall we strip possible [] from host name to get IPv6
  // proper address?
  if (TCPIP_Helper_StringToIPAddress(data->host, &data->ip_address.v4Add)) {
    HTTPS_DEBUG_MESSAGE("Use direct IPv4 connection.\r\n");
    HTTPS_DEBUG_PRINT("Using IPv4 address %d.%d.%d.%d for host '%s'.\r\n",
                      data->ip_address.v4Add.v[0],
                      data->ip_address.v4Add.v[1],
                      data->ip_address.v4Add.v[2],
                      data->ip_address.v4Add.v[3],
                      data->host);
    // TODO(sergey): Shall we warn here that configuration was set to IPv6?
    data->ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV4;
    data->state = APP_HTTPS_CLIENT_STATE_START_CONNECTION;
  } else if (TCPIP_Helper_StringToIPv6Address(data->host,
                                              &data->ip_address.v6Add)) {
    HTTPS_DEBUG_MESSAGE("Use direct IPv6 connection.\r\n");
    HTTPS_DEBUG_PRINT("Using IPv6 address %x:%x:%x:%x:%x:%x:%x:%x for host '%s'.\r\n",
                      data->ip_address.v6Add.w[0],
                      data->ip_address.v6Add.w[1],
                      data->ip_address.v6Add.w[2],
                      data->ip_address.v6Add.w[3],
                      data->ip_address.v6Add.w[4],
                      data->ip_address.v6Add.w[5],
                      data->ip_address.v6Add.w[6],
                      data->ip_address.v6Add.w[7],
                      data->host);
    // TODO(sergey): Shall we warn here that configuration was set to IPv4?
    data->ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV6;
    data->state = APP_HTTPS_CLIENT_STATE_START_CONNECTION;
  } else {
    // TODO(sergey): What is the better default value?
    // NOTE: This is more like a silence of stypid compiler issue, which
    // doesn't detect that with proepr switch() statement below result
    // can not be uninitialized.
    TCPIP_DNS_RESULT result = TCPIP_DNS_RES_OK;
    HTTPS_DEBUG_PRINT("Using DNS to Resolve '%s'.\r\n", data->host);
    switch (data->ip_mode_config) {
      case APP_HTTPS_CLIENT_IP_MODE_IPV6:
        HTTPS_DEBUG_MESSAGE("Resolving to IPv6.\r\n");
        result = TCPIP_DNS_Resolve(data->host, TCPIP_DNS_TYPE_AAAA);
        data->dns_query_ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV6;
        break;
      case APP_HTTPS_CLIENT_IP_MODE_IPV4:
        HTTPS_DEBUG_MESSAGE("Resolving to IPv4.\r\n");
        result = TCPIP_DNS_Resolve(data->host, TCPIP_DNS_TYPE_A);
        data->dns_query_ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV4;
        break;
    }
    SYS_ASSERT(result != TCPIP_DNS_RES_NAME_IS_IPADDRESS,
               "DNS Result is TCPIP_DNS_RES_NAME_IS_IPADDRESS, "
               "which should not happen since we already checked");
    if (result >= 0) {
        data->state = APP_HTTPS_CLIENT_STATE_WAIT_ON_DNS;
    } else {
      HTTPS_ERROR_PRINT("DNS Resolve returned %d, aborting.\r\n", result);
      enterErrorState(data);
    }
  }
}

static void checkDNSResolveStatus(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(
      data->host,
      &data->ip_address,
      (data->dns_query_ip_mode == APP_HTTPS_CLIENT_IP_MODE_IPV4)
          ? IP_ADDRESS_TYPE_IPV4
          : IP_ADDRESS_TYPE_IPV6);
  switch (result) {
    case TCPIP_DNS_RES_PENDING:
      // Still waiting for reply, nothing to do here. We just keep wait.
      break;
    case TCPIP_DNS_RES_OK:
      // Debug prints, for possible troubleshooting.
      //
      // TODO(sergey): How can we reduce number of lines here, seems similar to
      // logging we do in other places.
      switch (data->dns_query_ip_mode) {
        case APP_HTTPS_CLIENT_IP_MODE_IPV4:
          HTTPS_DEBUG_PRINT("DNS resolved host '%s' to "
                            "IPv4 address %d.%d.%d.%d.\r\n",
                            data->host,
                            data->ip_address.v4Add.v[0],
                            data->ip_address.v4Add.v[1],
                            data->ip_address.v4Add.v[2],
                            data->ip_address.v4Add.v[3]);
          data->ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV4;
          break;
        case APP_HTTPS_CLIENT_IP_MODE_IPV6:
          HTTPS_DEBUG_PRINT("DNS resolved host '%s' to IPv6 "
                            "address %x:%x:%x:%x:%x:%x:%x:%x.\r\n",
                            data->host,
                            data->ip_address.v6Add.w[0],
                            data->ip_address.v6Add.w[1],
                            data->ip_address.v6Add.w[2],
                            data->ip_address.v6Add.w[3],
                            data->ip_address.v6Add.w[4],
                            data->ip_address.v6Add.w[5],
                            data->ip_address.v6Add.w[6],
                            data->ip_address.v6Add.w[7]);
          data->ip_mode = APP_HTTPS_CLIENT_IP_MODE_IPV6;
          break;
      }
      // Schedule actual connection.
      data->state = APP_HTTPS_CLIENT_STATE_START_CONNECTION;
      break;
    default:
      if (data->dns_query_ip_mode == APP_HTTPS_CLIENT_IP_MODE_IPV6) {
        HTTPS_ERROR_PRINT("DNS IsResolved returned %d for IPv6, "
                          "trying IPv4.\r\n",
                          result);
        // TODO(sergey): Somehow de-duplicate with inital lookup query?
        result = TCPIP_DNS_Resolve(data->host, TCPIP_DNS_TYPE_A);
        if (result >= 0) {
            data->state = APP_HTTPS_CLIENT_STATE_WAIT_ON_DNS;
        } else {
          HTTPS_ERROR_PRINT("DNS Resolve returned %d, aborting.\r\n", result);
          enterErrorState(data);
        }
      } else {
        HTTPS_ERROR_PRINT("DNS IsResolved returned %d, aborting.\r\n", result);
        enterErrorState(data);
      }
      break;
  }
}

static void startNetworkConnection(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  // TODO(sergey): This assignment is only to silence stupid compiler bug.
  IP_ADDRESS_TYPE socket_type = IP_ADDRESS_TYPE_IPV4;
  // TODO(sergey): How can we reduce number of lines here, seems similar to
  // logging we do in other places.
  switch (data->ip_mode) {
    case APP_HTTPS_CLIENT_IP_MODE_IPV4:
      HTTPS_DEBUG_PRINT("Starting TCP/IPv4 connection to "
                        "%d.%d.%d.%d, port %d.\r\n",
                        data->ip_address.v4Add.v[0],
                        data->ip_address.v4Add.v[1],
                        data->ip_address.v4Add.v[2],
                        data->ip_address.v4Add.v[3],
                        data->port);
      socket_type = IP_ADDRESS_TYPE_IPV4;
      break;
    case APP_HTTPS_CLIENT_IP_MODE_IPV6:
      HTTPS_DEBUG_PRINT("Starting TCP/IPv6 connection to "
                        "%x:%x:%x:%x:%x:%x:%x:%x, port %d.\r\n",
                        data->ip_address.v6Add.w[0],
                        data->ip_address.v6Add.w[1],
                        data->ip_address.v6Add.w[2],
                        data->ip_address.v6Add.w[3],
                        data->ip_address.v6Add.w[4],
                        data->ip_address.v6Add.w[5],
                        data->ip_address.v6Add.w[6],
                        data->ip_address.v6Add.w[7],
                        data->port);
      socket_type = IP_ADDRESS_TYPE_IPV6;
      break;
  }
  // Create socket.
  data->socket = NET_PRES_SocketOpen(
      0,
      NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT,
      socket_type,
      data->port,
      (NET_PRES_ADDRESS*)&data->ip_address,
      NULL);
  NET_PRES_SocketWasReset(data->socket);
  if (data->socket == INVALID_SOCKET) {
    HTTPS_ERROR_MESSAGE("Could not create socket - aborting.\r\n");
    enterErrorState(data);
    return;
  }
  data->state = APP_HTTPS_CLIENT_STATE_WAIT_FOR_CONNECTION;
}

static void waitNetworkConnection(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (!NET_PRES_SocketIsConnected(data->socket)) {
    // TODO(sergey): Check for timeout?
    return;
  }
  if (STREQ_LEN(data->request_url, "https://", 8)) {
    HTTPS_DEBUG_MESSAGE("Connection opened, starting SSL negotiation.\r\n");
    if (!NET_PRES_SocketEncryptSocket(data->socket)) {
      SYS_CONSOLE_MESSAGE("SSL negotiation failed, aborting\r\n");
      data->state = APP_HTTPS_CLIENT_STATE_CLOSE_CONNECTION;
    } else {
      data->state = APP_HTTPS_CLIENT_STATE_WAIT_FOR_SSL_CONNECT;
    }
  } else {
    HTTPS_DEBUG_MESSAGE("Connection opened, "
                        "starting clear text communication.\r\n");
    data->state = APP_HTTPS_CLIENT_STATE_SEND_REQUEST;
  }
}

static void closeNetworkConnection(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  NET_PRES_SocketClose(data->socket);
  HTTPS_DEBUG_MESSAGE("Network connection closed.\r\n");
  data->state = APP_HTTPS_CLIENT_STATE_IDLE;
}

static void waitForSSLConnect(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (NET_PRES_SocketIsNegotiatingEncryption(data->socket)) {
    // TODO(sergey): Check on timeout?
    return;
  }
  if (!NET_PRES_SocketIsSecure(data->socket)) {
    HTTPS_ERROR_MESSAGE("SSL connection negotiation failed, aborting.\r\n");
    NET_PRES_SocketClose(data->socket);
    enterErrorState(data);
    return;
  }
  HTTPS_DEBUG_MESSAGE("SSL connection opened, "
                      "starting clear text communication.\r\n");
  data->state = APP_HTTPS_CLIENT_STATE_SEND_REQUEST;
}

static void sendRequest(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (NET_PRES_SocketWriteIsReady(data->socket,
                                  sizeof(data->network_buffer),
                                  sizeof(data->network_buffer)) == 0) {
    return;
  }
  // TODO(sergey): Ensure null terminator?
  const uint16_t len = safe_snprintf(data->network_buffer,
                                     sizeof(data->network_buffer),
                                     "GET %s HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Connection: close\r\n\r\n",
                                     data->path, data->host);
  uint16_t num_bytes_written = NET_PRES_SocketWrite(
      data->socket,
      data->network_buffer,
      strlen(data->network_buffer));
  if (num_bytes_written != len) {
    HTTPS_ERROR_PRINT("Error sending request: %d of %d bytes written.\r\n",
                      num_bytes_written, len);
    enterErrorState(data);
    return;
  }
  data->state = APP_HTTPS_CLIENT_STATE_WAIT_FOR_RESPONSE;
}

static void waitForResponse(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (NET_PRES_SocketReadIsReady(data->socket) == 0) {
    if (NET_PRES_SocketWasReset(data->socket)) {
      // TODO(sergey): Check whether connection was aborted?
      if (data->callbacks.request_handled != NULL) {
        data->callbacks.request_handled(data->callbacks.user_data);
      }
      data->state = APP_HTTPS_CLIENT_STATE_CLOSE_CONNECTION;
    }
    return;
  }
  uint16_t num_bytes_read = NET_PRES_SocketRead(data->socket,
                                                data->network_buffer,
                                                sizeof(data->network_buffer));
  if (data->callbacks.buffer_received != NULL) {
    data->callbacks.buffer_received((const uint8_t*)data->network_buffer,
                                    num_bytes_read,
                                    data->callbacks.user_data);
  }
}

static void handleError(AppHTTPSClientData* app_https_client_data) {
  AppHTTPSClientData* data = app_https_client_data;
  if (data->callbacks.error != NULL) {
    data->callbacks.error(data->callbacks.user_data);
  }
  // We go back to an idle state to wait for further commands.
  app_https_client_data->state = APP_HTTPS_CLIENT_STATE_IDLE;
}

////////////////////////////////////////////////////////////////////////////////
// Public API.

void APP_HTTPS_Client_Initialize(AppHTTPSClientData* app_https_client_data) {
#ifdef WITH_WOLFSSL_DEBUG
  wolfSSL_SetLoggingCb(wolfssl_logging_cb);
  wolfSSL_Debugging_ON();
#endif
  app_https_client_data->state = APP_HTTPS_CLIENT_STATE_IDLE;
  app_https_client_data->ip_mode_config = APP_HTTPS_CLIENT_IP_MODE_IPV4;
}

void APP_HTTPS_Client_Tasks(AppHTTPSClientData* app_https_client_data) {
  switch (app_https_client_data->state) {
    case APP_HTTPS_CLIENT_STATE_IDLE:
      // Nothing to do.
      break;
    case APP_HTTPS_CLIENT_STATE_BEGIN_SEQUENCE:
      HTTPS_DEBUG_PRINT("Begin HTTPS client sequence for URL %s.\r\n",
                        app_https_client_data->request_url);
      app_https_client_data->state = APP_HTTPS_CLIENT_STATE_WAIT_FOR_NETWORK;
      app_https_client_data->timeout =
          SYS_TMR_SystemCountGet() + SYS_TMR_SystemCountFrequencyGet();
      break;
    case APP_HTTPS_CLIENT_STATE_WAIT_FOR_NETWORK:
      waitForNetworkAvailable(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_PARSE_REQUEST_URL:
      pasreRequestURL(app_https_client_data); 
      break;
    case APP_HTTPS_CLIENT_STATE_PROCESS_REQUEST:
      processRequest(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_WAIT_ON_DNS:
      checkDNSResolveStatus(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_START_CONNECTION:
      startNetworkConnection(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_WAIT_FOR_CONNECTION:
      waitNetworkConnection(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_CLOSE_CONNECTION:
      closeNetworkConnection(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_WAIT_FOR_SSL_CONNECT:
      waitForSSLConnect(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_SEND_REQUEST:
      sendRequest(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_WAIT_FOR_RESPONSE:
      waitForResponse(app_https_client_data);
      break;
    case APP_HTTPS_CLIENT_STATE_ERROR:
      handleError(app_https_client_data);
      break;
  }
}

bool APP_HTTPS_Client_IsBusy(AppHTTPSClientData* app_https_client_data) {
  return (app_https_client_data->state != APP_HTTPS_CLIENT_STATE_IDLE);
}

bool APP_HTTPS_Client_Request(AppHTTPSClientData* app_https_client_data,
                              const char url[MAX_URL],
                              const AppHttpsClientCallbacks* callbacks) {
  // Check we are ready for the next request.
  if (APP_HTTPS_Client_IsBusy(app_https_client_data)) {
    return false;
  }
  // Copy settings, so caller can free the data.
  safe_strncpy(app_https_client_data->request_url,
               url,
               sizeof(app_https_client_data->request_url));
  app_https_client_data->callbacks = *callbacks;
  // Enter the request routines.
  app_https_client_data->state = APP_HTTPS_CLIENT_STATE_BEGIN_SEQUENCE;
  return true;
}

void APP_HTTPS_Client_SetWolfSSLDebug(bool enabled)
{
  g_wolfssl_debug = enabled;
}
