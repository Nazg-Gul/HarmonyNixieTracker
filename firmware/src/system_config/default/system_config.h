/*******************************************************************************
  MPLAB Harmony System Configuration Header

  File Name:
    system_config.h

  Summary:
    Build-time configuration header for the system defined by this MPLAB Harmony
    project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options that are not instantiated
    until used by another MPLAB Harmony module or application.

    Created with MPLAB Harmony Version 2.02
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _SYSTEM_CONFIG_H
#define _SYSTEM_CONFIG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Common System Service Configuration Options
*/
#define SYS_VERSION_STR           "2.02"
#define SYS_VERSION               20200

// *****************************************************************************
/* Clock System Service Configuration Options
*/
#define SYS_CLK_FREQ                        80000000ul
#define SYS_CLK_BUS_PERIPHERAL_1            40000000ul
#define SYS_CLK_UPLL_BEFORE_DIV2_FREQ       48000000ul
#define SYS_CLK_CONFIG_PRIMARY_XTAL         8000000ul
#define SYS_CLK_CONFIG_SECONDARY_XTAL       32768ul
#define SYS_CLK_CONFIG_FREQ_ERROR_LIMIT     10
#define SYS_CLK_WAIT_FOR_SWITCH             true
#define SYS_CLK_ON_WAIT                     OSC_ON_WAIT_IDLE 
 
/*** Ports System Service Configuration ***/
#define SYS_PORT_AD1PCFG        ~0xffc3
#define SYS_PORT_CNPUE          0x0
#define SYS_PORT_CNEN           0x0
#define SYS_PORT_A_TRIS         0xFFBF
#define SYS_PORT_A_LAT          0x0000
#define SYS_PORT_A_ODC          0x0000

#define SYS_PORT_B_TRIS         0xFFC3
#define SYS_PORT_B_LAT          0x003C
#define SYS_PORT_B_ODC          0x0000

#define SYS_PORT_C_TRIS         0x9FFF
#define SYS_PORT_C_LAT          0x6000
#define SYS_PORT_C_ODC          0x0000

#define SYS_PORT_D_TRIS         0xFFFF
#define SYS_PORT_D_LAT          0x0000
#define SYS_PORT_D_ODC          0x0000

#define SYS_PORT_E_TRIS         0xFFE0
#define SYS_PORT_E_LAT          0x0000
#define SYS_PORT_E_ODC          0x0000

#define SYS_PORT_F_TRIS         0xFFFF
#define SYS_PORT_F_LAT          0x0000
#define SYS_PORT_F_ODC          0x0000

#define SYS_PORT_G_TRIS         0xFFFF
#define SYS_PORT_G_LAT          0x0000
#define SYS_PORT_G_ODC          0x0000


/*** Command Processor System Service Configuration ***/
#define SYS_CMD_ENABLE
#define SYS_CMD_DEVICE_MAX_INSTANCES    SYS_CONSOLE_DEVICE_MAX_INSTANCES
#define SYS_CMD_PRINT_BUFFER_SIZE       1024
#define SYS_CMD_BUFFER_DMA_READY
#define SYS_CMD_REMAP_SYS_CONSOLE_MESSAGE
#define SYS_CMD_REMAP_SYS_DEBUG_MESSAGE

/*** Console System Service Configuration ***/

#define SYS_CONSOLE_OVERRIDE_STDIO
#define SYS_CONSOLE_DEVICE_MAX_INSTANCES        2
#define SYS_CONSOLE_INSTANCES_NUMBER            1
#define SYS_CONSOLE_UART_IDX               DRV_USART_INDEX_0
#define SYS_CONSOLE_UART_BAUD_RATE_IDX     DRV_USART_BAUD_RATE_IDX0
#define SYS_CONSOLE_UART_RD_QUEUE_DEPTH    1
#define SYS_CONSOLE_UART_WR_QUEUE_DEPTH    128
#define SYS_CONSOLE_BUFFER_DMA_READY



/*** Debug System Service Configuration ***/
#define SYS_DEBUG_ENABLE
#define DEBUG_PRINT_BUFFER_SIZE       512
#define SYS_DEBUG_BUFFER_DMA_READY
#define SYS_DEBUG_USE_CONSOLE

/*** File System Service Configuration ***/

#define SYS_FS_MEDIA_NUMBER         	1

#define SYS_FS_VOLUME_NUMBER		1

#define SYS_FS_AUTOMOUNT_ENABLE		false
#define SYS_FS_MAX_FILES	    	1
#define SYS_FS_MAX_FILE_SYSTEM_TYPE 	1
#define SYS_FS_MEDIA_MAX_BLOCK_SIZE  	512
#define SYS_FS_MEDIA_MANAGER_BUFFER_SIZE 512


#define SYS_FS_MEDIA_TYPE_IDX0 				
#define SYS_FS_TYPE_IDX0 					









/*** Interrupt System Service Configuration ***/
#define SYS_INT                     true
// *****************************************************************************
/* Random System Service Configuration Options
*/

