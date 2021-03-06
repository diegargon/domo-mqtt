/*

	Diego Garcia Gonzalez (diegargon)
	
	Config file:
	[MQTT]
	BrokerAddress="ssl://xxx:8883"
	username=xxx
	password=xxxº
	clientID=ExampleClient
	QOS=1


	Dependency: libglib2.0-dev

*/

#if !defined(DOMO_CONFIG_H)
#define DOMO_CONFIG_H

#define CONFIG_FILE "/etc/domo.conf"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

void domoCfg_Init(GKeyFile* keyfile);
char* domoCfg_getString(GKeyFile* keyfile, char *section, char *key);
int domoCfg_getInt(GKeyFile* keyfille, char* section, char* key);
char** domoCfg_getStringList(GKeyFile *keyfile, const char *section, const char* key, gsize *size);
gsize domCfg_getNumElementsList( GKeyFile* keyfile, char* section, char* key );
int domoCfg_hasGroup(GKeyFile *keyfile, char* group);

#endif
