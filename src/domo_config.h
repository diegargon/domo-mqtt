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


#if !defined(DOMO_CONFIG_H)
#define DOMO_CONFIG_H

#define CONFIG_FILE "/etc/domo.conf"

#include <stdio.h>
#include <glib.h>

char* domoCfg_getString(GKeyFile* keyfile, char *section, char *key);
int domoCfg_getInt(GKeyFile* keyfille, char* section, char* key);

#endif
