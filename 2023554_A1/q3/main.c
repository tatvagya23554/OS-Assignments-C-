#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    const char programs[3][10] = {//An array of strings(1D in terms of strings, 2D in terms of characters)
        "./date", 
        "./cal", 
        "./uptime"
    };

    for(int i = 0; i < 3; i++){
        pid_t pid = fork();
        if(pid < 0){
            perror("Process forking failed");
            exit(1);
        }else if(pid == 0){
            execlp(programs[i], programs[i], (char *)NULL);
            perror("execlp");//In case execlp fails
            exit(1);
        }
    }

    for(int i = 0; i < 3; i++){
        wait(NULL);
    }
    return 0;
}