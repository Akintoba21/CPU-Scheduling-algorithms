# CPU-Scheduling-algorithms
A Project to simulate fcfs, sjf, srt, and rr cpu scheduling algorithms
This is a project to simulate 4 different CPU scheduling algorithms: first come first serve, shortest job first, shortest remaining time first, and round robbin. The first function in the project, generate bursts, will generate a number of cpu and io bound processes with randomized arrival and burst times, which are then stored in a map. The following functions then simulate cpu scheduling for each of the scheduling algorithms.

For first come first serve, the processes are scheduled according to their arrival times and there is no interruption that takes place
For shortest job first, the processes are scheduled according to their burst times. While this algorithm can be preeemptive, for the purpose of this project I used a non-preemptive sjf algorithm
For shortest remaining time, the processes are scheduled according the the remaining time of their burst, preempting the current running proccess if the burst time is shorter
Lastly, for round robin, there is a set time allocated for each process to run. If the burst time is longer than the allocated time, the process will be interrupted and moved to the end of the queue, cycling through until all of the processes have finished running.
