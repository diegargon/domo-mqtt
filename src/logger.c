/*
	Diego Garcia Gonzalez (diegargon)
*/

#include "logger.h"


void log_init(char *appname) {
	//TODO: FILTER config file	
	setlogmask (LOG_UPTO (LOG_NOTICE));

	openlog (appname, LOG_PID | LOG_NDELAY, LOG_DAEMON);

	log_msg(LOG_INFO, "Program started by user %d\n", getuid());
	
}

void log_close() {
	closelog();
}

void log_msg(int level, char * format, ...) {

	const char *level_name[] = 
	{
		"[EMERGENCY]", "[ALERT]", "[CRIRICAL]","[ERROR]","[WARNING]", "[NOTICE]", "[INFO]", "[DEBUG]"
	};
	int len;
	va_list args;
	char *msg;
	int new_text_len;
		
	va_start(args, format);
	
	len = vsnprintf(NULL, 0, format, args);	
	msg = malloc(len +1);
	vsprintf(msg, format, args);
	
	new_text_len = len + strlen(level_name[level]) + 1;
	
	char new_text[new_text_len];
	strcpy(new_text, level_name[level]);
	strcat(new_text, msg);
	
	syslog(level, new_text);
	
	va_end(args);
	free(msg);
}
