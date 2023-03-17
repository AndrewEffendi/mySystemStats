#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <utmp.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>

typedef struct Memory {
    double usedPhysical;
    double totalPhysical;
    double usedVirtual;
    double totalVirtual;
}Memory;

typedef struct CPU {
    double used;
    double total;
}CPU;

void substr(char *str, char *subStr , int start, int len){
    strncpy(subStr, &str[start], len);
    subStr[len] = '\0';
}

int roundNum(double number){
    return (int)(number+0.5);
}

double round2Decimal(double number){
    return roundNum(number * 100)/100.0;
}

int ceiling(double number){
    int temp = (int)number;
    // for numbers already integer
    if(number ==(float)temp) return temp;
    // for negative number
    if(number<0) return temp-1;
    // for poisitve number
    return temp + 1;
}

/*****************
* memory
*****************/
// print memory usage of current program (mySystemStats.c)
int getCurrentProgramMemoryUsage(){
    int memUsage = 0;
    FILE *file = fopen("/proc/self/status", "r");
    if(file == NULL){
        perror("could not open /proc/self/status");
        exit(1);
    }
    char temp[512];
    char *memoryUsage = malloc(sizeof(char*));
    while(fgets(temp, 512, file) != NULL) {
		if(strncmp(temp, "VmSize:", 7) == 0) {
            substr(temp, memoryUsage, 12, (int)strlen(temp)-16);
            printf(" memory usage: %s kilobytes\n",memoryUsage);
            memUsage = atoi(memoryUsage);
            free(memoryUsage);
            break;
		}
	}
    if(fclose(file)!= 0){
        perror("could not close /proc/self/status");
        exit(1);
    }
    return memUsage;
}

void printMemoryHeader(int memoryUsage){
    printf(" memory usage: %d kilobytes\n",memoryUsage);
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}

double getTotalPMemory(struct sysinfo sysinfo){
    double totalPMemory = (double)sysinfo.totalram;
    return round2Decimal(totalPMemory * sysinfo.mem_unit / 1024 / 1024 /1024); // convert bytes to GB with 2 decimal places
}

double getUsedPMemory(struct sysinfo sysinfo){
    double totalUsedPMemory = (double)(sysinfo.totalram - sysinfo.freeram);
    return round2Decimal(totalUsedPMemory * sysinfo.mem_unit / 1024 / 1024 /1024); // convert bytes to GB with 2 decimal places
}

double getTotalVMemory(struct sysinfo sysinfo){
    double totalVMemory = (double)(sysinfo.totalram + sysinfo.totalswap);
    return round2Decimal(totalVMemory * sysinfo.mem_unit / 1024 / 1024 /1024); // convert bytes to GB with 2 decimal places
}

double getUsedVMemory(struct sysinfo sysinfo){
    double totalUsedVMemory = (double)(sysinfo.totalram - sysinfo.freeram + sysinfo.totalswap - sysinfo.freeswap);
    return round2Decimal(totalUsedVMemory * sysinfo.mem_unit / 1024 / 1024 /1024); // convert bytes to GB with 2 decimal places
}

//helper funtion to print memory graphics
void printMemoryGraphics(double currentVMemory, double previousVMemory){
    double difference = 0;
    if(previousVMemory != -1){
        difference = currentVMemory - previousVMemory;
    }
    int barUnits = roundNum(difference * 100);
    printf("   |");
    if(difference >= 0){
        for(int i = 0; i <barUnits; i++) printf("#");
        printf("*");
    }else{
        for(int i = 0; i >=barUnits; i--) printf(":");
        printf("@");
    }
    printf(" %.2f (%.2f)",difference,currentVMemory);
}

Memory getMemoryUsage(){
    struct sysinfo memoryInfo;
    sysinfo (&memoryInfo);
    Memory memory;
    memory.usedPhysical = getUsedPMemory(memoryInfo);
    memory.totalPhysical = getTotalPMemory(memoryInfo);
    memory.usedVirtual = getUsedVMemory(memoryInfo);
    memory.totalVirtual = getTotalVMemory(memoryInfo);
    return memory;
}

