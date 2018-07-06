/*

	Diego García Gonzalez (diegargon)
	WiringPI for Orange PI H3
	git clone https://github.com/zhaolei/WiringOP.git -b h3 
	
*/

#include "wiring.h"



int WiringInit (GKeyFile *conf, pinConfig *pinConf, int pinConfSize) {
	int i;
	int DefQoS;
	//gsize pinConfSize = 0;
	
	if(domoCfg_getInt(conf, "WIRING","SysMode") == 1) {
		log_msg(LOG_NOTICE, "WiringPI starting in sys mode\n");
		if (wiringPiSetupSys() < 0)
		{
			log_msg(LOG_ERR, "oops:	%s\n", strerror (errno)) ;
			exit(EXIT_FAILURE);
		}	
		
	} else {
		log_msg(LOG_NOTICE, "WiringPI starting in normal mode\n");
		if(geteuid() != 0) 
		{
			log_msg(LOG_ERR, "Must be root\n");
			exit(EXIT_FAILURE);
		}
		if (wiringPiSetup() < 0)
		{
			log_msg(LOG_ERR, "oops:	%s\n", strerror (errno)) ;
			exit(EXIT_FAILURE);
		}	
	}
	
	DefQoS = domoCfg_getInt(conf, "MQTT","DefaultQoS");
	
	//char **pins = domoCfg_getStringList(conf, "WIRING", "PinConf", &pinConfSize);
	char **pins = domoCfg_getStringList(conf, "WIRING", "PinConf", NULL);
	
	for(i = 0; i < pinConfSize ; i++) {
		//int pullMode;
		//int pin = domoCfg_getInt(conf, pins[i], "Pin");
		if (!domoCfg_hasGroup(conf, pins[i])) 
		{
			log_msg(LOG_ERR, "Configuration group %s not exist\n", pins[i]);
			exit(EXIT_FAILURE);
		}
		pinConf[i].pin = domoCfg_getInt(conf, pins[i], "Pin");
		if (pinConf[i].pin < 0) 
		{
			log_msg(LOG_ERR, "pinConf %s has not pin defined\n", pins[i]);
			exit(EXIT_FAILURE);
		}
		
		
		//PIN MODE
		
		if ( strcmp(domoCfg_getString(conf, pins[i], "Mode"), "input") == 0 ) 
		{
			pinConf[i].pinMode = PIN_INPUT;
		
			// SETUP INTERNAL PULLS		
			log_msg(LOG_DEBUG, "PIN %d -> INPUT mode settings pulls\n", pinConf[i].pin);
		
			if (strcmp(domoCfg_getString(conf, pins[i], "Pull"),"up") == 0)  {			
				pinConf[i].pullMode = PULL_UP;
			} else if (strcmp(domoCfg_getString(conf, pins[i], "Pull"), "down") == 0) {			
				pinConf[i].pullMode = PULL_DOWN;
			} else {				
				pinConf[i].pullMode = PULL_OFF;
			} 				
			pullUpDnControl(pinConf[i].pin, pinConf[i].pullMode);
			pinConf[i].pinInitState = -1;
			
		} 
		else if ( strcmp(domoCfg_getString(conf, pins[i], "Mode"), "output") == 0) {
			pinConf[i].pinMode = PIN_OUTPUT;
			// SETUP PIN OUTPUT INITIAL STATUS, DEFAULT 0
			int pinInitState = domoCfg_getInt(conf, pins[i], "PinInitState");
			if (pinInitState != 1) {
				pinInitState = 0;
			}
			digitalWrite(pinConf[i].pin, pinInitState);
			pinConf[i].pinInitState = pinInitState;							
			log_msg(LOG_DEBUG, "PIN %d -> Setting inital state to %d\n", pinConf[i].pin, pinInitState);
			pinConf[i].pullMode = -1;
			
			
		} else if ( strcmp(domoCfg_getString(conf, pins[i], "Mode"), "pwm") == 0) {
			pinConf[i].pinMode = PIN_PWM;
			pinConf[i].pullMode = PULL_OFF;
			pinConf[i].pinInitState = -1;
		} else {
			log_msg(LOG_WARNING, "PIN %d -> Incorrect mode configuration\n", pinConf[i].pin);  
		}

		pinConf[i].pub_topic = domoCfg_getString(conf, pins[i], "Publish");
		pinConf[i].subs_topic = domoCfg_getString(conf, pins[i], "Subscribe");
		pinConf[i].subscribed = 0;
		pinConf[i].logic = domoCfg_getString(conf, pins[i], "Logic");
		pinConf[i].pubValue = domoCfg_getString(conf, pins[i], "PubValue");
		pinConf[i].pinState0 = domoCfg_getString(conf, pins[i], "PinState0");
		pinConf[i].pinState1 = domoCfg_getString(conf, pins[i], "PinState1");
		pinConf[i].QoS = domoCfg_getInt(conf, pins[i], "QoS");
		if (pinConf[i].QoS == -1 ){
			pinConf[i].QoS = DefQoS;
		}
		pinConf[i].pub_retained = domoCfg_getInt(conf, pins[i], "Retained");
	}
	

	return 0;
}	

