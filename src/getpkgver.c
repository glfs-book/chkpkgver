#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "getpkgver.h"

void checkpackageversions(void) {
	printf("Detecting NSS version...\n");
	FILE *packages_ent = fopen(gargv[gargc - 1], "r");
	char packages_entbuffer[1024];
	while(fgets(packages_entbuffer, 1024, packages_ent)) {
		packages_entbuffer[strcspn(packages_entbuffer, "\n")] = '\0';
		if(strstr(packages_entbuffer, "nss") != NULL) {
			printf("%s\n", packages_entbuffer);
		}
	}
	fclose(packages_ent);
}
