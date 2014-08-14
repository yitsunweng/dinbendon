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

/*===================================
   FUNCTION get_query_data
====================================*/
/*!
@brief
  Tell this function what your expecy data size, and the query command result,
  This function will help to cram result into the array address you gave it.

@param
  char **result_array
  int array_size1
  int array_size2
  char *input_command

@return
  1 - success
  0 - find nothing
 -1 - error
*/
int get_query_data(char (*result_array)[], int array_size1, int array_size2,char *input_command){

	char temp_array[array_size1][array_size2];
	unsigned long num_fields;
	int state,empty=1;
	unsigned int string_lenth, count, i;
	char command[MAXCHARSIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	MYSQL *mysql=NULL;

	DEBUGP("[SQL] sql test start!\n");
	/* first, initialize mysql structure */
	mysql = mysql_init(mysql);
	if (!mysql) return -1;
	/* connect to server with host "127.0.0.1", db username "root"
	   , db password "rdd3rdd3", db name of mysq "cloud", port 3306
	   , not assign protocol and no option */
	mysql = mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DBNAME, MYSQL_PORT, NULL, 0);
	if (!mysql) {
		fprintf(stderr, "[SQL] connect failed!\n");
		return -1;
	}
	DEBUGP("[SQL] connected!\n");
	string_lenth = snprintf(command, sizeof(command),"%s",input_command);
	DEBUGP("[SQL] send query: %s\n", command);
	/* send query to list id and account which password is test in table user */
	/* int mysql_real_query(MYSQL *mysql, const char *stmt_str, unsigned long length) */
	state = mysql_real_query(mysql, command, string_lenth);
	if (state) {
		DEBUGP("Could not execute statement(s)");
		mysql_close(mysql);
		exit(0);
	}
	DEBUGP("[SQL] query success\n");
	do {
		/* did current statement return data? */
		result = mysql_store_result(mysql);
		if (result) {
			num_fields = mysql_num_fields(result);
			count = 0;
			/* yes; process rows then free the result set */
			while (row = mysql_fetch_row(result)) {
				count++;
				unsigned long *lengths;
				lengths = mysql_fetch_lengths(result);
				for (i=0; i<num_fields; i++) {
					empty=0;
					/* print returned [row], [column name], [content length], [content] */
					DEBUGP("[SQL] row: %d, name: %s,\tlength: %d,\tcontent: %s \n",
						count, result->fields[i].name, (int) lengths[i], row[i] ? row[i] : "NULL");
					if(i>=array_size1) break;
					strcpy(temp_array[i],row[i]);
				}
			}
			mysql_free_result(result);
		}
		else {
			/* no result set or error */
			if (mysql_field_count(mysql) == 0) {
				DEBUGP("%lld rows affected\n", mysql_affected_rows(mysql));
			}
			else {
				/* some error occurred */
				DEBUGP("Could not retrieve result set\n");
				break;
			}
		}
		/* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
		if ((state = mysql_next_result(mysql)) > 0)
			DEBUGP("Could not execute statement, state=%d\n", state);
	} while (state == 0);

	/* close the session and dealloctes mysql object */
	mysql_close(mysql);
	if(empty){
		return 0;
	}else{
		memcpy(result_array,temp_array,array_size1*array_size2);
	}
	return 1;
}

/*===================================
   FUNCTION loginCheck
====================================*/
/*!
@brief
  This function is for communicate with database to check the authentication.

@param
  enum login_method
  Variable arguments by login_method, WISPR for 2 and NORMAL for 4

@return
  0 - authenticate fail
  1 - authenticate success
*/
int loginCheck(login_method loginMethod, ...){
	/* Args: all char*
		loginMethod:WISPR	: userName userpw
		loginMethod:NORMAL	: userName userpw is_fb facebook_auth sid
	*/
	char command[MAXCHARSIZE];
	va_list valist;
	unsigned int string_lenth;
	int auth_result=0;
	int result;

	/* initialize valist for num number of arguments */
	va_start(valist, loginMethod);

	/* access all the arguments assigned to valist */
	if(loginMethod == WISPR){ // WISPr method, 2 args
		char *userName, *userpw;
		char db_data[2][64]; // 1.Account 2. Password
		userName = va_arg(valist, char*);
		userpw = va_arg(valist, char*);
		DEBUGP(
			"[loginCheck] method:WISPR, userName:%s, userpw:%s\n"
			,userName,userpw
		);
		sprintf(command,"SELECT account,password FROM user WHERE account=\"%s\";",userName);
		result = get_query_data(db_data,2,64,command);
		if(result == -1){
			DEBUGP("Query SQL errot.\n");
		}
		/*  Check Account and password.
			Note: After Query DB may get more than one result, but process only the first find.
		*/
		if(result && strcmp(userName,db_data[0])==0 && strcmp(userpw,db_data[1])==0){
			auth_result = 1;
		}
	}else{ // NORMAL login, 4 args
		char *userName,*userpw,*facebook_auth,*sid;
		char db_data[2][64]; // 1.Account 2. Password
		int provider;
		userName = va_arg(valist, char*);
		userpw = va_arg(valist, char*);
		provider = va_arg(valist, int);
		facebook_auth = va_arg(valist, char*);
		sid = va_arg(valist, char*);

		DEBUGP(
			"[loginCheck] method:NORMAL, userName:%s, userpw:%s, provider:%d, facebook_auth:%s, sid:%s\n"
			,userName,userpw,provider,facebook_auth,sid
		);

		if(provider == 1){
			/*TODO:
				- FB
					1. Server side FB,User data exchange.
					2. find account
					3. update DB.user.facebook_auth with accesstoken
					4. update DB.clientinformation.auth and user_id
					5. Return fail / success
			*/
		}else{
			/* Find account */
			/*  Check Account and password.
				Note: After Query DB may get more than one result, but process only the first find.
			*/
			sprintf(command,"SELECT account,password FROM user WHERE account=\"%s\";",userName);
			result = get_query_data(db_data,2,64,command);
			if(result == -1){
				DEBUGP("Query SQL errot.\n");
			}
			DEBUGP("result:%d\n",result);
			if( result && strcmp(userName,db_data[0])==0 && strcmp(userpw,db_data[1])==0){
				auth_result = 1;
				/* Update auth */
				sprintf(command,"UPDATE client_information SET auth = \"1\" WHERE client_sid=\"%s\";",sid);
				result = get_query_data(db_data,2,64,command);
				if(result == -1){
					DEBUGP("[CGI] Update auth fail.\n");
				}

				/* Update user_id */
				sprintf(command,"update client_information  \
							set user_id = (  \
								select user_id from user where account = \"%s\"\
							)\
							where client_sid=\"%s\";"\
					,userName,sid);

				result = get_query_data(db_data,2,64,command);
				if(result == -1){
					DEBUGP("[CGI] Update user_id fail.\n");
				}
			}
		}//end if(is_fb == "true"){
	}//end if(loginMethod == WISPR){

	/* clean memory reserved for valist */
	va_end(valist);

	// Check is auth success.
	if(auth_result==1){
		return 1;
	}
	return 0;
}

int main(){
	// This code can review all the lighttpd string
	//system("env > /tmp/lighttpd.env");
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

	/* Process WISPr http POST message */
	if (strcmp(page, "wisprpostrequest") == 0)
	{
		char *device_mac,*client_mac,*WISPrVersion,*userName,*userpw;
		int isWisprAuth = 0;
		device_mac = get_cgi("device_mac");
		client_mac = get_cgi("client_mac");
		WISPrVersion = get_cgi("WISPrVersion");
		userName = get_cgi("UserName");
		userpw = get_cgi("Password");

		// Check Authentication
		isWisprAuth = loginCheck(WISPR,userName,userpw);

		// TODO: Send Updatefirewall

		// Response WISPr respose
		if(isWisprAuth){ // Authenticated
			fprintf(stdout, \
			"<html>\n" \
			"<!—\n" \
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
			"<WISPAccessGatewayParam\n" \
			"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n" \
			"  xsi:noNamespaceSchemaLocation=\"http://www.wballiance.net/wispr/wispr_2_0.xsd\">\n" \
			"  <AuthenticationReply>\n" \
			"  <MessageType>120</MessageType>\n" \
			"  <ResponseCode>50</ResponseCode>\n" \
			"  <MaxSessionTime>3600</MaxSessionTime>\n" \
			"  <StatusURL>https://portal.yourkarma.com/wispr/%s/status</StatusURL>\n" \
			"  <LogoffURL>https://portal.yourkarma.com/wispr/%s/logoff</LogoffURL>\n" \
			"</WISPAccessGatewayParam>\n" \
			"-->\n" \
			"</html>\n"
			,client_mac,client_mac);
			fprintf(stdout, "\r\n\r\n");
		}else{ // Unauthenticated
			fprintf(stdout, \
			"<HTML> <!--\n" \
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
			"<WISPAccessGatewayParam\n" \
			"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n" \
			"  xsi:noNamespaceSchemaLocation=\"http://www.wballiance.net/wispr/wispr_2_0.xsd\">\n" \
			"  <AuthenticationReply>\n" \
			"    <MessageType>120</MessageType>\n" \
			"    <ResponseCode>100</ResponseCode>\n" \
			"    <ReplyMessage>Invalid Username or Password</ReplyMessage>\n" \
			"  </AuthenticationReply>\n" \
			"</WISPAccessGatewayParam>\n" \
			"--> </HTML>\n");
			fprintf(stdout, "\r\n\r\n");
		}
	}
	else if (strcmp(page, "signin") == 0)
	{
		// Load json format from post
		json_t *is_fb=NULL, *account=NULL, *password=NULL, *provider=NULL, *sid=NULL ,*facebook_auth=NULL;
		int is_auth = 0;

		DEBUGP("What I get in patload:\n%s\n",post_payload);
		json = json_loads(post_payload, 0 , &error);
		if(!json)
		{
			DEBUGP("json_loads did not detect JSON payload\n");
		}

		provider = json_object_get(json,"provider");
		if(!json_is_integer(provider))
		{
			DEBUGP("provider is bot integer\n");
		}

		// Get the value "account" from loaded json format
		account = json_object_get(json,"account");
		if(!json_is_string(account))
		{
			DEBUGP("account is not string.\n");
		}

		// Get the value "password" from loaded json format
		password = json_object_get(json,"password");
		if(!json_is_string(password))
		{
			DEBUGP("password is not string.\n");
		}

		// Get the value "facebook_auth"(fb_access_token) from loaded json format
		facebook_auth = json_object_get(json,"facebook_auth");
		if(!json_is_string(facebook_auth))
		{
			DEBUGP("facebook_auth is not string.\n");
		}

		// Get the value "sid" from loaded json format
		sid = json_object_get(json,"sid");
		if(!json_is_string(sid))
		{
			DEBUGP("sid is not string.\n");
		}

		// Take input data to check authentication.
		is_auth = loginCheck(NORMAL,json_string_value(account),json_string_value(password),json_integer_value(provider),json_string_value(facebook_auth),json_string_value(sid));

		// Response authenticate result to carl, by JSON.
		fprintf(stdout, "Content-Type: application/json; charset=utf-8\r\n\r\n");
		fprintf(stdout,"{\r\n\"is_auth\":%s\r\n}",(is_auth == 1)?"true":"false");

	}
	else if (strcmp(page, "signup") == 0)
	{
		// JSON parser.
		json_t *account=NULL, *password=NULL, *facebook_auth=NULL, *provider=NULL;
		int is_authed = 0, query_result=0;
		char command[MAXCHARSIZE];
		char db_data[1][64];

		json = json_loads(post_payload, 0 , &error);
		if(!json)
		{
			DEBUGP("json_loads did not detect JSON payload\n");
		}

		// get provider first.
		provider = json_object_get(json,"provider");
		if(!json_is_integer(provider)){DEBUGP("account is bot integer\n");}

		// Get the value "account" from loaded json format
		account = json_object_get(json,"account");
		if(!json_is_string(account)){DEBUGP("account is not string.\n");}

		sprintf(command,"SELECT user_id FROM user WHERE account=\"%s\";",json_string_value(account));
		query_result = get_query_data(db_data,1,64,command);
		if(query_result == -1){
			DEBUGP("Query SQL errot.\n");
			return -1;
		}else if (query_result == 1){ // account exist
			fprintf(stdout, "Content-Type: application/json; charset=utf-8\r\n\r\n");
			fprintf(stdout,"{\r\n\"signup_result\":false\r\n}");
			return 0;
		}

		// Add data into DB
		switch(json_integer_value(provider)){
			case 0: // Normal login
				// Get the value "password" from loaded json format
				password = json_object_get(json,"password");
				if(!json_is_string(password)){DEBUGP("password is not string\n");}
				sprintf(command,"INSERT INTO user( user_id, account, password, facbook_auth) VALUES (NULL , \'%s\', \'%s\',  \'\' );",json_string_value(account),json_string_value(password));
				query_result = get_query_data(db_data,1,64,command);
				if(query_result == -1){
					DEBUGP("Query SQL errot.\n");
					return -1;
				}
				break;

			case 1: // FB login
				// Get the value "facebook_auth" from loaded json format
				facebook_auth = json_object_get(json,"facebook_auth");
				if(!json_is_string(facebook_auth)){DEBUGP("facebook_auth is not string\n");}
				sprintf(command,"INSERT INTO user( user_id, account, password, facbook_auth) VALUES (NULL , \'%s\', \'\',  \'%s\' );",json_string_value(account),json_string_value(facebook_auth));
				query_result = get_query_data(db_data,1,64,command);
				if(query_result == -1){
					DEBUGP("Query SQL errot.\n");
					return -1;
				}
				break;
		} // end switch(atoi(provider))

		// Response to javascript.
		fprintf(stdout, "Content-Type: application/json; charset=utf-8\r\n\r\n");
		fprintf(stdout,"{\r\n\"signup_result\":true\r\n}");
	}
	else if (strcmp(page, "bill") == 0)
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
