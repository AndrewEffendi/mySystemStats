#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utmp.h>

// get users and sessions
void getUsers(char **users);

// print users and sessions
void printUsers(char *users, int users_strlen);