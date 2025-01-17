#include <stdio.h>//has the rename function
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void move(const char *source_path, const char *destination_path){
    struct stat directoryInfo;
    if(stat(source_path, &directoryInfo) == -1){perror("stat_source_failed"); return;}

    if(S_ISDIR(directoryInfo.st_mode)){//st_mode has information of the file mode -- It also tells if our stat corresponds to a directory & if so it returns true
        DIR *dir = opendir(source_path);
        if(!dir){perror("opendir_source_failed"); return;}

        struct dirent *entry;
        char source_file_path[1024]; char destination_file_path[1024];

        mkdir(destination_path, 0755);

        while((entry = readdir(dir)) != NULL){//We assign and compare in a single snippet
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;//d_name is "." in case of current directory and ".." in case of parent directory
            snprintf(source_file_path, sizeof(source_file_path), "%s/%s", source_path, entry->d_name); snprintf(destination_file_path, sizeof(destination_file_path), "%s/%s", destination_path, entry->d_name);
            move(source_file_path, destination_file_path);
        }

        closedir(dir); rmdir(source_path);
    }else{rename(source_path, destination_path);}//int rename(const char *oldPath, const char *newPath)
}
int main(int argc, char *argv[]){
    if(argc != 3){fprintf(stderr, "Usage: %s <source_path> <destination_path>\n", argv[0]); exit(EXIT_FAILURE);}
    move(argv[1], argv[2]);
    exit(EXIT_SUCCESS);
}