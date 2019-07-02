/*
	Diego Garcia Gonzalez (diegargon) - diego@envigo.net
*/

#include "logger.h"

int LogToTTY = 0;

void log_init(char *appname, GKeyFile *conf, int daemon) {

	(!daemon) ? LogToTTY = domoCfg_getInt(conf, "LOGGING", "LogToTTY") : 0;
	
	int min_level = domoCfg_getInt(conf, "LOGGING", "MIN_LOG_LEVEL");	
	(min_level > -1 ) ? setlogmask (LOG_UPTO (min_level)) : 0;
		
	openlog (appname, LOG_PID | LOG_NDELAY, LOG_DAEMON);

	log_msg(LOG_INFO, "Log system enabled to level %d", min_level);
	
}

void log_close() {
	log_msg(LOG_DEBUG, "Log close");
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
	LogToTTY ? printf("%s\n", new_text) : 0;
		
	va_end(args);
	free(msg);
}
