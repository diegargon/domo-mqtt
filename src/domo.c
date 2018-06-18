/*
	by Diego Garcia (diegargon)
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "domo.h"


int main() {
	int ch;// rc;
	
	log_init(APP_NAME);
	
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context));	
	if(MQTT_Init(ctx) == MQTTASYNC_SUCCESS) {
		log_msg(LOG_INFO, "Successfull MQTT Init\n");	
	}
	if(MQTT_connect(ctx) == MQTTASYNC_SUCCESS) {
		log_msg(LOG_INFO, "Successfull MQTT connect\n");
	}	
	
	
    printf("Press Q <Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));
	
	//usleep(5000000);
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	return 0;
}