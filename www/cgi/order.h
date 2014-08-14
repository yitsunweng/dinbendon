#ifndef _ORDER_H
#define _ORDER_H
#define FAIL	0
#define SUCCESS	1
#define NO_DATA	2
int process_result_set(MYSQL *mysql, MYSQL_RES *result);
int no_result_or_error(MYSQL *mysql);
int get_result(MYSQL *mysql, MYSQL_RES *result, int state);
int get_table(MYSQL *mysql, MYSQL_RES *result);
int order(const char *item, const char *user, const char *quantity, const char *sugar, const char *ice, const char *shop, const char *note);
#endif
