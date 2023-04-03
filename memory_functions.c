#include "memory_functions.h"
#include "tools.h"

// round to 2 decimal places
double round2Decimal(double number)
{
    return round(number * 100) / 100.0;
}

// get memory usage of current program (mySystemStats.c)
long getCurrentProgramMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

// get total physical memory
double getTotalPMemory(struct sysinfo sysinfo)
{
    double totalPMemory = (double)sysinfo.totalram;
    return round2Decimal(totalPMemory * sysinfo.mem_unit / 1024 / 1024 / 1024); // convert bytes to GB with 2 decimal places
}

// get used physical memory
double getUsedPMemory(struct sysinfo sysinfo)
{
    double totalUsedPMemory = (double)(sysinfo.totalram - sysinfo.freeram);
    return round2Decimal(totalUsedPMemory * sysinfo.mem_unit / 1024 / 1024 / 1024); // convert bytes to GB with 2 decimal places
}

// get total virtual memory
double getTotalVMemory(struct sysinfo sysinfo)
{
    double totalVMemory = (double)(sysinfo.totalram + sysinfo.totalswap);
    return round2Decimal(totalVMemory * sysinfo.mem_unit / 1024 / 1024 / 1024); // convert bytes to GB with 2 decimal places
}

// get used virtual memory
double getUsedVMemory(struct sysinfo sysinfo)
{
    double totalUsedVMemory = (double)(sysinfo.totalram - sysinfo.freeram + sysinfo.totalswap - sysinfo.freeswap);
    return round2Decimal(totalUsedVMemory * sysinfo.mem_unit / 1024 / 1024 / 1024); // convert bytes to GB with 2 decimal places
}

// get the memory usage
Memory getMemoryUsage()
{
    struct sysinfo memoryInfo;
    sysinfo(&memoryInfo);
    Memory memory;
    memory.usedPhysical = getUsedPMemory(memoryInfo);
    memory.totalPhysical = getTotalPMemory(memoryInfo);
    memory.usedVirtual = getUsedVMemory(memoryInfo);
    memory.totalVirtual = getTotalVMemory(memoryInfo);
    return memory;
}

// print memory header
void printMemoryHeader()
{
    printf(" memory usage: %ld kilobytes\n", getCurrentProgramMemoryUsage());
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}

// helper funtion to print memory graphics
void printMemoryGraphics(double currentVMemory, double previousVMemory)
{
    double difference = 0;
    if (previousVMemory != -1)
    {
        difference = currentVMemory - previousVMemory;
    }
    int barUnits = round(difference * 100);
    printf("   |");
    if (difference >= 0)
    {
        for (int i = 0; i < barUnits; i++)
            printf("#");
        printf("*");
    }
    else
    {
        for (int i = 0; i >= barUnits; i--)
            printf(":");
        printf("@");
    }
    printf(" %.2f (%.2f)", difference, currentVMemory);
}

// print the memory usage
void printMemoryUsage(Memory *memoryArray, int index, int samples, int graphics)
{
    for (int i = 0; i <= index; i++)
    {
        printf(" %.2f GB / %.2f GB  --  %.2f GB / %.2f GB", memoryArray[i].usedPhysical, memoryArray[i].totalPhysical, memoryArray[i].usedVirtual, memoryArray[i].totalVirtual);
        if (graphics == 1 && i != 0)
            printMemoryGraphics(memoryArray[i].usedVirtual, memoryArray[i - 1].usedVirtual);
        if (graphics == 1 && i == 0)
            printMemoryGraphics(memoryArray[i].usedVirtual, -1);
        printf("\n");
    }
    for (int i = index + 1; i < samples; i++)
        printf("\n");
}