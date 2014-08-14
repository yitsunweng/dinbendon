#ifndef _ORDER_H
#define _ORDER_H
#define FAIL	0
#define SUCCESS	1
#define NO_DATA	2
#define INFO	3
#define SUM	4
int process_result_set2(MYSQL *mysql, MYSQL_RES *result, int type);
int no_result_or_error2(MYSQL *mysql);
int get_result2(MYSQL *mysql, MYSQL_RES *result, int state, int type);
int get_info2(MYSQL *mysql, MYSQL_RES *result, char* condition);
int get_sum2(MYSQL *mysql, MYSQL_RES *result, char* condition);
int order(const char *item, const char *user, const char *quantity, const char *sugar, const char *ice, const char *shop, const char *note);
#endif
