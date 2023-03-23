#include "./cpu_functions.h"
#include "./tools.h"

// get the cpu usage and store it in an cpuUsageArray
CPU getCPUValues()
{
    CPU cpuValue;
    double user, nice, system, idle, ioWait, irq, softIrq;
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        perror("Could not open /proc/stat\n");
        exit(1);
    }
    fscanf(file, "cpu %lf %lf %lf %lf %lf %lf %lf", &user, &nice, &system, &idle, &ioWait, &irq, &softIrq); // read the first line
    if (fclose(file) != 0)
    {
        perror("Could not close /proc/stat\n");
        exit(1);
    }
    cpuValue.total = user + nice + system + idle + ioWait + irq + softIrq;
    cpuValue.used = cpuValue.total - idle;
    return cpuValue;
}

double getCPUUsage(CPU *t1)
{
    CPU t2 = getCPUValues();
    return (t2.used - t1->used) / (t2.total - t1->total) * 100;
}

// print all values in cpuUsageArray except the first one (base sample) (used for non-sequential output)
void printCPUUsage(double *cpuUsageArray, int index, int samples, int graphics)
{
    printf(" total cpu use = %.2f%%\n", cpuUsageArray[index]);
    if (graphics == 1)
    {
        for (int i = 0; i <= index; i++)
        {
            printf("         |");
            if (cpuUsageArray[i] >= 0)
            {
                for (int j = 0; j < round(cpuUsageArray[i]); j++)
                    printf("#");
                printf("* %.2f%%\n", cpuUsageArray[i]);
            }
            else
            {
                for (int j = 0; j > round(cpuUsageArray[i]); j--)
                    printf(":");
                printf("@ %.2f%%\n", cpuUsageArray[i]);
            }
        }
        for (int i = index + 1; i < samples; i++)
            printf("\n");
    }
}

// get the number of cores
int getCoreCount()
{
    int coreCount = 0;
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file == NULL)
    {
        perror("Could not open /proc/cpuinfo\n");
        exit(1);
    }
    char temp[512];
    char *cpuCoreCount = malloc(sizeof(char *));
    while (fgets(temp, 512, file) != NULL)
    {
        if ((strstr(temp, "cpu cores")) != NULL)
        {
            substr(temp, cpuCoreCount, 12, (int)strlen(temp) - 12);
            coreCount = atoi(cpuCoreCount);
            free(cpuCoreCount);
            break;
        }
    }
    if (fclose(file) != 0)
    {
        perror("Could not close /proc/cpuinfo\n");
        exit(1);
    }
    return coreCount;
}

// print the number of cores
void printCoreCount(int coreCount)
{
    printf("---------------------------------------\n");
    printf("Number of cores: %d\n", coreCount);
}