void  printPinConfig(pinConfig *pinConf, int size) 
{
	int i;
	
	for (i = 0; i < size; i++) {
		log_msg(LOG_DEBUG, "------------------------------------------------------\n");	
		log_msg(LOG_DEBUG, "[PINCONFIG] Pin: %d\n", pinConf[i].pin);
		log_msg(LOG_DEBUG, "[PINCONFIG] Pin Mode: %d\n", pinConf[i].pinMode);
		log_msg(LOG_DEBUG, "[PINCONFIG] Pull Mode: %d\n", pinConf[i].pullMode);	
		log_msg(LOG_DEBUG, "[PINCONFIG] Pin Initial State: %d\n", pinConf[i].pinInitState);
		log_msg(LOG_DEBUG, "[PINCONFIG] Publish topic: %s\n", pinConf[i].pub_topic);
		log_msg(LOG_DEBUG, "[PINCONFIG] Publish retained: %d\n", pinConf[i].pub_retained);
		log_msg(LOG_DEBUG, "[PINCONFIG] QoS: %d\n", pinConf[i].QoS);
		log_msg(LOG_DEBUG, "[PINCONFIG] Publish Value: %s\n", pinConf[i].pubValue);
		log_msg(LOG_DEBUG, "[PINCONFIG] PinState0: %s\n", pinConf[i].pinState0);
		log_msg(LOG_DEBUG, "[PINCONFIG] PinState1: %s\n", pinConf[i].pinState1);
		log_msg(LOG_DEBUG, "[PINCONFIG] Subscribe topic: %s\n", pinConf[i].subs_topic);
		log_msg(LOG_DEBUG, "[PINCONFIG] Subscribed: %d\n", pinConf[i].subscribed);
		log_msg(LOG_DEBUG, "[PINCONFIG] Logic: %s\n", pinConf[i].logic);
	}
	log_msg(LOG_DEBUG, "------------------------------------------------------\n");	
}



void WiringPinMonitor(void *context, pinConfig *pinConf, int size) {
	int i;
	
	//mqtt_context *ctx = (mqtt_context *)context;
	
	//log_msg(LOG_DEBUG, "WiringLoop\n");
	
	for (i=0; i < size ; i++) 
	{
		//log_msg(LOG_DEBUG, "PIN %d -> status %d\n",  pinConf[i].pin, digitalRead(pinConf[i].pin));
		
		if ( (pinConf[i].pinMode == OUTPUT) && (pinConf[i].subs_topic != NULL) && (pinConf[i].subscribed == 0) )
		{
			MQTT_subscribe(context, pinConf[i].subs_topic, pinConf[i].QoS );
			pinConf[i].subscribed = 1; //TODO doit when successfull subscribe			
		}

	}
	
}
