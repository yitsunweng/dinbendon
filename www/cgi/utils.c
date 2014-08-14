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

int get_interface_info(char *interface, unsigned char *mac, char *ip, char *mask)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in *addr;
	memset(&ifr, 0, sizeof(struct ifreq));

	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0)
	{
		ifr.ifr_addr.sa_family = AF_INET;
		strcpy(ifr.ifr_name, interface);

		if(mac)
		{
			if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
				memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
			else
			{
				close(fd);
				return 0;
			}
		}

		if(ip)
		{
			if(ioctl(fd, SIOCGIFADDR, &ifr) == 0)
			{
				addr = (struct sockaddr_in *) &ifr.ifr_addr;
				strcpy(ip, (const char *)(inet_ntoa(addr->sin_addr)));
			}
			else
			{
				close(fd);
				return 0;
			}
		}

		if(mask)
		{
			if(ioctl(fd, SIOCGIFNETMASK, &ifr) == 0)
			{
				addr = (struct sockaddr_in *) &ifr.ifr_netmask;
				strcpy(mask, (const char *)(inet_ntoa( addr->sin_addr)));
			}
			else
			{
				close(fd);
				return 0;
			}
		}
	}
	else
		return 0;

	close(fd);
	return 1;
}

int mkfile(const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "w");

	if(fp)
	{
		fclose(fp);
		return 1;
	}

	return 0;
}

char hextochar(unsigned char in)
{
	return (in<10)?in+48 : in+55;
}

unsigned char chartohex(unsigned char in)
{
	in = toupper(in);

	if((in >= 'A') && (in <= 'F'))
		in = in - 'A' + 10;
	else if((in >= '0') && (in <= '9'))
		in = in - '0';

	return in;
}

unsigned char* strtohex(const char *in, int *len)
{
	unsigned char *out;
	int out_size = 0;
	int i, j;
	out_size = strlen(in) / 2;

	if(out_size == 0)
		return NULL;

	out =(unsigned char *)malloc(sizeof(unsigned char) * out_size + 1);
	memset(out, 0, out_size + 1);

	for(i=0; i<out_size; i++)
	{
		j = 2 * i;
		out[i] = chartohex(in[j]);
		out[i] = out[i] << 4;
		out[i] = out[i] + chartohex(in[j+1]);
	}

	*len = out_size;
	return out;
}

char *hextostr(unsigned char *in, int in_size)
{
	unsigned char low = 0x0F, c;
	char *out;
	int i, size;

	if(!in_size)
		return NULL;

	size = in_size * 2 + 1;
	out = (char*)malloc(sizeof(char) * size);
	memset(out, 0, size);

	for(i=0; i<in_size; i++)
	{
		c = in[i] >> 4;

		if(c < 16)
			*(out + 2*i) = hextochar(c);

		c = in[i] & low;

		if(c < 16)
			*(out + 2*i + 1) = hextochar(c);
	}

	return out;
}

int conn_from_lan(char* interface, char *remote_ip)
{
	char lan_ip[16], netmask[16];
	get_interface_info(interface, NULL, lan_ip, netmask);

	if(((inet_addr(lan_ip) ^ inet_addr(remote_ip)) & inet_addr(netmask)) == 0)
		return 1;

	return 0;
}

char *get_file_content(const char *file, char *content, int len)
{
	FILE *fp;
	bzero(content, len);

	if(!(fp = fopen(file, "r")))
		return content;

	while(fgets(content, len, fp))
	{
		trim(content);

		if(strncmp(content, VERSION_TAG, strlen(VERSION_TAG)) == 0)
		{
			bzero(content, len);
			continue;
		}
		else
			break;
	}

	fclose(fp);
	return content;
}

int set_file_content(const char *file, const char *content)
{
	char *temp;
	FILE *fp;
	temp = calloc(1, sizeof(char) * BSIZE);

	if(!temp)
		return 0;

	if(NULL != (fp = fopen(file, "r")))
	{
		fgets(temp, BSIZE, fp);
		fclose(fp);
	}

	if(!(fp = fopen(file, "w")))
	{
		free(temp);
		return 0;
	}

	if(strncmp(temp, VERSION_TAG, strlen(VERSION_TAG)) == 0)
		fputs(temp, fp);

	fputs(content, fp);
	fputc('\n', fp);
	free(temp);
	fclose(fp);
	return 1;
}

int uptime_format(unsigned long uptime, char *buffer, unsigned int buffer_size)
{
	unsigned long h = 0, m = 0, s = 0;

	if(buffer == NULL)
		return 0;

	h = uptime / 3600;
	m = (uptime / 60) - (h * 60);
	s = uptime % 60;
	snprintf(buffer, buffer_size, "%lu:%02lu:%02lu", h, m, s);
	return 1;
}

void get_sw_list(int flag)
{
	char buffer[256], *ptr, *ptr2;
	char *sw_name, *sw_version;
	FILE *read_fp = NULL;
	unsigned int find_one = 0;
	char *sp = " - ";

	switch(flag)
	{
		case IPKG_LIST:
			unlink(IPKG_PACKAGES_LIST_DEVICESW);
			unlink(IPKG_PACKAGES_LIST_OPENWMX);
			sprintf(buffer, "ipkg -f %s update > /dev/null", IPKG_PATH);
			system(buffer);
			sprintf(buffer, "ipkg -f %s list", IPKG_PATH);
			read_fp = popen(buffer, "r");
			break;
		case IPKG_INSTALLED:
			sprintf(buffer, "ipkg -f %s list_installed", IPKG_PATH);
			read_fp = popen(buffer, "r");
			break;
		default:
			break;
	}

	if(read_fp != NULL)
	{
		bzero(buffer, sizeof(buffer));
		find_one = 0;

		while(fgets(buffer, sizeof(buffer), read_fp))
		{
			if(NULL == (ptr = strstr(buffer, sp)))
				break;

			if(find_one)
				printf(";");

			*ptr = 0;
			sw_name = buffer;
			sw_version = ptr + strlen(sp);
			ptr2 = strstr(sw_version, sp);

			if(ptr2 != NULL)
				*ptr2 = 0;

			printf("%s:%s", trim(sw_name), trim(sw_version));
			find_one = 1;
			bzero(buffer, sizeof(buffer));
		}

		pclose(read_fp);
	}
}

void sys_reboot()
{
	//printf("[System Reboot] : %s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	system("reboot");
}
void urldecode(char *p)

{
	register i=0;

	while(*(p+i))
	{
		if ((*p=*(p+i)) == '%')
		{
			*p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');
			*p=(*p) * 16;
			*p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');
			i+=2;
		}
		else if (*(p+i)=='+')
		{
			*p=' ';
		}

		p++;
	}

	*p='\0';
}


