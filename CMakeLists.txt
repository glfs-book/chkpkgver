cmake_minimum_required(VERSION 3.5)
project(chkpkgver)
set(SOURCES
	src/main.c
	src/getpkginfo.c
	src/getpkgver.c
)
include_directories(src)
add_executable(chkpkgver ${SOURCES})
message(STATUS "Detecting libcurl")
find_library(CURL_LIBRARY curl)
if(CURL_LIBRARY)
	message(STATUS "Detecting libcurl - found")
else()
	message(FATAL_ERROR "Detecting libcurl - not found")
endif()
target_link_libraries(chkpkgver ${CURL_LIBRARY})
install(TARGETS chkpkgver
	DESTINATION bin
)
