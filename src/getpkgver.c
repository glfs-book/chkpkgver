#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <curl/curl.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <json-c/json.h>

#include "getpkgver.h"
#include "pkglist.h"

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

const char *version_dictionary(const char *pkg[4]) {
	const char *pattern;
	if(pkg[0] == "NSPR") {
		pattern = "([0-4]+\\.[0-3]+[0-9]+)";
	} else if(pkg[0] == "NSS") {
		pattern = "([0-9]+\\_[0-9]+[0-9]+[0-9]+)";
	} else if(pkg[0] == "make-ca") {
		pattern = "([0-9]+\\.[0-9]+)";	
	} else if(pkg[0] == "libunistring") {
		pattern = "([0-1]+\\.[0-9]+)";
	} else if(pkg[0] == "Wget" ||
		pkg[0] == "git") {
		pattern = "([0-9]+\\.[0-7]+[0-9]\\.[0-9]+)";
	} else if(pkg[0] == "alsa-lib" ||
		pkg[0] == "alsa-plugins" ||
		pkg[0] == "alsa-utils") {
		pattern = "([0-9]+\\.[0-9]+\\.[0-9]+[0-9]+)";
	} else if (pkg[0] == "PulseAudio") {
		pattern = "([1-5]+[0-9]\\.[0-9]+)";
	} else {
		pattern = "([0-9]+\\.[0-9]+\\.[0-9]+)";
	}
	return pattern;
}

void take_out_data_sizes(char *temp) {
	const char *pattern = "[0-9]+\\.[0-9]+M";
	pcre2_code *regex;
	PCRE2_SIZE erroffset;
	int errorcode;
	regex = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0,
		&errorcode, &erroffset, NULL);
	if (regex == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		fprintf(stderr, "Could not compile regex: %s\n", (char *)buffer);
	}
	PCRE2_SIZE subject_length = strlen(temp);
	pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	PCRE2_SIZE start_offset = 0;
	int rc;
	while ((rc = pcre2_match(regex, (PCRE2_SPTR)temp, subject_length, start_offset, 0,
		match_data, NULL)) >= 0) {
		PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
		PCRE2_SIZE match_start = ovector[0];
		PCRE2_SIZE match_end = ovector[1];
		memmove(temp + match_start, temp + match_end, subject_length - match_end + 1);
		subject_length -= (match_end - match_start);
		start_offset = match_start;
	}
	pcre2_match_data_free(match_data);
	pcre2_code_free(regex);
}

void take_out_conflicts(char *temp_ver, char *new_ver) {
	char *line;
	char **buffer;
	int line_count = 0;
	buffer = malloc(200000 * sizeof(char *));
	if(buffer == NULL) {
		fprintf(stderr, "Cannot allocate memory\n");
	}
	line = strtok(temp_ver, "\n");
	while(line != NULL) {
		buffer[line_count] = malloc(20000 * sizeof(char));
		if(buffer[line_count] == NULL) {
			fprintf(stderr, "Cannot allocate memory\n");
		}
		strncpy(buffer[line_count++], line, 20000 - 1);
		buffer[line_count - 1][20000 - 1] = '\0';
		line = strtok(NULL, "\n");
	}
	new_ver[0] = '\0';
	for(int i = 0; i < line_count; i++) {
		take_out_data_sizes(buffer[i]);
		if(strstr(buffer[i], "Apache") == NULL &&
			strstr(buffer[i], "DOCTYPE") == NULL &&
			strstr(buffer[i], "topology") == NULL &&
			strstr(buffer[i], "ucm-conf") == NULL &&
			strstr(buffer[i], "vorbis-tools") == NULL) {
			strcat(new_ver, buffer[i]);
			strcat(new_ver, "\n");
		}
		free(buffer[i]);
	}
	free(buffer);
}

