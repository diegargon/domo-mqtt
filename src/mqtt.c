/*
	by Diego Garcia (diegargon)
*/
#include "domo_config.h"
#include "mqtt.h"

int MQTT_Init(mqtt_context* ctx) {
	int rc = 0;
	
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	
	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file (keyfile, CONFIG_FILE, flags, &error))
	{
		printf("Reading config file failed: \n");
		exit(EXIT_FAILURE);
	} else {
		printf("Successful reading config file: \n");
	}
	

	ctx->MQTT_Broker_Addr = domoCfg_getString(keyfile, "MQTT", "BrokerAddress");
	ctx->ClientID = domoCfg_getString(keyfile, "MQTT","ClientID");
	ctx->Username = domoCfg_getString(keyfile, "MQTT","Username");
	ctx->Password = domoCfg_getString(keyfile, "MQTT","Password");
	ctx->Topic = domoCfg_getString(keyfile, "MQTT","Topic");
	ctx->Qos = domoCfg_getInt(keyfile, "MQTT","Qos");;	

	MQTT_printConfig(ctx);

   if( (rc = MQTTAsync_create(&ctx->client, ctx->MQTT_Broker_Addr, ctx->ClientID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) {	
		printf("Error MQTT_CREATE %d \n", rc);
		exit(EXIT_FAILURE);
	} else {
		printf("Successful MQTT_CREATE\n");
	}

	if( (rc = MQTTAsync_setCallbacks(ctx->client, ctx, MQTT_connLost, MQTT_msgArrived, NULL))  != MQTTASYNC_SUCCESS) {
		printf("Error settting callbacks %d\n", rc);
		exit(EXIT_FAILURE);
	} else {
		printf("Successful setting MQTT callbacks\n");
	}
	return MQTTASYNC_SUCCESS;
}

void MQTT_printConfig(mqtt_context *ctx)
{
	printf("MQTT Broker Addr: %s\n", ctx->MQTT_Broker_Addr);
	printf("MQTT ClientID: %s\n", ctx->ClientID);
	printf("MQTT Username: %s\n", ctx->Username);
	printf("MQTT Password: %s\n", ctx->Password);
	printf("MQTT Qos: %d\n", ctx->Qos);	
	printf("MQTT Topic: %s\n", ctx->Topic);
	printf("MQTT Retained: %d\n", ctx->retained);
	printf("MQTT msg: %s\n", ctx->msg);

}
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
    //mqtt_context *ctx = (mqtt_context *)context;

    printf("MQTT:OnConnect\n");
	MQTT_subscribe(context); 
}

void MQTT_subscribe(void* context)
{
	mqtt_context *ctx = (mqtt_context *)context;
	
    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
    ropts.context = ctx;
    printf("Subscribing to topic %s\nfor client %s using QoS %d\n\n", ctx->Topic, ctx->ClientID, ctx->Qos);

    MQTTAsync_subscribe(ctx->client, ctx->Topic, ctx->Qos, &ropts);
	
}
int MQTT_msgArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
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

	MQTT_sendMsg(context, "test2", "hello", 0, 1);
	
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return TRUE;

}

void MQTT_connLost(void *context, char *cause)
{
	//MQTTAsync client = (MQTTAsync)context;
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos) {

    int rc;
	
	mqtt_context *ctx = (mqtt_context *)context;
		
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = MQTT_onSend;
    opts.context = context;
	
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = qos;
    pubmsg.retained = retained;
	
	    printf("Sending message\n");

    if ((rc = MQTTAsync_sendMessage(ctx->client, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
	return 0;
}

void MQTT_disconnect_success(void *context, MQTTAsync_successData *response) {
	printf("MQTT Client discconect success\n");
	//mqtt_context *ctx = (mqtt_context *)context;
	//MQTT_destroy(&ctx);
}

void MQTT_disconnect_failure(void *context, MQTTAsync_failureData *response) {
	printf("MQTT Client discconect failure\n");
	//mqtt_context *ctx = (mqtt_context *)context;
	//MQTT_destroy(&ctx);
}

int MQTT_Disconnect(mqtt_context* ctx) {
	printf("MQTT discconect called\n");
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

	opts.onSuccess = MQTT_disconnect_success;
	opts.onFailure = MQTT_disconnect_failure;
	opts.context = ctx;

    return MQTTAsync_disconnect(ctx->client, &opts);

}

void MQTT_destroy(mqtt_context **ptr) 
{
	printf("MQTT destroy called\n");
	mqtt_context *ctx = (mqtt_context *)*ptr;
	
	if(ctx) {
		MQTTAsync_destroy(&ctx->client);
		g_free(ctx->MQTT_Broker_Addr);

		g_free(ctx->Username);
		g_free(ctx->Password);
		g_free(ctx->ClientID);
		g_free(ctx->Topic);
		g_free(ctx->msg);
		g_free(ctx);
		*ptr = NULL;
	}
	
}

int MQTT_connect(void* context) {

	mqtt_context *ctx = (mqtt_context *)context;
	
    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;
		
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
	conn_opts.onSuccess = MQTT_onConnect;
	conn_opts.onFailure = MQTT_onConnectFailure;
	conn_opts.context = ctx;
    conn_opts.username = ctx->Username;
    conn_opts.password = ctx->Password;
    conn_opts.ssl = &ssl_opts;

	if( MQTTAsync_connect(ctx->client, &conn_opts) != MQTTASYNC_SUCCESS) {
		printf("Error MQTT connect\n");
		exit(EXIT_FAILURE);
	}
	
    return MQTTASYNC_SUCCESS;
 
}

