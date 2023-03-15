# Notes #
<ul>
    <li>When not using sequential mode, make sure the whole program is visible. The cursor will only update visible areas of the screen.</li>
    <li>You can do this by maximizing the terminal window size and reducing font size of terminal window</li>
</ul>

# Flags #
## Types: ##
### --system ###
to indicate that only the system usage should be generated
### --user ###
to indicate that only the users usage should be generated
### Notes: ###
<ul>
    <li>if both flags are specified, the program will generate an error message indicating both flags are mutually exclusive </li>
    <li> if no flags are specified, the system usage and user usage will be generated </li>
</ul>

## Samples and Delay: ##
### --samples=N ###
if used, the value N will indicate how many times the statistics are going to be collected and results will be reported based on the N number of repetitions. If no value is indicated, the default value will be 10.
### --tdelay=T ###
to indicate how frequently to sample in seconds. If no value is indicated, the default value will be 1 second.
### Notes: ###
<ul>
    <li>The last two arguments can also be considered as positional arguments. if no flag is indicated, the corresponding is order: samples tdelay. </li>
    <li> User can only choose to either specify with flag or specify with corresponding order. But not both. If the user specify samples and/or delay using both method, the program will give an error message</li>
    <li> samples and tdelay should be integers </li>
</ul>

## Graphics: ##
### --graphics or --g ###
to include graphical output in the cases where a graphical outcome is possible as indicated below.
### Notes: ###
#### Each '#' represents 0.01 ####
#### Each ':' represents -0.01 ####
#### The Graphics represents virtual memory usage and NOT physical memory usage ####
    example:
        |* 0.00
        |#* 0.01 
        |##* 0.02
        |@ -0.00 
        |:@ -0.01
        |::@ -0.02

#### The graphics for cpu usage is ROUNDED UP to the nearest integer. ####
    example:
        |* 0.00  
        |##* 1.86
        |##* 1.23 
        |@ -0.00 
        |::@ -1.86
        |::@ -1.23
        
## Sequential: ##
### --sequential ###
to display sequentially without needing to "refresh" the screen

# Calculations: #
## Memory: ##
    total physical Memory = sysinfo.totalram
    used physical Memory  = sysinfo.totalram - sysinfo.freeram
    total virtual Memory  = sysinfo.totalram + sysinfo.totalswap
    Used virtual Memory   = sysinfo.totalram - sysinfo.freeram + sysinfo.totalswap - sysinfo.freeswap
        source: sysinfo.h
        functions: getTotalPMemory(), getUsedPMemory(), getTotalVMemory(), getUsedVMemory()

    (current program) memory usage = VmSize
        source: /proc/self/status
        function: printMemoryUsage()

### Notes: ###
    1 gb = bytes / 1024 / 1024 /1024

## CPU: ##
    Number of cores = cpu cores
        source: /proc/cpuinfo
        function: printCPUInfo();

    totalUsed = (totalUser + totalNice + totalSystem + totalIrq + totalSoftIrq) 
                - (*prevTotalUser + *prevTotalNice + *prevTotalSystem + *prevTotalIrq + *prevTotalSoftIrq) ;
    total = totalUsed + (totalIdle + totalIoWait) - (*prevTotalIdle + *prevTotalIoWait);
    percent = (totalUsed / total) * 100;
        source: /proc/stat
        function: getCPUUsage();
    There will be 0.1 seconds delay before the program starts displaying the first output to fetch the base sample of the cpu.
    this will be the prevTotalUser,prevTotalNice,prevTotalSystem and prevTotalIdle for the first cpu usage calculation.

## Users: ##
        source: utmp.h
        functions: printUsers();