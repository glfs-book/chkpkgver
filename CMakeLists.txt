cmake_minimum_required(VERSION 3.5)
project(chkpkgver)
set(SOURCES
	src/main.c
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
message(STATUS "Detecting libxml2")
find_library(XML2_LIBRARY xml2)
if(XML2_LIBRARY)
	message(STATUS "Detecting libxml2 - found")
else()
	message(FATAL_ERROR "Detecting libxml2 - not found")
endif()
target_link_libraries(chkpkgver ${CURL_LIBRARY})
target_link_libraries(chkpkgver ${XML2_LIBRARY})
install(TARGETS chkpkgver
	DESTINATION bin
)
