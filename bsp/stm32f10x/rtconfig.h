#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Project Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32

/* Tick per Second */
#define RT_TICK_PER_SECOND 1000

/* SECTION: RT_DEBUG */

#define RT_USING_OVERFLOW_CHECK

/* Using Hook */
#define RT_USING_HOOK

/* Thread Debug */
#define RT_DEBUG
#define RT_THREAD_DEBUG

/* Using Software Timer */
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512
#define RT_TIMER_TICK_PER_SECOND 1000

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE

/* Using Mutex */
#define RT_USING_MUTEX

/* Using Event */
#define RT_USING_EVENT

/* Using MailBox */
#define RT_USING_MAILBOX

/* Using Message Queue */
#define RT_USING_MESSAGEQUEUE

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
#define RT_USING_MEMPOOL
/* Using Small MM */
#define RT_USING_SMALL_MEM
/* Using Dynamic Heap Management */
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 512
#define RT_CONSOLE_DEVICE_NAME "uart5"
#define RT_VER_NUM 0x40001
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M3

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT

/* SECTION: finsh, a C-Express shell */
#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 4
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_0
#define RT_DFS_ELM_USE_LFN 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_RTC
#define RT_USING_ALARMS

/* SECTION: device filesystem */

/* Network */

/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_POSIX
/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_AT
#define SAL_USING_POSIX
#define SAL_PROTO_FAMILIES_NUM 4

/* AT commands */

#define RT_USING_AT
#define AT_USING_CLIENT
#define AT_CLIENT_NUM_MAX 1
#define AT_USING_SOCKET
#define AT_CMD_MAX_LEN 128
#define AT_SW_VERSION_NUM 0x10200

/* IoT - internet of things */

#define PKG_USING_PAHOMQTT
#define PAHOMQTT_PIPE_MODE
#define PKG_USING_PAHOMQTT_EXAMPLE
//#define MQTT_USING_TLS
#define RT_PKG_MQTT_THREAD_STACK_SIZE 0x1000
#define PKG_PAHOMQTT_SUBSCRIBE_HANDLERS 4
#define MQTT_DEBUG
#define PKG_USING_PAHOMQTT_LATEST
#define PKG_USING_CJSON
#define PKG_USING_CJSON_V102

/* Wiced WiFi */

#define PKG_USING_AT_DEVICE
#define PKG_AT_INIT_BY_THREAD
#define AT_DEVICE_ESP8266
#define AT_DEVICE_SIM7600
#define AT_DEVICE_SOCKETS_NUM 5
#define AT_DEVICE_NAME "uart3"
#define AT_DEVICE_RECV_BUFF_LEN 1024
#define AT_DEVICE_WIFI_SSID "Cloudwater"
#define AT_DEVICE_WIFI_PASSWORD "tqcd2018"
#define PKG_USING_AT_DEVICE_LATEST_VERSION

/* IoT Cloud */

/* peripheral libraries and drivers */
#define PKG_USING_I2C_TOOLS
#define I2C_TOOLS_USE_SW_I2C
#define I2C_TOOLS_SW_ACK_TIMEOUT 100
#define PKG_USING_I2C_TOOLS_LATEST_VERSION

/* security packages */

#define PKG_USING_MBEDTLS

/* Select Root Certificate */

#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_WINDOW_SIZE 2
#define MBEDTLS_SSL_MAX_CONTENT_LEN 3584
#define PKG_USING_MBEDTLS_V260
/* SECTION: lwip, a lighwight TCP/IP protocol stack */
/* #define RT_USING_LWIP */
/* LwIP uses RT-Thread Memory Management */
#define RT_LWIP_USING_RT_MEM
/* Enable ICMP protocol*/
#define RT_LWIP_ICMP
/* Enable UDP protocol*/
#define RT_LWIP_UDP
/* Enable TCP protocol*/
#define RT_LWIP_TCP
/* Enable DNS */
#define RT_LWIP_DNS

/* the number of simulatenously active TCP connections*/
#define RT_LWIP_TCP_PCB_NUM 5

/* Using DHCP */
/* #define RT_LWIP_DHCP */

/* ip address of target*/
#define RT_LWIP_IPADDR0 192
#define RT_LWIP_IPADDR1 168
#define RT_LWIP_IPADDR2 1
#define RT_LWIP_IPADDR3 30

/* gateway address of target*/
#define RT_LWIP_GWADDR0 192
#define RT_LWIP_GWADDR1 168
#define RT_LWIP_GWADDR2 1
#define RT_LWIP_GWADDR3 1

/* mask address of target*/
#define RT_LWIP_MSKADDR0 255
#define RT_LWIP_MSKADDR1 255
#define RT_LWIP_MSKADDR2 255
#define RT_LWIP_MSKADDR3 0

/* tcp thread options */
#define RT_LWIP_TCPTHREAD_PRIORITY 12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 10
#define RT_LWIP_TCPTHREAD_STACKSIZE 1024

/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY 15
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 10
#define RT_LWIP_ETHTHREAD_STACKSIZE 512

/* TCP sender buffer space */
#define RT_LWIP_TCP_SND_BUF 8192
/* TCP receive window. */
#define RT_LWIP_TCP_WND 8192

/* SECTION: RT-Thread/GUI */
/* #define RT_USING_RTGUI */

/* name length of RTGUI object */
#define RTGUI_NAME_MAX 12
/* support 16 weight font */
#define RTGUI_USING_FONT16
/* support Chinese font */
#define RTGUI_USING_FONTHZ
/* use DFS as file interface */
#define RTGUI_USING_DFS_FILERW
/* use font file as Chinese font */
#define RTGUI_USING_HZ_FILE
/* use Chinese bitmap font */
#define RTGUI_USING_HZ_BMP
/* use small size in RTGUI */
#define RTGUI_USING_SMALL_SIZE
/* use mouse cursor */
/* #define RTGUI_USING_MOUSE_CURSOR */
/* default font size in RTGUI */
#define RTGUI_DEFAULT_FONT_SIZE 16

/* image support */
/* #define RTGUI_IMAGE_XPM */
/* #define RTGUI_IMAGE_BMP */

// <bool name="RT_USING_CMSIS_OS" description="Using CMSIS OS API" default="true" />
// #define RT_USING_CMSIS_OS
// <bool name="RT_USING_RTT_CMSIS" description="Using CMSIS in RTT" default="true" />
#define RT_USING_RTT_CMSIS
// <bool name="RT_USING_BSP_CMSIS" description="Using CMSIS in BSP" default="true" />
// #define RT_USING_BSP_CMSIS

#endif
