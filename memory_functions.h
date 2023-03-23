#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/sysinfo.h>

typedef struct Memory
{
    double usedPhysical;
    double totalPhysical;
    double usedVirtual;
    double totalVirtual;
} Memory;

double round2Decimal(double number);
// print memory usage of current program (mySystemStats.c)
int getCurrentProgramMemoryUsage();

void printMemoryHeader(int memoryUsage);

double getTotalPMemory(struct sysinfo sysinfo);

double getUsedPMemory(struct sysinfo sysinfo);

double getTotalVMemory(struct sysinfo sysinfo);

double getUsedVMemory(struct sysinfo sysinfo);
// helper funtion to print memory graphics
void printMemoryGraphics(double currentVMemory, double previousVMemory);

Memory getMemoryUsage();

// print used physical memory, total physical memory, used virtual memory, and total virtual memory
void printMemoryUsage(Memory *memoryArray, int index, int samples, int graphics);
