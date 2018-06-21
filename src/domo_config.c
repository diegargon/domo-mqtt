/*

	Diego Garcia Gonzalez (diegargon)
	Dependency: libglib2.0-dev

*/

#include "domo_config.h"

void domoCfg_Init(GKeyFile *keyfile) {
	
	GKeyFileFlags flags;
	GError *error = NULL;
		
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file (keyfile, CONFIG_FILE, flags, &error))
	{
		log_msg(LOG_ERR, "Reading config file failed: \n");
		exit(EXIT_FAILURE);
	} else {
		log_msg(LOG_INFO, "Successful reading config file: \n");
	}

}

char* domoCfg_getString(GKeyFile* keyfile, char *section, char *key) {
	return g_key_file_get_string(keyfile, section, key, NULL);	
}

char** domoCfg_getStringList(GKeyFile *keyfile, const char *section, const char* key, gsize *length) {
	return g_key_file_get_string_list(keyfile, section, key, length, NULL);
}

int domoCfg_getInt(GKeyFile* keyfile, char* section, char* key) {
	int field = g_key_file_get_integer(keyfile, section, key, NULL);
	if (field) {
		return field;
	} else {
		return -1;
	}
}

