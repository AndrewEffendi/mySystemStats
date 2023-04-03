#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"
#include "memory_functions.h"
#include "users_functions.h"
#include "cpu_functions.h"
#include "system_info_functions.h"

/*****************
 * Main Function
 *****************/
int main(int argc, char **argv)
{
    // intercept signal handlers
    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, c_handler);
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
    char *sub = malloc(sizeof(char *));

    for (int i = 1; i < argc; i++)
    {
        // check type
        if (strcmp(argv[i], "--system") == 0)
        {
            if (type == 2)
            {
                fprintf(stderr, "system flag and user flag are mutually exclusive. please select one option. if you want to display both, simply don't use both flags.\n");
                exit(1);
            }
            type = 1;
        }
        if (strcmp(argv[i], "--user") == 0)
        {
            if (type == 1)
            {
                fprintf(stderr, "system flag and user flag are mutually exclusive. please select one option. if you want to display both, simply don't use both flags.\n");
                exit(1);
            }
            type = 2;
        }

        // check sequential
        if (strcmp(argv[i], "--sequential") == 0)
            sequentialFlag = 1;

        // check samples with flag
        if (strstr(argv[i], "--samples=") != NULL)
        {
            if (sdFlag == 0)
            {
                fprintf(stderr, "Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            substr(argv[i], sub, 10, (int)strlen(argv[i]) - 10);
            samples = atoi(sub);
            sdFlag = 1;
        }

        // check delay with flag
        if (strstr(argv[i], "--tdelay=") != NULL)
        {
            if (sdFlag == 0)
            {
                fprintf(stderr, "Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            substr(argv[i], sub, 9, (int)strlen(argv[i]) - 9);
            tdelay = atoi(sub);
            sdFlag = 1;
        }

        // check samples and delay without flag
        for (int j = 0; j < strlen(argv[i]); j++)
        {
            if (!isdigit(argv[i][j]))
                break; // its not a number or specify sample and delay with flag
            if (sdFlag == 1)
            {
                fprintf(stderr, "Please specify samples and tdelay with flags or with correspoinding order, but not both.\n");
                exit(1);
            }
            if (sampleDelayOrder == 0)
            {
                samples = atoi(argv[i]);
                sampleDelayOrder = 1;
            }
            else if (sampleDelayOrder == 1)
            {
                tdelay = atoi(argv[i]);
            }
            sdFlag = 0;
        }
        // check graphics
        if (strcmp(argv[i], "--graphics") == 0)
        {
            graphicsFlag = 1;
        }
    }
    free(sub);

    double CPU_Array[samples];
    Memory Memory_Array[samples];
    CPU t1;
    int coreCount = 0;
    long currentProgMemUsage = 0;
    double cpuUsage = 0;
    Memory memoryUsage;
    int child_num = 0;
    // fd for pipe
    int fd[3][2];
    int pid1; // for cpu usage
    int pid2; // for memory usage
    int pid3; // for user
    int status;
    char *users;
    char result[2048];
    int users_strlen;
    // get core count and current progam memory usage (header)
    if (type == 0 || type == 1)
    {
        coreCount = getCoreCount();
        currentProgMemUsage = getCurrentProgramMemoryUsage();
    }

    // Initial Header
    if (sequentialFlag == 0)
    {
        printf("\033[1J"); // delete all above
        printf("\033[H");  // go home
    }
    printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);

    // main loop for each sample
    for (int i = 0; i < samples; i++)
    {
        users = NULL;
        users_strlen = 0;
        // pipe
        for (int i = 0; i < 3; i++)
        {
            if (pipe(fd[i]) < 0)
            {
                perror("pipe failed");
                exit(1);
            }
        }

        if (type == 0 || type == 1)
        {
            // fork for cpu usage
            if ((pid1 = fork()) < 0)
            {
                perror("fork failed");
                exit(1);
            }
            else if (pid1 == 0)
            {
                // ignore signals to prevent triggering in multiple processes
                signal(SIGINT, SIG_IGN);
                // close unused end
                if (close(fd[0][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[1][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[1][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[2][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[2][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }

                t1 = getCPUValues();
                sleep(tdelay);
                cpuUsage = getCPUUsage(&t1);
                if (write(fd[0][1], &cpuUsage, sizeof(double)) < 0)
                {
                    fprintf(stderr, "write CPU pipe failed\n");
                    exit(1);
                }
                // close write end
                if (close(fd[0][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                exit(0);
            }

            // fork for memory usage
            if ((pid2 = fork()) < 0)
            {
                perror("fork failed");
                exit(1);
            }
            else if (pid2 == 0)
            {  
                // ignore signals to prevent triggering in multiple processes
                signal(SIGINT, SIG_IGN);
                // close unused end
                if (close(fd[1][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[0][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[0][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[2][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[2][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                memoryUsage = getMemoryUsage();
                if (write(fd[1][1], &memoryUsage, sizeof(Memory)) < 0)
                {
                    fprintf(stderr, "write Memory pipe failed\n");
                    exit(1);
                }
                // close write end
                if (close(fd[1][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                exit(0);
            }
        }

        if (type == 0 || type == 2)
        {
            // fork for users
            if ((pid3 = fork()) < 0)
            {
                perror("fork failed");
                exit(1);
            }
            else if (pid3 == 0)
            {
                // ignore signals to prevent triggering in multiple processes
                signal(SIGINT, SIG_IGN);
                // close unused end
                if (close(fd[2][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[0][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[0][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[1][0]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                if (close(fd[1][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                getUsers(&users);
                if (users == NULL)
                {
                    exit(0);
                }
                users_strlen = strlen(users);
                if (write(fd[2][1], &users_strlen, (sizeof(int))) < 0)
                {
                    fprintf(stderr, "write Users pipe failed\n");
                    exit(1);
                }
                if (write(fd[2][1], users, sizeof(char) * (strlen(users) + 1)) < 0)
                {
                    fprintf(stderr, "write Users pipe failed\n");
                    exit(1);
                }
                free(users);
                // close write end
                if (close(fd[2][1]) == -1)
                {
                    perror("close failed");
                    exit(1);
                }
                exit(0);
            }
        }

        // parent process
        // wait all children
        if (type == 0)
            child_num = 3;
        if (type == 1)
            child_num = 2;
        if (type == 2){
            child_num = 1;
            // if type 0 or type 1, don't need to sleep again since cpu forked process already sleep for tdelay seconds
            // but if type 2 (--user flag), need to sleep since cpu forked process not executed. (i.e. not sleep in cpu forked process)
            sleep(tdelay);
        }
            
        for (int i = 0; i < child_num; i++)
        {
            wait(&status);
        }

        // close write end
        if (close(fd[0][1]) == -1)
        {
            perror("close failed");
            exit(1);
        }
        if (close(fd[1][1]) == -1)
        {
            perror("close failed");
            exit(1);
        }
        if (close(fd[2][1]) == -1)
        {
            perror("close failed");
            exit(1);
        }
        // closed unused  read ends
        if (type == 1)
        {
            if (close(fd[2][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
        }
        if (type == 2)
        {
            if (close(fd[0][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
            if (close(fd[1][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
        }

        // read from pipe
        if (type == 0 || type == 1)
        {
            if (read(fd[0][0], &CPU_Array[i], sizeof(double)) < 0)
            {
                fprintf(stderr, "read CPU pipe failed\n");
                exit(1);
            }
            if (close(fd[0][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
            if (read(fd[1][0], &Memory_Array[i], sizeof(Memory)) < 0)
            {
                fprintf(stderr, "read Memory pipe failed\n");
                exit(1);
            }
            // close read end
            if (close(fd[1][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
        }
        if (type == 0 || type == 2)
        {
            if (read(fd[2][0], &users_strlen, sizeof(int)) < 0)
            {
                fprintf(stderr, "read Users pipe failed\n");
                exit(1);
            }
            if (users_strlen > 0)
            {
                if (read(fd[2][0], result, sizeof(char) * (users_strlen + 1)) < 0)
                {
                    fprintf(stderr, "read Users pipe failed\n");
                    exit(1);
                }
            }
            // close read end
            if (close(fd[2][0]) == -1)
            {
                perror("close failed");
                exit(1);
            }
        }

        // main print
        if (sequentialFlag == 0)
        {
            printf("\033[1J"); // delete all above
            printf("\033[H");  // go home
            printf("Nbr of samples: %d -- every %d sec\n", samples, tdelay);
        }
        else
        {
            printf(">>> itertion %d\n", i + 1);
        }

        if (type == 0 || type == 1)
        {
            printMemoryHeader(currentProgMemUsage);
            printMemoryUsage(Memory_Array, i, samples, graphicsFlag);
        }
        if (type == 0 || type == 2)
        {
            printUsers(result, users_strlen);
        }
        if (type == 0 || type == 1)
        {
            printCoreCount(coreCount);
            printCPUUsage(CPU_Array, i, samples, graphicsFlag);
        }
    }
    printSystemInfo();
}