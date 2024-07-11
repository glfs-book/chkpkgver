#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <curl/curl.h>
#include "getpkgver.h"

#define BUFFERLEN 1024

typedef struct {
	char name[100];
	char value[100];
} Entity;

// fuck indentation
void expand_entities(Entity *entities, int entity_count) {
	for(int i = 0; i < entity_count; i++) {
		for(int j = 0; j < entity_count; j++) {
		char *pos = strstr(entities[i].value, entities[j].name);
		if(pos) {
		char expanded_value[100];
		snprintf(expanded_value, 100, "%.*s%s%s",
		(int)(pos - entities[i].value), entities[i].value,
		entities[j].value, pos + strlen(entities[j].name));
		strcpy(entities[i].value, expanded_value);
		} // if(pos) {
		} // for(int j = 0; j < entity_count; j++) {
	}
}

// again, fuck it
int parse_packages_ent(const char *filepath, Entity *entities, int max_entities) {
	FILE *file = fopen(filepath, "r");
	char line[BUFFERLEN];
	int entity_count = 0;
	while(fgets(line, sizeof(line), file) && entity_count < max_entities) {
	if(sscanf(line, "<!ENTITY %99s %99[^\n]>",
	entities[entity_count].name, entities[entity_count].value) == 2) {
	size_t len = strlen(entities[entity_count].value);
	if(entities[entity_count].value[0] == '"' &&
	entities[entity_count].value[len - 1] == '"') {
	entities[entity_count].value[len - 1] = '\0';
	memmove(entities[entity_count].value, entities[entity_count].value + 1, len - 1);
	}
	entity_count++;
	} }
	fclose(file);
	expand_entities(entities, entity_count);
	return entity_count;
}

void clean_entity(char *entity) {
	char clean_entity_char[100] = {0};
	char *src = entity;
	char *dst = clean_entity_char;
	while(*src) {
		if(isdigit(*src) || *src == '.' || *src == '_') {
			*dst++ = *src;
		}
		src++;
	}
	strcpy(entity, clean_entity_char);
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t total_size = size * nmemb;
	strncat((char *)stream, (char *)ptr, total_size);
	return total_size;
}

int compare_versions(const char *v1, const char *v2) {
	int num1 = 0, num2 = 0;
	while (*v1 || *v2) {
		if (*v1 == '.' || *v2 == '.') {
			if (*v1 == '.') v1++;
			if (*v2 == '.') v2++;
		} else {
			if (*v1) num1 = num1 * 10 + (*v1 - '0');
			if (*v2) num2 = num2 * 10 + (*v2 - '0');
			if (*v1 && *v1 != '.') v1++;
			if (*v2 && *v2 != '.') v2++;
		}
		if (num1 != num2) return num1 - num2;
		num1 = num2 = 0;
	}
	return num1 - num2;
}

const char *version_dictionary(const char *version_url) {
	const char *pattern;
	if(version_url == "https://archive.mozilla.org/pub/nspr/releases/") {
		pattern = "([0-4]+\\.[0-3]+[0-9]+)";
	} else if (version_url == "https://archive.mozilla.org/pub/security/nss/releases/") {
		pattern = "([0-9]+\\_[0-9]+[0-9]+[0-9]+)";
	} else {
		pattern = "([0-9]+\\.[0-9]+\\.[0-9]+)";
	}
	return pattern;
}

