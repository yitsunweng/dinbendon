#ifndef _ORDER_H
#define _ORDER_H
#define FAIL	0
#define SUCCESS	1
#define NO_DATA	2
#define INFO	3
#define SUM	4
int process_result_set(MYSQL *mysql, MYSQL_RES *result, int type);
int no_result_or_error(MYSQL *mysql);
int get_result(MYSQL *mysql, MYSQL_RES *result, int state, int type);
int get_table(MYSQL *mysql, MYSQL_RES *result);
int get_sum(MYSQL *mysql, MYSQL_RES *result);
int order(const char *item, const char *user, const char *price, const char *quantity, const char *sugar, const char *ice, const char *note);
int orderd();
#endif
