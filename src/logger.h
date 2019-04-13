/*
	Diego Garcia Gonzalez (diegargon)

LOG_EMERG	0
LOG_ALERT	1	
LOG_CRIT		2	
LOG_ERR		3
LOG_WARNING	4
LOG_NOTICE	5
LOG_INFO		6
LOG_DEBUG	7

*/

#if !defined(LOGGER_H)
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h> 
#include <unistd.h>
#include <syslog.h>

#include "domo_config.h"
 


void log_init(char* appname, GKeyFile *conf);
void log_msg(int level, char * format, ...);
void log_close();


#endif
