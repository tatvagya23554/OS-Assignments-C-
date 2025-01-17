#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>//O_CREAT and O_EXCL are in this header -- used as flags in sem_open
//Each network channel - a semaphore; each server - a thread
//There are total 5 servers with us and each is required to process data 3 times

sem_t *networkChannels[2];//for left(0) and right(1) channel

void* simulateDesiredServerWorking(void* arg){
    int serverID = *(int*)arg;
    for(int i = 0; i < 3; i++)
    {
    printf("Server %d is waiting to acquire channels(0 and 1)\n", serverID + 1);//Added 1 for output formatting purpose
    sem_wait(networkChannels[0]);
    sem_wait(networkChannels[1]);

    printf("Server %d is processing data\n", serverID + 1);
    sleep(1);

    sem_post(networkChannels[0]);
    sem_post(networkChannels[1]);

    printf("Server %d has finished processing and has released both channels(0 and 1)\n", serverID + 1);
    sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t servers[5];
    int serverIDs[5] = {0, 1, 2, 3, 4};
    int i = 0;

    while(i < 2)
    {
    char semaphoreName[9];
    snprintf(semaphoreName, sizeof(semaphoreName), "Network %d", i);
    sem_unlink(semaphoreName);
    networkChannels[i] = sem_open(semaphoreName, O_CREAT | O_EXCL, 0644, 1);//Create semaphore if it doesn't already exist
    if(networkChannels[i] == SEM_FAILED){perror("sem_open failed"); exit(1);}
    i += 1;
    }

    i = 0;
    while(i < 5){pthread_create(&servers[i], NULL, simulateDesiredServerWorking, &serverIDs[i]); i += 1;}
    i = 0;
    while(i < 5){pthread_join(servers[i], NULL); i += 1;}

    i = 0;
    while(i < 5)
    {
    sem_close(networkChannels[i]);
    char semaphoreName[9];
    snprintf(semaphoreName, sizeof(semaphoreName), "Network %d", i);
    sem_unlink(semaphoreName);
    i += 1;
    }

    printf("All servers have completed processing.\n");
    return 0;
}
/*
Sample run:
Server 1 is waiting to acquire channels(0 and 1)
Server 5 is waiting to acquire channels(0 and 1)
Server 1 is processing data
Server 2 is waiting to acquire channels(0 and 1)
Server 3 is waiting to acquire channels(0 and 1)
Server 4 is waiting to acquire channels(0 and 1)
Server 1 has finished processing and has released both channels(0 and 1)
Server 5 is processing data
Server 1 is waiting to acquire channels(0 and 1)
Server 5 has finished processing and has released both channels(0 and 1)
Server 2 is processing data
Server 5 is waiting to acquire channels(0 and 1)
Server 2 has finished processing and has released both channels(0 and 1)
Server 3 is processing data
Server 2 is waiting to acquire channels(0 and 1)
Server 3 has finished processing and has released both channels(0 and 1)
Server 4 is processing data
Server 3 is waiting to acquire channels(0 and 1)
Server 4 has finished processing and has released both channels(0 and 1)
Server 1 is processing data
Server 4 is waiting to acquire channels(0 and 1)
Server 1 has finished processing and has released both channels(0 and 1)
Server 5 is processing data
Server 1 is waiting to acquire channels(0 and 1)
Server 5 has finished processing and has released both channels(0 and 1)
Server 2 is processing data
Server 5 is waiting to acquire channels(0 and 1)
Server 2 has finished processing and has released both channels(0 and 1)
Server 3 is processing data
Server 2 is waiting to acquire channels(0 and 1)
Server 3 has finished processing and has released both channels(0 and 1)
Server 4 is processing data
Server 4 has finished processing and has released both channels(0 and 1)
Server 1 is processing data
Server 3 is waiting to acquire channels(0 and 1)
Server 1 has finished processing and has released both channels(0 and 1)
Server 5 is processing data
Server 4 is waiting to acquire channels(0 and 1)
Server 5 has finished processing and has released both channels(0 and 1)
Server 2 is processing data
Server 2 has finished processing and has released both channels(0 and 1)
Server 3 is processing data
Server 3 has finished processing and has released both channels(0 and 1)
Server 4 is processing data
Server 4 has finished processing and has released both channels(0 and 1)
All servers have completed processing.
*/