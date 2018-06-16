/*
	by Diego Garcia (diegargon)
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "domo-mqtt.h"

	

int main() {
	int ch;
	
	Config *conf;
	
	conf = g_slice_new(Config);
	int rc = read_config(conf);
	MQTTAsync mqtt_client;
	
	if (rc == 0 ) {
		print_config(conf);
	} else {
		printf("Read configuration error\n");
	}
	
	rc = MQTT_connect(&mqtt_client, conf);
	
    printf("Press Q <Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));
	//MQTT_disconnect(mqtt_client);
	//MQTT_destroy(&mqtt_client);
	return 0;
}