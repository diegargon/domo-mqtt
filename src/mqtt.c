/*

	Diego Garcia (diegargon) - diego@envigo.net
	paho.mqtt.c
	
*/

#include "mqtt.h"

int MQTT_Init(MQTTContext* ctx, GKeyFile *conf) {
	
	int rc = 0;
	//gsize SubsSize;
	log_msg(LOG_DEBUG, "MQTT Init called");
	
	ctx->MQTT_broker_addr = domoCfg_getString(conf, "MQTT", "BrokerAddress");
	ctx->clientID = domoCfg_getString(conf, "MQTT","ClientID");
	ctx->username = domoCfg_getString(conf, "MQTT","Username");
	ctx->password = domoCfg_getString(conf, "MQTT","Password");
	ctx->defaultQOS = domoCfg_getInt(conf, "MQTT","DefaultQoS");;	
	//ctx->Topic = domoCfg_getString(conf, "MQTT","Topic");
	
	ctx->connected = 0;
	
	/*
	char **subs = domoCfg_getStringList(conf, "MQTT", "SubsTopics", &SubsSize);
	ctx->SubsSize = SubsSize;
	
	for (int i = 0; i < SubsSize; i++) {
		ctx->Subs[i] = strdup(subs[i]);
    }
	*/
	
	MQTT_printConfig(ctx);

   if( (rc = MQTTAsync_create(&ctx->client, ctx->MQTT_broker_addr, ctx->clientID, MQTTCLIENT_PERSISTENCE_NONE, MQTT_delivery_complete)) != MQTTASYNC_SUCCESS) 
	{	
		log_msg(LOG_ERR, "Error MQTT_CREATE %d", rc);
		exit(EXIT_FAILURE);
	} else {
		log_msg(LOG_DEBUG, "Successful MQTT_CREATE");
	}

	if( (rc = MQTTAsync_setCallbacks(ctx->client, ctx, MQTT_connLost, MQTT_msgArrived, NULL))  != MQTTASYNC_SUCCESS) 
	{
		log_msg(LOG_ERR, "Error settting callbacks %d", rc);
		exit(EXIT_FAILURE);
	} else {
		log_msg(LOG_DEBUG, "Successful setting MQTT callbacks");
	}
	
	return 0;
}


int MQTT_connect(void* context) {

	MQTTContext *ctx = (MQTTContext *)context;

	ctx->connected = -1;
	log_msg(LOG_DEBUG, "MQTT connect called");
	
    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 0;
		
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
	//conn_opts.automaticReconnect = 1;  // problems
	//conn_opts.connectTimeout = 3;
	conn_opts.minRetryInterval = 4;
	conn_opts.onSuccess = MQTT_onConnectSuccess;
	conn_opts.onFailure = MQTT_onConnectFailure;
	conn_opts.context = ctx;
    conn_opts.username = ctx->username;
    conn_opts.password = ctx->password;
    conn_opts.ssl = &ssl_opts;

	if ( MQTTAsync_connect(ctx->client, &conn_opts) != MQTTASYNC_SUCCESS) 
	{
		log_msg(LOG_ERR, "Error MQTT connect");
		exit(EXIT_FAILURE);
	}
	
    return MQTTASYNC_SUCCESS;
 
}


void MQTT_printConfig(MQTTContext *ctx)
{	
	log_msg(LOG_DEBUG, "MQTT Broker Addr: %s", ctx->MQTT_broker_addr);
	log_msg(LOG_DEBUG, "MQTT clientID: %s", ctx->clientID);
	log_msg(LOG_DEBUG, "MQTT username: %s", ctx->username);
	log_msg(LOG_DEBUG, "MQTT password: %s", ctx->password);
	log_msg(LOG_DEBUG, "MQTT Default QoS: %d", ctx->defaultQOS);	
}
void MQTT_onSend(void* context, MQTTAsync_successData* response)
{	
	log_msg(LOG_INFO,"(MQTT OnSend) Message with token value %d delivery confirmed", response->token);	
}

void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response)
{    
	MQTTContext *ctx = (MQTTContext *)context;
	
	!response->code ? response->code = 0 : response->code;
	ctx->connected = 0;
	log_msg(LOG_WARNING,"MQTT OnConnectFailure  code: %d, message: %s)",  response->code, response->message);
}


void MQTT_onConnectSuccess(void* context, MQTTAsync_successData* response)
{
    MQTTContext *ctx = (MQTTContext *)context;
	
	log_msg(LOG_INFO, "MQTT successful conected (OnConnect)");
		
	ctx->connected = 1;
}

void MQTT_subscribe(void* context, char* topic, int QoS )
{
	MQTTContext *ctx = (MQTTContext *)context;
		
    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
	ropts.onSuccess = MQTT_onSubscribeSuccess;
	ropts.onFailure = MQTT_onSubscribeFailure;
    ropts.context = ctx;
    log_msg(LOG_INFO, "Subscribing to topic %s for client %s using QoS %d", topic, ctx->clientID, QoS);

    MQTTAsync_subscribe(ctx->client, topic, QoS, &ropts);
	
}

void MQTT_subscribeMany(void* context, char **topics, int count, int QoS)
{
	int i;	
	MQTTContext *ctx = (MQTTContext *)context;
	
    log_msg(LOG_INFO, "Subscribing many topics called");
	
    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;	
    ropts.context = ctx;	

	for (i=0 ; i < count; i++) {	
		MQTTAsync_subscribeMany(ctx->client, count, topics, &QoS, &ropts);
	}

}

