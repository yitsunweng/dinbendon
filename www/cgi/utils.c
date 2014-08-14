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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "config.h"
#include "utils.h"

char *trim(char *str)
{
	size_t p;
	int i;
	char *head;

	if(strlen(str) == 0)
		return str;

	if((p = strcspn(str, "\r\n")) > 0)
		*(str + p) = 0;

	head = str + strspn(str, " \t");

	for(i=strlen(head); i>0 && isspace(head[i-1]); i--)
		head[i-1] = 0;

	/* make a string terminated with a zero */
	if(strlen(head) == 0)
		*str = 0;
	else if(head != str)
		memmove(str, head, strlen(head) + 1);

	return str;
}
