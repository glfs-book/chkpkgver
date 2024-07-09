#ifndef GETPKGVER_H
#define GETPKGVER_H

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>

extern CURL *curl;
extern CURLcode res;

extern char checkpackageversions(void);

#endif
