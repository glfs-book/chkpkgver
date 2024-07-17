#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

#include <curl/curl.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <json-c/json.h>

#include "getpkgver.h"

#define BUFFERLEN 1024

typedef struct {
	char name[100];
	char value[100];
} Entity;

typedef struct {
	const char* pkg[4];
	CURL* curl;
	char latest_version[100];
	char changelog[4096];
} ThreadArgs;

pthread_mutex_t lock;

// fuck indentation
void expand_entities(Entity* entities, int entity_count) {
	for (int i = 0; i < entity_count; i++) {
		for (int j = 0; j < entity_count; j++) {
		char* pos = strstr(entities[i].value, entities[j].name);
		if (pos) {
		char expanded_value[100];
		snprintf(expanded_value, 100, "%.*s%s%s",
		(int)(pos - entities[i].value), entities[i].value,
		entities[j].value, pos + strlen(entities[j].name));
		strcpy(entities[i].value, expanded_value);
		}
		}
	}
}

// again, fuck it
int parse_packages_ent(const char* filepath, Entity* entities, int max_entities) {
	FILE* file = fopen(filepath, "r");
	char line[BUFFERLEN];
	int entity_count = 0;
	while (fgets(line, sizeof(line), file) && entity_count < max_entities) {
	if (sscanf(line, "<!ENTITY %99s %99[^\n]>",
	entities[entity_count].name, entities[entity_count].value) == 2) {
	size_t len = strlen(entities[entity_count].value);
	if (entities[entity_count].value[0] == '"' &&
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

void clean_entity(char* entity) {
	char clean_entity_char[100] = {0};
	char* src = entity;
	char* dst = clean_entity_char;
	while (*src) {
		if (isdigit(*src) || *src == '.' || *src == '_') {
			*dst++ = *src;
		}
		src++;
	}
	strcpy(entity, clean_entity_char);
}

size_t write_callback(void* ptr, size_t size, size_t nmemb, void* stream) {
	size_t total_size = size * nmemb;
	strncat((char*)stream, (char*)ptr, total_size);
	return total_size;
}

int compare_versions(const char* v1, const char* v2) {
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

const char* version_dictionary(const char* pkg[4]) {
	const char* pattern;
	if (pkg[0] == "NSPR") {
		pattern = "([0-4]+\\.[0-3]+[0-9]+)";
	} else if (pkg[0] == "NSS") {
		pattern = "([0-9]+\\_[0-9]+[0-9]+[0-9]+)";
	} else if (pkg[0] == "make-ca") {
		pattern = "([0-9]+\\.[0-9]+)";	
	} else if (pkg[0] == "libunistring") {
		pattern = "([0-1]+\\.[0-9]+)";
	} else if (pkg[0] == "Wget" ||
		pkg[0] == "git") {
		pattern = "([0-9]+\\.[0-7]+[0-9]\\.[0-9]+)";
	} else if (pkg[0] == "alsa-lib" ||
		pkg[0] == "alsa-plugins" ||
		pkg[0] == "alsa-utils" ||
		pkg[0] == "libXau" ||
		pkg[0] == "libXpm" ||
		pkg[0] == "libXaw" ||
		pkg[0] == "libXrender" ||
		pkg[0] == "libXv" ||
		pkg[0] == "libXvMC") {
		pattern = "([0-9]+\\.[0-9]+\\.[0-9]+[0-9]+)";
	} else if (pkg[0] == "PulseAudio") {
		pattern = "([1-5]+[0-9]\\.[0-9]+)";
	} else if (pkg[0] == "util-macros" ||
		pkg[0] == "libxcb" ||
		pkg[0] == "xcb-proto" ||
		pkg[0] == "Python" ||
		pkg[0] == "Fontconfig" ||
		pkg[0] == "libpciaccess" ||
		pkg[0] == "CMake" ||
		pkg[0] == "Pixman" ||
		pkg[0] == "libxml2") {
		pattern = "([0-9]+\\.[0-9]+[0-9]+\\.[0-9]+)";
	} else if (pkg[0] == "xorgproto") {
		pattern = "([0-9]+[0-9]+[0-9]+[0-9]+\\.[0-9]+)";
	} else if (pkg[0] == "Which" ||
		pkg[0] == "Nettle" ||
		pkg[0] == "icu" ||
		pkg[0] == "Wayland-Protocols") {
		pattern = "([0-9]+\\.[0-9]+[0-9]+)";
	} else if (pkg[0] == "libXcomposite") {
		pattern = "([0]+\\.[0-9]+\\.[0-9]+)";	
	} else if (pkg[0] == "dbus") {
		pattern = "([0-9]+\\.[0-9]+[0-9]+\\.[0-9]+\\.[0-9]+)";	
	} else {
		pattern = "([0-9]+\\.[0-9]+\\.[0-9]+)";
	}
	return pattern;
}

void take_out_data_sizes(char* temp) {
	const char* pattern = "[0-9]+\\.[0-9]+M";
	pcre2_code* regex;
	PCRE2_SIZE erroffset;
	int errorcode;
	regex = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0,
		&errorcode, &erroffset, NULL);
	if (regex == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		fprintf(stderr, "Could not compile regex: %s\n", (char*)buffer);
	}
	PCRE2_SIZE subject_length = strlen(temp);
	pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	PCRE2_SIZE start_offset = 0;
	int rc;
	while ((rc = pcre2_match(regex, (PCRE2_SPTR)temp, subject_length, start_offset, 0,
		match_data, NULL)) >= 0) {
		PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
		PCRE2_SIZE match_start = ovector[0];
		PCRE2_SIZE match_end = ovector[1];
		memmove(temp + match_start, temp + match_end, subject_length - match_end + 1);
		subject_length -= (match_end - match_start);
		start_offset = match_start;
	}
	pcre2_match_data_free(match_data);
	pcre2_code_free(regex);
}

void take_out_conflicts(char* temp_ver, char* new_ver) {
	char* line;
	char** buffer;
	int line_count = 0;
	buffer = malloc(500000 * sizeof(char*));
	if (buffer == NULL) {
		fprintf(stderr, "Cannot allocate memory\n");
	}
	line = strtok(temp_ver, "\n");
	while (line != NULL) {
		buffer[line_count] = malloc(20000 * sizeof(char));
		if (buffer[line_count] == NULL) {
			fprintf(stderr, "Cannot allocate memory\n");
		}
		strncpy(buffer[line_count++], line, 20000 - 1);
		buffer[line_count - 1][20000 - 1] = '\0';
		line = strtok(NULL, "\n");
	}
	new_ver[0] = '\0';
	for (int i = 0; i < line_count; i++) {
		take_out_data_sizes(buffer[i]);
		if (strstr(buffer[i], "Apache") == NULL &&
			strstr(buffer[i], "DOCTYPE") == NULL &&
			strstr(buffer[i], "topology") == NULL &&
			strstr(buffer[i], "ucm-conf") == NULL &&
			strstr(buffer[i], "vorbis-tools") == NULL &&
			strstr(buffer[i], "tanuki-shape") == NULL &&
			strstr(buffer[i], "span class") == NULL &&
			strstr(buffer[i], "script") == NULL) {
			strcat(new_ver, buffer[i]);
			strcat(new_ver, "\n");
		}
		free(buffer[i]);
	}
	free(buffer);
	//printf("DEBUG: temp_ver, new_ver:\n%s\n%s\n", temp_ver, new_ver);
}

void extract_version_html(const char* pkg[4], char* temp_ver, char* new_ver) {
	char new_temp[500000] = {0};
	take_out_conflicts(temp_ver, new_temp);
	pcre2_code* regex;
	PCRE2_SIZE erroffset;
	int errorcode;
	const char* pattern = version_dictionary(pkg);
	regex = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
		0, &errorcode, &erroffset, NULL);
	if(regex == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		fprintf(stderr, "Could not compile regex: %s\n", (char*)buffer);
	}
	PCRE2_SIZE subject_length = strlen(new_temp);
	pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
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

void extract_version_github(const char* pkg[4], char* temp_ver, char* new_ver) {
	//printf("Debug: JSON:\n%s", temp_ver);
	char new_temp[500000] = {0};
	struct json_object* s_json_obj;
	struct json_object* tag_name;
	s_json_obj = json_tokener_parse(temp_ver);
	if (s_json_obj == NULL) {
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

void extract_info_html(char* temp_info, char* new_info) {
}

void fetch_latest_version_and_changelog(const char* pkg[4], char* latest_version, char* changelog, CURL* curl) {
	pthread_mutex_lock(&lock);
	char temp_ver[500000] = {0};
	char temp_info[500000] = {0};
	CURLcode resfetch;
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, pkg[2]);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp_ver);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "chkpkgver/pre-0.70");
		resfetch = curl_easy_perform(curl);
		if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", pkg[2]);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
		}
		temp_ver[499999] = '\0';
		if (pkg[3] != "\0") {
			curl_easy_setopt(curl, CURLOPT_URL, pkg[3]);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp_info);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "chkpkgver/pre-0.70");
			resfetch = curl_easy_perform(curl);
			if (resfetch != CURLE_OK) {
			printf("Read from: %s but failed...", pkg[3]);
			fprintf(stderr, "%s\n",
				curl_easy_strerror(resfetch));
			return;
			}
		}
		temp_info[499999] = '\0';
	if (pkg[0] == "p11-kit" ||
	pkg[0] == "make-ca" ||
	pkg[0] == "libpsl" ||
	pkg[0] == "libsndfile" ||
	pkg[0] == "HarfBuzz") {
			extract_version_github(pkg, temp_ver, latest_version);
		} else {
			extract_version_html(pkg, temp_ver, latest_version);
		}
		latest_version[100 - 1] = '\0';
		extract_info_html(temp_info, changelog);
		changelog[4096 - 1] = '\0';
	}
	pthread_mutex_unlock(&lock);
	curl_easy_cleanup(curl);
}