#define SYS_RANDOM_CRYPTO_SEED_SIZE  55

/*** Timer System Service Configuration ***/
#define SYS_TMR_POWER_STATE             SYS_MODULE_POWER_RUN_FULL
#define SYS_TMR_DRIVER_INDEX            DRV_TMR_INDEX_0
#define SYS_TMR_MAX_CLIENT_OBJECTS      5
#define SYS_TMR_FREQUENCY               1000
#define SYS_TMR_FREQUENCY_TOLERANCE     10
#define SYS_TMR_UNIT_RESOLUTION         10000
#define SYS_TMR_CLIENT_TOLERANCE        10
#define SYS_TMR_INTERRUPT_NOTIFICATION  true

// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************

/*** MIIM Driver Configuration ***/
#define DRV_MIIM_ETH_MODULE_ID              ETH_ID_0
#define DRV_MIIM_INSTANCES_NUMBER           1
#define DRV_MIIM_INSTANCE_OPERATIONS        4
#define DRV_MIIM_INSTANCE_CLIENTS           2
#define DRV_MIIM_CLIENT_OP_PROTECTION   false
#define DRV_MIIM_COMMANDS   true
#define DRV_MIIM_DRIVER_OBJECT              DRV_MIIM_OBJECT_BASE_Default
#define DRV_MIIM_DRIVER_INDEX               DRV_MIIM_INDEX_0              
// *****************************************************************************
/* I2C Driver Configuration Options
*/
#define DRV_I2C_INTERRUPT_MODE                    		true
#define DRV_I2C_CLIENTS_NUMBER                    		1
#define DRV_I2C_INSTANCES_NUMBER                  		1

#define DRV_I2C_PERIPHERAL_ID_IDX0                		I2C_ID_2
#define DRV_I2C_OPERATION_MODE_IDX0               		DRV_I2C_MODE_MASTER
#define DRV_SCL_PORT_IDX0                               PORT_CHANNEL_A
#define DRV_SCL_PIN_POSITION_IDX0                       PORTS_BIT_POS_2
#define DRV_SDA_PORT_IDX0                               PORT_CHANNEL_A
#define DRV_SDA_PIN_POSITION_IDX0                       PORTS_BIT_POS_3
#define DRV_I2C_BIT_BANG_IDX0                           false
#define DRV_I2C_STOP_IN_IDLE_IDX0                       false
#define DRV_I2C_SMBus_SPECIFICATION_IDX0			    false
#define DRV_I2C_BAUD_RATE_IDX0                    		50000
#define DRV_I2C_BRG_CLOCK_IDX0	                  		40000000
#define DRV_I2C_SLEW_RATE_CONTROL_IDX0      			false
#define DRV_I2C_MASTER_INT_SRC_IDX0               		INT_SOURCE_I2C_2_MASTER
#define DRV_I2C_SLAVE_INT_SRC_IDX0                		
#define DRV_I2C_ERR_MX_INT_SRC_IDX0               		INT_SOURCE_I2C_2_ERROR
#define DRV_I2C_INT_VECTOR_IDX0                         INT_VECTOR_I2C2
#define DRV_I2C_ISR_VECTOR_IDX0                         _I2C_2_VECTOR
#define DRV_I2C_INT_PRIORITY_IDX0                 		INT_PRIORITY_LEVEL1
#define DRV_I2C_INT_SUB_PRIORITY_IDX0             		INT_SUBPRIORITY_LEVEL0
#define DRV_I2C_POWER_STATE_IDX0                  		SYS_MODULE_POWER_RUN_FULL
#define DRV_I2C_INTERRUPT_MODE                    		true


/*** SPI Driver Configuration ***/
#define DRV_SPI_NUMBER_OF_MODULES		4
/*** Driver Compilation and static configuration options. ***/
/*** Select SPI compilation units.***/
#define DRV_SPI_POLLED 				1
#define DRV_SPI_ISR 				1
#define DRV_SPI_MASTER 				1
#define DRV_SPI_SLAVE 				0
#define DRV_SPI_RM 					0
#define DRV_SPI_EBM 				1
#define DRV_SPI_8BIT 				1
#define DRV_SPI_16BIT 				0
#define DRV_SPI_32BIT 				0
#define DRV_SPI_DMA 				0

/*** SPI Driver Static Allocation Options ***/
#define DRV_SPI_INSTANCES_NUMBER 		2
#define DRV_SPI_CLIENTS_NUMBER 			2
#define DRV_SPI_ELEMENTS_PER_QUEUE 		10
/* SPI Driver Instance 0 Configuration */
#define DRV_SPI_SPI_ID_IDX0 				SPI_ID_4
#define DRV_SPI_TASK_MODE_IDX0 				DRV_SPI_TASK_MODE_POLLED
#define DRV_SPI_SPI_MODE_IDX0				DRV_SPI_MODE_MASTER
#define DRV_SPI_ALLOW_IDLE_RUN_IDX0			false
#define DRV_SPI_SPI_PROTOCOL_TYPE_IDX0 		DRV_SPI_PROTOCOL_TYPE_STANDARD
#define DRV_SPI_COMM_WIDTH_IDX0 			SPI_COMMUNICATION_WIDTH_8BITS
#define DRV_SPI_SPI_CLOCK_IDX0 				CLK_BUS_PERIPHERAL_1
#define DRV_SPI_BAUD_RATE_IDX0 				8000000
#define DRV_SPI_BUFFER_TYPE_IDX0 			DRV_SPI_BUFFER_TYPE_ENHANCED
#define DRV_SPI_CLOCK_MODE_IDX0 			DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_FALL
#define DRV_SPI_INPUT_PHASE_IDX0 			SPI_INPUT_SAMPLING_PHASE_AT_END
#define DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX0      0xFF

