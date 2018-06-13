/*
	by Diego Garcia Gonzalez (diegargon)

	Read config file test 
	Config syntax:
	[General]
	Username: XXX
	Password: XXX
	
	Dependency: libglib2.0-dev
	
	Compile:
	gcc $(pkg-config --cflags --libs glib-2.0) file.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#define CONFIG_FILE "/etc/mqtt_client.conf"

typedef struct
{
	gchar *Username;
	gchar *Password;
} Config;

int show_file_config()
{

 GKeyFile *keyfile;
 GKeyFileFlags flags;
 GError *error = NULL;

 gchar *username, *password;

 keyfile = g_key_file_new ();
 flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

 if (!g_key_file_load_from_file (keyfile, CONFIG_FILE, flags, &error))
 {
   g_error (error->message);
   return -1;
 } 
 username=g_key_file_get_string(keyfile,"General","Username",NULL);
 password=g_key_file_get_string(keyfile,"General","Password",NULL);
 printf("Username:%s\n", username);
 printf("Password:%s\n", password);
 
 return 0;
 
}
 
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
	conf->Username = g_key_file_get_string(keyfile,"General","Username",NULL);
	conf->Password= g_key_file_get_string(keyfile,"General","Password",NULL);	
		
	return 0;
}
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
 
