#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <my_global.h>
#include "../include/cloud.h"
#include "order.h"
#include "cgilib.h"
#include "api.h"

void mysql_exit(MYSQL *mysql, const char* debug, const char *result)
{
	DEBUGP("SQL error: %s\n", debug);
	fprintf(stdout, "],\n\t\"result\" : \"%s\"\n}", result);
	mysql_close(mysql);
	exit(0);
}

void mysql_success(MYSQL *mysql)
{
	fprintf(stdout, "],\n\t\"result\" : \"Success\"\n}");
	mysql_close(mysql);
	exit(0);
}

void process_result_set(MYSQL *mysql, MYSQL_RES *result, int type)
{
	MYSQL_ROW row;
	unsigned int i, num_fields = mysql_num_fields(result);

	row = mysql_fetch_row(result);

	do{
		if (!row)	mysql_exit(mysql, "── NO DATA FOUND ──", "NO DATA FOUND");
		fprintf(stdout, "\n\t\t{");
		for (i=0;i<num_fields;i++){
			if (i)	fprintf(stdout, ", ");
			fprintf(stdout, "\"%s\":\"%s\"", result->fields[i].name, row[i] ? row[i] : "NULL");
		}
		fprintf(stdout, "%s", (type == INFO) ? "}," : "}");
	} while (row = mysql_fetch_row(result));

	return;
}

int no_result_or_error(MYSQL *mysql)
{
	if (mysql_field_count(mysql) == 0){
		DEBUGP("%lld rows affected\n", mysql_affected_rows(mysql));
	}
	else{
		DEBUGP("Could not retrieve result set\n");
	}
	return 1;
}

void get_result(MYSQL *mysql, int state, int type)
{
	MYSQL_RES *result;
	do {
		result = mysql_store_result(mysql);
		if (result) {
			process_result_set(mysql, result, type);
			mysql_free_result(result);
		}
		else if (no_result_or_error(mysql) == 1)	break;
		state = mysql_next_result(mysql);
	} while (state == 0);
	return;
}

void query(MYSQL *mysql, int type, char* condition)
{
	char cmd[512];
	int state, len;

	if (type == INFO)
		len = snprintf(cmd, sizeof(cmd), "SELECT timestamp, user, item, price, quantity, sugar, ice, note FROM `order` WHERE 1;");
	else if (type == SUM)
		len = snprintf(cmd , sizeof(cmd), "SELECT SUM(quantity) AS Tquantity, SUM(price) AS Tprice FROM `order` WHERE 1;");
	else if (type == INST)
		len = snprintf(cmd, sizeof(cmd), "INSERT INTO `order`(user, item, price, quantity, sugar, ice, note) VALUES (%s);", condition);

	if (mysql_real_query(mysql, cmd, len))
		mysql_exit(mysql, mysql_error(mysql), (type == INST) ? "Fail" : mysql_error(mysql));
	else if (type != INST)
		get_result(mysql, state, type);
}

int order(const char *item, const char *user, const char *price, const char *quantity, const char *sugar, const char *ice, const char *note)
{
	char condition[512];

	MYSQL *mysql = mysql_init(NULL);;
	MYSQL_RES *result;
	MYSQL_FIELD *field;

	fprintf(stdout, "{\n\t\"data\" : [");

	if (mysql == NULL)	mysql_exit(mysql, "MYSQL init failed!", "Query failed!");

	mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	if (mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DBNAME, MYSQL_PORT, NULL, 0) == NULL)
		mysql_exit(mysql, mysql_error(mysql), "Query failed!");

	snprintf(condition, sizeof(condition), "'%s', '%s', '%s', '%s', '%s', '%s', '%s'", user, item, price, quantity, sugar, ice, note);

	query(mysql, INST, condition);

	mysql_success(mysql);
}

int orderd()
{
	MYSQL *mysql = mysql_init(NULL);;
	MYSQL_RES *result;
	MYSQL_FIELD *field;

	fprintf(stdout, "{\n\t\"data\" : [");

	if (mysql == NULL) mysql_exit(mysql, "MYSQL init failed!", "Query failed!");

	mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	if (mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DBNAME, MYSQL_PORT, NULL, 0) == NULL)
		mysql_exit(mysql, mysql_error(mysql), "Query failed!");

	query(mysql, INFO, "");
	query(mysql, SUM, "");

	mysql_success(mysql);
}
