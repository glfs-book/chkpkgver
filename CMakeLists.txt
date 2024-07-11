cmake_minimum_required(VERSION 3.5)
project(chkpkgver)
set(SOURCES
	src/main.c
	src/getpkgver.c
)
include_directories(src)
add_executable(chkpkgver ${SOURCES})
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
target_link_libraries(chkpkgver ${CURL_LIBRARIES})
target_include_directories(chkpkgver PUBLIC ${CURL_INCLUDE_DIRS})
target_compile_options(chkpkgver PUBLIC ${CURL_CFLAGS_OTHER})
pkg_check_modules(PCRE2 REQUIRED libpcre2-8)
target_link_libraries(chkpkgver ${PCRE2_LIBRARIES})
target_include_directories(chkpkgver PUBLIC ${PCRE2_INCLUDE_DIRS})
target_compile_options(chkpkgver PUBLIC ${PCRE2_CFLAGS_OTHER})
pkg_check_modules(CJSON REQUIRED json-c)
target_link_libraries(chkpkgver ${CJSON_LIBRARIES})
target_include_directories(chkpkgver PUBLIC ${CJSON_INCLUDE_DIRS})
target_compile_options(chkpkgver PUBLIC ${CJSON_CFLAGS_OTHER})
install(TARGETS chkpkgver
	DESTINATION bin
)
