#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct data{char name[64]; double min_temperature; double max_temperature; double sum_all_temperatures; int count;}StationWiseData;

StationWiseData stations[50000000]; int station_count = 0;//A maximum of 50 million permissible rows in txt file

void parse_line(char *line){
    char *station_name = strtok(line, ";"); char *temperature_tokenized_string = strtok(NULL, "\n");

    if(station_name && temperature_tokenized_string){
        double temperature = atof(temperature_tokenized_string);
        for(int i = 0; i < station_count; i++){
            if(strcmp(stations[i].name, station_name) == 0){
                if(temperature < stations[i].min_temperature) stations[i].min_temperature = temperature;
                if(temperature > stations[i].max_temperature) stations[i].max_temperature = temperature;
                stations[i].sum_all_temperatures += temperature;
                stations[i].count++;
                return;
            }
        }
        strcpy(stations[station_count].name, station_name);
        stations[station_count].min_temperature = temperature; stations[station_count].max_temperature = temperature; stations[station_count].sum_all_temperatures = temperature;
        stations[station_count].count = 1; station_count++;
    }
}

void case_mmap(const char *file_name){
    int fd = open(file_name, O_RDONLY);
    if(fd < 0){perror("open_mmap_failed"); exit(EXIT_FAILURE);}

    struct stat my_stat;
    if(fstat(fd, &my_stat) == -1){perror("fstat_mmap_failed"); exit(EXIT_FAILURE);}

    char *file_content = mmap(NULL, my_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(file_content == MAP_FAILED){perror("mmap_failed"); exit(EXIT_FAILURE);}

    madvise(file_content, my_stat.st_size, MADV_SEQUENTIAL);

    char *line_start = file_content;
    char *line_end;
    while((line_end = memchr(line_start, '\n', file_content + my_stat.st_size - line_start)) != NULL){*line_end = '\0'; parse_line(line_start); line_start = line_end + 1;}
    munmap(file_content, my_stat.st_size);
    close(fd);
}

void case_fread(const char *file_name){
    FILE *file = fopen(file_name, "r");
    if(!file){perror("fopen_fread_failed"); exit(EXIT_FAILURE);}
    char line[128];//Assuming a reasonable length of 128 characters for a line in our corresponding txt file
    while(fgets(line, sizeof(line), file)) parse_line(line);
    fclose(file);
}

void outputstationspecificMinsMeansMaxs(){
    for(int i = 0; i < station_count; i++){
        double mean_temp = stations[i].sum_all_temperatures / stations[i].count;
        printf("%s min=%.3f mean=%.3f max=%.3f\n", stations[i].name, stations[i].min_temperature, mean_temp, stations[i].max_temperature);
    }
}

int main(int argc, char *argv[]){
    if(argc != 2){fprintf(stderr, "Usage: %s <file_name>\n", argv[0]); exit(EXIT_FAILURE);}

    const char *file_name = argv[1];
    clock_t begin = clock(); case_mmap(file_name); clock_t finish = clock();
    double mmap_time = (double)(finish - begin) / CLOCKS_PER_SEC;
    printf("\nOutput using mmap:\n");
    outputstationspecificMinsMeansMaxs();
    printf("Time taken with mmap: %.3f seconds\n", mmap_time);

    memset(stations, 0, sizeof(stations)); station_count = 0;

    begin = clock(); case_fread(file_name); finish = clock();
    double fread_time = (double)(finish - begin) / CLOCKS_PER_SEC;
    printf("Output using fread:\n");
    outputstationspecificMinsMeansMaxs();
    printf("Time taken with fread: %.3f seconds\n", fread_time);

    exit(EXIT_SUCCESS);
}