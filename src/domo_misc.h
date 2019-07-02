/*
	Diego Garcia (diegargon) diego@envigo.net
*/

#if !defined(DOMO_MISC_H)
#define DOMO_MISC_H

#include "mqtt.h"
#include "wiring.h"


void print_help(char *app_name);
void DomoPinMonitor(void *context);
void domo_subs_msg(char *topicName, PinConfig *pinConf, int numPins);

#endif