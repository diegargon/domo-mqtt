/*
	by Diego Garcia Gonzalez (diegargon)

	Dependency: libglib2.0-dev

*/

#include "config.h"

int read_config(Config *conf) 
{

	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	
	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file (keyfile, CONFIG_FILE, flags, &error))
	{
		return -1;
	} 
	conf->Username = g_key_file_get_string(keyfile,"MQTT","Username",NULL);
	conf->Password = g_key_file_get_string(keyfile,"MQTT","Password",NULL);
	conf->MQTT_Broker_Addr = g_key_file_get_string(keyfile,"MQTT","MQTT_Broker_Addr",NULL);
	conf->QOS =  g_key_file_get_integer(keyfile,"MQTT","QOS",NULL);
	conf->ClientID = g_key_file_get_string(keyfile,"MQTT","ClientID",NULL);	
		
	return 0;
}

void print_config(Config *conf)
{
	printf("Username: %s\n", conf->Username);
	printf("Password: %s\n", conf->Password);
	printf("ClientID %s\n", conf->ClientID);
	printf("MQTT Broker Address %s\n", conf->MQTT_Broker_Addr);
	printf("QOS: %i\n", conf->QOS);		
}

/*
int main (int argc, char** argv)
{
	Config *conf;
	conf = g_slice_new(Config);
	int rc = read_config(conf);
	if (rc == 0 ) {
		printf("Username: %s\n", conf->Username);
		printf("Password: %s\n", conf->Password);
	} else {
		printf("Configuration error\n");
	}
	
	//show_file_config();
	
	return 0;	
 }
 
*/
