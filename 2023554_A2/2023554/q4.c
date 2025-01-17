#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>//To store a random value

enum Constants{
    PD_TOTAL_ENTRIES = 1024,//For general simulation purpose
    PT_TOTAL_ENTRIES = 1024,
    ENTRY_SIZE = 4,
    MEMORY_SIZE = 1048576,//Simulating 1MB main memory
    PAGE_SIZE = 4096
};

int page_hits = 0;
int page_misses = 0;

uint32_t* page_directory[PD_TOTAL_ENTRIES];
uint32_t main_memory[MEMORY_SIZE / sizeof(uint32_t)];

uint32_t handlePageFault(uint32_t pd_index, uint32_t pt_index){//Returns the eventual address of the frame to go to
    if(page_directory[pd_index] == NULL){//In case fault is owing to the non-exixtence of a corresponding page table
        page_directory[pd_index] = (uint32_t *)calloc(PT_TOTAL_ENTRIES, sizeof(uint32_t));
        printf("Page fault: Allocated a new page table for PD index %u.\n", pd_index);
    }
    //The below if clause would of course be executed if the above if clause holds good
    if(page_directory[pd_index][pt_index] == 0){//In case fault is owing to an unmapped page -- allocate a new frame
        static uint32_t next_free_frame = 1;//Start from frame 1(frame 0 is reserved)
        /*Reserving frame 0 helps us delegate when a null pointer(0x0) might be incorrectly interpreted as a valid memory
        address -- the system ensures that any invalid access to a null address results in a fault*/
        page_directory[pd_index][pt_index] = next_free_frame * PAGE_SIZE;//Corresponding main memory address of the frame
        next_free_frame++;//To ensure a unique frame for addressing each page fault
        printf("Page fault: Mapped page number(PD index %u, PT index %u) to frame.\n", pd_index, pt_index);
    }
    return page_directory[pd_index][pt_index];
}

//Functions to parse VA -- we get 32 bit return values in each
uint32_t getPDindex(uint32_t va){
    return (va >> 22) & 0x3FF;//Extract the top 10 bits(MSBs)(31-22)
}
uint32_t getPTindex(uint32_t va){
    return (va >> 12) & 0x3FF;//Extract the middle 10 bits(21-12)
}
uint32_t getOffset(uint32_t va){
    return va & 0xFFF;//Extract the bottom 12 bits(11-0)
}

uint8_t load(uint32_t va){//Returns the loaded value
    uint32_t pd_index = getPDindex(va);
    uint32_t pt_index = getPTindex(va);
    uint32_t offset = getOffset(va);
    //offset tells of the specific byte in consideration within a particular page(in a frame -- can have multiple bytes)
    if(page_directory[pd_index] != NULL && page_directory[pd_index][pt_index] != 0){//Hit
        page_hits++;
        uint32_t main_memory_address = page_directory[pd_index][pt_index] + offset;
        printf("Loading(on page hit) from main memory address: 0x%08X(for VA 0x%08X)\n", main_memory_address, va);
        return main_memory[main_memory_address / sizeof(uint32_t)];/*[Converting byte address into an index of 
        main_memory array]; sizeof(uint32_t) = 4 bytes*/
    }else{//Miss
        page_misses++;
        uint32_t frame_address = handlePageFault(pd_index, pt_index);
        uint32_t main_memory_address = frame_address + offset;
        printf("Loading(after page fault) from main memory address: 0x%08X(for VA 0x%08X)\n", main_memory_address, va);
        return main_memory[main_memory_address / sizeof(uint32_t)];
    }
}

void store(uint32_t va){
    uint8_t value = (uint8_t)(rand() % 256);//a value between 0 and 255
    uint32_t pd_index = getPDindex(va);
    uint32_t pt_index = getPTindex(va);
    uint32_t offset = getOffset(va);
    if(page_directory[pd_index] != NULL && page_directory[pd_index][pt_index] != 0){//Hit
        page_hits++;
        uint32_t main_memory_address = page_directory[pd_index][pt_index] + offset;
        printf("Storing %u(on page hit) at main memory address: 0x%08X(for VA 0x%08X)\n", value, main_memory_address, va);
        main_memory[main_memory_address / sizeof(uint32_t)] = value;
    }else{//Miss
        page_misses++;
        uint32_t frame_address = handlePageFault(pd_index, pt_index);
        uint32_t main_memory_address = frame_address + offset;
        printf("Storing %u(after page fault) at main memory address: 0x%08X(for VA 0x%08X)\n", value, main_memory_address, va);
        main_memory[main_memory_address / sizeof(uint32_t)] = value;
    }
}

int main(){
    srand((unsigned int)time(NULL));/* -- We seed the random number generator only once per program and hence don't 
    do so in the stor function*/
    //8 bit values at virtual addresses
    printf("Size of each page table: %d\n", PT_TOTAL_ENTRIES * ENTRY_SIZE);
    printf("Size of page directory: %d\n", PD_TOTAL_ENTRIES * ENTRY_SIZE);
    printf("\n");
    //To avoid page fault set the second VA below same as the first one
    store(0x00000321);
    printf("\n");
    store(0x00000321);//67891A4F
    printf("\n");

    uint8_t first_value = load(0x00000321);
    uint8_t second_value = load(0x00000321);
    printf("\n");

    //conversion character for hexadecimal number in all caps: X
    printf("Value loaded using VA 0x00000ABC: 0x%02X\n", first_value);
    printf("Value loaded using VA 0x12345FFF: 0x%02X\n", second_value);
    printf("\n");
    printf("hit/miss ratio = %.2f (hits = %d; misses = %d)\n", (float)page_hits / page_misses, page_hits, page_misses);
    return 0;
}