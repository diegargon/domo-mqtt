/*
	by Diego Garcia Gonzalez (diegargon)

	Dependency: libglib2.0-dev

*/

#include "domo_config.h"

char* domoCfg_getString(GKeyFile* keyfile, char *section, char *key) {
	return g_key_file_get_string(keyfile, section, key, NULL);	
}

int domoCfg_getInt(GKeyFile* keyfile, char* section, char* key) {
	return g_key_file_get_integer(keyfile, section, key, NULL);
}