#define DRV_SPI_QUEUE_SIZE_IDX0 			10
#define DRV_SPI_RESERVED_JOB_IDX0 			1
/* SPI Driver Instance 1 Configuration */
#define DRV_SPI_SPI_ID_IDX1 				SPI_ID_1
#define DRV_SPI_TASK_MODE_IDX1 				DRV_SPI_TASK_MODE_ISR
#define DRV_SPI_SPI_MODE_IDX1				DRV_SPI_MODE_MASTER
#define DRV_SPI_ALLOW_IDLE_RUN_IDX1			false
#define DRV_SPI_SPI_PROTOCOL_TYPE_IDX1 		DRV_SPI_PROTOCOL_TYPE_STANDARD
#define DRV_SPI_COMM_WIDTH_IDX1 			SPI_COMMUNICATION_WIDTH_8BITS
#define DRV_SPI_SPI_CLOCK_IDX1 				CLK_BUS_PERIPHERAL_1
#define DRV_SPI_BAUD_RATE_IDX1 				10000000
#define DRV_SPI_BUFFER_TYPE_IDX1 			DRV_SPI_BUFFER_TYPE_ENHANCED
#define DRV_SPI_CLOCK_MODE_IDX1 			DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL
#define DRV_SPI_INPUT_PHASE_IDX1 			SPI_INPUT_SAMPLING_PHASE_AT_END
#define DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX1      0xFF

#define DRV_SPI_TX_INT_SOURCE_IDX1 			INT_SOURCE_SPI_1_TRANSMIT
#define DRV_SPI_RX_INT_SOURCE_IDX1 			INT_SOURCE_SPI_1_RECEIVE
#define DRV_SPI_ERROR_INT_SOURCE_IDX1 		INT_SOURCE_SPI_1_ERROR
#define DRV_SPI_INT_VECTOR_IDX1				INT_VECTOR_SPI1
#define DRV_SPI_INT_PRIORITY_IDX1			INT_PRIORITY_LEVEL1
#define DRV_SPI_INT_SUB_PRIORITY_IDX1		INT_SUBPRIORITY_LEVEL0        
#define DRV_SPI_QUEUE_SIZE_IDX1 			10
#define DRV_SPI_RESERVED_JOB_IDX1 			1
// *****************************************************************************
/* SST25 Driver Configuration Options
*/
#define DRV_SST25_BUFFER_OBJ_NUMBER                       5
#define DRV_SST25_CLIENTS_NUMBER                          2
#define DRV_SST25_INSTANCES_NUMBER                        1

#define DRV_SST25_POWER_STATE_IDX0                        SYS_MODULE_POWER_RUN_FULL
#define DRV_SST25_SPI_DRIVER_INSTANCE_IDX0                1
#define DRV_SST25_HOLD_PIN_PORT_CHANNEL_IDX0              PORT_CHANNEL_C
#define DRV_SST25_HOLD_PIN_PORT_BIT_POS_IDX0              PORTS_BIT_POS_13
#define DRV_SST25_WRITE_PROTECT_PIN_PORT_CHANNEL_IDX0     PORT_CHANNEL_C
#define DRV_SST25_WRITE_PROTECT_PIN_BIT_POS_IDX0          PORTS_BIT_POS_14
#define DRV_SST25_CHIP_SELECT_PORT_CHANNEL_IDX0           PORT_CHANNEL_D
#define DRV_SST25_CHIP_SELECT_PORT_BIT_POS_IDX0           PORTS_BIT_POS_1




/*** Timer Driver Configuration ***/
#define DRV_TMR_INTERRUPT_MODE             true
#define DRV_TMR_INSTANCES_NUMBER           1
#define DRV_TMR_CLIENTS_NUMBER             1

/*** Timer Driver 0 Configuration ***/
#define DRV_TMR_PERIPHERAL_ID_IDX0          TMR_ID_2
#define DRV_TMR_INTERRUPT_SOURCE_IDX0       INT_SOURCE_TIMER_2
#define DRV_TMR_INTERRUPT_VECTOR_IDX0       INT_VECTOR_T2
#define DRV_TMR_ISR_VECTOR_IDX0             _TIMER_2_VECTOR
#define DRV_TMR_INTERRUPT_PRIORITY_IDX0     INT_PRIORITY_LEVEL4
#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0 INT_SUBPRIORITY_LEVEL0
#define DRV_TMR_CLOCK_SOURCE_IDX0           DRV_TMR_CLKSOURCE_INTERNAL
#define DRV_TMR_PRESCALE_IDX0               TMR_PRESCALE_VALUE_256
#define DRV_TMR_OPERATION_MODE_IDX0         DRV_TMR_OPERATION_MODE_16_BIT
#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX0     false
#define DRV_TMR_POWER_STATE_IDX0            SYS_MODULE_POWER_RUN_FULL

 // *****************************************************************************
