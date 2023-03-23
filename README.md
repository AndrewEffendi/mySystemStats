# Notes #
<ul>
    <li>When not using sequential mode, make sure the whole program is visible. The cursor will only update visible areas of the screen.</li>
    <li>You can do this by maximizing the terminal window size and reducing font size of terminal window</li>
</ul>

# Concurency: #
    each iteration will create 3 pipes
    pipe[0] for cpu Usage
    pipe[1] for memory Usage
    pipe[2] for users

    each iteration will fork 1, 2 or 3 times depending on the flags that the user has specified
    the first fork will be for cpu usage, it will use pipe[0][1] to write to the parent
    the second fork will be for memory usage, it will use pipe[1][1] to write to the parent
    the third fork will be for users, it will use pipe[2][1] to write to the parent

    the parrent will wait for the child processes to finish and then read from pipe[0][0], pipe[1][0] and pipe[2][0] and prints the output 

# Graphics: #
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

# Calculations: #
## Memory: ##
### memory usage: ### 
    source: sysinfo.h
    
    getTotalPMemory()
    total physical Memory = sysinfo.totalram

    getUsedPMemory()
    used physical Memory  = sysinfo.totalram - sysinfo.freeram

    getTotalVMemory()
    total virtual Memory  = sysinfo.totalram + sysinfo.totalswap

    getUsedVMemory()
    Used virtual Memory   = sysinfo.totalram - sysinfo.freeram + sysinfo.totalswap - sysinfo.freeswap
        
### current program memory usage (the one on the header) ###
    getCurrentProgramMemoryUsage()
    source: /proc/self/status  

### Notes: ###
    1 gb = bytes / 1024 / 1024 /1024

## CPU: ##
    source: /proc/cpuinfo

    getCPUValues()
    total = user + nice + system + idle + ioWait + irq + softIrq;
    used = total - idle;

    getCPUUsage()
    (t2.used - t1.used) / (t2.total - t1.total) * 100


## Users: ##
    source: utmp.h
    functions: printUsers();