void MQTT_onSubscribeSuccess(void *context, MQTTAsync_successData *response) {
	//char *topic = (char*) context;
	log_msg(LOG_INFO, "Subscribe success");	
	//log_msg(LOG_INFO, "Subscribe success: %d", response->alt.qos);
	//MQTTContext *ctx = (MQTTContext *)context;
	
}
void MQTT_onSubscribeFailure(void *context, MQTTAsync_failureData *response) {
	log_msg(LOG_INFO, "Subscribe fail [ %d ] -> %s ", response->code, response->message);
}

void MQTT_unsubscribe(MQTTContext *ctx, char* topic) {
	log_msg(LOG_DEBUG, "Unsubscribe topic %s called", topic);
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.context = ctx->client;	
	MQTTAsync_unsubscribe (ctx->client, topic, &opts);
}

void MQTT_unsubscribeMany(MQTTContext *ctx, int count, char **topics) {
	log_msg(LOG_DEBUG, "UnsubscribeMany called");
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.context = ctx->client;	
	MQTTAsync_unsubscribeMany (ctx->client, count, topics, &opts);	
}

void MQTT_delivery_complete(void *context, MQTTAsync_token token) {
	log_msg(LOG_DEBUG, "MQTT delivery complete");
}

int MQTT_msgArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	MQTTContext *ctx = (MQTTContext *)context;
    char* payloadptr;
	
	payloadptr = message->payload;
	char *msg = malloc(message->payloadlen);
	for(int i=0; i<message->payloadlen; i++) {
		msg[i] = *payloadptr++;
	}
		
	log_msg(LOG_DEBUG, "Message arrived - Topic: %s Message: %s", topicName, msg);
	free(msg);

	domo_subs_msg(topicName, ctx->pinConf, ctx->numPins);

	//MQTT_sendMsg(context, "test2", "hello", 0, 1);
	
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return TRUE;
}

void MQTT_connLost(void *context, char *cause)
{
	MQTTContext *ctx = (MQTTContext *)context;
	
    log_msg(LOG_WARNING,"\nConnection lost\t cause: %s", cause ? cause : "unknown");		
	ctx->connected = 0;
	
	/* Mark as unsubcribre on connection lost */
	for (int i=0; i < ctx->numPins ; i++) 
	{
		if ( (ctx->pinConf[i].pinMode == OUTPUT) && (ctx->pinConf[i].subs_topic != NULL) && (ctx->pinConf[i].subscribed == 1) )
		{			
			ctx->pinConf[i].subscribed = 0;
		}
	}
	
}

int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos) {	
    int rc;
	
	MQTTContext *ctx = (MQTTContext *)context;

	log_msg(LOG_DEBUG, "Sending message - topic: %s, msg: %s, retained: %d, qos: %d", topic, msg, retained, qos);	
	
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = MQTT_onSend;
    opts.context = context;
	
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = qos;
    pubmsg.retained = retained;
	
    if ((rc = MQTTAsync_sendMessage(ctx->client, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        log_msg(LOG_ERR, "Failed to start sendMessage, return code %d", rc);
        exit(EXIT_FAILURE);
    }
	return 0;
}


int MQTT_Disconnect(MQTTContext* ctx) {
	log_msg(LOG_DEBUG, "MQTT Disconnect called");
	
	MQTT_cancel_subs(ctx, ctx->pinConf, ctx->numPins);
	
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

	opts.onSuccess = MQTT_disconnect_success;
	opts.onFailure = MQTT_disconnect_failure;
	opts.context = ctx;

    return MQTTAsync_disconnect(ctx->client, &opts);

}

void MQTT_disconnect_success(void *context, MQTTAsync_successData *response) {	
	MQTTContext *ctx = (MQTTContext *)context;
	
	log_msg(LOG_NOTICE, "MQTT Client discconect success");

	ctx->connected = 0;
	MQTT_destroy(&ctx);
}

void MQTT_disconnect_failure(void *context, MQTTAsync_failureData *response) {
	log_msg(LOG_WARNING, "MQTT Client discconect failure");
	MQTTContext *ctx = (MQTTContext *)context;
	MQTT_destroy(&ctx);
	exit(EXIT_FAILURE);
}

void MQTT_destroy(MQTTContext **ptr) 
{
	log_msg(LOG_DEBUG, "MQTT destroy called");
	MQTTContext *ctx = (MQTTContext *)*ptr;
	
	if (ctx) 
	{
		MQTTAsync_destroy(&ctx->client);
		g_free(ctx->MQTT_broker_addr);

		g_free(ctx->username);
		g_free(ctx->password);
		g_free(ctx->clientID);		
		//TODO: pinConf?

		g_free(ctx);
		*ptr = NULL;
	}
	
}

int MQTT_cancel_subs(MQTTContext* ctx, PinConfig *pinConf, int size) {	
	int i;

	log_msg(LOG_DEBUG, "MQTT cancel subscriptions called");
	
	for (i=0; i < size ; i++)  {
		if ( (pinConf[i].subscribed == 1) )
		{
			MQTT_unsubscribe(ctx, pinConf[i].subs_topic );
			pinConf[i].subscribed = 0; //TODO doit when successfull unsubscribe?					
		}
	
	}

	return 0;
	
}