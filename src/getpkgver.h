#ifndef GETPKGVER_H
#define GETPKGVER_H

#include <stdio.h>
#include <curl/curl.h>

extern int gargc;
extern char **gargv;

extern CURL *curl;
extern CURLcode res;

extern void checkpackageversions(void);

#endif
