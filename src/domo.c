/*

	Diego Garcia (diegargon)

*/


#include "domo.h"


int main() {
	int ch;// rc;

	GKeyFile *conf = g_key_file_new ();
	
	domoCfg_Init(conf);
	
	log_init(APP_NAME, conf);
	
	Wiring_Init(conf);
	
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context));	
	if(MQTT_Init(ctx, conf) == MQTTASYNC_SUCCESS) {
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