/* USART Driver Configuration Options
*/
#define DRV_USART_INTERRUPT_MODE                    true

#define DRV_USART_BYTE_MODEL_SUPPORT                false

#define DRV_USART_READ_WRITE_MODEL_SUPPORT          true

#define DRV_USART_BUFFER_QUEUE_SUPPORT              true

#define DRV_USART_CLIENTS_NUMBER                    1
#define DRV_USART_INSTANCES_NUMBER                  1

#define DRV_USART_PERIPHERAL_ID_IDX0                USART_ID_1
#define DRV_USART_OPER_MODE_IDX0                    DRV_USART_OPERATION_MODE_NORMAL
#define DRV_USART_OPER_MODE_DATA_IDX0               
#define DRV_USART_INIT_FLAG_WAKE_ON_START_IDX0      false
#define DRV_USART_INIT_FLAG_AUTO_BAUD_IDX0          false
#define DRV_USART_INIT_FLAG_STOP_IN_IDLE_IDX0       false
#define DRV_USART_INIT_FLAGS_IDX0                   0
#define DRV_USART_BRG_CLOCK_IDX0                    40000000
#define DRV_USART_BAUD_RATE_IDX0                    115200
#define DRV_USART_LINE_CNTRL_IDX0                   DRV_USART_LINE_CONTROL_8NONE1
#define DRV_USART_HANDSHAKE_MODE_IDX0               DRV_USART_HANDSHAKE_NONE
#define DRV_USART_XMIT_INT_SRC_IDX0                 INT_SOURCE_USART_1_TRANSMIT
#define DRV_USART_RCV_INT_SRC_IDX0                  INT_SOURCE_USART_1_RECEIVE
#define DRV_USART_ERR_INT_SRC_IDX0                  INT_SOURCE_USART_1_ERROR
#define DRV_USART_INT_VECTOR_IDX0                   INT_VECTOR_UART1
#define DRV_USART_INT_PRIORITY_IDX0                 INT_PRIORITY_LEVEL1
#define DRV_USART_INT_SUB_PRIORITY_IDX0             INT_SUBPRIORITY_LEVEL0

#define DRV_USART_XMIT_QUEUE_SIZE_IDX0              10
#define DRV_USART_RCV_QUEUE_SIZE_IDX0               10


#define DRV_USART_POWER_STATE_IDX0                  SYS_MODULE_POWER_RUN_FULL

#define DRV_USART_QUEUE_DEPTH_COMBINED              20
/*** Wi-Fi Driver Configuration ***/
#define MRF_INT_SOURCE INT_SOURCE_EXTERNAL_4
#define MRF_INT_VECTOR INT_VECTOR_INT4

#define DRV_WIFI_SPI_INDEX 0
#define DRV_WIFI_SPI_INSTANCE sysObj.spiObjectIdx0

// I/O mappings for general control pins, including CS, HIBERNATE, INTERRUPT and RESET.
#define WF_CS_PORT_CHANNEL          PORT_CHANNEL_F
#define WF_CS_BIT_POS               12

#define WF_HIBERNATE_PORT_CHANNEL   PORT_CHANNEL_G
#define WF_HIBERNATE_BIT_POS        1

#define WF_INT_PORT_CHANNEL         PORT_CHANNEL_A
#define WF_INT_BIT_POS              15

#define WF_RESET_PORT_CHANNEL       PORT_CHANNEL_G
#define WF_RESET_BIT_POS            0

#define DRV_WIFI_DEFAULT_NETWORK_TYPE       DRV_WIFI_NETWORK_TYPE_INFRASTRUCTURE
#define DRV_WIFI_DEFAULT_CHANNEL_LIST       {} /* Channel list in domain - use default in module */
#define DRV_WIFI_DEFAULT_LIST_RETRY_COUNT   (DRV_WIFI_RETRY_FOREVER) /* Number (1..255) of times to try to connect to AP when using Infrastructure network type */
#define DRV_WIFI_DEFAULT_SSID               "MySSID"

#define DRV_WIFI_DEFAULT_SECURITY_MODE      DRV_WIFI_SECURITY_WPA2_WITH_PASS_PHRASE
#define DRV_WIFI_DEFAULT_WEP_PHRASE         "WEP Phrase" // default WEP passphrase
#define DRV_WIFI_DEFAULT_WEP_KEY_40         "5AFB6C8E77" // default WEP40 key
#define DRV_WIFI_DEFAULT_WEP_KEY_104        "90E96780C739409DA50034FCAA" // default WEP104 key
#define DRV_WIFI_DEFAULT_PSK_PHRASE         "MyPSKPhrase" // customized WPA passphrase
#define DRV_WIFI_DEFAULT_WPS_PIN            "12390212" // default WPS PIN