void process_pkg_info(const char* pkg[4], CURL* curl, char* latest_version, char* changelog) {
	//printf("DEBUG: Fetching %s...\n", pkg[0]);
	Entity entities[100] = {0};
	char old_version[100] = {0};
	int entity_count = parse_packages_ent(g_argv[g_argc - 1], entities, 100);
	for (int i = 0; i < entity_count; i++) {
		if (strcmp(entities[i].name, pkg[1]) == 0) {
			strncpy(old_version, entities[i].value, 100 - 1);
			old_version[100 - 1] = '\0';
			break;
		}
	}
	clean_entity(old_version);
	fetch_latest_version_and_changelog(pkg, latest_version, changelog, curl);
	//printf("DEBUG: %s:\n", pkg[0]);
	//printf("DEBUG:  Old version: %s\n", old_version);
	//printf("DEBUG:  New version: %s\n", latest_version);
	if (strcmp(old_version, latest_version) != 0) {
		printf("%s:\n", pkg[0]);
		printf("  Old version: %s\n", old_version);
		printf("  New version: %s\n", latest_version);
		if (pkg[0] == "Python") {
			printf("  3.13 is in beta.\n  Check schedule: https://peps.python.org/pep-0719/\n");
		}
		if (changelog[0] != '\0') {
			printf("  Changelog: %s\n", changelog);
		} else {
			printf("  There is no changelog with this release.\n");
		}
		printf("\n");
	}
	strcpy(old_version, "\0");
	strcpy(latest_version, "\0");
	strcpy(changelog, "\0");
}

