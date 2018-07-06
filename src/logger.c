/*
	Diego Garcia Gonzalez (diegargon)
*/

#include "logger.h"

int LogToTTY = 0;

void log_init(char *appname, GKeyFile *conf) {

	LogToTTY = domoCfg_getInt(conf, "LOGGING", "LogToTTY");	
	int min_level = domoCfg_getInt(conf, "LOGGING", "MIN_LOG_LEVEL");	
	if (min_level > -1 ) 
	{
		log_msg(min_level -1 , "Setting loggin min_level to %d\n", min_level);		
		setlogmask (LOG_UPTO (min_level));
	}
		
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
	if (LogToTTY) {
		printf("%s", new_text);
	}
	
	va_end(args);
	free(msg);
}
