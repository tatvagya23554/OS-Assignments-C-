#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//#define MY_CLOCKS_PER_SEC 100
//Below two macros are for part 2
#define QUEUE_LENGTH 100
#define NUM_THREADS 8//Set based on my CPU cores -- 4 performance and 4 efficiency macOS M1 chip

//Part 1
typedef struct myStruct{
    int row, col;
    int n;//Common dimension among matrices A and B -- useful for finding the dot product of a row of A with a column of B(to find an element of matrix C -- in computeElement method)
    //Pointers to matrices -- each pointer individually points to an array of pointers where each of the later pointers points to a row of elements
    int** A;
    int** B;
    int** C;
}ThreadData;

void* computeElement(void* arg){
    ThreadData* data = (ThreadData*)arg;
    int sum = 0;
    for(int i = 0; i < data->n; i++){//To find an element, we iterate over that dimension that makes the matrices compatible for multiplication
        sum += data->A[data->row][i] * data->B[i][data->col];
    }
    data->C[data->row][data->col] = sum;
    pthread_exit(NULL);//Function meant to be used as a thread function -- equivalent to returning NULL in the context of threads
}

//Part 2
typedef struct myStruct_1{
    ThreadData tasks[QUEUE_LENGTH];
    int head, tail, count;//Count keeps track of the number of tasks currently in the queue.
    pthread_mutex_t lock;//Ensures mutual exclusion when accessing or modifying the queue.
    pthread_cond_t blockingCondition;//Used to signal and block threads when the queue is empty or full.
}TaskQueue;

TaskQueue queue;
int areThreadsBusy = 1;//To manage the termination of worker threads when one has no more tasks to complete

void enqueue(ThreadData task){
    pthread_mutex_lock(&queue.lock);
    while(queue.count == QUEUE_LENGTH){
        pthread_cond_wait(&queue.blockingCondition, &queue.lock);
    }
    queue.tasks[queue.tail] = task;
    queue.tail = (queue.tail + 1) % QUEUE_LENGTH;
    queue.count += 1;
    pthread_cond_signal(&queue.blockingCondition);
    pthread_mutex_unlock(&queue.lock);
}

ThreadData dequeue(){
    pthread_mutex_lock(&queue.lock);
    while(queue.count == 0 && areThreadsBusy){
        pthread_cond_wait(&queue.blockingCondition, &queue.lock);
    }
    ThreadData task = queue.tasks[queue.head];
    queue.head = (queue.head + 1) % QUEUE_LENGTH;
    queue.count--;
    pthread_cond_signal(&queue.blockingCondition);
    pthread_mutex_unlock(&queue.lock);
    return task;
}

void* handleWorkerThread(){//void* arg is not needed as a parameter for pthread_create method does not necessarily require such an argument in that way -- we also handle the error we get while running our Makefile by doing so
    while(areThreadsBusy){
        ThreadData task = dequeue();
        if(task.row == -1){
            continue;
        }
        int sum = 0;
        for(int k = 0; k < task.n; k++){
            sum += task.A[task.row][k] * task.B[k][task.col];
        }
        task.C[task.row][task.col] = sum;
    }
    pthread_exit(NULL);
}

//Input and Output
void inputMatrix(int** matrix, char matrixName, int rows, int columns){//char must be given as an argument in '' -- single quotes
    printf("Enter elements of matrix %c\n", matrixName);//Array of characters as second argument to print string in C -- however, here we use a capital letter as matrix name
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            scanf("%d", &matrix[i][j]);
        }
    }
}

