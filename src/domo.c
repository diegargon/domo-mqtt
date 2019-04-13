/*

	Diego Garcia (diegargon)

*/


#include "domo.h"



int main() {
	
	//unsigned long int SubsNcount = 0;
	GKeyFile *conf = g_key_file_new ();
	
	domoCfg_Init(conf);
	log_init(APP_NAME, conf);	
	
	//pinConfig *pinConf = g_malloc0(sizeof(struct pinConfig));
	int NumPins = domCfg_getNumElementsList( conf, "WIRING", "PinConf" );	
	pinConfig pinConf[NumPins];
		
		
	WiringInit(conf, pinConf, NumPins);
	printPinConfig(pinConf, NumPins);
	
	//SubsNcount = domCfg_getNumElementsList( conf, "MQTT", "SubsTopics" );		
	//mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context) +  (sizeof(char *) * SubsNcount));
	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context) + (sizeof(struct pinConfig) * NumPins) );
	ctx->NumPins = NumPins;
	ctx->pinConf = pinConf;

	
	MQTT_Init(ctx, conf);	
	MQTT_connect(ctx);

	

	while(ctx->Connected == 0) 
	{
		log_msg(LOG_DEBUG,"MQTT_connect:Waiting to connected.\n");
		usleep(500000);
	}

	/*
	int ch;
    printf("Press Q <Enter> to quit\n\n");
    do
    {
		WiringPinMonitor(ctx, pinConf, NumPins);
		usleep(1000000);
        ch = getchar();
		WiringPinMonitor(ctx, pinConf, NumPins);
		
    } while( (ch!='Q' && ch != 'q'));	
	
	*/
	
	//int loop = 0;
	
	for(;;) {
		
		//printf("\033[2J");  //clear
		//printf("Loop %d\n", loop++);
		//if (loop == 8132 ) {
		//	loop = 0;
		//}
		WiringPinMonitor(ctx);
		usleep(1000000);
	}
	
	
	//usleep(3000000);
	MQTT_Disconnect(ctx, pinConf, NumPins);
	MQTT_destroy(&ctx);
	return 0;
}
