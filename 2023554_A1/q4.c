#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>//For input

typedef struct Process{
    int pid;
    int remaining_time;//For SRTF and RR
    int arrival_time;
    int burst_time;
    int completion_time;
    int tat;
    int waiting_time;
    int response_time;
    bool finished;//Process status
}Process;

void inputProcesses(Process arrP[], int num_processes){
    for(int i = 0; i < num_processes; i++){
        printf("Enter the process ID, arrival time and burst time for Process %d: ", i + 1);
        scanf("%d %d %d", &arrP[i].pid, &arrP[i].arrival_time, &arrP[i].burst_time);
        arrP[i].remaining_time = arrP[i].burst_time;
        arrP[i].finished = false;
    }
}

int cpuIdle(int chosen_process, int prev_process, Process arrP[], int i, int current_execution_time){
    if(i == 0){
        current_execution_time += arrP[chosen_process].arrival_time;
    }else if(arrP[chosen_process].arrival_time - arrP[prev_process].completion_time > 0){
        current_execution_time += arrP[chosen_process].arrival_time - arrP[prev_process].completion_time;
    }
    return current_execution_time;
}

void updateProcessParameters(Process arrP[], int process_index, int* current_execution_time){
    arrP[process_index].completion_time = *current_execution_time;
    arrP[process_index].tat = arrP[process_index].completion_time - arrP[process_index].arrival_time;
    arrP[process_index].waiting_time = arrP[process_index].tat - arrP[process_index].burst_time;
    arrP[process_index].finished = true;
}

void displayTableAndAverageTimes(Process arrP[], int num_processes){
    int sumTAT = 0;
    int sumRT = 0;

    printf("PID\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\tResponse Time\n");
    for(int i = 0; i < num_processes; i++){
        sumTAT += arrP[i].tat;
        sumRT += arrP[i].response_time;

        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", arrP[i].pid, arrP[i].arrival_time,
               arrP[i].burst_time, arrP[i].completion_time, arrP[i].tat, arrP[i].waiting_time,
               arrP[i].response_time);
    }
    printf("\n");
    printf("Average Turnaround Time: %.2f\n", (float) sumTAT / num_processes);
    printf("Average Response Time: %.2f\n", (float) sumRT / num_processes);
}

int compareByID(const void *a, const void *b){
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return (processA->pid - processB->pid);
}

int findMinArrivalTimeProcess(Process arrP[], int num_processes){
    qsort(arrP, num_processes, sizeof(Process), compareByID);//When two processes have same AT we so consider PID by default
    int process_index = 0;
    int minAT = __INT_MAX__;//So as because at some point the first process of the array would have finished
    for(int i = 0; i < num_processes; i++){
        if(arrP[i].arrival_time < minAT && !arrP[i].finished){
            minAT = arrP[i].arrival_time;
            process_index = i;
        }
    }
    return process_index;
}

void FIFO(Process arrP[], int num_processes){
    printf("\n--First In First Out(FIFO) Process Scheduling Algorithm--\n");

    int current_execution_time = 0;
    int prev_process = -1;//The index of previous process; -1 in case of no previous process yet
    for(int i = 0; i < num_processes; i++){
        int chosen_process = findMinArrivalTimeProcess(arrP, num_processes);
        current_execution_time = cpuIdle(chosen_process, prev_process, arrP, i, current_execution_time);

        arrP[chosen_process].response_time = current_execution_time - arrP[chosen_process].arrival_time;
        current_execution_time += arrP[chosen_process].burst_time;
        updateProcessParameters(arrP, chosen_process, &current_execution_time);

        printf("Process %d -> ", arrP[chosen_process].pid);
        prev_process = chosen_process;
    }
    printf("Completion\n\n");
    displayTableAndAverageTimes(arrP, num_processes);
}

void SJF(Process arrP[], int num_processes){
    printf("\n--Shortest Job First (SJF) Process Scheduling Algorithm--\n");

    int current_execution_time = 0;
    int completedP = 0;
    int chosen_process;
    while(completedP < num_processes){
        int min_BT = __INT_MAX__;
        chosen_process = -1; // The index of the chosen process; -1 if none are available
        for(int i = 0; i < num_processes; i++){//Finding process with minimum burst time among the arrived ones
            if(!arrP[i].finished && arrP[i].arrival_time <= current_execution_time && arrP[i].burst_time < min_BT){
                min_BT = arrP[i].burst_time;
                chosen_process = i;
            }
        }
        if(chosen_process == -1){//In case no process is chosen
            current_execution_time++;
            continue;
        }
        arrP[chosen_process].response_time = current_execution_time - arrP[chosen_process].arrival_time;
        current_execution_time += arrP[chosen_process].burst_time;
        updateProcessParameters(arrP, chosen_process, &current_execution_time);

        completedP++;

        printf("Process %d -> ", arrP[chosen_process].pid);
    }

    printf("Completion\n\n");
    displayTableAndAverageTimes(arrP, num_processes);
}

