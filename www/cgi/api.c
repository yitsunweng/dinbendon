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
#include <stdarg.h>
#include <string.h>
#include <mysql.h>
#include <my_global.h>
#include "cloud.h"
#include "cgilib.h"
#include "jansson.h"
#include "api.h"
#include "bill.h"

int main(){
	DEBUGP(" = = = = = = = = = = %s %s = = = = = = = = = = \n",__DATE__,__TIME__);
	char *query = NULL;
	char page[50], *method = NULL;
	char *post_payload=NULL;
	json_error_t error;
	json_t *json;

	/* There are two get_query function in cgilib.c
	 * - For wispr we'll get simple data in post payload ,ie. a=1&b=2&...
	 *   so we use get_query to make query string and post data merge toghter.
	 * - For others, we'll get some complicated data like JSON format
	 *   thus we use get_query2 to get get query string and pay_load.
	 */
	if(strstr(getenv("QUERY_STRING"),"wisprpostrequest")==NULL){
		DEBUGP("Is normal post request, using get_query2\n");
		query = get_query2(stdin,&post_payload);
	}else{
		DEBUGP("Is wispr request, using get_query\n");
		query = get_query(stdin);
	}

	init_cgi(query);
	strcpy(page, get_cgi((char*)"page"));

	if (strcmp(page, "bill") == 0)
	{
		DEBUGP("Fuckin' bill\n");
		json_t *account=NULL, *date_begin=NULL, *date_end=NULL;

		if(!(json = json_loads(post_payload, 0 , &error)))	DEBUGP("json_loads did not detect JSON payload\n");

		account = json_object_get(json, "account");
		if(!json_is_string(account))	DEBUGP("account is not string.[%s]\n", json_string_value(account));
		date_begin = json_object_get(json, "date_begin");
		if(!json_is_string(date_begin))	DEBUGP("date_begin is not string.\n");
		date_end = json_object_get(json, "date_end");
		if(!json_is_string(date_end))	DEBUGP("date_end is not string.\n");
		DEBUGP("USER: [%s], DATE: [%s] ~ [%s]\n", json_string_value(account), json_string_value(date_begin), json_string_value(date_end));

		fprintf(stdout, "Content-Type: application/json; charset=utf-8\r\n\r\n");

		if (bill(json_string_value(account), json_string_value(date_begin), json_string_value(date_end)) == SUCCESS){
			DEBUGP("Query bill success!\n");
		}
		else{
			DEBUGP("Query bill fail!\n");
		}
		return 0;
	}
	init_cgi(NULL);
	if(query)
		free(query);
	return 0;
}