// print used physical memory, total physical memory, used virtual memory, and total virtual memory
void printMemoryUsage(Memory *memoryArray, int index, int samples, int graphics){
    for(int i=0;i<=index;i++){
        printf (" %.2f GB / %.2f GB  --  %.2f GB / %.2f GB", memoryArray[i].usedPhysical,memoryArray[i].totalPhysical,memoryArray[i].usedVirtual,memoryArray[i].totalVirtual);
        if(graphics == 1 && i!=0) printMemoryGraphics(memoryArray[i].usedVirtual,memoryArray[i-1].usedVirtual);
        if(graphics == 1 && i==0) printMemoryGraphics(memoryArray[i].usedVirtual,-1);
        printf ("\n");
    }
    for(int i=index+1;i<samples;i++) printf("\n");
}

/*****************
* User
*****************/
// print users and sessions
int printUsers(){
    printf("---------------------------------------\n");
    printf("### Sessions/users ###\n");
    int userCount = 0;
    struct utmp *utmp;
    setutent();
    utmp=getutent();
    while(utmp) {
        if(utmp->ut_type == USER_PROCESS) {
            printf(" %-9s%-11s (%s)\n", utmp->ut_user, utmp->ut_line, utmp->ut_host);
            userCount++;
        }
        utmp=getutent();
    }
    endutent();
    return userCount;
}

/*****************
* CPU
*****************/
// get the cpu usage and store it in an cpuUsageArray
CPU getCPUValues(){
    CPU cpuValue;
    double user, nice, system, idle, ioWait, irq, softIrq;
    FILE *file = fopen("/proc/stat", "r");
    if(file == NULL){
        perror("Could not open /proc/stat\n"); 
        exit(1);
    }
    fscanf(file, "cpu %lf %lf %lf %lf %lf %lf %lf", &user, &nice, &system, &idle, &ioWait, &irq, &softIrq); // read the first line
    if(fclose(file)!= 0){
        perror("Could not close /proc/stat\n");
        exit(1);
    }
    cpuValue.used = user + nice + system + ioWait + irq + softIrq;
    cpuValue.total = user + nice + system + idle + ioWait + irq + softIrq;
    return cpuValue;
}

double getCPUUsage(CPU *t1){
    CPU t2 = getCPUValues();
    return (t2.used - t1->used) / (t2.total - t1->total);
}

// print all values in cpuUsageArray except the first one (base sample) (used for non-sequential output)
void printCPUUsage(double *cpuUsageArray,int index, int samples, int graphics){
    printf(" total cpu use = %.2f%%\n",cpuUsageArray[index]);
    if (graphics == 1){
        //for(int i=0;i<index-1;i++)printf("\n"); //print upper gap
        for(int i=0;i<=index;i++){
            printf("         |");
            if(cpuUsageArray[i]>=0){
                for(int j=0;j<ceiling(cpuUsageArray[i]);j++) printf("#");
                printf("* %.2f%%\n",cpuUsageArray[i]);
            }else {
                for(int j=0;j>ceiling(cpuUsageArray[i]);j--) printf(":");
                printf("@ %.2f%%\n",cpuUsageArray[i]);
            }
        }
    } 
    for(int i=index+1;i<samples;i++) printf("\n");
}

// get the number of cores
int getCoreCount(){
    int coreCount = 0;
    FILE *file = fopen("/proc/cpuinfo", "r");
    if(file == NULL){
        perror("Could not open /proc/cpuinfo\n"); 
        exit(1);
    }
    char temp[512];
    char *cpuCoreCount = malloc(sizeof(char*));
    while(fgets(temp, 512, file) != NULL) {
		if((strstr(temp, "cpu cores")) != NULL) {
            substr(temp, cpuCoreCount, 12, (int)strlen(temp)-12);
            coreCount = atoi(cpuCoreCount);
            free(cpuCoreCount);
            break;
		}
	}
    if(fclose(file)!= 0){
        perror("Could not close /proc/cpuinfo\n");
        exit(1);
    }
    return coreCount;
}

// print the number of cores
void printCoreCount(int coreCount){
    printf("---------------------------------------\n");
    printf("Number of cores: %d\n",coreCount);
}

/*****************
* System Info
*****************/
// print the system information
void printSystemInfo(){
    struct utsname utsname;
    uname(&utsname);
    printf("---------------------------------------\n");
    printf("### System Information ###\n");
    printf(" System Name = %s\n",utsname.sysname);
    printf(" Machine Name = %s\n",utsname.nodename);
    printf(" Version = %s\n",utsname.version);
    printf(" Release = %s\n",utsname.release);
    printf(" Architecture = %s\n",utsname.machine);
    printf("---------------------------------------\n");
}