#define DRV_WIFI_SAVE_WPS_CREDENTIALS       DRV_WIFI_DISABLED

#define DRV_WIFI_CHECK_LINK_STATUS          DRV_WIFI_DISABLED /* Gets the MRF to check the link status relying on Tx failures. */
#define DRV_WIFI_LINK_LOST_THRESHOLD        40 /* Consecutive Tx transmission failures to be considered the AP is gone away. */

#define DRV_WIFI_DEFAULT_POWER_SAVE         DRV_WIFI_DISABLED /* DRV_WIFI_ENABLED or DRV_WIFI_DISABLED */

/*
 * MRF24W FW has a built-in connection manager, and it is enabled by default.
 * If you want to run your own connection manager in host side, you should
 * disable the FW connection manager to avoid possible conflict between the two.
 * Especially these two APIs can be affected if you do not disable it.
 * A) uint16_t DRV_WIFI_Disconnect(void)
 * B) uint16_t DRV_WIFI_Scan(bool scanAll)
 * These APIs will return failure when the conflict occurs.
 */
#define DRV_WIFI_MODULE_CONNECTION_MANAGER  DRV_WIFI_ENABLED

#define DRV_WIFI_SOFTWARE_MULTICAST_FILTER  DRV_WIFI_ENABLED

#define DRV_WIFI_CONFIG_MHC

#define DRV_WIFI_ASSERT(condition, msg) DRV_WIFI_Assert(condition, msg, __FILE__, __LINE__)

// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
/* MPLAB Harmony Net Presentation Layer Definitions*/
#define NET_PRES_NUM_INSTANCE 1
#define NET_PRES_NUM_SOCKETS 10



// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Stack Configuration
// *****************************************************************************
// *****************************************************************************
#define TCPIP_STACK_USE_IPV4
#define TCPIP_STACK_USE_TCP
#define TCPIP_STACK_USE_UDP

#define TCPIP_STACK_TICK_RATE		        		5
#define TCPIP_STACK_SECURE_PORT_ENTRIES             10

/* TCP/IP stack event notification */
#define TCPIP_STACK_USE_EVENT_NOTIFICATION
#define TCPIP_STACK_USER_NOTIFICATION   false
#define TCPIP_STACK_DOWN_OPERATION   true
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   true
#define TCPIP_STACK_MAC_DOWN_OPERATION  true
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   true
/*** TCPIP Heap Configuration ***/
#define TCPIP_STACK_USE_INTERNAL_HEAP
#define TCPIP_STACK_DRAM_SIZE                       42000
#define TCPIP_STACK_DRAM_RUN_LIMIT                  2048
#define TCPIP_STACK_MALLOC_FUNC                     malloc

#define TCPIP_STACK_CALLOC_FUNC                     calloc

#define TCPIP_STACK_FREE_FUNC                       free


#define TCPIP_STACK_DRAM_DEBUG_ENABLE

#define TCPIP_STACK_HEAP_USE_FLAGS                   TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED

#define TCPIP_STACK_HEAP_USAGE_CONFIG                TCPIP_STACK_HEAP_USE_DEFAULT

#define TCPIP_STACK_SUPPORTED_HEAPS                  1

/*** ARP Configuration ***/
#define TCPIP_ARP_CACHE_ENTRIES                 		5
#define TCPIP_ARP_CACHE_DELETE_OLD		        	true
#define TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO			1200
#define TCPIP_ARP_CACHE_PENDING_ENTRY_TMO			60
#define TCPIP_ARP_CACHE_PENDING_RETRY_TMO			2
#define TCPIP_ARP_CACHE_PERMANENT_QUOTA		    		50
#define TCPIP_ARP_CACHE_PURGE_THRESHOLD		    		75
#define TCPIP_ARP_CACHE_PURGE_QUANTA		    		1
#define TCPIP_ARP_CACHE_ENTRY_RETRIES		    		3
#define TCPIP_ARP_GRATUITOUS_PROBE_COUNT			1
#define TCPIP_ARP_TASK_PROCESS_RATE		        	2

/*** DHCP Configuration ***/
#define TCPIP_STACK_USE_DHCP_CLIENT
#define TCPIP_DHCP_TIMEOUT		        		10
#define TCPIP_DHCP_TASK_TICK_RATE	    			200
#define TCPIP_DHCP_HOST_NAME_SIZE	    			20
#define TCPIP_DHCP_CLIENT_CONNECT_PORT  			68
#define TCPIP_DHCP_SERVER_LISTEN_PORT				67
#define TCPIP_DHCP_CLIENT_ENABLED             			true



