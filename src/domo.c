/*

	Diego Garcia (diegargon)

*/


#include "domo.h"


int main() {
	//int ch;// rc;
	unsigned long int SubsElements = 0;
	
	GKeyFile *conf = g_key_file_new ();
	
	domoCfg_Init(conf);
	
	log_init(APP_NAME, conf);
	
	Wiring_Init(conf);
	
	SubsElements = domCfg_getNumElementsList( conf, "MQTT", "SubsTopics" );	
	
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context) +  (sizeof(char *) * SubsElements));	
/*	
	printf("Sizeof ct_x: %d\n", sizeof(*ct_x));
	printf("Sizeof ctx: %d\n", sizeof(*ctx));
	printf("Sizeof mqtt_context: %d\n", sizeof(struct mqtt_context));
	printf("Sizeof mqtt_context + char: %d\n", sizeof(struct mqtt_context) + (sizeof(char*)));
	printf("Sizeof mqtt_context + char*n: %lu\n", sizeof(struct mqtt_context) + (sizeof(char*) * SubsElements));
	*/
	if(MQTT_Init(ctx, conf) == MQTTASYNC_SUCCESS) {
		log_msg(LOG_INFO, "Successfull MQTT Init\n");	
	}
	if(MQTT_connect(ctx) == MQTTASYNC_SUCCESS) {
		log_msg(LOG_INFO, "Successfull MQTT connect\n");
	}		
/*	
    printf("Press Q <Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));
	*/
	usleep(4000000);
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	return 0;
}