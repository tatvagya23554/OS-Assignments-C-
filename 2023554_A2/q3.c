#include <stdio.h>
#include <stdlib.h>

enum Constants{
    PAGE_SIZE = 4096,//4KB
    TLB_SIZE = 4,
    MEMORY_SIZE = 65536,//64KB
    NUM_PAGES = (MEMORY_SIZE / PAGE_SIZE)
};
//0 -> Invalid, 1 -> Valid
typedef struct PageTableEntry{
    int frame_number;
    int valid;
}PgTentry;

typedef struct TLBEntry{
    int page_number;
    int frame_number;
    int valid;
}TLBentry;

PgTentry page_table[NUM_PAGES];//Global page table
TLBentry tlb[TLB_SIZE];

void initializePTnTLB(){//C does not have constructors for structs
    for(int i = 0; i < NUM_PAGES; i++){
        page_table[i].valid = 0;
    }
    for(int i = 0; i < TLB_SIZE; i++){
        tlb[i].valid = 0;
    }
}

void updateTLB(int page_number, int frame_number){
    static int next_tlb_index = 0;
    tlb[next_tlb_index].page_number = page_number;
    tlb[next_tlb_index].frame_number = frame_number;
    tlb[next_tlb_index].valid = 1;
    next_tlb_index = (next_tlb_index + 1) % TLB_SIZE;
}

int tlbLookup(int page_number, int* frame_number){
    for(int i = 0; i < TLB_SIZE; i++){
        if(tlb[i].valid && tlb[i].page_number == page_number){//If the page we are in search of has a frame associated
            *frame_number = tlb[i].frame_number;
            return 1;//TLB hit
        }
    }
    return 0;//TLB miss
}

int pageTableLookup(int page_number, int* frame_number){
    if(page_table[page_number].valid){
        *frame_number = page_table[page_number].frame_number;
        return 1;//Page hit
    }
    return 0;//Page fault
}

void handlePageFault(int page_number){//load corresponding page in main memory
    static int next_free_frame = 0;//Static nature ensures that the variable retains its value between function calls
    if(next_free_frame < NUM_PAGES){
        page_table[page_number].valid = 1;
        page_table[page_number].frame_number = next_free_frame;
        next_free_frame++;
    }else{
        printf("Error: No more free physical frames.\n");
    }
}

void translateAddress(unsigned int logical_address){
    int page_number = (logical_address >> 12) & 0xF;//Top 4 bits
    int offset = logical_address & 0xFFF;//Bottom 12 bits
    int frame_number;
    int physical_address;
    //We assign frame_number a value simultaneously as we use it
    if(tlbLookup(page_number, &frame_number)){//For this if block try running using 43fd
        physical_address = (frame_number << 12) | offset;//frame_number may implicitly be treated as an unsigned int - for calculation putposes
        printf("Physical address corresponding to logical address: 0x%04X is: 0x%04X(TLB hit)\n", logical_address, physical_address);
    }else{//Go to page table
        printf("TLB miss\n");
        if(pageTableLookup(page_number, &frame_number)){//IMP output scenario
            physical_address = (frame_number << 12) | offset;
            printf("Physical address corresponding to logical address: 0x%04X is: 0x%04X(Page table hit)\n", logical_address, physical_address);
            updateTLB(page_number, frame_number);//Associating page with TLB for faster access next time
        }else{//Page fault
            printf("Page fault\n");
            handlePageFault(page_number);
            frame_number = page_table[page_number].frame_number;
            physical_address = (frame_number << 12) | offset;
            printf("Physical address corresponding to logical address: 0x%04X is: 0x%04X(After page load)\n", logical_address, physical_address);
            updateTLB(page_number, frame_number);//Associating page with TLB for faster access next time
        }
    }
}

int main(){//TLB_SIZE = 4
    unsigned int logical_address;
    initializePTnTLB();
    while(1){
        printf("Enter a 16-bit logical address in hexadecimal format: ");
        if(scanf("%x", &logical_address) != 1){
            printf("Invalid input\n");
            while(getchar() != '\n');
        }else{
            translateAddress(logical_address);
            continue;
        }
    }
    return 0;
}