/*** DNS Client Configuration ***/
#define TCPIP_STACK_USE_DNS
#define TCPIP_DNS_CLIENT_SERVER_TMO					60
#define TCPIP_DNS_CLIENT_TASK_PROCESS_RATE			200
#define TCPIP_DNS_CLIENT_CACHE_ENTRIES				5
#define TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO			0
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS		5
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS		1
#define TCPIP_DNS_CLIENT_ADDRESS_TYPE			    IP_ADDRESS_TYPE_IPV4
#define TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL		1200
#define TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO	10
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO			5
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN			32
#define TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES		4
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			true
#define TCPIP_DNS_CLIENT_USER_NOTIFICATION   false





/*** ICMPv4 Server Configuration ***/
#define TCPIP_STACK_USE_ICMP_SERVER

/*** ICMPv4 Client Configuration ***/
#define TCPIP_STACK_USE_ICMP_CLIENT
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION   true
#define TCPIP_ICMP_ECHO_REQUEST_TIMEOUT       500
#define TCPIP_ICMP_TASK_TICK_RATE             33
/*** iperf Configuration ***/
#define TCPIP_STACK_USE_IPERF
#define TCPIP_IPERF_TX_BUFFER_SIZE		4096
#define TCPIP_IPERF_RX_BUFFER_SIZE  	4096
#define TCPIP_IPERF_TX_WAIT_TMO     	400
#define TCPIP_IPERF_TX_QUEUE_LIMIT  	2
#define TCPIP_IPERF_TIMING_ERROR_MARGIN 0
#define TCPIP_IPERF_MAX_INSTANCES       1
#define TCPIP_IPERF_TX_BW_LIMIT  		100


/*** NBNS Configuration ***/
#define TCPIP_STACK_USE_NBNS
#define TCPIP_NBNS_TASK_TICK_RATE   110







/*** TCP Configuration ***/
#define TCPIP_TCP_MAX_SEG_SIZE_TX		        	1460
#define TCPIP_TCP_MAX_SEG_SIZE_RX_LOCAL		    		1460
#define TCPIP_TCP_MAX_SEG_SIZE_RX_NON_LOCAL			536
#define TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE			512
#define TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE			512
#define TCPIP_TCP_DYNAMIC_OPTIONS             			true
#define TCPIP_TCP_START_TIMEOUT_VAL		        	1000
#define TCPIP_TCP_DELAYED_ACK_TIMEOUT		    		100
#define TCPIP_TCP_FIN_WAIT_2_TIMEOUT		    		5000
#define TCPIP_TCP_KEEP_ALIVE_TIMEOUT		    		10000
#define TCPIP_TCP_CLOSE_WAIT_TIMEOUT		    		200
#define TCPIP_TCP_MAX_RETRIES		            		5
#define TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES			6
#define TCPIP_TCP_MAX_SYN_RETRIES		        	2
#define TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL			40
#define TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL			200
#define TCPIP_TCP_MAX_SOCKETS		                10
#define TCPIP_TCP_TASK_TICK_RATE		        	5
#define TCPIP_TCP_MSL_TIMEOUT		        	    0
#define TCPIP_TCP_QUIET_TIME		        	    0
#define TCPIP_TCP_COMMANDS   true

/*** announce Configuration ***/
#define TCPIP_STACK_USE_ANNOUNCE
#define TCPIP_ANNOUNCE_MAX_PAYLOAD 	512
#define TCPIP_ANNOUNCE_TASK_RATE    333

/*** TCPIP MAC Configuration ***/
#define TCPIP_EMAC_TX_DESCRIPTORS				8
#define TCPIP_EMAC_RX_DESCRIPTORS				10
#define TCPIP_EMAC_RX_DEDICATED_BUFFERS				4
#define TCPIP_EMAC_RX_INIT_BUFFERS				    0
#define TCPIP_EMAC_RX_LOW_THRESHOLD				    1
#define TCPIP_EMAC_RX_LOW_FILL				        2
#define TCPIP_EMAC_RX_BUFF_SIZE		    			1536
#define TCPIP_EMAC_RX_MAX_FRAME		    			1536
#define TCPIP_EMAC_RX_FILTERS                       \
                                                    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
                                                    0
#define TCPIP_EMAC_RX_FRAGMENTS		    			1
#define TCPIP_EMAC_ETH_OPEN_FLAGS       			\
                                                    TCPIP_ETH_OPEN_AUTO |\
                                                    TCPIP_ETH_OPEN_FDUPLEX |\
                                                    TCPIP_ETH_OPEN_HDUPLEX |\
                                                    TCPIP_ETH_OPEN_100 |\
                                                    TCPIP_ETH_OPEN_10 |\
                                                    TCPIP_ETH_OPEN_MDIX_AUTO |\
                                                    TCPIP_ETH_OPEN_RMII |\
                                                    0