void* thread_function(void* arg) {
	ThreadArgs* threadArgs = (ThreadArgs*)arg;
	threadArgs->curl = curl_easy_init();
	if (!threadArgs->curl) {
		fprintf(stderr, "Failed to create cURL handle\n");
		return NULL;
	}
	process_pkg_info(threadArgs->pkg, threadArgs->curl,
		threadArgs->latest_version, threadArgs->changelog);
	curl_easy_cleanup(threadArgs->curl);
	return NULL;
}

void copy_package(const char* dest[4], const char* src[4]) {
	for (int i = 0; i < 4; i++) {
		dest[i] = src[i];
	}
}

void check_package_versions(void) {
	const char* packages[][4] = {
		{ "libtasn1", "libtasn1-version",
			"https://ftp.gnu.org/gnu/libtasn1/",
			"\0"
		}, { "NSPR", "nspr-version",
			"https://archive.mozilla.org/pub/nspr/releases/",
			"\0"
		}, { "NSS", "nss-dir",
			"https://archive.mozilla.org/pub/security/nss/releases/",
			"\0"
		}, { "p11-kit", "p11-kit-version",
			"https://api.github.com/repos/p11-glue/p11-kit/releases/latest",
			"\0"
		}, { "make-ca", "make-ca-version",
			"https://api.github.com/repos/lfs-book/make-ca/releases/latest",
			"\0"
		}, { "libunistring", "libunistring-version",
			"https://ftp.gnu.org/gnu/libunistring/",
			"\0"
		}, { "libidn2", "libidn2-version",
			"https://ftp.gnu.org/gnu/libidn/",
			"\0"
		}, { "libpsl", "libpsl-version",
			"https://api.github.com/repos/rockdaboot/libpsl/releases/latest",
			"\0"
		}, { "cURL", "curl-version",
			"https://curl.se/download/",
			"\0"
		}, { "Wget", "wget-version",
			"https://ftp.gnu.org/gnu/wget/",
			"\0"
		}, { "git", "git-version",
			"https://www.kernel.org/pub/software/scm/git/",
			"\0"
		}, { "alsa-lib", "alsa-lib-version",
			"https://www.alsa-project.org/files/pub/lib/",
			"\0"
		}, { "alsa-plugins", "alsa-plugins-version",
			"https://www.alsa-project.org/files/pub/plugins/",
			"\0"
		}, { "alsa-utils", "alsa-utils-version",
			"https://www.alsa-project.org/files/pub/utils/",
			"\0"
		}, { "libogg", "libogg-version",
			"https://downloads.xiph.org/releases/ogg/",
			"\0"
		}, { "libvorbis", "libvorbis-version",
			"https://downloads.xiph.org/releases/vorbis/",
			"\0"
		}, { "FLAC", "flac-version",
			"https://downloads.xiph.org/releases/flac/",
			"\0"
		}, { "Opus", "opus-version",
			"https://downloads.xiph.org/releases/opus/",
			"\0"
		}, { "libsndfile", "libsndfile-version",
			"https://api.github.com/repos/libsndfile/libsndfile/releases/latest",
			"\0"
		}, { "PulseAudio", "pulseaudio-version",
			"https://www.freedesktop.org/software/pulseaudio/releases/",
			"\0"
		}, { "util-macros", "util-macros-version",
			"https://gitlab.freedesktop.org/xorg/util/macros/-/tags",
			"\0"
		}, { "xorgproto", "xorgproto-version",
			"https://gitlab.freedesktop.org/xorg/proto/xorgproto/-/tags",
			"\0"
		}, { "libXau", "libXau-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXau/-/tags",
			"\0"
		}, { "libXdmcp", "libXdmcp-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXdmcp/-/tags",
			"\0"
		}, { "Python", "python3-version",
			"https://www.python.org/ftp/python/",
			"\0"
		}, { "xcb-proto", "xcb-proto-version",
			"https://gitlab.freedesktop.org/xorg/proto/xcbproto/-/tags",
			"\0"
		}, { "libxcb", "libxcb-version",
			"https://gitlab.freedesktop.org/xorg/lib/libxcb/-/tags",
			"\0"
		}, { "Which", "which-version",
			"https://ftp.gnu.org/gnu/which/",
			"\0"
		}, { "libpng", "libpng-version",
			"https://sourceforge.net/projects/libpng/files/",
			"\0"
		}, { "FreeType2", "freetype2-version",
			"https://sourceforge.net/projects/freetype/files/",
			"\0"
		}, { "HarfBuzz", "harfbuzz-version",
			"https://api.github.com/repos/harfbuzz/harfbuzz/releases/latest",
			"\0"
		}, { "Fontconfig", "fontconfig-version",
			"https://www.freedesktop.org/software/fontconfig/release/",
			"\0"
		}, { "xtrans", "xtrans-version",
			"https://gitlab.freedesktop.org/xorg/lib/libxtrans/-/tags",
			"\0"
		}, { "libX11", "libX11-version",
			"https://gitlab.freedesktop.org/xorg/lib/libX11/-/tags",
			"\0"
		}, { "libXext", "libXext-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXext/-/tags",
			"\0"
		}, { "libFS", "libFS-version",
			"https://gitlab.freedesktop.org/xorg/lib/libFS/-/tags",
			"\0"
		}, { "libICE", "libICE-version",
			"https://gitlab.freedesktop.org/xorg/lib/libICE/-/tags",
			"\0"
		}, { "libSM", "libSM-version",
			"https://gitlab.freedesktop.org/xorg/lib/libSM/-/tags",
			"\0"
		}, { "libXScrnSaver", "libXScrnSaver-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXScrnSaver/-/tags",
			"\0"
		}, { "libXt", "libXt-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXt/-/tags",
			"\0"
		}, { "libXmu", "libXmu-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXmu/-/tags",
			"\0"
		}, { "libXpm", "libXpm-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXpm/-/tags",
			"\0"
		}, { "libXaw", "libXaw-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXaw/-/tags",
			"\0"
		}, { "libXfixes", "libXfixes-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXfixes/-/tags",
			"\0"
		}, { "libXcomposite", "libXcomposite-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXcomposite/-/tags",
			"\0"
		}, { "libXrender", "libXrender-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXrender/-/tags",
			"\0"
		}, { "libXcursor", "libXcursor-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXcursor/-/tags",
			"\0"
		}, { "libXdamage", "libXdamage-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXdamage/-/tags",
			"\0"
		}, { "libfontenc", "libfontenc-version",
			"https://gitlab.freedesktop.org/xorg/lib/libfontenc/-/tags",
			"\0"
		}, { "libXfont2", "libXfont2-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXfont/-/tags",
			"\0"
		}, { "libXft", "libXft-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXft/-/tags",
			"\0"
		}, { "libXi", "libXi-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXi/-/tags",
			"\0"
		}, { "libXinerama", "libXinerama-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXinerama/-/tags",
			"\0"
		}, { "libXrandr", "libXrandr-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXrandr/-/tags",
			"\0"
		}, { "libXres", "libXres-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXres/-/tags",
			"\0"
		}, { "libXtst", "libXtst-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXtst/-/tags",
			"\0"
		}, { "libXv", "libXv-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXv/-/tags",
			"\0"
		}, { "libXvMC", "libXvMC-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXvMC/-/tags",
			"\0"
		}, { "libXxf86dga", "libXxf86dga-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXxf86dga/-/tags",
			"\0"
		}, { "libXxf86vm", "libXxf86vm-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXxf86vm/-/tags",
			"\0"
		}, { "libpciaccess", "libpciaccess-version",
			"https://gitlab.freedesktop.org/xorg/lib/libpciaccess/-/tags",
			"\0"
		}, { "libxkbfile", "libxkbfile-version",
			"https://gitlab.freedesktop.org/xorg/lib/libxkbfile/-/tags",
			"\0"
		}, { "libxshmfence", "libxshmfence-version",
			"https://gitlab.freedesktop.org/xorg/lib/libxshmfence/-/tags",
			"\0"
		}, { "libXpresent", "libXpresent-version",
			"https://gitlab.freedesktop.org/xorg/lib/libXpresent/-/tags",
			"\0"
		}, { "libxcvt", "libxcvt-version",
			"https://gitlab.freedesktop.org/xorg/lib/libxcvt/-/tags",
			"\0"
		}, { "CMake", "cmake-version",
			"https://cmake.org/download/",
			"\0"
		}, { "libunwind", "libunwind-version",
			"https://download.savannah.nongnu.org/releases/libunwind/",
			"\0"
		}, { "Nettle", "nettle-version",
			"https://ftp.gnu.org/gnu/nettle/",
			"\0"
		}, { "GnuTLS", "gnutls-version",
			"https://gitlab.com/gnutls/gnutls/-/tags",
			"\0"
		}, { "Pixman", "pixman-version",
			"https://gitlab.freedesktop.org/pixman/pixman/-/tags",
			"\0"
		}, { "libxml2", "libxml2-version",
			"https://gitlab.gnome.org/GNOME/libxml2/-/tags",
			"\0"
		}, { "Wayland", "wayland-version",
			"https://gitlab.freedesktop.org/wayland/wayland/-/tags",
			"\0"
		}, { "Wayland-Protocols", "wayland-protocols-version",
			"https://gitlab.freedesktop.org/wayland/wayland-protocols/-/tags",
			"\0"
		}, { "seatd", "seatd-version",
			"https://git.sr.ht/~kennylevinsen/seatd/refs/",
			"\0"
		}, { "libglvnd", "libglvnd-version",
			"https://gitlab.freedesktop.org/glvnd/libglvnd/-/tags",
			"\0"
		}
	};
	printf("WARNING - Checking dbus and icu cannot be done at the moment.\n\n");

	int package_count = sizeof(packages) / sizeof(packages[0]);
	int max_threads = sysconf(_SC_NPROCESSORS_ONLN);
	pthread_t threads[max_threads];
	ThreadArgs threadArgs[max_threads];
	int thread_count = 0;
	pthread_mutex_init(&lock, NULL);
	for (int i = 0; i < package_count; i++) {
		copy_package(threadArgs[thread_count].pkg, packages[i]);
		threadArgs[thread_count].curl = curl_easy_init();
		if (!threadArgs[thread_count].curl) {
			fprintf(stderr, "Failed to create cURL handle\n");
			continue;
		}
		if (pthread_create(&threads[thread_count], NULL, thread_function,
			&threadArgs[thread_count]) != 0) {
			perror("Failed to create thread");
			curl_easy_cleanup(threadArgs[thread_count].curl);
			continue;
		}
		thread_count++;
		if (thread_count == max_threads) {
			for (int j = 0; j < thread_count; j++) {
				pthread_join(threads[j], NULL);
			}
			thread_count = 0;
		}
	}
	for (int i = 0; i < thread_count; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_mutex_destroy(&lock);
}