void outputMatrix(int** matrix, char matrixName, int rows, int columns){
    printf("Resultant matrix %c\n", matrixName);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(){
    int m, n, p;
    printf("Enter dimensions of matrix A(m x n): ");
    scanf("%d %d", &m, &n);
    printf("Enter dimensions of matrix B(n x p): ");
    scanf("%d %d", &n, &p);

    int** A = (int**)malloc(m * sizeof(int*));
    int** B = (int**)malloc(n * sizeof(int*));
    int** C = (int**)malloc(m * sizeof(int*));

    int i = 0;
    while(i < m){
        A[i] = (int*)malloc(n * sizeof(int));
        i += 1;
    }
    i = 0;
    while(i < n){
        B[i] = (int*)malloc(p * sizeof(int));
        i += 1;
    }
    i = 0;
    while(i < m){
        C[i] = (int*)malloc(p * sizeof(int));
        i += 1;
    }

    inputMatrix(A, 'A', m, n);
    inputMatrix(B, 'B', n, p);
    printf("\nCLOCKS_PER_SEC = %d\n", CLOCKS_PER_SEC);

    //Part 1
    printf("\nPart 1:\n");
    //In both the arrays below, we make threads to be at specific indices when initializing them
    pthread_t threads[m * p];//Declaring an array of POSIX threads
    ThreadData tasks[m * p];//Declaring an array with data for each of the threads
    clock_t begin, finish;

    //Sequential multiplication
    begin = clock();
    for(int i = 0; i < m; i++){//Iterate over the number of rows, to be, in the resultant matrix C
        for(int j = 0; j < p; j++){//Iterate over the number of columns, to be, in the resultant matrix C
            int sum = 0;
            for(int k = 0; k < n; k++){//Multiply each element of a particular row of A with each element of a particular column of B -- to get to a row of C
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    finish = clock();
    double sequentialTime = (double)(finish - begin) / CLOCKS_PER_SEC;


    for(int i = 0; i < m; i++){
        for(int j = 0; j < p; j++){
            C[i][j] = 0;
        }
    }


    //Parallel multiplication
    begin = clock();
    int threadCount = 0;
    for(int i = 0; i < m; i++){
        for(int j = 0; j < p; j++){
            //Initialize thread with data necessary for element computation afterwards
            tasks[threadCount] = (ThreadData){.row = i, .col = j, .n = n, .A = A, .B = B, .C = C};//Using compound literals -- cleaner and readable code 
            //by directly initializing structure members at the point assignment -- (of type ThreadData) to initialize structures of types ThreadData
            //The part withing the braces is the initializer list
            //Create a thread, at threadCount index of the threads array, for computing an element, using computeElement function and relavant data found from tasks array, of the resultant matrix 
            pthread_create(&threads[threadCount], NULL, computeElement, &tasks[threadCount]);//NULL => default thread attributes
            threadCount += 1;
        }
    }
    //Waiting for all threads to finish before proceeding
    i = 0;
    while(i < threadCount){//By this time, threadCount would equal the total number of threads we have
        pthread_join(threads[i], NULL);//Waiting for i th thread to finish
        i += 1;
    }
    finish = clock();
    double parallelTime = (double)(finish - begin) / CLOCKS_PER_SEC;

    outputMatrix(C, 'C', m, p);

    printf("Sequential time(one thread per element): %f seconds\n", sequentialTime);
    printf("Parallel time(one thread per element): %f seconds\n", parallelTime);
    printf("Speed-up over sequential version: %.2f\n", sequentialTime / parallelTime);//Tells how fast the parallel computation is w.r.t sequential computation -- as the name suggests




    //Part 2
    printf("\nPart 2:\n");
    queue.head = 0;
    queue.tail = 0;
    queue.count = 0;
    //Initialize mutex(lock) and cond(blockingCondition) to their default values -- indicated by NULL
    pthread_mutex_init(&queue.lock, NULL);
    pthread_cond_init(&queue.blockingCondition, NULL);
    pthread_t threadsP2[NUM_THREADS];//Thread pool
    clock_t beginP2, finishP2;

    i = 0;
    while(i < NUM_THREADS){//Creating and assigning threads work
        pthread_create(&threadsP2[i], NULL, handleWorkerThread, NULL);
        i += 1;
    }
    
    beginP2 = clock();
    for(int i = 0; i < m; i++){
        for(int j = 0; j < p; j++){
            ThreadData task = {.row = i, .col = j, .n = n, .A = A, .B = B, .C = C};
            enqueue(task);
        }
    }
    finishP2 = clock();
    double parallelTimeP2 = (double)(finishP2 - begin) / CLOCKS_PER_SEC;

    areThreadsBusy = 0;//Signalling no more incoming tasks
    pthread_cond_broadcast(&queue.blockingCondition);
    i = 0;
    while(i < NUM_THREADS){//Note we create NUM_THREADS threads and not m * p threads
        pthread_join(threads[i], NULL);
        i += 1;
    }

    outputMatrix(C, 'C', m, p);

    printf("Parallel time(thread pool): %f seconds\n", parallelTimeP2);
    printf("Speed-up over sequential version: %.5f\n", sequentialTime / parallelTimeP2);

    //Free memory for each of the rows
    i = 0;
    while(i < m){
        free(A[i]);
        i += 1;
    }
    i = 0;
    while(i < n){
        free(B[i]);
        i += 1;
    }
    while(i < m){
        free(C[i]);
        i += 1;
    }
    //Free memory for the 2D array
    free(A);
    free(B);
    free(C);
    return 0;
}
/*
Sample run:
Enter dimensions of matrix A(m x n): 3 3
Enter dimensions of matrix B(n x p): 3 3
Enter elements of matrix A
1 2 3
4 5 6
7 8 9
Enter elements of matrix B
9 8 7
6 5 4
3 2 1

CLOCKS_PER_SEC = 1000000

Part 1:
Resultant matrix C
30 24 18 
84 69 54 
138 114 90 
Sequential time(one thread per element): 0.000005 seconds
Parallel time(one thread per element): 0.000478 seconds
Speed-up over sequential version: 0.01

Part 2:
Resultant matrix C
30 24 18 
84 69 54 
138 114 90 
Parallel time(thread pool): 0.000704 seconds
Speed-up over sequential version: 0.00710
*/