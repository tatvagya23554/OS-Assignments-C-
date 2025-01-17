#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student{int id; char name[20];};

int main(){
    struct Student students[5] = {{1, "John Doe"}, {2, "Akanksha Bajpai"}, {3, "Ronny Ridwig"}, {4, "Bajrang Jaiswal"}, {5, "Bethlehem Carl"}};
    FILE *file = fopen("students.rec", "wb");
    if(!file){perror("fopen_students.rec_wb"); exit(EXIT_FAILURE);}
    for(int i = 0; i < 5; i++) fwrite(&students[i], sizeof(struct Student), 1, file);
    fclose(file);
    exit(EXIT_SUCCESS);
}