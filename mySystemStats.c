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
void printMemoryUsage(){
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
            free(memoryUsage);
            break;
		}
	}
    if(fclose(file)!= 0){
        perror("could not close /proc/self/status");
        exit(1);
    }
}

void printMemoryHeader(){
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
void printMemoryGraphics(double currentVMemory, double *previousVMemory){
    double difference = 0;
    if(*previousVMemory != -1){
        difference = currentVMemory - *previousVMemory;
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

// print used physical memory, total physical memory, used virtual memory, and total virtual memory
void printMemoryInfo(int graphics,double *previousUsedVMemory){
    struct sysinfo memoryInfo;
    sysinfo (&memoryInfo);
    printf (" %.2f GB / %.2f GB  --  %.2f GB / %.2f GB", getUsedPMemory(memoryInfo),getTotalPMemory(memoryInfo),getUsedVMemory(memoryInfo),getTotalVMemory(memoryInfo));
    if(graphics == 1) printMemoryGraphics(getUsedVMemory(memoryInfo),previousUsedVMemory);
    printf ("\n");
    *previousUsedVMemory = getUsedVMemory(memoryInfo);
    
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
void getCPUUsage(double *cpuUsageArray, int i, long long *prevTotalUser, long long *prevTotalNice, long long *prevTotalSystem, long long *prevTotalIdle, long long *prevTotalIoWait, long long *prevTotalIrq, long long *prevTotalSoftIrq){
    double percent;
    double totalUser, totalNice, totalSystem, totalIdle, totalIoWait, totalIrq, totalSoftIrq, total;
    double totalUsed;
    FILE *file = fopen("/proc/stat", "r");
    if(file == NULL){
        perror("Could not open /proc/stat\n"); 
        exit(1);
    }
    fscanf(file, "cpu %lf %lf %lf %lf %lf %lf %lf", &totalUser, &totalNice, &totalSystem, &totalIdle, &totalIoWait, &totalIrq, &totalSoftIrq); // read the first line
    if(fclose(file)!= 0){
        perror("Could not close /proc/stat\n");
        exit(1);
    }
    totalUsed = (totalUser + totalNice + totalSystem + totalIrq + totalSoftIrq) - (*prevTotalUser + *prevTotalNice + *prevTotalSystem + *prevTotalIrq + *prevTotalSoftIrq) ;
    total = totalUsed + totalIdle + totalIoWait - (*prevTotalIdle + *prevTotalIoWait);
    percent = (totalUsed / total) * 100;
    if(i==0) percent = 0; //base sample have no change

    *prevTotalUser = totalUser;
    *prevTotalNice = totalNice;
    *prevTotalSystem = totalSystem;
    *prevTotalIdle = totalIdle;
    *prevTotalIoWait = totalIoWait;
    *prevTotalIrq = totalIrq;
    *prevTotalSoftIrq = totalSoftIrq;

    cpuUsageArray[i] = percent;
}

// print all values in cpuUsageArray except the first one (base sample) (used for non-sequential output)
void printCPUUsage(double *cpuUsageArray,int index,int graphics){
    printf(" total cpu use = %.2f%%\n",cpuUsageArray[index]);
    if (graphics == 1){
        for(int i=1;i<index+1;i++){
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
}

// print only the last value in cpuUsageArray (used for sequential flag)
void printLastCPUUsage(double *cpuUsageArray,int index, int samples,int graphics){
    printf(" total cpu use = %.2f%%\n",cpuUsageArray[index]);
    if (graphics == 1){
        //print upper gap
        for(int i=0;i<index-1;i++)printf("\n"); 
        // print graphics
        printf("         |");
        if(cpuUsageArray[index]>=0){
            for(int i=0;i<ceiling(cpuUsageArray[index]);i++) printf("#");
            printf("o %.2f%%\n",cpuUsageArray[index]);
        }else {
            for(int i=0;i>ceiling(cpuUsageArray[index]);i--) printf(":");
            printf("@ %.2f%%\n",cpuUsageArray[index]);
        }
        // print lower gap
        for(int i=0;i<samples-index;i++) printf("\n");
    }
}

// print the number of cores
void printCPUInfo(){
    printf("---------------------------------------\n");
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
            printf("Number of cores: %s",cpuCoreCount);
            free(cpuCoreCount);
            break;
		}
	}
    if(fclose(file)!= 0){
        perror("Could not close /proc/cpuinfo\n");
        exit(1);
    }
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
        if (strcmp(argv[i], "--graphics") == 0 || strcmp(argv[i], "--g") == 0){
            graphicsFlag = 1;
        }
    }
    free(sub);

    double previousUsedVMemory = -1;
    double *cpuUsageArray= malloc ((samples+1) * sizeof(double)) ;
    long long prevTotalUser,prevTotalUserLow,prevTotalSys,prevTotalIdle,prevTotalIoWait,prevTotalIrq,prevTotalSoftIrq;
    // print output
    if(type==0 || type==1) {
        //take base sample for cpu array
        getCPUUsage(cpuUsageArray,0,&prevTotalUser,&prevTotalUserLow,&prevTotalSys,&prevTotalIdle,&prevTotalIoWait,&prevTotalIrq,&prevTotalSoftIrq);
        usleep(100000); //sleep for 0.1 seconds
    }
    if (sequentialFlag == 0){
        printf("\033[1J"); //delete all above
        printf("\033[H"); //go home
        printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
        int cpuLine = 1;
        if(graphicsFlag == 1) cpuLine += samples;

        // template
        if(type==0 || type==1) {
            printMemoryUsage();
            printMemoryHeader();
            for (int i = 0;i<samples;i++)printf("\n");
            printCPUInfo();
            for (int i = 0;i<cpuLine;i++)printf("\n");
        }

        // refresh screen
        for(int i=0;i<samples;i++){
            if(type==0 || type==1){
                // memory
                printf("\033[s"); //save
                printf("\033[%dA", cpuLine + samples + 5); //up
                printMemoryUsage();
                printf("\033[%dB", 2 + i); //down
                printMemoryInfo(graphicsFlag,&previousUsedVMemory);
                printf("\033[u"); // restore
                //cpu
                printf("\033[s"); //save
                printf("\033[%dA", cpuLine); //up
                getCPUUsage(cpuUsageArray,i+1,&prevTotalUser,&prevTotalUserLow,&prevTotalSys,&prevTotalIdle,&prevTotalIoWait,&prevTotalIrq,&prevTotalSoftIrq);
                printCPUUsage(cpuUsageArray,i+1,graphicsFlag);
                printf("\033[u"); // restore
            }
            if(type==0 || type==2){
                //users
                printf("\033[s"); //save
                printf("\033[0J"); //delete all below
                printf("\033[u"); // restore
                printUsers();
                if(i<samples-1)printf("\033[u"); // don't restore last loop   
            }
            if(i!=samples-1) sleep(tdelay); // don't sleep after last sample
        }    
    }
    if (sequentialFlag == 1){
        printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
        for(int i=0;i<samples;i++){
            printf(">>> itertion %d\n", i+1);
            if(type==0 || type==1) {
                //memory
                printMemoryUsage();
                printMemoryHeader();
                for(int j=0;j<i;j++)printf("\n");
                printMemoryInfo(graphicsFlag,&previousUsedVMemory);
                for(int j=0;j<samples-i-1;j++)printf("\n");
                //cpu
                getCPUUsage(cpuUsageArray,i+1,&prevTotalUser,&prevTotalUserLow,&prevTotalSys,&prevTotalIdle,&prevTotalIoWait,&prevTotalIrq,&prevTotalSoftIrq);
                printCPUInfo();
                printLastCPUUsage(cpuUsageArray,i+1,samples,graphicsFlag);
            }
            if(type==0 || type==2){
                printUsers();
            }
            if(i!=samples-1) sleep(tdelay); // don't sleep after last sample
        }    
    }
    free(cpuUsageArray);
    printSystemInfo(); 
}