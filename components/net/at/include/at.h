/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-30     chenyong     first version
 * 2018-08-17     chenyong     multiple client support
 */

#ifndef __AT_H__
#define __AT_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AT_SW_VERSION                  "1.2.0"

#define AT_CMD_NAME_LEN                16
#define AT_END_MARK_LEN                4

#ifndef AT_CMD_MAX_LEN
#define AT_CMD_MAX_LEN                 128
#endif

/* the server AT commands new line sign */
#if defined(AT_CMD_END_MARK_CRLF)
#define AT_CMD_END_MARK                "\r\n"
#elif defined(AT_CMD_END_MARK_CR)
#define AT_CMD_END_MARK                "\r"
#elif defined(AT_CMD_END_MARK_LF)
#define AT_CMD_END_MARK                "\n"
#endif

#ifndef AT_SERVER_RECV_BUFF_LEN
#define AT_SERVER_RECV_BUFF_LEN        256
#endif

#ifndef AT_SERVER_DEVICE
#define AT_SERVER_DEVICE               "uart2"
#endif

/* the maximum number of supported AT clients */
#ifndef AT_CLIENT_NUM_MAX
#define AT_CLIENT_NUM_MAX              1
#endif

#define AT_CMD_EXPORT(_name_, _args_expr_, _test_, _query_, _setup_, _exec_)   \
    RT_USED static const struct at_cmd __at_cmd_##_test_##_query_##_setup_##_exec_ SECTION("RtAtCmdTab") = \
    {                                                                          \
        _name_,                                                                \
        _args_expr_,                                                           \
        _test_,                                                                \
        _query_,                                                               \
        _setup_,                                                               \
        _exec_,                                                                \
    };

enum at_status
{
    AT_STATUS_UNINITIALIZED = 0,
    AT_STATUS_INITIALIZED,
    AT_STATUS_BUSY,
};
typedef enum at_status at_status_t;

#ifdef AT_USING_SERVER
enum at_result
{
    AT_RESULT_OK = 0,                  /* AT result is no error */
    AT_RESULT_FAILE = -1,              /* AT result have a generic error */
    AT_RESULT_NULL = -2,               /* AT result not need return */
    AT_RESULT_CMD_ERR = -3,            /* AT command format error or No way to execute */
    AT_RESULT_CHECK_FAILE = -4,        /* AT command expression format is error */
    AT_RESULT_PARSE_FAILE = -5,        /* AT command arguments parse is error */
};
typedef enum at_result at_result_t;

struct at_cmd
{
    char name[AT_CMD_NAME_LEN];
    char *args_expr;
    at_result_t (*test)(void);
    at_result_t (*query)(void);
    at_result_t (*setup)(const char *args);
    at_result_t (*exec)(void);
};
typedef struct at_cmd *at_cmd_t;

struct at_server
{
    rt_device_t device;

    at_status_t status;
    char (*get_char)(void);
    rt_bool_t echo_mode;

    char recv_buffer[AT_SERVER_RECV_BUFF_LEN];
    rt_size_t cur_recv_len;
    rt_sem_t rx_notice;
    char end_mark[AT_END_MARK_LEN];

    rt_thread_t parser;
    void (*parser_entry)(struct at_server *server);
};
typedef struct at_server *at_server_t;
#endif /* AT_USING_SERVER */

#ifdef AT_USING_CLIENT
enum at_resp_status
{
     AT_RESP_OK = 0,                   /* AT response end is OK */
     AT_RESP_ERROR = -1,               /* AT response end is ERROR */
     AT_RESP_TIMEOUT = -2,             /* AT response is timeout */
     AT_RESP_BUFF_FULL= -3,            /* AT response buffer is full */
};
typedef enum at_resp_status at_resp_status_t;

struct at_response
{
    /* response buffer */
    char *buf;
    /* the maximum response buffer size */
    rt_size_t buf_size;
    /* the number of setting response lines
     * == 0: the response data will auto return when received 'OK' or 'ERROR'
     * != 0: the response data will return when received setting lines number data */
    rt_size_t line_num;
    /* the count of received response lines */
    rt_size_t line_counts;
    /* the maximum response time */
    rt_int32_t timeout;
};