void extract_version_html(const char *pkg[4], char *temp_ver, char *new_ver) {
	char new_temp[200000] = {0};
	take_out_conflicts(temp_ver, new_temp);
	pcre2_code *regex;
	PCRE2_SIZE erroffset;
	int errorcode;
	const char *pattern = version_dictionary(pkg);
	regex = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
		0, &errorcode, &erroffset, NULL);
	if(regex == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		fprintf(stderr, "Could not compile regex: %s\n", (char *)buffer);
	}
	PCRE2_SIZE subject_length = strlen(new_temp);
	pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	PCRE2_SIZE start_offset = 0;
	int rc;
	while ((rc = pcre2_match(regex, (PCRE2_SPTR)new_temp, subject_length,
		start_offset, 0, match_data, NULL)) >= 0) {
		PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
		PCRE2_SIZE match_start = ovector[0];
		PCRE2_SIZE match_end = ovector[1];
		PCRE2_SIZE match_size = match_end - match_start;
		char version[100];
		strncpy(version, new_temp + match_start, match_size);
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

void extract_version_github(const char *pkg[4], char *temp_ver, char *new_ver) {
	//printf("Debug: JSON:\n%s", temp_ver);
	char new_temp[200000] = {0};
	struct json_object *s_json_obj;
	struct json_object *tag_name;
	s_json_obj = json_tokener_parse(temp_ver);
	if(s_json_obj == NULL) {
		fprintf(stderr, "Cannot parse JSON... did it download?\n");
	} else {
		if (json_object_object_get_ex(s_json_obj, "tag_name", &tag_name)) {
		//printf("DEBUG: tag name: %s\n", json_object_get_string(tag_name));
		strcpy(new_temp, json_object_get_string(tag_name));
		} else {
		fprintf(stderr, "Tag name not found in JSON...\n");
		}
		json_object_put(s_json_obj);
	}
	extract_version_html(pkg, new_temp, new_ver);
	//printf("DEBUG: new version: %s\n", new_ver);
}

void extract_info_html(char *temp_info, char *new_info) {
}

void fetch_latest_version_and_changelog(const char *pkg[4], char *latest_version, char *changelog) {
	char temp_ver[200000] = {0};
	char temp_info[200000] = {0};
	CURL *curlfetch;
	CURLcode resfetch;
	curlfetch = curl_easy_init();
	if(curlfetch) {
		curl_easy_setopt(curlfetch, CURLOPT_URL, pkg[2]);
		curl_easy_setopt(curlfetch, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curlfetch, CURLOPT_WRITEDATA, temp_ver);
		curl_easy_setopt(curlfetch, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curlfetch, CURLOPT_USERAGENT, "chkpkgver/pre-0.6");
		resfetch = curl_easy_perform(curlfetch);
		if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", pkg[2]);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
		}
		temp_ver[199999] = '\0';
		if(pkg[3] != "\0") {
			curl_easy_setopt(curlfetch, CURLOPT_URL, pkg[3]);
			curl_easy_setopt(curlfetch, CURLOPT_WRITEDATA, temp_info);
			curl_easy_setopt(curlfetch, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curlfetch, CURLOPT_USERAGENT, "chkpkgver/pre-0.6");
			resfetch = curl_easy_perform(curlfetch);
			if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", pkg[3]);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
			}
		}
		temp_info[199999] = '\0';
	if(pkg[0] == "p11-kit" ||
	pkg[0] == "make-ca" ||
	pkg[0] == "libpsl" ||
	pkg[0] == "libsndfile") {
			extract_version_github(pkg, temp_ver, latest_version);
		} else {
			extract_version_html(pkg, temp_ver, latest_version);
		}
		latest_version[100 - 1] = '\0';
		extract_info_html(temp_info, changelog);
		changelog[4096 - 1] = '\0';
		curl_easy_cleanup(curlfetch);
	}
}

void process_pkg_info(const char *pkg[4]) {
	printf("DEBUG: Fetching %s...\n", pkg[0]);
	Entity entities[100] = {0};
	char old_version[100] = {0};
	char latest_version[100] = {0};
	char changelog[4096] = {0};
	int entity_count = parse_packages_ent(g_argv[g_argc - 1], entities, 100);
	for(int i = 0; i < entity_count; i++) {
		if (strcmp(entities[i].name, pkg[1]) == 0) {
			strncpy(old_version, entities[i].value, 100 - 1);
			old_version[100 - 1] = '\0';
			break;
		}
	}
	clean_entity(old_version);
	fetch_latest_version_and_changelog(pkg, latest_version, changelog);
	//printf("DEBUG: %s:\n", pkg[0]);
	//printf("DEBUG:  Old version: %s\n", old_version);
	//printf("DEBUG:  New version: %s\n", latest_version);
	if (strcmp(old_version, latest_version) != 0) {
		printf("%s:\n", pkg[0]);
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
	/* STATUS: */
	/* WORKING */
	// Commented out to focus on new packages
	/*
	process_pkg_info(pkg_libtasn1);
	process_pkg_info(pkg_NSPR);
	process_pkg_info(pkg_NSS);
	process_pkg_info(pkg_p11_kit);
	process_pkg_info(pkg_make_ca);
	process_pkg_info(pkg_libunistring);
	process_pkg_info(pkg_libidn2);
	process_pkg_info(pkg_libpsl);
	process_pkg_info(pkg_cURL);
	process_pkg_info(pkg_Wget);
	process_pkg_info(pkg_git);
	process_pkg_info(pkg_alsa_lib);
	process_pkg_info(pkg_alsa_plugins);
	process_pkg_info(pkg_alsa_utils);
	process_pkg_info(pkg_libogg);
	process_pkg_info(pkg_libvorbis);
	process_pkg_info(pkg_flac);
	process_pkg_info(pkg_opus);
	process_pkg_info(pkg_libsndfile);
	process_pkg_info(pkg_PulseAudio);
	*/
	/*
	printf("WARNING - checking Xorg Libraries is not available...\n");
	printf("WARNING - checking AMDGPU PRO is not available...\n");
	printf("WARNING - checking libva is not available...\n");
	printf("WARNING - checking libvdpau is not available...\n");
	printf("WARNING - checking libvdpau-va-gl is not available...\n");
	printf("WARNING - checking Xorg Applications is not available...\n");
	printf("WARNING - checking Xorg Fonts is not available...\n");
	printf("WARNING - checking Xorg Input Drivers is not available...\n");
	*/
}
