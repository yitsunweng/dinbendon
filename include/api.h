#define DEBUGMSG 1
#define MAXCHARSIZE 256
#if DEBUGMSG
	#define DEBUGP(fmt,arg...) {FILE *fp=fopen("/tmp/karma_server_api.log","a");fprintf(fp,"(%d/%s): "fmt,__LINE__,__FILE__,##arg);fclose(fp);}
#else
	#define DEBUGP(fmt,arg...)
#endif
typedef enum{
	WISPR   = 2,
	NORMAL  = 5,
}login_method;

