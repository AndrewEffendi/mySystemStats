#include "./system_info_functions.h"

// print the system information
void printSystemInfo()
{
    struct utsname utsname;
    uname(&utsname);
    printf("---------------------------------------\n");
    printf("### System Information ###\n");
    printf(" System Name = %s\n", utsname.sysname);
    printf(" Machine Name = %s\n", utsname.nodename);
    printf(" Version = %s\n", utsname.version);
    printf(" Release = %s\n", utsname.release);
    printf(" Architecture = %s\n", utsname.machine);
    printf("---------------------------------------\n");
}