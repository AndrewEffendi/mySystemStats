#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void substr(char *str, char *subStr, int start, int len);

void c_handler(int sig);