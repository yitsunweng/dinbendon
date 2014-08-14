/**************************************************************************
 *
 * GEMTEK CONFIDENTIAL
 *
 * Copyright 2006-2007 Gemtek Technology Co.,Ltd. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related
 * to the source code ("Material") are owned by Gemtek Technology Co.,Ltd.
 * or its suppliers or licensors. Title to the Material remains with
 * Gemtek Technology Co.,Ltd. or its suppliers and licensors. The Material
 * contains trade secrets and proprietary and confidential information of
 * Gemtek or its suppliers and licensors. The Material is protected by
 * worldwide copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified,
 * published, uploaded, posted, transmitted, distributed, or disclosed
 * in any way without Gemtek's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license under such
 * intellectual property rights must be express and approved by Gemtek in
 * writing.
 *
 **************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <netinet/if_ether.h>

#define MAC_BCAST_ADDR          (unsigned char *) "\xff\xff\xff\xff\xff\xff"

enum {
	IPKG_LIST, IPKG_INSTALLED
};

enum {
	ARC4_ENCODE, ARC4_DECODE
};

char *get_file_content(const char *file, char *content, int len);
int set_file_content(const char *file, const char *content);
char *change_ipaddr_prefix(char *content, const char *old_prefix, const char *new_prefix);
char *trim(char *str);
int get_interface_info(char *interface, unsigned char *mac, char *ip, char *mask);
int conn_from_lan(char* interface, char *remote_ip);
void get_sw_list(int flag);
int mkfile(const char *filename);
char *hextostr(unsigned char *in, int in_size);
unsigned char* strtohex(const char *in, int *len);
int uptime_format(unsigned long uptime, char *buffer, unsigned int buffer_size);
void sys_reboot();
#endif // _UTILS_H
