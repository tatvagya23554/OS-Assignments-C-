#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void merge(int arr[], int arr_1[], int arr_1_size, int arr_2[], int arr_2_size){
    int i = 0;
    int j = 0;
    int k = 0;//programming in C -- low level language so no push_back here; neither any .size()
    
    while(i < arr_1_size && j < arr_2_size){
        if(arr_1[i] <= arr_2[j]){
            arr[k] = arr_1[i];
            k++;
            i++;
        }else{
            arr[k] = arr_2[j];
            k++;
            j++;
        }
    }
    while(i < arr_1_size) {
        arr[k++] = arr_1[i++];
    }
    while(j < arr_2_size) {
        arr[k++] = arr_2[j++];
    }
}

void parallelMergeSort(int arr[], int size){
    if(size == 1){
        return;
    }else if(size < 1){
        printf("Cannot sort an array with no elements");
    }else{
        int mid = size / 2;
        int arr_1[mid];
        int arr_2[size - mid];
        for(int i = 0; i < mid; i++){
            arr_1[i] = arr[i];
        }
        for(int i = mid; i < size; i++){
            arr_2[i - mid] = arr[i];
        }
    
        //Size 2 for reading and writing ends
        int pipefd1[2];
        int pipefd2[2];

        //Create pipes for communication between parent and child processes
        //pipefdx[0] and pipefdx[1] are file descriptors
        if(pipe(pipefd1) == -1 || pipe(pipefd2) == -1){
            perror("Pipe failed");
            exit(1);
        }

        pid_t pid_1 = fork();
        if(pid_1 == 0){//cater to left half
            close(pipefd1[0]);//Close the reading end of the pipe
            parallelMergeSort(arr_1, mid);//Recursively sort arr_1 #Until you get to individual elements
            write(pipefd1[1], arr_1, sizeof(int) * mid);//Write sorted arr_1 to the pipe
            close(pipefd1[1]);//Close writing end of the pipe
            exit(0);
        }else if(pid_1 < 0){
            perror("First fork failed");
            exit(1);
        }

        pid_t pid_2 = fork();
        if(pid_2 == 0){//cater to right half
            close(pipefd2[0]);
            parallelMergeSort(arr_2, size - mid);
            write(pipefd2[1], arr_2, sizeof(int) * (size - mid));
            close(pipefd2[1]);
            exit(0);
        }else if(pid_2 < 0){
            perror("Second fork failed");
            exit(1);
        }
        for(int i = 0; i < 2; i++){
            wait(NULL);
        }

        //Closing the copy of file descriptors with the parent
        //If we do not close the writing end, the reading end would not see an EOF and would keep waiting for more data
        close(pipefd1[1]);
        close(pipefd2[1]);
        //Read sorted arr_1 and arr_2 from the pipes
        read(pipefd1[0], arr_1, sizeof(int) * mid);//Read sorted arr_1
        read(pipefd2[0], arr_2, sizeof(int) * (size - mid));//Read sorted arr_2
        close(pipefd1[0]);
        close(pipefd2[0]);
        merge(arr, arr_1, mid, arr_2, size - mid);
    }
}

int main(){
    int array[16] = {64, 89, 52, 38, 59, 18, 93, 55, 21, 60, 73, 42, 99, 1, 9, 83};//16 numbers allocated at compile time
    int size = 16;//Assigned after initializing above array as otherwise we cannot make a fixed lenght array, as above, during runtime #array[size]-->invalid
    printf("Initial unsorted array: ");
    for(int i = 0; i < size; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
    parallelMergeSort(array, size);
    printf("Final sorted array: ");
    for(int i = 0; i < size; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
    return 0;
}