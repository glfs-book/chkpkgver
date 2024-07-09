#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "getpkgver.h"

int gargc;
char **gargv;

int main(int argc, char *argv[]) {
	gargc = argc;
	gargv = argv;
	
	if(argc < 2) {
		printf("Usage: <args> /path/to/packages.ent\n");
		printf("Invoke just --help for info\n");
		return 1;
	}
	if(strncmp(argv[1], "--help", 7) == 0) {
		printf("  --help\tdisplay this help and exit\n");
		return 0;
	}
	if (strlen(argv[argc - 1]) >= 100) {
		printf("Error: The string for packages.ent is over %d chars long\n",
				100);
		return 1;
	}
	const char *pkgent = argv[argc - 1];
	size_t pkgentlen = strlen(pkgent);
	size_t suffixlen = strlen("packages.ent");
	if(pkgentlen < suffixlen ||
	strcmp(pkgent + pkgentlen - suffixlen, "packages.ent") != 0) {
		printf("Usage: <args> /path/to/packages.ent\n");
		printf("Invoke just --help for info\n");
		return 1;
	}

	printf("Checking if %s is valid... ", argv[argc - 1]);
	FILE *testpackages_ent = fopen(argv[argc - 1], "r");
	if(!testpackages_ent) {
		printf("no, error opening file (does it exist?)\n");
		return 1;
	}
	char testpackages_entbuffer[1024];
	int found = 0;
	while(fgets(testpackages_entbuffer, 1024, testpackages_ent)) {
		testpackages_entbuffer[strcspn(testpackages_entbuffer, "\n")] = '\0';
		if (strstr(testpackages_entbuffer, "version") != NULL) {
			fclose(testpackages_ent);
			found = 1;
			break;
		}
	}
	if(found) {
		printf("yes\n");
	} else {
		printf("no, it's invalid\n");
		return 1;
	}

	printf("Checking if cURL is working... ");
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	CURLcode res;
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			printf("no, cannot read from pages\n\t%s\n",
					curl_easy_strerror(res));
			return 1;
		} else {
			printf("yes\n");
		}
	} else {
		printf("no, cURL failed to startup\n");
		return 1;
	}

	checkpackageversions();

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return 0;
}
