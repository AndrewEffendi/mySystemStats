#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utmp.h>

// print users and sessions
void getUsers(char **users);

void printUsers(char *users, int users_strlen);