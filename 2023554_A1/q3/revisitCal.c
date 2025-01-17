#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int zellersCongruence(int month, int year, int day){
    if(month < 3){
        month += 12;
        year--;
    }
    int e = year % 100;//Remove all centuries -- year of the century
    int f = year / 100;//Zero based century
    int g = (int)(day + floor((13 * (month + 1)) / 5)+ e + floor(e / 4) + floor(f / 4) - 2 * f) % 7;
    if(g < 0){
        g += 7;
    }
    return g;
}
void displayCalendar(int month, int year){
    const char *month_names[] = {"January", "February", "March", "April", "May", "June","July", "August", 
                                        "September", "October", "November", "December"};

    printf("Calendar for %s %d\n", month_names[month - 1], year);
    printf("Su Mo Tu We Th Fr Sa\n");
    int first_day = zellersCongruence(month, year, 1);
    printf("First day is: %d\n", first_day);
    for(int i = 0; i < first_day - 1; i++){
        if(first_day - 1 == -1 || first_day == 0){//Saturday or Sunday
            continue;
        }else{
            printf("   ");//Three spaces will help us ensure uniformity in the output and much better readability
        }
    }
    int num_days;//Now calculation the number of days in the month
    if(month == 4 || month == 6 || month == 9 || month == 11){
        num_days = 30;
    }else if(month == 2){//February
        num_days = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;//The ternary conditional operator
    }else{
        num_days = 31;
    }
    for(int day = 1; day <= num_days; day++){
        if((first_day + day) % 7 == 2){//Moving to a new row of the calendar being printed
            printf("\n");
        }
        printf("%2d ", day);//Width of integer = 2
    }
    printf("\n");
}

int main(int argc, char *argv[]){//Nothing mentioned in Rubrics for command line input in this case
    if(argc != 3){
        fprintf(stderr, "Minimum 3 command line arguments required(./program name, month, year)");
        exit(1);
    }
    int year = atoi(argv[2]);//ASCII to integer conversion
    int month = atoi(argv[1]);
    if(month < 1 || month > 12 || year < 1){
        fprintf(stderr, "Please enter a valid month/year.\n");
        exit(1);
    }
    displayCalendar(month, year);
    return 0;
}