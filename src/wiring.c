/*

	Diego García Gonzalez (diegargon)
	
*/

#include "wiring.h"

void Wiring_Init(GKeyFile *conf) {
	int i;
	char **sections;
	gsize size = 0;
	
	sections = domoCfg_getStringList(conf, "BOARD_PINOUT", "Names", &size);
	
	printf("Size its %d \n", (int) size);
	for (i=0 ; i < size; i++) {
		printf("Section [%d] its %s\n", i, sections[i]);
	}
	
}