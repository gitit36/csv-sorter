# csv-sorter

### A C++ program to sort a CSV file with a number of columns using bubble sort and insert sort algorithms.

### After compiling the program with "make" command, the program can be invoked by: 

./myhie -i InputFile -k NumOfWorkers -r -a AttributeNumber -o Order -s OutputFile

### To explain briefly about the arguments, "NumOfWorkers" indicate the number of workers or sorter nodes you want to make so that each node/process can acquire a chunk of .csv to sort. The "Order" will be whether you want to have the sorting be done in an ascending or a descending order. "OutputFile" is basically the name you assign to the output of the sorted file. You can choose to include -r flag depending on whether you want to assign data ranges to the sorter nodes randomly.


### For example, you can invoke a commandline as such:

./myhie -i 1batch-1000.csv -k 2 -a 3 -o d -s wow.csv


### The output of the above command will look similar to this (NumOfWorkers = 2):

ROOT pid = 39231 and ppid = 83675

COORD pid = 39235 and ppid = 39231

SORTER pid = 39236 and ppid = 39235

SORTER[0] Run time was 0.000000 sec (REAL time) although we used the CPU for 0.000000 sec (CPU time).

SORTER pid = 39238 and ppid = 39235

SORTER[1] Run time was 0.000000 sec (REAL time) although we used the CPU for 0.000000 sec (CPU time).

MERGER pid = 39241 and ppid = 39235

MERGER Run time was 0.100000 sec (REAL time) although we used the CPU for 0.090000 sec (CPU time).

ENTIRE PROCESS Run time was 1.710000 sec (REAL time) although we used the CPU for 0.000000 sec (CPU time).

ROOT has seen 2 SIGUSR1 from Sorters

ROOT has seen 1 SIGUSR2 from Merger

### The output shows the total amount of processing time taken by each sorter, the total amount of time taken for the merger process, and the total amount of time taken for the entire process to complete. I have additionally decided to print a few lines showing the PID & PPID of root/sorter/merger processes just so you could see clearly how the hierarchy has been accurately implemented. Most importantly, you will be able to find a wow.csv (sorted output) in your directory.


### Before the execution of the command, you will see that this program is comprised of 6 c++ files in the directory as the following:

- myhie.cpp
- insertsort.cpp
- bubblesort.cpp
- bubble.h
- bubble.cpp
- Makefile

### Upon compilation, there will be 3 executables in your directory as the following:

- myhie
- insert
- bubble

### The two executables insert and bubble will be executed using execl() inside myhie.
