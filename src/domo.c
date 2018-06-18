/*
	by Diego Garcia (diegargon)
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "domo.h"

int main() {
	int ch;// rc;
	
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context));	
	if(MQTT_Init(ctx) == MQTTASYNC_SUCCESS) {
		printf("Successfull MQTT Init\n");	
	}
	if(MQTT_connect(ctx) == MQTTASYNC_SUCCESS) {
		printf("Successfull MQTT connect\n");
	}	
	
    printf("Press Q <Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	return 0;
}