void extract_version_html(const char *version_url, char *temp_ver, char *new_ver) {
	pcre2_code *regex;
	PCRE2_SIZE erroffset;
	int errorcode;
	const char *pattern = version_dictionary(version_url);
	regex = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
		0, &errorcode, &erroffset, NULL);
	if(regex == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		fprintf(stderr, "Could not compile regex: %s\n", (char *)buffer);
	}
	PCRE2_SIZE subject_length = strlen(temp_ver);
	pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	PCRE2_SIZE start_offset = 0;
	int rc;
	while ((rc = pcre2_match(regex, (PCRE2_SPTR)temp_ver, subject_length,
		start_offset, 0, match_data, NULL)) >= 0) {
		PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
		PCRE2_SIZE match_start = ovector[0];
		PCRE2_SIZE match_end = ovector[1];
		PCRE2_SIZE match_size = match_end - match_start;
		char version[100];
		strncpy(version, temp_ver + match_start, match_size);
		version[match_size] = '\0';
		if (new_ver[0] == '\0' || strcmp(version, new_ver) > 0) {
			//printf("DEBUG: Newer version: %s\n", version);
			strcpy(new_ver, version);
		}
		start_offset = match_end;
	}
	pcre2_match_data_free(match_data);
	pcre2_code_free(regex);
}

void extract_info_html(char *temp_info, char *new_info) {
}

void fetch_latest_version_and_changelog(const char *version_url, const char *info_url, char *latest_version, char *changelog) {
	char temp_ver[100000] = {0};
	char temp_info[100000] = {0};
	CURL *curlfetch;
	CURLcode resfetch;
	curlfetch = curl_easy_init();
	if(curlfetch) {
		curl_easy_setopt(curlfetch, CURLOPT_URL, version_url);
		curl_easy_setopt(curlfetch, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curlfetch, CURLOPT_WRITEDATA, temp_ver);
		curl_easy_setopt(curlfetch, CURLOPT_FOLLOWLOCATION, 1L);
		resfetch = curl_easy_perform(curlfetch);
		if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", version_url);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
		}
		temp_ver[99999] = '\0';
		if(info_url != NULL) {
			curl_easy_setopt(curlfetch, CURLOPT_URL, info_url);
			curl_easy_setopt(curlfetch, CURLOPT_WRITEDATA, temp_info);
			curl_easy_setopt(curlfetch, CURLOPT_FOLLOWLOCATION, 1L);
			resfetch = curl_easy_perform(curlfetch);
			if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", info_url);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
			}
		}
		temp_info[99999] = '\0';
		extract_version_html(version_url, temp_ver, latest_version);
		latest_version[100 - 1] = '\0';
		extract_info_html(temp_info, changelog);
		changelog[4096 - 1] = '\0';
		curl_easy_cleanup(curlfetch);
	}
}

void process_pkg_info(const char *pkg, const char *entity_keyword, const char *version_url, const char *info_url) {
	Entity entities[100] = {0};
	char old_version[100] = {0};
	char latest_version[100] = {0};
	char changelog[4096] = {0};
	int entity_count = parse_packages_ent(g_argv[g_argc - 1], entities, 100);
	for(int i = 0; i < entity_count; i++) {
		if (strcmp(entities[i].name, entity_keyword) == 0) {
			strncpy(old_version, entities[i].value, 100 - 1);
			old_version[100 - 1] = '\0';
			break;
		}
	}
	clean_entity(old_version);
	fetch_latest_version_and_changelog(version_url, info_url, latest_version, changelog);
	if (strcmp(old_version, latest_version) != 0) {
		printf("%s:\n", pkg);
		printf("  Old version: %s\n", old_version);
		printf("  New version: %s\n", latest_version);
		if (changelog[0] != '\0') {
			printf("  Changelog: %s\n", changelog);
		} else {
			printf("  There is no changelog with this release.\n");
		}
		printf("\n");
	}
}

void check_package_versions(void) {
	char latest_version[100] = {0};
	char changelog[4096] = {0};
	process_pkg_info("libtasn1", "libtasn1-version",
		"https://ftp.gnu.org/gnu/libtasn1/",
		NULL);
	process_pkg_info("NSPR", "nspr-version",
		"https://archive.mozilla.org/pub/nspr/releases/",
		NULL);
	process_pkg_info("NSS", "nss-dir",
		"https://archive.mozilla.org/pub/security/nss/releases/",
		NULL);
}
