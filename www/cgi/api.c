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
#include "order.h"

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

	if (strcmp(page, "orderd") == 0)
	{
		fprintf(stdout, "Content-Type: application/json; charset=UTF-8\r\n\r\n");
		
		if (orderd() == SUCCESS){
			DEBUGP("[%s] query success\n", page);
		}
		else{
			DEBUGP("[%s] query failed\n", page);
		}
		return 0;
	}
	else if (strcmp(page, "order") == 0)
	{
		json_t *item = NULL, *user = NULL, *price = NULL, *quantity = NULL, *sugar = NULL, *ice = NULL, *note = NULL;

		if(!(json = json_loads(post_payload, 0 , &error)))	DEBUGP("[%s] json_loads did not detect JSON payload\n", page);

		fprintf(stdout, "Content-Type: application/json; charset=UTF-8\r\n\r\n");
		
		item = json_object_get(json, "item");
		if(!json_is_string(item))	DEBUGP("[%s] item is not string.[%s]\n", page, json_string_value(item));
		
		user = json_object_get(json, "user");
		if(!json_is_string(user))	DEBUGP("[%s] user is not string.[%s]\n", page, json_string_value(user));
		
		price = json_object_get(json, "price");
		if(!json_is_string(price))	DEBUGP("[%s] price is not string.[%s]\n", page, json_string_value(quantity));
		
		quantity = json_object_get(json, "quantity");
		if(!json_is_string(quantity))	DEBUGP("[%s] quantity is not string.[%s]\n", page, json_string_value(quantity));
		
		sugar = json_object_get(json, "sugar");
		if(!json_is_string(sugar))	DEBUGP("[%s] sugar is not string.[%s]\n", page, json_string_value(sugar));
		
		ice = json_object_get(json, "ice");
		if(!json_is_string(ice))	DEBUGP("[%s] ice is not string.[%s]\n", page, json_string_value(ice));
		
		note = json_object_get(json, "note");
		if(!json_is_string(note))	DEBUGP("[%s] note is not string.[%s]\n", page, json_string_value(note));

		if (order(json_string_value(item), json_string_value(user), json_string_value(price), json_string_value(quantity),
			json_string_value(sugar), json_string_value(ice), json_string_value(note)) == SUCCESS){
			DEBUGP("[%s] Insert success!\n", page);
		}
		else{
			DEBUGP("[%s] Insert fail!\n", page);
		}
		return 0;
		
	}
	init_cgi(NULL);
	if(query)
		free(query);
	return 0;
}
