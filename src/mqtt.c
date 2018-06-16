/*
	by Diego Garcia (diegargon)
*/
#include "mqtt.h"

void MQTT_onSend(void* context, MQTTAsync_successData* response)
{
	printf("MQTT:OnSend\n");
	printf("Message with token value %d delivery confirmed\n", response->token);
}

void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("MQTT:OnConnectFailure\n");
}


void MQTT_onConnect(void* context, MQTTAsync_successData* response)
{

/*
    MQTTAsync client = (MQTTAsync)context;

    printf("MQTT:OnConnect\n");

    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
    ropts.context = client;
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n", "test_topic", conf->ClientID, conf->QOS);

    MQTTAsync_subscribe(client, "test_topic", DEF_QOS, &ropts);
*/
}

void MQTT_subscribe(void* contesxt, char* topic, char* conf)
{
	
}
int MQTT_msgArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
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

	MQTT_sendMsg(client, "test", 0, 1);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1;
}

void MQTT_connLost(void *context, char *cause)
{
	//MQTTAsync client = (MQTTAsync)context;
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int MQTT_sendMsg(void* context, char* msg, int retain, int qos) {

    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;

    printf("Sending message\n");

    opts.onSuccess = MQTT_onSend;
    opts.context = client;

    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    if ((rc = MQTTAsync_sendMessage(client, "other_topic", &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
	return 0;
}

void MQTT_disconnect(void *context) {
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

void MQTT_destroy(void *context) 
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_destroy(&client);
}


int MQTT_connect(void* context, Config* conf)
{
	int rc;

    //MQTTAsync client;
	MQTTAsync client = (MQTTAsync)context;

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;

    MQTTAsync_create(&client, conf->MQTT_Broker_Addr, conf->ClientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
	conn_opts.onSuccess = MQTT_onConnect;
	conn_opts.onFailure = MQTT_onConnectFailure;
	conn_opts.context = client;
    conn_opts.username = conf->Username;
    conn_opts.password = conf->Password;

    conn_opts.ssl = &ssl_opts;

	MQTTAsync_setCallbacks(client, client, MQTT_connLost, MQTT_msgArrived, NULL);
	
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }


    return 0;
}
