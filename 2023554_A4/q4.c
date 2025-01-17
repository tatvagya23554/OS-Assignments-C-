#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void functionB(const char *internal_command, const char *file_name){
    int file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(file < 0){perror("open_failed_functionB"); exit(EXIT_FAILURE);}

    pid_t pid = fork();
    if(pid < 0){perror("fork_failed_functionB"); close(file); exit(EXIT_FAILURE);}
    if(pid == 0){
        if(dup2(file, STDOUT_FILENO) < 0 || dup2(file, STDERR_FILENO) < 0){perror("dup2(stdout or stderr)_failed_functionB"); close(file); exit(EXIT_FAILURE);}
        if(execlp("sh", "sh", "-c", internal_command, (char *) NULL) == -1){perror("execlp_failed_functonB"); exit(EXIT_FAILURE);}//sh shell is available on macOS -- my OS
    }else{close(file); wait(NULL);}
}

void funcionA(const char *file_name){
    int file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(file < 0){perror("open_failed_functionA"); exit(EXIT_FAILURE);}
    if(dup2(file, STDOUT_FILENO) < 0){perror("dup2(stdout)_failed_functionA"); close(file); exit(EXIT_FAILURE);}//Signature: int dup2(int old_file_descriptor, int new_file_descriptor);
    printf("Solved q4 part 1\n");
    close(file);
}

int main(int argc, char *argv[]){
    if(argc < 3){fprintf(stderr, "Usage: %s -p1 <name_of_output_file> OR %s -p2 <internal_command> <name_of_output_file>\n", argv[0], argv[0]); exit(EXIT_FAILURE);}//3 because second argument must be the part to proceed with
    //in case file is non-existent, it will be created in the current directory in Terminal(macOS)
    if(strcmp("-p1", argv[1]) == 0){
        funcionA(argv[2]);
    }else if(strcmp("-p2", argv[1]) == 0){//use with ls
        if(argc != 4){fprintf(stderr, "Usage: %s -p2 <internal_command> <name_of_output_file>\n", argv[0]); exit(EXIT_FAILURE);}
        functionB(argv[2], argv[3]);
    }else{fprintf(stderr, "Enter valid part of question; currently entered: %s\n", argv[1]); exit(EXIT_FAILURE);}
    exit(EXIT_SUCCESS);
}