void SRTF(Process arrP[], int num_processes){
    printf("\n--Shortest Remaining Time First(SRTF) Process Scheduling Algorithm--\n");

    int current_execution_time = 0;
    int completedP = 0;
    int min_remaining_time = __INT_MAX__;
    int chosen_process = -1;
    for(int i = 0; i < num_processes; i++){
        arrP[i].response_time = -1;//Since this is preemptive scheduling, we see of the first time a proces is executed when calculating response time
    }
    while(completedP < num_processes){
        for(int i = 0; i < num_processes; i++){//Finding process with minimum remaining time among the arrived ones
            if(arrP[i].arrival_time <= current_execution_time && !arrP[i].finished && arrP[i].remaining_time < min_remaining_time){
                min_remaining_time = arrP[i].remaining_time;
                chosen_process = i;
            }
        }
        if(chosen_process == -1){//In case no process is chosen
            current_execution_time++;
            continue;
        }
        if(arrP[chosen_process].response_time == -1){//When catering to a process the first time
            arrP[chosen_process].response_time = current_execution_time - arrP[chosen_process].arrival_time;
        }
        arrP[chosen_process].remaining_time--;//Proceeding one uint at a time
        current_execution_time++;
        
        printf("Process %d -> ", arrP[chosen_process].pid);

        if(arrP[chosen_process].remaining_time == 0){//A process gets finished
            completedP++;
            updateProcessParameters(arrP, chosen_process, &current_execution_time);
            //We update in this if statement as it is only here that we get to a completion time(which is necessary for update)
        }
        min_remaining_time = __INT_MAX__;
        chosen_process = -1;
    }
    printf("Completion\n\n");
    displayTableAndAverageTimes(arrP, num_processes);
}

void RR(Process arrP[], int num_processes, int tQ){
    printf("\n--Round Robin(RR) Process Scheduling Algorithm--\n");
    
    int current_execution_time = 0, completedP = 0;
    bool preemption_status;
    for(int i = 0; i < num_processes; i++){
        arrP[i].response_time = -1;//Since this is preemptive scheduling, we see of the first time a proces is executed when calculating response time
    }
    while(completedP < num_processes){
        preemption_status = false;
        for(int i = 0; i < num_processes; i++){
            if(arrP[i].finished || arrP[i].arrival_time > current_execution_time){//Process already executed or not yet arrived
                continue;
            }
            if(arrP[i].remaining_time > tQ){
                if(arrP[i].response_time == -1){//When catering to a process the first time
                    arrP[i].response_time = current_execution_time - arrP[i].arrival_time;
                }
                current_execution_time += tQ;
                arrP[i].remaining_time -= tQ;
                preemption_status = true;

                printf("Process %d -> ", arrP[i].pid);
            }else{
                if(arrP[i].response_time == -1){//When catering to a process the first time
                    arrP[i].response_time = current_execution_time - arrP[i].arrival_time;
                }
                current_execution_time += arrP[i].remaining_time;
                arrP[i].remaining_time = 0;
                updateProcessParameters(arrP, i, &current_execution_time);

                completedP++;

                printf("Process %d -> ", arrP[i].pid);
            }
        }
        if(!preemption_status){//If no process got executed 
        /*SOME LOOPHOLE: or the one that did, got finished(towards its end)
        (in one iteration across the array) and so not preempted*/
            current_execution_time++;
        }
    }
    printf("Completion\n\n");
    displayTableAndAverageTimes(arrP, num_processes);
}

int main(){
    int num_processes, tQ;
    printf("Enter number of processes: ");
    scanf("%d", &num_processes);
    Process* arrP = (Process*)malloc(num_processes * sizeof(Process));
    inputProcesses(arrP, num_processes);

    char schedAlgo[6];//5 characters + null terminator
    printf("Choose scheduling algorithm(FIFO, SJF, SRTF or RR): ");
    scanf("%5s", schedAlgo);
    while(true){
        if(strcmp(schedAlgo, "FIFO") == 0 || strcmp(schedAlgo, "SJF") == 0 || strcmp(schedAlgo, "SRTF") == 0 || strcmp(schedAlgo, "RR") == 0){
            break;
        }
        printf("Please enter a valid scheduling algorithm: ");
        scanf("%5s", schedAlgo);
    }

    if(strcmp(schedAlgo, "FIFO") == 0){
        FIFO(arrP, num_processes);
    }else if(strcmp(schedAlgo, "SJF") == 0){
        SJF(arrP, num_processes);
    }else if(strcmp(schedAlgo, "SRTF") == 0){
        SRTF(arrP, num_processes);
    }else if(strcmp(schedAlgo, "RR") == 0){
        printf("Enter time quantum for Round Robin scheduling: ");
        scanf("%d", &tQ);
        RR(arrP, num_processes, tQ);
    }
    free(arrP);
    return 0;
}