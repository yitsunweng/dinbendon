#ifndef _ORDER_H
#define _ORDER_H
#define FAIL	0
#define SUCCESS	1
#define INST	2
#define INFO	3
#define SUM	4
void mysql_exit(MYSQL *mysql, const char* debug, const char *result);
void mysql_success(MYSQL *mysql);
void process_result_set(MYSQL *mysql, MYSQL_RES *result, int type);
int no_result_or_error(MYSQL *mysql);
void get_result(MYSQL *mysql, int state, int type);
void query(MYSQL *mysql, int type, char* condition);
int order(const char *item, const char *user, const char *price, const char *quantity, const char *sugar, const char *ice, const char *note);
int orderd();
#endif
