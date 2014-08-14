#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <my_global.h>
#include "../include/cloud.h"
#include "order.h"
#include "cgilib.h"
#include "api.h"

int process_result_set(MYSQL *mysql, MYSQL_RES *result){
	MYSQL_ROW row;

	row = mysql_fetch_row(result);

		int first_data = 0;
		do{
			if (!row){
				DEBUGP("── NO DATA FOUND ──\n");
				fprintf(stdout, "],\n\t\"result\" : \"NO DATA FOUND\"\n}");
				return NO_DATA;
			}
			unsigned long *lengths = mysql_fetch_lengths(result);
			if (first_data != 0)	fprintf(stdout, ",");
			fprintf(stdout, "\n\t\t{\"timestamp\":\"%s\", \"user\":\"%s\", \"item\":\"%s\", \"price\":\"%s\", "
					"\"quantity\":\"%s\", \"sugar\":\"%s\",	\"ice\":\"%s\", \"brandshop\":\"%s\", \"note\":\"%s\"}",
					row[0] ? row[0] : "NULL", row[1] ? row[1] : "NULL", row[2] ? row[2] : "NULL",
					row[3] ? row[3] : "NULL", row[4] ? row[4] : "NULL", row[5] ? row[5] : "NULL",
					row[6] ? row[6] : "NULL", row[7] ? row[7] : "NULL", row[8] ? row[8] : "NULL");
			first_data = 1;
		} while (row = mysql_fetch_row(result));
	return SUCCESS;
}

int no_result_or_error(MYSQL *mysql){
	if (mysql_field_count(mysql) == 0){
		DEBUGP("%lld rows affected\n", mysql_affected_rows(mysql));
	}
	else{
		DEBUGP("Could not retrieve result set\n");
	}
	return 1;
}

int get_result(MYSQL *mysql, MYSQL_RES *result, int state){
	do {
		result = mysql_store_result(mysql);
		if (result) {
			if (process_result_set(mysql, result) == NO_DATA){
				mysql_free_result(result);
				return FAIL;
			}
			mysql_free_result(result);
		}
		else if (no_result_or_error(mysql) == 1)	break;
		state = mysql_next_result(mysql);
	} while (state == 0);
	return SUCCESS;
}

int insert(MYSQL *mysql, MYSQL_RES *result, char* condition){
	char cmd[1024];
	int state;
	int len = snprintf(cmd, sizeof(cmd), "set name 'utf8';");
	mysql_real_query(mysql, cmd, len);
	len = snprintf(cmd, sizeof(cmd),
		"INSERT INTO `order`(user, item, price, quantity, sugar, ice, brandshop, note) "
		"VALUES (%s);", condition);
	if (state = mysql_real_query(mysql, cmd, len)){
		DEBUGP("[%s]\n", cmd);
		DEBUGP("SQL error: %s\n", mysql_error(mysql));
		fprintf(stdout, "],\n\t\"result\" : \"%s\"\n}", mysql_error(mysql));
		return FAIL;
	}
	return SUCCESS;
}

int get_table(MYSQL *mysql, MYSQL_RES *result){
	char cmd[512];
	int state;
	int len = snprintf(cmd, sizeof(cmd), "SELECT timestamp, user, item, price, quantity, sugar, ice, brandshop, note FROM `order` WHERE 1;");
	if (state = mysql_real_query(mysql, cmd, len)){
		DEBUGP("SQL error: %s\n", mysql_error(mysql));
		fprintf(stdout, "],\n\t\"result\" : \"%s\"\n}", mysql_error(mysql));
		return FAIL;
	}
	return get_result(mysql, result, state);
}

int order(const char *item, const char *user, const char *quantity, const char *sugar, const char *ice, const char *shop, const char *note){
	char condition[512];

	MYSQL *mysql = mysql_init(NULL);;
	MYSQL_RES *result;
	MYSQL_FIELD *field;

	fprintf(stdout, "{\n\t\"data\" : [");

	if (mysql == NULL){
		DEBUGP("MYSQL init failed!\n");
		fprintf(stdout, "],\n\t\"result\" : \"Query failed!\"\n}");
		goto close_end;
	}

	if (mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DBNAME, MYSQL_PORT, NULL, 0) == NULL){
		DEBUGP("SQL error: %s\n", mysql_error(mysql));
		fprintf(stdout, "],\n\t\"result\" : \"Query failed!\"\n}");
		goto close_end;
	}
	
	snprintf(condition, sizeof(condition), "'%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s'", user, item, "150", quantity, sugar, ice, shop, note);

	if (insert(mysql, result, condition) == FAIL)	goto close_end;
	if (get_table(mysql, result) == FAIL)	goto close_end;

	fprintf(stdout, "],\n\t\"result\" : \"success\"\n}");
	return SUCCESS;

close_end:
	mysql_close(mysql);
	return FAIL;
}
