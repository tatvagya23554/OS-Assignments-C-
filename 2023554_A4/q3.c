#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct Student{int id; char name[20];};

int main(){
    int studentID; printf("Enter student id: "); scanf("%d", &studentID);
    if(studentID < 1 || studentID > 5){fprintf(stderr, "No match found: invalid ID entered.\n"); exit(EXIT_FAILURE);}

    int fd = open("students.rec", O_RDONLY);
    if(fd < 0){perror("open_students.rec_ORDONLY"); exit(EXIT_FAILURE);}

    off_t offset = (studentID - 1) * 24;//Size of a student's record = 24 bytes in total
    if(lseek(fd, offset, SEEK_SET) == (off_t) -1){perror("lseek"); close(fd); exit(EXIT_FAILURE);}

    struct Student studt;
    if(read(fd, &studt, sizeof(struct Student)) != sizeof(struct Student)){perror("read"); close(fd); exit(EXIT_FAILURE);}

    printf("Student name: %s\n", studt.name);
    close(fd);
    exit(EXIT_SUCCESS);
}