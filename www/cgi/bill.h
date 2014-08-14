#ifndef _BILL_H
#define _BILL_H
#define FAIL	0
#define SUCCESS	1
#define NO_DATA	2
#define INFO	3
#define SUM	4
int process_result_set(MYSQL *mysql, MYSQL_RES *result, int type);
int no_result_or_error(MYSQL *mysql);
int get_result(MYSQL *mysql, MYSQL_RES *result, int state, int type);
int get_info(MYSQL *mysql, MYSQL_RES *result, char* condition);
int get_sum(MYSQL *mysql, MYSQL_RES *result, char* condition);
int bill(const char *account, const char *date_begin, const char *date_end);
#endif
