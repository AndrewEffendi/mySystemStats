#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// create substring from str starting at index start with length len
void substr(char *str, char *subStr, int start, int len);

// intercept SIGINT
void c_handler(int sig);