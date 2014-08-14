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

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <search.h>
#include <assert.h>
#include "cgilib.h"

static struct hsearch_data htab;

/* decode the URL query */
void query_decode(char *query)
{
	unsigned int c;

	while((query = strpbrk(query, "%+")))
	{
		if(*query == '%')
		{
			sscanf(query + 1, "%02x", &c);
			*query++ = (char)c;
			strncpy(query, query + 2, strlen(query) + 1);
		}
		else if(*query == '+')
			*query++ = ' ';
	}
}

char *get_query(FILE *stream)
{
	char *query = NULL, *content = NULL;
	int qlen = 0, clen = 0;

	if(getenv("QUERY_STRING"))
		qlen = strlen(getenv("QUERY_STRING"));

	if(getenv("CONTENT_TYPE"))
	{
		if(strncasecmp(getenv("CONTENT_TYPE"), "multipart/form-data; boundary=", 30) != 0)
		{
			if(getenv("CONTENT_LENGTH"))
			{
				clen = atoi(getenv("CONTENT_LENGTH"));
				content = calloc(1, sizeof(char) * (clen + 1));
				assert(content);
				fgets(content, clen + 1, stream);
			}
		}
	}

	query = malloc(sizeof(char) * (qlen + clen + 2));
	assert(query);

	if(qlen)
		strcat(query, getenv("QUERY_STRING"));

	if(qlen && clen)
		strcat(query, "&");

	if(clen)
		strcat(query, content);

	if(content)
		free(content);

	return query;
}

char *get_query2(FILE *stream,char ** str_xml)
{
	char *query = NULL, *xml = NULL;;
	int qlen = 0, clen = 0;

	if(getenv("QUERY_STRING"))
		qlen = strlen(getenv("QUERY_STRING"));

	if(getenv("CONTENT_LENGTH"))
	{
		clen = atoi(getenv("CONTENT_LENGTH"));
	}

	if(clen>0 && strcmp(getenv("REQUEST_METHOD"),"POST")==0)
	{
		xml = calloc(1, sizeof(char) * (clen + 1));
		assert(xml);
		fread(xml, clen + 1, 1,stream);
		*str_xml = xml;
	}

	query = malloc(sizeof(char) * (qlen + clen + 2));
	assert(query);
	memset(query,0,sizeof(char) * (qlen + clen + 2));
	if(qlen)
		strcat(query, getenv("QUERY_STRING"));

	return query;
}

void set_cgi(char *name, char *value)
{
	ENTRY e, *ep;

	if(!&htab)
		return;

	memset(&e, 0, sizeof(ENTRY));
	e.key = name;
	hsearch_r(e, FIND, &ep, &htab);

	if(ep)
		ep->data = value;
	else
	{
		e.data = value;
		hsearch_r(e, ENTER, &ep, &htab);
	}
}


char* get_cgi(char *name)
{
	ENTRY e= {NULL,NULL}, *ep;

	if(!&htab)
		return "";

	e.key = name;
	hsearch_r(e, FIND, &ep, &htab);
	return ep? ep->data: "";
}


void init_cgi(char *query)
{
	size_t nel;
	char *p, *name, *value;
	int len;

	if(!query)
	{
		hdestroy_r(&htab);
		return;
	}

	len = strlen(query);
	nel = 1;
	p = query;

	while(strsep(&p, "&"))
	{
		nel++;
	}

	if(!hcreate_r(nel, &htab))
		return;

	p = query;

	while(p < (query + len))
	{
		value = p;
		p += strlen(p) + 1;
		query_decode(value);
		name = strsep(&value, "=");

		if(!name || !value)
			continue;

		trim(name);
		trim(value);

		if(strstr(value,"<!--#") == NULL)
			set_cgi(name, value);
	}
}
