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
		printf("Reading config file failed \n");
		exit(EXIT_FAILURE);
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
	} 
	
	return -1;
}
gsize domCfg_getNumElementsList( GKeyFile* keyfile, char* section, char* key ) {
	gsize size = 0;

	g_key_file_get_string_list(keyfile, section, key, &size, NULL);

	return size;
}


int domoCfg_hasGroup(GKeyFile* keyfile, char* group) {
	if (g_key_file_has_group (keyfile, group)) {
		return 1;
	} 
	
	return 0;
}
