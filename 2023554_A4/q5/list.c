#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
//readdir returns NULL in case of no more entries in a directory or a pointer to a struct dirent - representing the next directory entry -- else an error occurs
void list(const char *path){//List all files in the current folder's parent folder
    struct dirent *entry;//dirent structs have an attribute d_name that is used to store the name of an entry in a directory
    DIR *directory_pointer = opendir(path);
    if(directory_pointer == NULL){perror("opendir_failed"); return;}
    while((entry = readdir(directory_pointer))) printf("%s\n", entry -> d_name);
    closedir(directory_pointer);
}

int main(int argc, char *argv[]){
    const char *path;
    if(argc == 1){
        path = ".";
    }else if(argc == 2){
        path = argv[1];
    }else{
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]); exit(EXIT_FAILURE);
    }
    list(path);
    exit(EXIT_SUCCESS);
}