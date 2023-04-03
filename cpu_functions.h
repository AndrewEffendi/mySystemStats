#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct CPU
{
    double used;
    double total;
} CPU;

// get the cpu values (user, nice, system, idle, ioWait, irq, softIrq)
CPU getCPUValues();

// get the cpu Usage using t1 and t2
double getCPUUtilization(CPU *t1);

// print all values in cpuUsageArray except the first one (base sample) (used for non-sequential output)
void printCPUUtilization(double *cpuUsageArray, int index, int samples, int graphics);

// get the number of cores
int getCoreCount();

// print the number of cores
void printCoreCount(int coreCount);