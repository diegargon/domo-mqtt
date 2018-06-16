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
#include "config.h"

#define ADDRESS     "ssl://mqtt.envigo.net:8883"
//#define ADDRESS     "tcp://localhost:1883"
#define TIMEOUT     10000L


typedef struct {
	char* msg;
	char* topic;
	int retained;
} mttqMessge;

int MQTT_connect(void* context, Config* conf);
void MQTT_destroy(void* context);
int MQTT_sendMsg(void* context, char* msg, int retain, int qos);
void MQTT_disconnect(void* context);
void MQTT_connLost(void* context, char *cause);
int MQTT_msgArrived(void* context, char *topicName, int topicLen, MQTTAsync_message *message);
void MQTT_onConnect(void* context, MQTTAsync_successData* response);
void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response);
void MQTT_onSend(void* context, MQTTAsync_successData* response);
void MQTT_subscribe(void* contesxt, char* topic, char* conf);

#endif
