#ifndef GETPKGVER_H
#define GETPKGVER_H

#include <stdio.h>
#include <curl/curl.h>

extern int g_argc;
extern char **g_argv;

extern void check_package_versions(void);

#endif
