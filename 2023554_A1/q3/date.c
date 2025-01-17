#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void printDate(const char *format){//A array of characters(a string) that remains constant throughout
    time_t now;//A very typical implementation of printing formatted time using time.h header
    char buffer[80];//Will hold the value of formatted time
    struct tm *tm_info;
    time(&now);
    tm_info = localtime(&now);//Converting current time to the local time
    strftime(buffer, 80, format, tm_info);//formatting local times
    printf("%s\n", buffer);
}
//NOTE: Rubrics mentions that we print the current date and time in "default format" and so the code below
int main(int argc, char *argv[]){
    if(argc > 1){
        if(strcmp(argv[1], "-r") != 1){//RFC 2822(Request for Comments 2822)
            printDate("%a, %d %b %Y %H:%M:%S %z");
        }else if(strcmp(argv[1], "-u") != 1){//UTC(Coordinated Universal Time)
            printDate("%Y-%m-%d %H:%M:%S UTC");
        }else{
            fprintf(stderr, "For %s use only: -u or -r\n", argv[0]);
            exit(1);
        }
    }else{
        printDate("%a %d %b %Y %H:%M:%S %d %Z");//Default format as given in Google Classroom class comments
    }
    return 0;
}