#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>//EXIT_FAILURE and EXIT_SUCCESS are defined here

pid_t pid;
int main(){
    char programs[4][10] = {"./list", "./countw", "./copy", "./move"};
    for(int i = 0; i < 4; i++){
        pid = fork();
        if(pid == 0){
            //Generally the second argument(to execlp) is the name of the program itself
            execlp(programs[i], programs[i], (char *)NULL); perror("execlp_failed"); exit(EXIT_FAILURE);
        }else if(pid < 0){
            perror("fork_failed"); exit(EXIT_FAILURE);
        }
    }
    for(int i = 0; i < 4; i++) wait(NULL);
    exit(EXIT_SUCCESS);
}