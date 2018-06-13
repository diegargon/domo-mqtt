/*
    by Diego Garcia Gonzalez (diegargon)
	
	Details:
    * SSL connection (mosquitto port 8883)
    * Use Username + Password 
	* Async
    * Send and Received test (receive from one topic reply to other topic
	* compiling:  gcc -o a.out file.c lpaho-mqtt3as  
*/

#include "MQTTAsync.h"
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>


#define ADDRESS     "ssl://mqtt.envigo.net:8883"
//#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientSub"
#define PAYLOAD     "Hello"
#define QOS         1
#define TIMEOUT     10000L
#define USERNAME ""
#define PASSWORD ""

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
    MQTTAsync client = (MQTTAsync)context;

    printf("MQTT:OnConnect\n");

    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
    ropts.context = client;
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n", "test_topic", CLIENTID, QOS);

    MQTTAsync_subscribe(client, "test_topic", QOS, &ropts);

}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	MQTTAsync client = (MQTTAsync)context;
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

	sendMsg(client);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int sendMsg(void* context) {

    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;

    printf("Successful connection\n");

    opts.onSuccess = onSend;
    opts.context = client;

    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((rc = MQTTAsync_sendMessage(client, "other_topic", &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

}

void Disconnect(void * context) {
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    opts.context = client;

    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
	int rc;
	int ch;
    MQTTAsync client;


    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;

    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    conn_opts.ssl = &ssl_opts;

	MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL);
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Press Q<Enter> to quit\n\n");
    do
    {
        ch = getchar();
    } while( (ch!='Q' && ch != 'q'));

	Disconnect(&client);
    MQTTAsync_destroy(&client);
    return rc;
}
