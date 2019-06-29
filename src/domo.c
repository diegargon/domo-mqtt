/*

	Diego Garcia (diegargon)

*/


#include "domo.h"

int main() {
	
	int reconnect_time = RECONNECT_TIME;
	
	//unsigned long int SubsNcount = 0;
	GKeyFile *conf = g_key_file_new ();
	
	/* Signal */
	//signal(SIGUSR1, sig_handler);
	//signal(SIGINT, sig_handler);
	/**/
	
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

	while (!MQTTAsync_isConnected(ctx->client)) 
	{
		log_msg(LOG_DEBUG,"MQTT_connect:Waiting to connected.\n");
		sleep(reconnect_time);
		if(ctx->Connected == 0) {
			MQTT_connect(ctx);
			if (reconnect_time < 5) {
				reconnect_time ++;
			}
		}		
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
	
	while(1) {
		
		//printf("\033[2J");  //clear
		//printf("Loop %d\n", loop++);
		//if (loop == 8132 ) {
		//	loop = 0;
		//}
		WiringPinMonitor(ctx);
		if(ctx->Connected == 0) {
			sleep(reconnect_time);
			if (!MQTTAsync_isConnected(ctx->client)) {
				MQTT_connect(ctx);
				if (reconnect_time < 5) {
					reconnect_time ++;
				}
			}		
		} else {
			reconnect_time = RECONNECT_TIME;
		}
		usleep(20 * 10000);
	}

	MQTT_cancel_subs(ctx, pinConf, NumPins);
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	
	return 0;
}

/*
void sig_handler(int signo) {
	log_msg(LOG_DEBUG,"Signal Hadnler called\n");
	
	MQTT_cancel_subs(ctx, pinConf, NumPins);
	MQTT_Disconnect(ctx);
	MQTT_destroy(&ctx);
	
	exit(1);
}
*/

