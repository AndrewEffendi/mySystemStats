#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

typedef struct Memory
{
    double usedPhysical;
    double totalPhysical;
    double usedVirtual;
    double totalVirtual;
} Memory;

// round to 2 decimal places
double round2Decimal(double number);

// get memory usage of current program (mySystemStats.c)
long getCurrentProgramMemoryUsage();

// print memory header
void printMemoryHeader();

// get total physical memory
double getTotalPMemory(struct sysinfo sysinfo);

// get used physical memory
double getUsedPMemory(struct sysinfo sysinfo);

// get total virtual memory
double getTotalVMemory(struct sysinfo sysinfo);

// get used virtual memory
double getUsedVMemory(struct sysinfo sysinfo);

// helper funtion to print memory graphics
void printMemoryGraphics(double currentVMemory, double previousVMemory);

// get the memory usage
Memory getMemoryUsage();

// print the memory usage
void printMemoryUsage(Memory *memoryArray, int index, int samples, int graphics);