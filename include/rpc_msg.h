#ifndef __RPC_MSG_H__
#define __RPC_MSG_H__

#include "request.pb-c.h"

typedef enum
{
	E_SUCCESS = 0,
	E_MALLOC_ERROR = -1,
	E_UNPACK_ERROR = -2,
	E_MQTT_ERROR = -3,
} rpc_error_t;

char* get_rpc_error_msg(const rpc_error_t err);
void rpc_msg_handler(char* topic, void* msg, int msg_len);
rpc_error_t redirect_uri_handler(const Karma__Request *request);

#endif

