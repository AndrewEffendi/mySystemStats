#include "tools.h"

// create substring from str starting at index start with length len
void substr(char *str, char *subStr, int start, int len)
{
    strncpy(subStr, &str[start], len);
    subStr[len] = '\0';
}

// intercept SIGINT
void c_handler(int sig)
{
    char c;
    signal(sig, SIG_IGN);
    printf(" Do you want to quit the program? [y/n]: ");
    c = getchar();
    if (c == 'y' || c == 'Y')
        exit(0);
    else
        signal(SIGINT, c_handler);
    getchar(); // Get new line character
}