#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

//Note: we have to generate 4 random numbers for each of the 7 children processes so accordingly know of when we have 4 & 7 in code
void findMean(){
    int arr[4];
    int sum = 0;
    float mean;

    srand(getpid() + time(NULL));

    for(int i = 0; i < 4; i++){
        arr[i] = rand() % 100 + 1;
        sum += arr[i];
    }
    mean = (float) sum / 4;

    printf("Child process (PID %d) -- Mean: %.2f\n", getpid(), mean);
}

int main(){
    pid_t pid;
    for(int i = 0; i < 7; i++){
        pid = fork();
        if(pid < 0){
            perror("Child process creation failed!");
            exit(1);
        }else if(pid == 0){
            findMean();
            exit(0);
        }
    }

    for(int i = 0; i < 7; i++){
        wait(NULL);
    }
    return 0;
}