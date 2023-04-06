# Notes #
<ul>
    <li>To compile the program: make</li>
    <li>To run the program: ./mySystemStats</li>
    <li>--system and --user flags are mutually exclusive, use one at a time. if you want to display both, simply don't use either flag and by default it will display both</li>
    <li>samples and tdelay can be specified using flags (samples=N tdelay=T) [order doesn't matter] or corresponding order (N T) [order matters]. but don't use both methods in the same time</li>
    <li>When not using sequential mode, make sure the whole program is visible. The cursor will only update visible areas of the screen.</li>
    <li>You can do this by maximizing the terminal window size and reducing font size of terminal window</li>
</ul>

# Concurency: #
    there will be 3 pipes created for each function
    pipe[0] for cpu Utilization
    pipe[1] for memory Utilization
    pipe[2] for users

    each iteration will fork 1, 2 or 3 times depending on the flags that the user has specified and each process will exit after it has finished writing to the pipe
    if no flags are specified, it will fork 3 times:
        - the first fork will be for cpu utilization, it will use pipe[0][1] to write
        - the second fork will be for memory utilization, it will use pipe[1][1] to write
        - the third fork will be for users, it will use pipe[2][1] to write
    if --system flag is specified, it will fork 2 times:
        - the first fork will be for cpu utilization, it will use pipe[0][1] to write
        - the second fork will be for memory utilization, it will use pipe[1][1] to write
    if --user flag is specified, it will fork 1 times:
        - the fork will be for users, it will use pipe[2][1] to write

    the parrent will wait for the child processes to finish and then read from pipe[0][0], pipe[1][0] and pipe[2][0] and prints the output 

# Graphics: #
#### Each '#' represents +0.01 ####
#### Each ':' represents -0.01 ####
#### There might be a +- 1 discrepancy in the graphics from rounding since only 2 decimal places are displayed ####
#### The Graphics represents virtual memory utilization and NOT physical memory utilization ####
    example:
        |* 0.00
        |#* 0.01    [actual 0.011...]
        |##* 0.01   [actual 0.016...]
        |##* 0.02
        |@ -0.00 
        |:@ -0.01   [actual -0.011...]
        |::@ -0.01  [actual -0.016...]
        |::* 0.02

#### Each '#' represents +1 ####
#### Each ':' represents -1 ####
#### The graphics for cpu utilization is ROUNDED UP to the nearest integer. ####
    example:
        |* 0.00  
        |##* 1.86
        |##* 1.23 
        |@ -0.00 
        |::@ -1.86
        |::@ -1.23

# Calculations: #
## Memory: ##
### Memory Utilization: ### 
    source: <sysinfo.h>
    
    Functions:
    getTotalPMemory()
    total physical Memory = sysinfo.totalram

    getUsedPMemory()
    used physical Memory  = sysinfo.totalram - sysinfo.freeram

    getTotalVMemory()
    total virtual Memory  = sysinfo.totalram + sysinfo.totalswap

    getUsedVMemory()
    Used virtual Memory   = sysinfo.totalram - sysinfo.freeram + sysinfo.totalswap - sysinfo.freeswap

    Note:
    1 gb = bytes / 1024 / 1024 /1024
        
### Current Program Memory Usage (the one on the header) ###
    source: <resource.h>
    Functions: getCurrentProgramMemoryUsage()
    
## CPU: ##
### CPU Utilization: ### 
    source: /proc/stat

    Functions:
    getCPUValues()
    total = user + nice + system + idle + ioWait + irq + softIrq;
    used = total - idle;

    getCPUUtilization(t1)
    t2 = getCPUValues();
    (t2.used - t1.used) / (t2.total - t1.total) * 100;

    note:
    t1 is from getCPUValues() before delay, after the delay, t1 is passed to getCPUValues() and t2 is from getCPUValues() after delay
    i.e.:
    t1 = getCPUValues();
    sleep(tdelay);
    cpuUtilization = getCPUUtilization(t1);

### Core Count: ###
    source: /proc/cpuinfo
    Functions: getCoreCount()


## Users: ##
    source: <utmp.h>
    Functions: getUsers();