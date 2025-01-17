#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void copy(const char *file_path, const char *directory_path){
    FILE *source_file = fopen(file_path, "rb");//read in binary mode
    if(!source_file){perror("fopen_source_failed"); return;}

    char dest_direc_path[1024];
    snprintf(dest_direc_path, sizeof(dest_direc_path), "%s/%s", directory_path, strrchr(file_path, '/') + 1);

    FILE *destination_file = fopen(dest_direc_path, "wb");//write, after truncating, in binary mode
    if(!destination_file){perror("fopen_directory_failed");
        fclose(source_file);//We close source file in case we cannot open the destination directory
        return;
    }

    char buffer[1024]; size_t bytes;
    while((bytes = fread(buffer, 1, sizeof(buffer), source_file)) > 0) fwrite(buffer, 1, bytes, destination_file);

    fclose(source_file); fclose(destination_file);

    printf("%s copied to %s\n", file_path, directory_path);
}

int main(int argc, char *argv[]){
    if(argc != 3){fprintf(stderr, "Usage: %s <file_path> <directory_path>\n", argv[0]); exit(EXIT_FAILURE);}
    copy(argv[1], argv[2]);
    exit(EXIT_SUCCESS);
}