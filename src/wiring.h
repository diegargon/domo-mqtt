/*

	Diego García Gonzalez (diegargon)
	
*/


#if !defined(WIRING_H)
#define WIRING_H

#include <errno.h>
#include <wiringPi.h>
#include "domo_config.h"
#include "logger.h"

#define PIN_INPUT 0 
#define PIN_OUTPUT 1
#define PIN_PWM 2

#define PULL_OFF  0
#define PULL_DOWN  1
#define PULL_UP  2



typedef struct pinConfig {
	int pin;
	int pinMode;
	int pullMode;
	int pinInitState;
	int pinPrevState;
	char *pub_topic;
	int pub_retained;
	int QoS;
	char *pubValue; // PinState or PinStateVar0/1
	char *pinState0;
	char *pinState1;
	char *subs_topic;
	int subscribed;
	char *logic; // INPUT (PUSHPIN or SWITCHPIB)	
} pinConfig;


#include "mqtt.h"


int WiringInit(GKeyFile *conf, pinConfig *pinConf, int pinConfSize);

void WiringPinMonitor(void *context);
void  printPinConfig(pinConfig *pinConf, int size);

#endif
