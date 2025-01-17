#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
//INVENTORY MANAGEMENT OF A WAREHOUSE
#define BUFFER_SIZE 10
#define MAX_PRODUCTS 10//Maximum number of products that can be delivered or stored at a time

int buffer[BUFFER_SIZE];
int numberOfProductsInInventory = 0;
int in = 0, out = 0;

sem_t *emptySlots;
sem_t *filledSlots;
pthread_mutex_t circularBufferMutex = PTHREAD_MUTEX_INITIALIZER;

void* simulateDesiredTruckWorking(void* arg){
    int ID = *(int*)arg;
    int numberOfProductsToDeliver = rand() % MAX_PRODUCTS + 1;
    printf("Truck %d wants to deliver %d products.\n", ID, numberOfProductsToDeliver);
    for(int i = 0; i < numberOfProductsToDeliver; i += 1){
    sem_wait(emptySlots);
    pthread_mutex_lock(&circularBufferMutex);
    buffer[in] = 1;
    in = (in + 1) % BUFFER_SIZE;
    numberOfProductsInInventory += 1;
    printf("Truck %d delivered 1 product. Current inventory: %d products\n", ID, numberOfProductsInInventory);
    pthread_mutex_unlock(&circularBufferMutex);
    sem_post(filledSlots);
    sleep(1);}//After delivering a product, sleep for a second
    sleep(rand() % 3 + 1);
    return NULL;
}

void* simulateDesiredManagerWorking(void* arg){
    int ID = *(int*)arg;
    int numberOfProductsToStore = rand() % MAX_PRODUCTS + 1;//Remove a random number of products from the buffer
    printf("Manager %d wants to store %d products.\n", ID, numberOfProductsToStore);
    for(int i = 0; i < numberOfProductsToStore; i += 1){
    sem_wait(filledSlots);
    pthread_mutex_lock(&circularBufferMutex);
    buffer[out] = 0;
    out = (out + 1) % BUFFER_SIZE;
    numberOfProductsInInventory -= 1;
    printf("Manager %d stored 1 product. Current inventory: %d products\n", ID, numberOfProductsInInventory);
    pthread_mutex_unlock(&circularBufferMutex);
    sem_post(emptySlots);
    sleep(1);}//After simulating storage of a product, sleep for a second
    sleep(rand() % 3 + 1);
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc != 3){fprintf(stderr, "Usage: %s <numberOfManagers> <numberOfTrucks>\n", argv[0]); exit(1);}//When running this program on terminal, argv[0] will always be the name of its corresponding executable
    int numberOfManagers = atoi(argv[1]);
    int numberOfTrucks = atoi(argv[2]);
    pthread_t trucks[numberOfTrucks], managers[numberOfManagers];
    int truckIDs[numberOfTrucks], managerIDs[numberOfManagers];

    emptySlots = sem_open("Semaphore for empty slots", O_CREAT, 0644, BUFFER_SIZE);//Initialize its value to BUFFER_SIZE
    filledSlots = sem_open("Semaphore for filled slots", O_CREAT, 0644, 0);//Initialize its value to 0
    if(emptySlots == SEM_FAILED || filledSlots == SEM_FAILED){perror("sem_open"); exit(EXIT_FAILURE);}

    for(int i = 0; i < numberOfTrucks; i += 1){truckIDs[i] = i + 1; pthread_create(&trucks[i], NULL, simulateDesiredTruckWorking, &truckIDs[i]);}
    for(int i = 0; i < numberOfManagers; i += 1){managerIDs[i] = i + 1; pthread_create(&managers[i], NULL, simulateDesiredManagerWorking, &managerIDs[i]);}

    while(numberOfProductsInInventory < 10) sleep(1);//Check every second -- question says: "The program should run for a defined period or until a certain number of deliveries have been processed"
    printf("Simulation complete. Final warehouse state: %d products\n", numberOfProductsInInventory);

    for(int i = 0; i < numberOfTrucks; i += 1){pthread_cancel(trucks[i]); pthread_join(trucks[i], NULL);}
    for(int i = 0; i < numberOfManagers; i += 1){pthread_cancel(managers[i]); pthread_join(managers[i], NULL);}

    sem_close(emptySlots);
    sem_close(filledSlots);
    sem_unlink("Semaphore for empty slots");
    sem_unlink("Semaphore for filled slots");
    pthread_mutex_destroy(&circularBufferMutex);
    return 0;
}
/*
Sample run:
Truck 1 wants to deliver 8 products.
Manager 1 wants to store 3 products.
Truck 2 wants to deliver 10 products.
Truck 1 delivered 1 product. Current inventory: 1 products
Truck 3 wants to deliver 4 products.
Manager 2 wants to store 5 products.
Manager 3 wants to store 9 products.
Truck 5 wants to deliver 1 products.
Truck 5 delivered 1 product. Current inventory: 2 products
Truck 2 delivered 1 product. Current inventory: 3 products
Truck 3 delivered 1 product. Current inventory: 4 products
Manager 1 stored 1 product. Current inventory: 3 products
Manager 2 stored 1 product. Current inventory: 2 products
Truck 4 wants to deliver 9 products.
Truck 4 delivered 1 product. Current inventory: 3 products
Manager 3 stored 1 product. Current inventory: 2 products
Manager 1 stored 1 product. Current inventory: 1 products
Truck 4 delivered 1 product. Current inventory: 2 products
Truck 2 delivered 1 product. Current inventory: 3 products
Truck 1 delivered 1 product. Current inventory: 4 products
Manager 3 stored 1 product. Current inventory: 3 products
Truck 3 delivered 1 product. Current inventory: 4 products
Manager 2 stored 1 product. Current inventory: 3 products
Truck 2 delivered 1 product. Current inventory: 4 products
Manager 1 stored 1 product. Current inventory: 3 products
Manager 3 stored 1 product. Current inventory: 2 products
Manager 2 stored 1 product. Current inventory: 1 products
Truck 4 delivered 1 product. Current inventory: 2 products
Truck 1 delivered 1 product. Current inventory: 3 products
Truck 3 delivered 1 product. Current inventory: 4 products
Truck 2 delivered 1 product. Current inventory: 5 products
Manager 3 stored 1 product. Current inventory: 4 products
Truck 4 delivered 1 product. Current inventory: 5 products
Truck 1 delivered 1 product. Current inventory: 6 products
Manager 2 stored 1 product. Current inventory: 5 products
Truck 3 delivered 1 product. Current inventory: 6 products
Truck 2 delivered 1 product. Current inventory: 7 products
Manager 2 stored 1 product. Current inventory: 6 products
Truck 4 delivered 1 product. Current inventory: 7 products
Manager 3 stored 1 product. Current inventory: 6 products
Truck 1 delivered 1 product. Current inventory: 7 products
Truck 2 delivered 1 product. Current inventory: 8 products
Truck 4 delivered 1 product. Current inventory: 9 products
Manager 3 stored 1 product. Current inventory: 8 products
Truck 1 delivered 1 product. Current inventory: 9 products
Truck 2 delivered 1 product. Current inventory: 10 products
Manager 3 stored 1 product. Current inventory: 9 products
Truck 4 delivered 1 product. Current inventory: 10 products
Simulation complete. Final warehouse state: 10 products
*/