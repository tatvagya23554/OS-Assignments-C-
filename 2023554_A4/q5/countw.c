#include <stdio.h>
#include <stdlib.h>

int countw(FILE *file){
    int count = 0;
    char curr, prev; curr = ' '; prev = ' ';
    while((curr = fgetc(file)) != EOF){
        if((curr == ' ' || curr == '\n' || curr == '\t') && prev != ' ' && prev != '\n' && prev != '\t'){count++;}
        prev = curr;
    }
    return count;
}

int main(int argc, char *argv[]){//Note that argv is an array of strings
    if(argc != 2){fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);exit(EXIT_FAILURE);}

    FILE *file = fopen(argv[1], "r");//Returns NULL in case of failure to open file
    if(!file){perror("fopen_failed"); exit(EXIT_FAILURE);}

    int word_count = countw(file);
    fclose(file);

    printf("Word count: %d\n", word_count);
    exit(EXIT_SUCCESS);
}