typedef struct at_response *at_response_t;

/* URC(Unsolicited Result Code) object, such as: 'RING', 'READY' request by AT server */
struct at_urc
{
    const char *cmd_prefix;
    const char *cmd_suffix;
    void (*func)(const char *data, rt_size_t size);
};
typedef struct at_urc *at_urc_t;

struct at_client
{
    rt_device_t device;

    at_status_t status;
    char end_sign;

    char *recv_buffer;
    rt_size_t recv_bufsz;
    rt_size_t cur_recv_len;
    rt_sem_t rx_notice;
    rt_mutex_t lock;

    at_response_t resp;
    rt_sem_t resp_notice;
    at_resp_status_t resp_status;

    const struct at_urc *urc_table;
    rt_size_t urc_table_size;

    rt_thread_t parser;
};
typedef struct at_client *at_client_t;
#endif /* AT_USING_CLIENT */

#ifdef AT_USING_SERVER
/* AT server initialize and start */
int at_server_init(void);

/* AT server send command execute result to AT device */
void at_server_printf(const char *format, ...);
void at_server_printfln(const char *format, ...);
void at_server_print_result(at_result_t result);

/* AT server request arguments parse */
int at_req_parse_args(const char *req_args, const char *req_expr, ...);
#endif /* AT_USING_SERVER */

#ifdef AT_USING_CLIENT

/* AT client initialize and start*/
int at_client_init(const char *dev_name,  rt_size_t recv_bufsz);

/* ========================== multiple AT client function ============================ */

/* get AT client object */
at_client_t at_client_get(const char *dev_name);
at_client_t at_client_get_first(void);

/* AT client wait for connection to external devices. */
int at_client_obj_wait_connect(at_client_t client, rt_uint32_t timeout);

/* AT client send or receive data */
rt_size_t at_client_obj_send(at_client_t client, const char *buf, rt_size_t size);
rt_size_t at_client_obj_recv(at_client_t client, char *buf, rt_size_t size, rt_int32_t timeout);

/* set AT client a line end sign */
void at_obj_set_end_sign(at_client_t client, char ch);

/* Set URC(Unsolicited Result Code) table */
void at_obj_set_urc_table(at_client_t client, const struct at_urc * table, rt_size_t size);

/* AT client send commands to AT server and waiter response */
int at_obj_exec_cmd(at_client_t client, at_response_t resp, const char *cmd_expr, ...);

/* AT response object create and delete */
at_response_t at_create_resp(rt_size_t buf_size, rt_size_t line_num, rt_int32_t timeout);
void at_delete_resp(at_response_t resp);
at_response_t at_resp_set_info(at_response_t resp, rt_size_t buf_size, rt_size_t line_num, rt_int32_t timeout);

/* AT response line buffer get and parse response buffer arguments */
const char *at_resp_get_line(at_response_t resp, rt_size_t resp_line);
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword);
int at_resp_parse_line_args(at_response_t resp, rt_size_t resp_line, const char *resp_expr, ...);
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...);

/* ========================== single AT client function ============================ */

/**
 * NOTE: These functions can be used directly when there is only one AT client.
 * If there are multiple AT Client in the program, these functions can operate on the first initialized AT client.
 */

#define at_exec_cmd(resp, ...)                   at_obj_exec_cmd(at_client_get_first(), resp, __VA_ARGS__)
#define at_client_wait_connect(timeout)          at_client_obj_wait_connect(at_client_get_first(), timeout)
#define at_client_send(buf, size)                at_client_obj_send(at_client_get_first(), buf, size)
#define at_client_recv(buf, size, timeout)       at_client_obj_recv(at_client_get_first(), buf, size, timeout)
#define at_set_end_sign(ch)                      at_obj_set_end_sign(at_client_get_first(), ch)
#define at_set_urc_table(urc_table, table_sz)    at_obj_set_urc_table(at_client_get_first(), urc_table, table_sz)

#endif /* AT_USING_CLIENT */

/* ========================== User port function ============================ */

#ifdef AT_USING_SERVER
/* AT server device reset */
void at_port_reset(void);

/* AT server device factory reset */
void at_port_factory_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AT_H__ */