/*****************
* Main Function
*****************/
int main(int argc, char **argv){
    // type 0: system and user, type 1: system, type 2: user
    int type = 0;
    int samples = 10;
    int tdelay = 1;
    int sequentialFlag = 0;
    int graphicsFlag = 0;
    // -1: no sample or delay declared, 0: declared sample or delay with flag, 1: decaled sample or delay with corresponding order
    int sdFlag = -1;
    // 0: current value for sample, 1: current value for delay (when declared using corresponding order)
    int sampleDelayOrder = 0;
    char *sub = malloc(sizeof(char*));
    
    for(int i = 1; i < argc; i++){
        // check type
        if (strcmp(argv[i], "--system") == 0) {
            if (type == 2){
                fprintf(stderr,"system flag and user flag are mutually exclusive. please select one option. if you want to display both, simply don't use both flags.\n");
                exit(1);
            }
            type = 1;
        }
        if (strcmp(argv[i], "--user") == 0) {
            if (type == 1){
                fprintf(stderr,"system flag and user flag are mutually exclusive. please select one option. if you want to display both, simply don't use both flags.\n");
                exit(1);
            }
            type = 2;
        }

        // check sequential
        if (strcmp(argv[i], "--sequential") == 0) sequentialFlag = 1;

        // check samples with flag
       if(strstr(argv[i], "--samples=")!=NULL){
            if (sdFlag == 0){
                fprintf(stderr,"Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            substr(argv[i], sub, 10, (int)strlen(argv[i])-10);
            samples = atoi(sub);
            sdFlag = 1;
       }
        
        // check delay with flag
        if(strstr(argv[i], "--tdelay=")!=NULL){
            if (sdFlag == 0){
                fprintf(stderr,"Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            substr(argv[i], sub, 9, (int)strlen(argv[i])-9);
            tdelay = atoi(sub);
            sdFlag = 1;
        }

        // check samples and delay without flag
        for (int j=0;j<strlen (argv[i]); j++){
            if (!isdigit(argv[i][j])) break; // its not a number or specify sample and delay with flag
            if (sdFlag == 1){
                fprintf(stderr,"Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            if(sampleDelayOrder == 0){
                samples = atoi(argv[i]); 
                sampleDelayOrder = 1;
            }else if(sampleDelayOrder == 1){
                tdelay = atoi(argv[i]);
            }
            sdFlag = 0;
        }
        // check graphics
        if (strcmp(argv[i], "--graphics") == 0){
            graphicsFlag = 1;
        }
    }
    free(sub);

    double CPU_Array[samples] ;
    Memory Memory_Array[samples] ;
    CPU t1;
    int coreCount = 0;
    int currentProgMemUsage = 0;
    if(type==0 || type==1) {
        coreCount = getCoreCount();
        currentProgMemUsage = getCurrentProgramMemoryUsage();
    }

    if (sequentialFlag == 0){
        printf("\033[1J"); //delete all above
        printf("\033[H"); //go home
        printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
        for(int i=0;i<samples;i++){
            if(type==0 || type==1) 
                t1 = getCPUValues();
            sleep(tdelay);
            printf("\033[1J"); //delete all above
            printf("\033[H"); //go home
            printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
            if(type==0 || type==1) {
                printMemoryHeader(currentProgMemUsage);
                Memory_Array[i] = getMemoryUsage();
                printMemoryUsage(Memory_Array, i, samples, graphicsFlag);
            }
            if(type==0 || type==2){
                printUsers();
            }
            if(type==0 || type==1) {
                printCoreCount(coreCount);
                CPU_Array[i] = getCPUUsage(&t1);
                printCPUUsage(CPU_Array, i, samples, graphicsFlag);
            }
        }    
    }
    if (sequentialFlag == 1){
        printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
        for(int i=0;i<samples;i++){\
            if(type==0 || type==1) 
                t1 = getCPUValues();
            sleep(tdelay);
            printf(">>> itertion %d\n", i+1);
            if(type==0 || type==1) {
                //memory
                printMemoryHeader(currentProgMemUsage);
                Memory_Array[i] = getMemoryUsage();
                printMemoryUsage(Memory_Array, i, samples, graphicsFlag);
            }
            if(type==0 || type==2){
                printUsers();
            }
            if(type==0 || type==1) {
                printCoreCount(coreCount);
                CPU_Array[i] = getCPUUsage(&t1);
                printCPUUsage(CPU_Array, i, samples, graphicsFlag);
            }
        }    
    }
    printSystemInfo(); 
}