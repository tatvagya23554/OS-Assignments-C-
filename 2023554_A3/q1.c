#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//THREAD SCHEDULING AND DEADLOCK AVOIDANCE USING A SIMPLIFIED RESOURCE ALLOCATION SYSTEM
//DEADLOCK AVOIDANCE USING RESOURCE INSTANCE ORDERING -- A SPECIFIC LOCK ACQUISITION ORDER
//Wherever we have 3 in code, that is for the number of times a thread needs to acquire a particular lock -- As asked in the question

pthread_mutex_t lockA, lockB;
//5 \t when acquired and released
void* simulateDesiredThreadWorking(void* arg){
    int threadID = *(int*)arg;//getting value of pointer to integer(obtained after typecasting generic pointer to integer pointer) -- parsing the threadID
    for(int i = 0; i < 3; i++){
        printf("Proceeding to acquire Lock A\t\t\t-----     (T%d)\n", threadID);
        pthread_mutex_lock(&lockA);
        printf("Lock A acquired\t\t\t\t\t-----     (T%d)\n", threadID);

        printf("Proceeding to acquire Lock B\t\t\t-----     (T%d)\n", threadID);
        pthread_mutex_lock(&lockB);
        printf("Lock B acquired\t\t\t\t\t-----     (T%d)\n", threadID);

        //Simulating critical section
        printf("In critical section with Lock A and Lock B\t-----     (T%d)\n", threadID);
        sleep(1);

        pthread_mutex_unlock(&lockB);
        printf("Lock B released\t\t\t\t\t-----     (T%d)\n", threadID);

        pthread_mutex_unlock(&lockA);
        printf("Lock A released\t\t\t\t\t-----     (T%d)\n", threadID);

        //Simulating critical section
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_mutex_init(&lockA, NULL);
    pthread_mutex_init(&lockB, NULL);

    pthread_t threads[3];
    int threadIDs[3] = {1, 2, 3};

    for(int i = 0; i < 3; i++) pthread_create(&threads[i], NULL, simulateDesiredThreadWorking, &threadIDs[i]);

    for(int i = 0; i < 3; i++) pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&lockA);
    pthread_mutex_destroy(&lockB);

    printf("Tasks completed for each thread.\n");
    return 0;
}
/*
Sample run:
Proceeding to acquire Lock A                    -----     (T1)
Lock A acquired                                 -----     (T1)
Proceeding to acquire Lock B                    -----     (T1)
Lock B acquired                                 -----     (T1)
In critical section with Lock A and Lock B      -----     (T1)
Proceeding to acquire Lock A                    -----     (T2)
Proceeding to acquire Lock A                    -----     (T3)
Lock B released                                 -----     (T1)
Lock A released                                 -----     (T1)
Lock A acquired                                 -----     (T2)
Proceeding to acquire Lock B                    -----     (T2)
Lock B acquired                                 -----     (T2)
In critical section with Lock A and Lock B      -----     (T2)
Proceeding to acquire Lock A                    -----     (T1)
Lock B released                                 -----     (T2)
Lock A released                                 -----     (T2)
Lock A acquired                                 -----     (T3)
Proceeding to acquire Lock B                    -----     (T3)
Lock B acquired                                 -----     (T3)
In critical section with Lock A and Lock B      -----     (T3)
Proceeding to acquire Lock A                    -----     (T2)
Lock B released                                 -----     (T3)
Lock A released                                 -----     (T3)
Lock A acquired                                 -----     (T1)
Proceeding to acquire Lock B                    -----     (T1)
Lock B acquired                                 -----     (T1)
In critical section with Lock A and Lock B      -----     (T1)
Proceeding to acquire Lock A                    -----     (T3)
Lock B released                                 -----     (T1)
Lock A released                                 -----     (T1)
Lock A acquired                                 -----     (T2)
Proceeding to acquire Lock B                    -----     (T2)
Lock B acquired                                 -----     (T2)
In critical section with Lock A and Lock B      -----     (T2)
Proceeding to acquire Lock A                    -----     (T1)
Lock B released                                 -----     (T2)
Lock A released                                 -----     (T2)
Lock A acquired                                 -----     (T3)
Proceeding to acquire Lock B                    -----     (T3)
Lock B acquired                                 -----     (T3)
In critical section with Lock A and Lock B      -----     (T3)
Proceeding to acquire Lock A                    -----     (T2)
Lock B released                                 -----     (T3)
Lock A released                                 -----     (T3)
Lock A acquired                                 -----     (T1)
Proceeding to acquire Lock B                    -----     (T1)
Lock B acquired                                 -----     (T1)
In critical section with Lock A and Lock B      -----     (T1)
Proceeding to acquire Lock A                    -----     (T3)
Lock B released                                 -----     (T1)
Lock A released                                 -----     (T1)
Lock A acquired                                 -----     (T2)
Proceeding to acquire Lock B                    -----     (T2)
Lock B acquired                                 -----     (T2)
In critical section with Lock A and Lock B      -----     (T2)
Lock B released                                 -----     (T2)
Lock A released                                 -----     (T2)
Lock A acquired                                 -----     (T3)
Proceeding to acquire Lock B                    -----     (T3)
Lock B acquired                                 -----     (T3)
In critical section with Lock A and Lock B      -----     (T3)
Lock B released                                 -----     (T3)
Lock A released                                 -----     (T3)
Tasks completed for each thread.
*/