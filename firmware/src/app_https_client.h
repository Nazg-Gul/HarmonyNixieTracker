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

#ifndef _APP_HTTPS_CLIENT_H
#define _APP_HTTPS_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include "system_definitions.h"

#define MAX_URL         128
#define MAX_URL_SCHEME  6
#define MAX_URL_HOST    32
#define MAX_URL_PATH    64

#define HTTPS_CLIENT_NETWORK_BUFFER_SIZE 256

typedef enum {
  // HTTPS client has nothing to do.
  APP_HTTPS_CLIENT_STATE_IDLE = 0,
  // Begin fetching session. Initializes all the initial ime-outs and other
  // data which is to be re-set for the request.
  APP_HTTPS_CLIENT_STATE_BEGIN_SEQUENCE,
  // HTTPS client is waiting for network connection to be established.
  APP_HTTPS_CLIENT_STATE_WAIT_FOR_NETWORK,
  // Parse URL into its components.
  APP_HTTPS_CLIENT_STATE_PARSE_REQUEST_URL,
  // Process request by checking whether DNS lookup is needed (and scheduling it
  // when needed), whether it's IPv4 or IPv6 request.
  APP_HTTPS_CLIENT_STATE_PROCESS_REQUEST,
  // Wait reply from DNS server for the IP address for host name.
  APP_HTTPS_CLIENT_STATE_WAIT_ON_DNS,
  // Start connection to decoded IPv4 or IPv6 socket.
  APP_HTTPS_CLIENT_STATE_START_CONNECTION,
  // Wait for network connection to be fully established.
  APP_HTTPS_CLIENT_STATE_WAIT_FOR_CONNECTION,
  // Close netowrk connection and socket.
  APP_HTTPS_CLIENT_STATE_CLOSE_CONNECTION,
  // Wait SSL negotiation to be finished.
  APP_HTTPS_CLIENT_STATE_WAIT_FOR_SSL_CONNECT,
  // Send page request to the server.
  APP_HTTPS_CLIENT_STATE_SEND_REQUEST,
  // Waiting for response from server.
  APP_HTTPS_CLIENT_STATE_WAIT_FOR_RESPONSE,
} AppHTTPSClientState;

typedef enum {
  APP_HTTPS_CLIENT_IP_MODE_IPV4,
  APP_HTTPS_CLIENT_IP_MODE_IPV6,
} AppHTTPSClientIPMode;

typedef struct AppHTTPSClientData {
  // Configured IP mode.
  //
  // If it's set to IPv4 then all DNS lookups and network connections
  // will happen in IPv4 mode only. Otherwise DNS lookup will prefer
  // to use IPv6 address, but will fall back to IPv4 if there's no
  // IPv6 address for the host.
  AppHTTPSClientIPMode ip_mode_config;

  // Current mache state.
  AppHTTPSClientState state;

  // ======== Input patameters ========

  // This is an URL which user requested us to fetch.
  char request_url[MAX_URL];

  // ======== Fields shared across multiple tasks ========

  // Timeout for the current state to be finished.
  // Used or:
  //   - Timeout waiting for network configuration to become active.
  uint64_t timeout;

  // ======== Task or step specific fields ========

  // Parts of URL to be used to query the server.
  //
  // Those becomes available after PARSE_REQUEST_URL stepis finished.
  char scheme[MAX_URL_SCHEME];
  char host[MAX_URL_HOST];
  uint16_t port;
  char path[MAX_URL_PATH];

  // Current DNS query mode.
  //
  // The idea here is to first lookup IPv6 address (if ip_mode_config allows
  // that), if that fails then we'll try again with IPv4 address.
  //
  // Only used by WAIT_ON_DNS, initially configured by PROCESS_REQUEST.
  // Only used if host requires a DNS lookup.
  AppHTTPSClientIPMode dns_query_ip_mode;

  // Network connection configuration.
  // Ip address and mode used for connection.
  //
  // If host is an IP address, then configured by PROCESS_REQUEST, otherwise
  // is configured by WAIT_ON_DNS when DNS lookup is over.
  //
  // Used by START_CONNECTION to know which address/mode to use for socket.
  AppHTTPSClientIPMode ip_mode;
  IP_MULTI_ADDRESS ip_address;

  // Actual netowrk socket.
  //
  // Initialized by START_CONNECTION.
  NET_PRES_SKT_HANDLE_T socket;

  // Buffer used for network communication.
  char network_buffer[HTTPS_CLIENT_NETWORK_BUFFER_SIZE];
} AppHTTPSClientData;

// Initialize HTTPS client related application routines.
void APP_HTTPS_Client_Initialize(AppHTTPSClientData* app_https_client_data);

// Perform all HTTPS client related tasks.
void APP_HTTPS_Client_Tasks(AppHTTPSClientData* app_https_client_data);

// Check whether HTTPS client is busy with any tasks.
bool APP_HTTPS_Client_IsBusy(AppHTTPSClientData* app_https_client_data);

#endif  // _APP_HTTPS_CLIENT_H
