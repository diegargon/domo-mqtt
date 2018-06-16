/*
	by Diego Garcia Gonzalez (diegargon)
	
	Config file:
	[MQTT]
	BrokerAddress="ssl://xxx:8883"
	Username=xxx
	Password=xxxº
	ClientID=ExampleClient
	QOS=1


	Dependency: libglib2.0-dev

*/


#if !defined(CONFIG_H)
#define CONFIG_H

#define CONFIG_FILE "/etc/mqtt_client.conf"

#include <stdio.h>
#include <glib.h>


typedef struct
{
	gchar *MQTT_Broker_Addr;
	gchar *Username;
	gchar *Password;
	gchar *ClientID;
	int QOS;
} Config;

int read_config(Config *conf);
void print_config(Config *conf);

#endif
