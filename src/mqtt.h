/*
	by Diego Garcia (diegargon)
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
#include "logger.h"
#include "wiring.h"

#define TIMEOUT     10000L

typedef struct mqtt_context {
	MQTTAsync client;
	char *MQTT_Broker_Addr;
	char *Username;
	char *Password;
	char *ClientID;
	int Connected;
	int DefaultQoS;
	int NumPins;
	pinConfig *pinConf;
	//char *Topic;
	//char *msg;

	//int retained;
} mqtt_context;

int MQTT_Init(mqtt_context* ctx, GKeyFile *keyfile);
void MQTT_printConfig(mqtt_context *ctx);
int MQTT_connect(void* context);
int MQTT_Disconnect(mqtt_context *ctx);

void MQTT_subscribe(void* context, char* topic, int QoS );
void MQTT_subscribeMany(void* context, char **topics, int count, int QoS);

void MQTT_onSubscribeSuccess(void *context, MQTTAsync_successData *response);
void MQTT_onSubscribeFailure(void *context, MQTTAsync_failureData *response);
void MQTT_unsubscribe(mqtt_context *ctx, char* topic);
void MQTT_unsubscribeMany(mqtt_context *ctx, int count, char **topics);
int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos);
void MQTT_destroy(mqtt_context **ctx);
void MQTT_connLost(void* context, char *cause);
int MQTT_msgArrived(void* context, char *topicName, int topicLen, MQTTAsync_message *message);
void MQTT_onConnectSuccess(void* context, MQTTAsync_successData* response);
void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response);
void MQTT_onSend(void* context, MQTTAsync_successData* response);

int MQTT_cancel_subs(mqtt_context* ctx, pinConfig *pinConf, int size);

#endif
