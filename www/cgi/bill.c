#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <my_global.h>
#include "../include/cloud.h"
#include "bill.h"
#include "cgilib.h"
#include "api.h"

int process_result_set(MYSQL *mysql, MYSQL_RES *result, int type){
	MYSQL_ROW row;

	row = mysql_fetch_row(result);

	if (type == INFO){
		int first_data = 0;
		do{
			if (!row){
				DEBUGP("── NO DATA FOUND ──\n");
				fprintf(stdout, "],\n\t\"result\" : \"NO DATA FOUND\"\n}");
				return NO_DATA;
			}
			unsigned long *lengths = mysql_fetch_lengths(result);
			if (first_data != 0)	fprintf(stdout, ",");
			fprintf(stdout, "\n\t\t{\"uid\":\"%s\", \"time\":\"%s\", \"wanif\":\"%s\", "
					"\"ip\":\"%s\", \"tx\":\"%s\", \"rx\":\"%s\"}",
					row[0] ? row[0] : "NULL", row[1] ? row[1] : "NULL", row[2] ? row[2] : "NULL",
					row[3] ? row[3] : "NULL", row[4] ? row[4] : "NULL", row[5] ? row[5] : "NULL");
			first_data = 1;
		} while (row = mysql_fetch_row(result));
	}
	else if(type == SUM){
		do {
			unsigned long *lengths = mysql_fetch_lengths(result);
			fprintf(stdout, ",\n\t\t{\"tx_sum\":\"%s\", \"rx_sum\":\"%s\"}", row[0] ? row[0] : "NULL", row[1] ? row[1] : "NULL");
		} while (row = mysql_fetch_row(result));
	}
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

int get_result(MYSQL *mysql, MYSQL_RES *result, int state, int type){
	do {
		result = mysql_store_result(mysql);
		if (result) {
			if (process_result_set(mysql, result, type) == NO_DATA){
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

int get_info(MYSQL *mysql, MYSQL_RES *result, char* condition){
	char cmd[512];
	int state;
	int len = snprintf(cmd, sizeof(cmd),
		"SELECT client_datasession.user_id, client_datasession.timestamp, interface, ip_addr, client_tx, client_rx %s", condition);
	if (state = mysql_real_query(mysql, cmd, len)){
		DEBUGP("SQL error: %s\n", mysql_error(mysql));
		fprintf(stdout, "],\n\t\"result\" : \"%s\"\n}", mysql_error(mysql));
		return FAIL;
	}
	return get_result(mysql, result, state, INFO);
}

int get_sum(MYSQL *mysql, MYSQL_RES *result, char* condition){
	char cmd[512];
	int state;
	int len = snprintf(cmd, sizeof(cmd), "SELECT SUM(client_tx),SUM(client_rx) %s", condition);
	if (state = mysql_real_query(mysql, cmd, len)){
		DEBUGP("SQL error: %s\n", mysql_error(mysql));
		return FAIL;
	}
	return get_result(mysql, result, state, SUM);
}

int bill(const char *account, const char *date_begin, const char *date_end){
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

	memset(condition, 0, sizeof(condition));
	snprintf(condition, sizeof(condition),
		"FROM heartbeat, client_datasession, user WHERE "
		"heartbeat.id = heartbeat_id AND "
		"user.account = '%s' AND "
		"client_datasession.user_id = user.user_id AND "
		"client_datasession.timestamp >= '%s 00:00:00' AND "
		"client_datasession.timestamp <= '%s 23:59:59';",
		account, date_begin, date_end);

	if (get_info(mysql, result, condition) == FAIL)	goto close_end;
	if (get_sum(mysql, result, condition) == FAIL)	goto close_end;

	fprintf(stdout, "],\n\t\"result\" : \"success\"\n}");
	return SUCCESS;

close_end:
	mysql_close(mysql);
	return FAIL;
}
