#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//Note: we have a fixed 16 number of elements in our array so accordingly know when we have 16 in code
void binarySearch(int arr[], int low, int high, int target){
    if(low > high){
        printf("Target value's index: -1\n");
        exit(0);
    }

    int mid = (low + high) / 2;
    if(arr[mid] == target){
        printf("Target value's index: %d", mid);
        exit(0);
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("Child process creation failed!");
        exit(1);
    }else if(pid == 0){
        if(target < arr[mid]){
            binarySearch(arr, low, mid - 1, target);
        }else{
            binarySearch(arr, mid + 1, high, target);
        }
    }else{
        wait(NULL);
    }
}

int main(){
    int arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int target;
    printf("Array: ");
    for(int i = 0; i < 16; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("Enter a target value: ");
    scanf("%d", &target);
    binarySearch(arr, 0, 16 - 1, target);
    return 0;
}