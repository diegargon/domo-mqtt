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

#define TIMEOUT     10000L

typedef struct mqtt_context{
	MQTTAsync client;
	char *MQTT_Broker_Addr;
	char *Username;
	char *Password;
	char *ClientID;
	char *Topic;
	char *msg;
	int Qos;
	int retained;	
} mqtt_context;

int MQTT_Init(mqtt_context* ctx, GKeyFile *keyfile);
void MQTT_printConfig(mqtt_context *ctx);
int MQTT_connect(void* context);
int MQTT_Disconnect(mqtt_context *ctx);
void MQTT_subscribe(void* context);
int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos);
void MQTT_destroy(mqtt_context **ctx);
void MQTT_connLost(void* context, char *cause);
int MQTT_msgArrived(void* context, char *topicName, int topicLen, MQTTAsync_message *message);
void MQTT_onConnect(void* context, MQTTAsync_successData* response);
void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response);
void MQTT_onSend(void* context, MQTTAsync_successData* response);

#endif
