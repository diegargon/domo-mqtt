/*

	Diego Garcia (diegargon) - diego@envigo.net

*/
#if !defined(MQTT_H)
#define MQTT_H

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "MQTTAsync.h"
#include "domo_config.h"
#include "domo_misc.h"
#include "logger.h"
#include "wiring.h"

#define TIMEOUT     10000L

typedef struct MQTTContext {
	MQTTAsync client;
	/* void (*gateway)();	*/
	char *MQTT_broker_addr;
	char *username;
	char *password;
	char *clientID;
	int connected;
	int defaultQOS;
	int numPins;
	
	PinConfig *pinConf;

} MQTTContext;

int MQTT_Init(MQTTContext* ctx, GKeyFile *keyfile);
void MQTT_printConfig(MQTTContext *ctx);
int MQTT_connect(void* context);
int MQTT_Disconnect(MQTTContext *ctx);
void MQTT_subscribe(void* context, char* topic, int QoS );
void MQTT_subscribeMany(void* context, char **topics, int count, int QoS);
void MQTT_onSubscribeSuccess(void *context, MQTTAsync_successData *response);
void MQTT_onSubscribeFailure(void *context, MQTTAsync_failureData *response);
void MQTT_unsubscribe(MQTTContext *ctx, char* topic);
void MQTT_unsubscribeMany(MQTTContext *ctx, int count, char **topics);
int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos);
void MQTT_destroy(MQTTContext **ctx);
void MQTT_connLost(void* context, char *cause);
int MQTT_msgArrived(void* context, char *topicName, int topicLen, MQTTAsync_message *message);
void MQTT_onConnectSuccess(void* context, MQTTAsync_successData* response);
void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response);
void MQTT_onSend(void* context, MQTTAsync_successData* response);
void MQTT_delivery_complete(void *context, MQTTAsync_token token);
int  MQTT_cancel_subs(MQTTContext* ctx, PinConfig *pinConf, int size);
void MQTT_disconnect_success(void *context, MQTTAsync_successData *response);
void MQTT_disconnect_failure(void *context, MQTTAsync_failureData *response);

#endif
