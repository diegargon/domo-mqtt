/*
    by Diego Garcia Gonzalez (diegargon)
	
	pass context test
	
	Details:
    * SSL connection (mosquitto port 8883)
    * Use Username + Password 
	* Async
    * Send and Received test (receive from one topic reply to other topic
	* compiling:  gcc -o a.out file.c -lpaho-mqtt3as  `pkg-config --cflags --libs glib-2.0`
*/

#include "MQTTAsync.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define TIMEOUT     10000L

typedef struct mqtt_context{
	MQTTAsync client;
	char *MQTT_Broker_Addr;
	char *Username;
	char *Password;
	char *ClientID;
	char *Payload;
	int Qos;
} mqtt_context;


int sendMsg(void* context);


volatile MQTTAsync_token deliveredtoken;

void onSend(void* context, MQTTAsync_successData* response)
{
	printf("MQTT:OnSend\n");
	printf("Message with token value %d delivery confirmed\n", response->token);
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("MQTT:OnConnectFailure\n");
}


void onConnect(void* context, MQTTAsync_successData* response)
{

	mqtt_context *ctx = (mqtt_context *)context;
    printf("MQTT:OnConnect\n");

    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
    ropts.context = ctx;
    printf("Subscribing to topic %s\nfor client %s using QoS %d\n\n", "test_topic", ctx->ClientID, ctx->Qos);

    MQTTAsync_subscribe(ctx->client, "test", ctx->Qos, &ropts);

}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	//mqtt_context *ctx = (mqtt_context *)context;
	
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');

	//sendMsg(ctx);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return TRUE;
	
}

	
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

void Destroy(mqtt_context **ptr) {
	mqtt_context *ctx = (mqtt_context *)*ptr;
	MQTTAsync_destroy(&ctx->client);
	//g_free(ctx->
	//...
	g_free(ctx);
	*ptr = NULL;
}



int sendMsg(void* context) {
    
	mqtt_context *ctx = (mqtt_context *)context;
	
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;

    printf("Successful connection\n");

    opts.onSuccess = onSend;
    opts.context = ctx;

    pubmsg.payload = ctx->Payload;
    pubmsg.payloadlen = strlen(ctx->Payload);
    pubmsg.qos = ctx->Qos;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((rc = MQTTAsync_sendMessage(ctx->client, "other_topic", &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
	return 0;

}

void Disconnect(void * context) {
	
	mqtt_context *ctx = (mqtt_context *)context;
	
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    opts.context = ctx;

    if ((rc = MQTTAsync_disconnect(ctx->client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
	
}

int main(int argc, char* argv[])
{
	int rc = 0;
	int ch;    

	mqtt_context *ctx = g_malloc0(sizeof(struct mqtt_context));

	ctx->MQTT_Broker_Addr = g_strdup ("xxxx");

	ctx->ClientID = "ExampleClientSub";
	ctx->Username = "xxxx";
	ctx->Password = "xxxx";
	ctx->Qos = 1;


    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;

    if( (rc = MQTTAsync_create(&ctx->client, ctx->MQTT_Broker_Addr, ctx->ClientID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) {	
		printf("Error MTQQ_CREATE %d \n", rc);
		exit(EXIT_FAILURE);
	}

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = ctx;
    conn_opts.username = ctx->Username;
    conn_opts.password = ctx->Password;

    conn_opts.ssl = &ssl_opts;

	if( (rc = MQTTAsync_setCallbacks(ctx->client, ctx, connlost, msgarrvd, NULL))  != MQTTASYNC_SUCCESS) {
		printf("Error settting callbacks %d\n", rc);
		exit(EXIT_FAILURE);
	}
	
	
    if ((rc = MQTTAsync_connect(ctx->client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    printf("Press Q<Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));

	Disconnect(ctx);
	Destroy(&ctx);
    printf("Leaving succesfull\n");
	
	
    return rc;
	
}