#define TCPIP_EMAC_MODULE_ID		    			ETH_ID_0
#define TCPIP_EMAC_INTERRUPT_MODE        			true
#define DRV_ETHMAC_INSTANCES_NUMBER				1
#define DRV_ETHMAC_CLIENTS_NUMBER				1
#define DRV_ETHMAC_INDEX	    	    			1
#define DRV_ETHMAC_PERIPHERAL_ID				1
#define DRV_ETHMAC_INTERRUPT_VECTOR				INT_VECTOR_ETHERNET
#define DRV_ETHMAC_INTERRUPT_SOURCE				INT_SOURCE_ETH_1
#define DRV_ETHMAC_POWER_STATE		    			SYS_MODULE_POWER_RUN_FULL

#define DRV_ETHMAC_INTERRUPT_MODE        			true


#define TCPIP_EMAC_PHY_CONFIG_FLAGS     			\
                                                    DRV_ETHPHY_CFG_RMII | \
                                                    DRV_ETHPHY_CFG_AUTO | \
                                                    0                                                    

#define TCPIP_EMAC_PHY_LINK_INIT_DELAY  			500
#define TCPIP_EMAC_PHY_ADDRESS		    			0
#define DRV_ETHPHY_INSTANCES_NUMBER				1
#define DRV_ETHPHY_CLIENTS_NUMBER				1
#define DRV_ETHPHY_INDEX		        		1
#define DRV_ETHPHY_PERIPHERAL_ID				1
#define DRV_ETHPHY_NEG_INIT_TMO		    			500
#define DRV_ETHPHY_NEG_DONE_TMO		    			2000
#define DRV_ETHPHY_RESET_CLR_TMO				500
#define DRV_ETHPHY_USE_DRV_MIIM                     true


/*** telnet Configuration ***/
#define TCPIP_STACK_USE_TELNET_SERVER
#define TCPIP_TELNET_MAX_CONNECTIONS    2
#define TCPIP_TELNET_USERNAME           "admin"
#define TCPIP_TELNET_PASSWORD           "microchip"
#define TCPIP_TELNET_TASK_TICK_RATE     100


/*** UDP Configuration ***/
#define TCPIP_UDP_MAX_SOCKETS		                	10
#define TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE		    	512
#define TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT    	 	3
#define TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT			5
#define TCPIP_UDP_USE_POOL_BUFFERS   false
#define TCPIP_UDP_USE_TX_CHECKSUM             			true
#define TCPIP_UDP_USE_RX_CHECKSUM             			true
#define TCPIP_UDP_COMMANDS   true

#define TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
#define TCPIP_ZC_LL_PROBE_WAIT 1
#define TCPIP_ZC_LL_PROBE_MIN 1
#define TCPIP_ZC_LL_PROBE_MAX 2
#define TCPIP_ZC_LL_PROBE_NUM 3
#define TCPIP_ZC_LL_ANNOUNCE_WAIT 2
#define TCPIP_ZC_LL_ANNOUNCE_NUM 2
#define TCPIP_ZC_LL_ANNOUNCE_INTERVAL 2
#define TCPIP_ZC_LL_MAX_CONFLICTS 10
#define TCPIP_ZC_LL_RATE_LIMIT_INTERVAL 60
#define TCPIP_ZC_LL_DEFEND_INTERVAL 10
#define TCPIP_ZC_LL_IPV4_LLBASE 0xa9fe0100
#define TCPIP_ZC_LL_IPV4_LLBASE_MASK 0x0000FFFF
#define TCPIP_ZC_LL_TASK_TICK_RATE 333
#define TCPIP_STACK_USE_ZEROCONF_MDNS_SD
#define TCPIP_ZC_MDNS_TASK_TICK_RATE 63
#define TCPIP_ZC_MDNS_PORT 5353
#define TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE 32
#define TCPIP_ZC_MDNS_MAX_LABEL_SIZE 64
#define TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE 256
#define TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE 32
#define TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE 64
#define TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE 128
#define TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL 3600
#define TCPIP_ZC_MDNS_MAX_RR_NUM 4
#define TCPIP_ZC_MDNS_PROBE_WAIT 750
#define TCPIP_ZC_MDNS_PROBE_INTERVAL 250
#define TCPIP_ZC_MDNS_PROBE_NUM 3
#define TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM 30
#define TCPIP_ZC_MDNS_ANNOUNCE_NUM 3
#define TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL 250
#define TCPIP_ZC_MDNS_ANNOUNCE_WAIT 250

/*** Network Configuration Index 0 ***/
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME		"PIC32INT"
#define TCPIP_IF_PIC32INT
#define TCPIP_NETWORK_DEFAULT_HOST_NAME				"MCHPBOARD_E"
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR				0
#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS			"192.168.100.115"
#define TCPIP_NETWORK_DEFAULT_IP_MASK				"255.255.255.0"
#define TCPIP_NETWORK_DEFAULT_GATEWAY				"192.168.100.1"
#define TCPIP_NETWORK_DEFAULT_DNS					"192.168.100.1"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS			"0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE			"full"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS			\
													TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON |\
													TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON |\
													TCPIP_NETWORK_CONFIG_IP_STATIC
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER			DRV_ETHMAC_PIC32MACObject
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS			0
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH	0
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY			0
/*** Network Configuration Index 0 ***/
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX1		"MRF24W"
#define TCPIP_IF_MRF24W
#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX1			"MCHPBOARD_W"
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX1				0
#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX1			"0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX1				"255.255.255.0"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX1				"192.168.1.1"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX1					"192.168.1.1"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX1			"0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX1			"full"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX1			\
														TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON |\
														TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON |\
														TCPIP_NETWORK_CONFIG_IP_STATIC
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX1			DRV_MRF24W_MACObject
#define TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX1			0
#define TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX1	0
#define TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX1			0
/*** tcpip_cmd Configuration ***/
#define TCPIP_STACK_COMMAND_ENABLE
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS         4
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY    1000
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT          5000
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE             	true
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE    2000
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE    100


