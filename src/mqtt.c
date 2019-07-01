/*

	Diego Garcia (diegargon) - diego@envigo.net
	paho.mqtt.c
	
*/

#include "mqtt.h"

int MQTT_Init(mqtt_context* ctx, GKeyFile *conf) {
	
	int rc = 0;
	//gsize SubsSize;
	log_msg(LOG_DEBUG, "MQTT Init called");
	
	ctx->MQTT_Broker_Addr = domoCfg_getString(conf, "MQTT", "BrokerAddress");
	ctx->ClientID = domoCfg_getString(conf, "MQTT","ClientID");
	ctx->Username = domoCfg_getString(conf, "MQTT","Username");
	ctx->Password = domoCfg_getString(conf, "MQTT","Password");
	ctx->DefaultQoS = domoCfg_getInt(conf, "MQTT","DefaultQoS");;	
	//ctx->Topic = domoCfg_getString(conf, "MQTT","Topic");
	
	ctx->Connected = 0;
	
	/*
	char **subs = domoCfg_getStringList(conf, "MQTT", "SubsTopics", &SubsSize);
	ctx->SubsSize = SubsSize;
	
	for (int i = 0; i < SubsSize; i++) {
		ctx->Subs[i] = strdup(subs[i]);
    }
	*/
	
	MQTT_printConfig(ctx);

   if( (rc = MQTTAsync_create(&ctx->client, ctx->MQTT_Broker_Addr, ctx->ClientID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) 
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

	mqtt_context *ctx = (mqtt_context *)context;

	ctx->Connected = -1;
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
    conn_opts.username = ctx->Username;
    conn_opts.password = ctx->Password;
    conn_opts.ssl = &ssl_opts;

	if ( MQTTAsync_connect(ctx->client, &conn_opts) != MQTTASYNC_SUCCESS) 
	{
		log_msg(LOG_ERR, "Error MQTT connect");
		exit(EXIT_FAILURE);
	}
	
    return MQTTASYNC_SUCCESS;
 
}


void MQTT_printConfig(mqtt_context *ctx)
{	
	log_msg(LOG_DEBUG, "MQTT Broker Addr: %s", ctx->MQTT_Broker_Addr);
	log_msg(LOG_DEBUG, "MQTT ClientID: %s", ctx->ClientID);
	log_msg(LOG_DEBUG, "MQTT Username: %s", ctx->Username);
	log_msg(LOG_DEBUG, "MQTT Password: %s", ctx->Password);
	log_msg(LOG_DEBUG, "MQTT Default QoS: %d", ctx->DefaultQoS);	
	//log_msg(LOG_DEBUG, "MQTT Topic: %s", ctx->Topic);
	//log_msg(LOG_DEBUG, "MQTT Retained: %d", ctx->retained);
	//log_msg(LOG_DEBUG, "MQTT msg: %s", ctx->msg);
	/*
	log_msg(LOG_DEBUG, "MQTT SubsSize %d",  ctx->SubsSize);
	for (int i = 0; i < ctx->SubsSize; i++) {
		log_msg(LOG_DEBUG, "MQTT Subs values %s", ctx->Subs[i]);
    }
	*/

}
void MQTT_onSend(void* context, MQTTAsync_successData* response)
{	
	log_msg(LOG_INFO,"(MQTT OnSend) Message with token value %d delivery confirmed", response->token);	
}

void MQTT_onConnectFailure(void* context, MQTTAsync_failureData* response)
{    
	mqtt_context *ctx = (mqtt_context *)context;
	
	!response->code ? response->code = 0 : response->code;
	ctx->Connected = 0;
	log_msg(LOG_WARNING,"MQTT OnConnectFailure  code: %d, message: %s)",  response->code, response->message);
}


void MQTT_onConnectSuccess(void* context, MQTTAsync_successData* response)
{
    mqtt_context *ctx = (mqtt_context *)context;
	
	log_msg(LOG_INFO, "MQTT successful conected (OnConnect)");
		
	ctx->Connected = 1;
	/*
    
	if (ctx->SubsSize > 1) {
		MQTT_subscribeMany(context); 
	} else if (ctx->SubsSize == 1) {		
		MQTT_subscribe(context);
	}
	*/

	}

void MQTT_subscribe(void* context, char* topic, int QoS )
{
	mqtt_context *ctx = (mqtt_context *)context;
	
    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
	ropts.onSuccess = MQTT_onSubscribeSuccess;
	ropts.onFailure = MQTT_onSubscribeFailure;
    ropts.context = ctx;
    log_msg(LOG_INFO, "Subscribing to topic %s for client %s using QoS %d", topic, ctx->ClientID, QoS);

    MQTTAsync_subscribe(ctx->client, topic, QoS, &ropts);
	
}

void MQTT_subscribeMany(void* context, char **topics, int count, int QoS)
{
	int i;	
	mqtt_context *ctx = (mqtt_context *)context;
	
    log_msg(LOG_INFO, "Subscribing many topics called");
	
    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;	
    ropts.context = ctx;	

	for (i=0 ; i < count; i++) {	
		MQTTAsync_subscribeMany(ctx->client, count, topics, &QoS, &ropts);
	}

}

void MQTT_onSubscribeSuccess(void *context, MQTTAsync_successData *response) {
	log_msg(LOG_INFO, "Subscribe success");
	//mqtt_context *ctx = (mqtt_context *)context;
}
void MQTT_onSubscribeFailure(void *context, MQTTAsync_failureData *response) {
	log_msg(LOG_INFO, "Subscribe fail [ %d ] -> %s ", response->code, response->message);
}

void MQTT_unsubscribe(mqtt_context *ctx, char* topic) {
	log_msg(LOG_DEBUG, "Unsubscribe topic %s called", topic);
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.context = ctx->client;	
	MQTTAsync_unsubscribe (ctx->client, topic, &opts);
}

void MQTT_unsubscribeMany(mqtt_context *ctx, int count, char **topics) {
	log_msg(LOG_DEBUG, "UnsubscribeMany called");
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.context = ctx->client;	
	MQTTAsync_unsubscribeMany (ctx->client, count, topics, &opts);	
}

int MQTT_msgArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	mqtt_context *ctx = (mqtt_context *)context;
    int i;
    char* payloadptr;
	
	log_msg(LOG_DEBUG, "Message arrived");	
		
		
/*
	for (i = 0; i < ctx->NumPins; i++) {
		log_msg(LOG_DEBUG,"PINCONF %d\n", ctx->pinConf[i].pin);
	}
	
	for (i = 0; i < ctx->NumPins; i++) {
		log_msg(LOG_DEBUG,"PINCONF %d, %s\n", i, ctx->pinConf[i].subs_topic);
	}	
*/
	for (i = 0; i < ctx->NumPins; i++) 
	{
		if ( ctx->pinConf[i].subs_topic != NULL && topicName != NULL) 
		{
			if ( (strcmp(topicName, ctx->pinConf[i].subs_topic) == 0) ) 
			{
				log_msg(LOG_DEBUG,"PINCONF, %d, %d",i , ctx->pinConf[i].pin);
				if ( (ctx->pinConf[i].logic != NULL)   
					&& (strcmp(ctx->pinConf[i].logic , "PushPin") == 0)
				)
				{
					log_msg(LOG_DEBUG,"PINCONF, %d, %d PushPin",i , ctx->pinConf[i].pin);
					int pinState = digitalRead(ctx->pinConf[i].pin);
					digitalWrite(ctx->pinConf[i].pin, !pinState);
					usleep(100000);
					digitalWrite(ctx->pinConf[i].pin, pinState);
				} else {
					log_msg(LOG_DEBUG,"PINCONF, %d, %d SwitchPin",i , ctx->pinConf[i].pin);
					int pinState = digitalRead(ctx->pinConf[i].pin);
					digitalWrite(ctx->pinConf[i].pin, !pinState);				
				}
			}
		}
	}

	payloadptr = message->payload;
	char *msg = malloc(message->payloadlen);
	for(i=0; i<message->payloadlen; i++) {
		msg[i] = *payloadptr++;
	}
		
	log_msg(LOG_DEBUG, "Message arrived - Topic: %s Message: %s", topicName, msg);
	free(msg);

	//MQTT_sendMsg(context, "test2", "hello", 0, 1);
	
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return TRUE;
}

void MQTT_connLost(void *context, char *cause)
{
	mqtt_context *ctx = (mqtt_context *)context;
	
    log_msg(LOG_WARNING,"\nConnection lost\t cause: %s", cause ? cause : "unknown");		
	ctx->Connected = 0;
	
	/* Mark as unsubcribre on connection lost */
	for (int i=0; i < ctx->NumPins ; i++) 
	{
		if ( (ctx->pinConf[i].pinMode == OUTPUT) && (ctx->pinConf[i].subs_topic != NULL) && (ctx->pinConf[i].subscribed == 1) )
		{			
			ctx->pinConf[i].subscribed = 0;
		}
	}
	
}

int MQTT_sendMsg(void* context, char* topic, char* msg, int retained, int qos) {	
    int rc;
	
	mqtt_context *ctx = (mqtt_context *)context;

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

void MQTT_disconnect_success(void *context, MQTTAsync_successData *response) {	
	mqtt_context *ctx = (mqtt_context *)context;
	
	log_msg(LOG_NOTICE, "MQTT Client discconect success");
	
	ctx->Connected = 0;
	//MQTT_destroy(&ctx);
}

void MQTT_disconnect_failure(void *context, MQTTAsync_failureData *response) {
	log_msg(LOG_WARNING, "MQTT Client discconect failure");
	//mqtt_context *ctx = (mqtt_context *)context;
	//MQTT_destroy(&ctx);
}

int MQTT_cancel_subs(mqtt_context* ctx, pinConfig *pinConf, int size) {	
	int i;

	log_msg(LOG_DEBUG, "MQTT cancel subscriptions called");
	
	for (i=0; i < size ; i++)  {
		if ( (pinConf[i].subscribed == 1) )
		{
			MQTT_unsubscribe(ctx, pinConf[i].subs_topic );
			pinConf[i].subscribed = 0; //TODO doit when successfull unsubscribe					
		}
	
	}

	return 0;
	
}
int MQTT_Disconnect(mqtt_context* ctx) {
	log_msg(LOG_DEBUG, "MQTT Disconnect called");
	
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

	opts.onSuccess = MQTT_disconnect_success;
	opts.onFailure = MQTT_disconnect_failure;
	opts.context = ctx;

    return MQTTAsync_disconnect(ctx->client, &opts);

}

void MQTT_destroy(mqtt_context **ptr) 
{
	log_msg(LOG_DEBUG, "MQTT destroy called");
	mqtt_context *ctx = (mqtt_context *)*ptr;
	
	if (ctx) 
	{
		MQTTAsync_destroy(&ctx->client);
		g_free(ctx->MQTT_Broker_Addr);

		g_free(ctx->Username);
		g_free(ctx->Password);
		g_free(ctx->ClientID);		
		//TODO: pinConf? how?
		//g_free(ctx->Topic);
		//g_free(ctx->msg);
		g_free(ctx);
		*ptr = NULL;
	}
	
}
