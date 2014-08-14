#ifndef __CLOUD_PROVISION_H__
#define __CLOUD_PROVISION_H__

enum {
    GET_SESSIONS,
    GET_DEVICE_ID,
    INSERT_DEVICE_INFO,
    ENUMEND,
};

int provision_msg_handler(char* topic,void* msg, int msg_len);

#endif