/*** IPv4 Configuration ***/

/*** USB Driver Configuration ***/


/* Enables Device Support */
#define DRV_USBFS_DEVICE_SUPPORT      true

/* Disable Device Support */
#define DRV_USBFS_HOST_SUPPORT      false

/* Maximum USB driver instances */
#define DRV_USBFS_INSTANCES_NUMBER    1


/* Interrupt mode enabled */
#define DRV_USBFS_INTERRUPT_MODE      true


/* Number of Endpoints used */
#define DRV_USBFS_ENDPOINTS_NUMBER    2




/*** USB Device Stack Configuration ***/










/* The USB Device Layer will not initialize the USB Driver */
#define USB_DEVICE_DRIVER_INITIALIZE_EXPLICIT

/* Maximum device layer instances */
#define USB_DEVICE_INSTANCES_NUMBER     1

/* EP0 size in bytes */
#define USB_DEVICE_EP0_BUFFER_SIZE      8









/* Maximum instances of HID function driver */
#define USB_DEVICE_HID_INSTANCES_NUMBER     1










/* HID Transfer Queue Size for both read and
   write. Applicable to all instances of the
   function driver */
#define USB_DEVICE_HID_QUEUE_DEPTH_COMBINED 2





// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************
/*** Application Defined Pins ***/

/*** Functions for STATUS_LED_1_ pin ***/
#define STATUS_LED_1_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define STATUS_LED_1_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define STATUS_LED_1_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define STATUS_LED_1_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define STATUS_LED_1_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5, Value)

/*** Functions for STATUS_LED_2_ pin ***/
#define STATUS_LED_2_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define STATUS_LED_2_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define STATUS_LED_2_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define STATUS_LED_2_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define STATUS_LED_2_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4, Value)

/*** Functions for STATUS_LED_3_ pin ***/
#define STATUS_LED_3_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define STATUS_LED_3_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define STATUS_LED_3_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define STATUS_LED_3_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define STATUS_LED_3_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3, Value)

/*** Functions for STATUS_LED_4_ pin ***/
#define STATUS_LED_4_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define STATUS_LED_4_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define STATUS_LED_4_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define STATUS_LED_4_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define STATUS_LED_4_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2, Value)

/*** Functions for FLASH_HOLD_ pin ***/
#define FLASH_HOLD_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_13)
#define FLASH_HOLD_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_13)
#define FLASH_HOLD_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_13)
#define FLASH_HOLD_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_13)
#define FLASH_HOLD_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_13, Value)

/*** Functions for FLASH_WP_ pin ***/
#define FLASH_WP_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14)
#define FLASH_WP_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14)
#define FLASH_WP_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14)
#define FLASH_WP_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14)
#define FLASH_WP_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14, Value)

/*** Functions for ETH_NRST pin ***/
#define ETH_NRSTToggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define ETH_NRSTOn() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define ETH_NRSTOff() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define ETH_NRSTStateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6)
#define ETH_NRSTStateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6, Value)

/*** Functions for SHIFT_DATA_ pin ***/
#define SHIFT_DATA_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)
#define SHIFT_DATA_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)
#define SHIFT_DATA_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)
#define SHIFT_DATA_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)
#define SHIFT_DATA_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0, Value)

/*** Functions for SHIFT_EN_ pin ***/
#define SHIFT_EN_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)
#define SHIFT_EN_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)
#define SHIFT_EN_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)
#define SHIFT_EN_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)
#define SHIFT_EN_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1, Value)

/*** Functions for SHIFT_RCK_ pin ***/
#define SHIFT_RCK_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define SHIFT_RCK_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define SHIFT_RCK_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define SHIFT_RCK_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define SHIFT_RCK_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2, Value)

/*** Functions for SHIFT_SRCK_ pin ***/
#define SHIFT_SRCK_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define SHIFT_SRCK_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define SHIFT_SRCK_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define SHIFT_SRCK_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define SHIFT_SRCK_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3, Value)

/*** Functions for HV_EN_ pin ***/
#define HV_EN_Toggle() PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)
#define HV_EN_On() PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)
#define HV_EN_Off() PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)
#define HV_EN_StateGet() PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)
#define HV_EN_StateSet(Value) PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4, Value)


/*** Application Instance 0 Configuration ***/

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _SYSTEM_CONFIG_H
/*******************************************************************************
 End of File
*/
