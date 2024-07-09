#include "getpkginfo.h"

int main() {
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
			fprintf(stderr, "no, cannot read from pages\n\t%s\n",
					curl_easy_strerror(res));
		} else {
			printf("yes\n");
		}
	} else {
		fprintf(stderr, "no, cURL failed to startup\n");
	}

	checkpackageinfo();

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return 0;
}
