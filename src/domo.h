/*

	Diego Garcia (diegargon) - diego@envigo.net

*/

#if !defined(DOMO_H)
#define DOMO_H

#define RECONNECT_TIME 2

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "domo_config.h"
#include "logger.h"
#include "mqtt.h"
#include "domo_misc.h"
#include "wiring.h"

#define APP_VERSION	0.1
#define APP_NAME "domo"
#define PID_FILE "domo.pid"
#define MAX_RECONNECT_TIME 10

void sig_handler(int sig);

#endif
