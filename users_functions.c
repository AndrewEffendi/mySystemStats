#include "./users_functions.h"

// print users and sessions
void getUsers(char **users)
{
    int userCount = 0;
    struct utmp *utmp;
    char buffer[323];
    setutent();
    utmp = getutent();
    while (utmp)
    {
        if (utmp->ut_type == USER_PROCESS)
        {
            userCount++;
        }
        utmp = getutent();
    }
    if (userCount == 0)
    {
        *users = NULL;
        return;
    }
    *users = (char *)malloc(sizeof(char) * userCount * 323);
    setutent();
    utmp = getutent();
    while (utmp)
    {
        if (utmp->ut_type == USER_PROCESS)
        {
            sprintf(buffer, " %-9s%-11s (%s)\n", utmp->ut_user, utmp->ut_line, utmp->ut_host);
            strcat(*users, buffer);
        }
        utmp = getutent();
    }
    endutent();
}

void printUsers(char *users, int users_strlen)
{
    printf("---------------------------------------\n");
    printf("### Sessions/users ###\n");
    if (users_strlen > 0)
    {
        printf("%s", users);
    }
}