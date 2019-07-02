/*
	Diego Garcia (diegargon) diego@envigo.net
*/
#include "domo_misc.h"

void DomoPinMonitor(void *context) {
	MQTTContext *ctx = (MQTTContext *)context;
	
	int numPins = ctx->numPins;
	PinConfig *pinConf = ctx->pinConf;
	int qos = ctx->defaultQOS;
	

	for (int i=0; i < numPins ; i++)  {
		
		//Check subs
		if ( (pinConf[i].pinMode == OUTPUT) && (pinConf[i].subscribed == 0) && (pinConf[i].subs_topic != NULL)  )
		{
			(pinConf[i].QoS >= 0) ? qos = pinConf[i].QoS :0;
			MQTT_subscribe(context, pinConf[i].subs_topic, qos );
			pinConf[i].subscribed = 1; //TODO doit when successfull subscribe			
		}
		
		//Check pin changes
		if ( (pinConf[i].pinMode == INPUT) && (pinConf[i].pub_topic != NULL) ) {			
			if ( (pinConf[i].pinPrevState == -1) || ( pinConf[i].pinPrevState != digitalRead(pinConf[i].pin) ) ){	
				log_msg(LOG_INFO, "Pin %d , change or setting state", pinConf[i].pin);

				pinConf[i].notify = 1;
				pinConf[i].pinPrevState = digitalRead(pinConf[i].pin);
			}	
		}
		//NOTIFY
		if(pinConf[i].notify == 1) {
			
			int retained = 0;
			char msg[10];
			
			(pinConf[i].QoS >= 0 ) ? qos = pinConf[i].QoS : 0;
			(pinConf[i].pub_retained >= 0 ) ? retained =  pinConf[i].pub_retained :0;
		
			if ( strcmp(pinConf[i].pubValue, "PinState") == 0) {
				sprintf(msg, "%d", digitalRead(pinConf[i].pin) );
			} else if ( strcmp(pinConf[i].pubValue, "PinStateVar") == 0 ) {
				if (digitalRead(pinConf[i].pin)) {
					sprintf(msg, "%s", pinConf[i].pinState1);
				} else {
					sprintf(msg, "%s", pinConf[i].pinState0);
				}
			} else {
				log_msg(LOG_ERR, "Error, pubValue on pin %d not set", pinConf[i].pin);
				exit(EXIT_FAILURE);
			}
			ctx->pinConf[i].notify = 0;
			log_msg(LOG_INFO, "Sending message to topic %s", pinConf[i].pub_topic);
			MQTT_sendMsg(ctx, pinConf[i].pub_topic, msg, retained, qos);
			pinConf[i].notify = 0;
		}		

	}
		
}


void domo_subs_msg(char *topicName, PinConfig *pinConf, int numPins) {
	log_msg(LOG_DEBUG, "Domo subs msg arrive called for topic %s", topicName);
	
	for (int i = 0; i < numPins; i++)  {
		if ( pinConf[i].subs_topic != NULL && (strcmp(topicName, pinConf[i].subs_topic) == 0)) {
			log_msg(LOG_DEBUG,"PINCONF, %d, %d",i , pinConf[i].pin);
			if ( (pinConf[i].logic != NULL) && (strcmp(pinConf[i].logic , "PushPin") == 0)) {
				log_msg(LOG_DEBUG,"PINCONF, %d, %d PushPin",i , pinConf[i].pin);
				int pinState = digitalRead(pinConf[i].pin);
				digitalWrite(pinConf[i].pin, !pinState);
				usleep(100000);
				digitalWrite(pinConf[i].pin, pinState);
			} else {
				log_msg(LOG_DEBUG,"PINCONF, %d, %d SwitchPin",i ,pinConf[i].pin);
				int pinState = digitalRead(pinConf[i].pin);
				digitalWrite(pinConf[i].pin, !pinState);				
			}
		}
	}
}	


void print_help(char *app_name) {
	printf("Diego García Gonzalez (diegargon) - diego@envigo.net\n");
	printf("\n Usage: %s [OPTIONS]\n\n", app_name);
	printf("  Options:\n");
	printf("   -p --pid-file filename   Daemonize this application\n");
	printf("   -d --daemon              Daemonize this application\n");
	printf("   -h --help               	This help\n");
	printf("\n");
}

