#include <stdio.h>
#include <stdlib.h>

enum SegmentParameters{//First 2 bits -> segment number; next 14 bits -> offset
    SEG_MASK = 0xC000,
    RIGHT_SHIFT = 14,
    OFFSET_MASK = 0x3FFF,
    //Assigning numerical identifiers to segments
    CODE_SEGMENT = 0,
    HEAP_SEGMENT = 1,
    STACK_SEGMENT = 2
};

enum BaseLimits{
    //In bytes
    CODE_BASE = 32768,//32KB
    CODE_BOUND = 2048,//2KB

    HEAP_BASE = 34816,//34KB
    HEAP_BOUND = 3072,//3KB

    STACK_BASE = 28672,//28KB
    STACK_BOUND = 2048//2KB
};

void translateAddress(unsigned int logical_address){
    unsigned int segment = (logical_address & SEG_MASK) >> RIGHT_SHIFT;//Binary automatically gets converted to unsigned int
    unsigned int offset = logical_address & OFFSET_MASK;
    unsigned int physical_address = 0;

    printf("Segment number: %u\n", segment);
    if(segment == CODE_SEGMENT){
        if(offset >= CODE_BOUND){
            printf("Segmentation fault(Code segment offset - %u - out of bounds)\n", offset);
            return;
        }
        physical_address = CODE_BASE + offset;
    }else if(segment == HEAP_SEGMENT){
        if(offset >= HEAP_BOUND){
            printf("Segmentation fault(Heap segment offset - %u - out of bounds)\n", offset);
            return;
        }
        physical_address = HEAP_BASE + offset;
    }else if(segment == STACK_SEGMENT){
        if(offset >= STACK_BOUND){
            printf("Segmentation fault(Stack segment offset - %u - out of bounds)\n", offset);
            return;
        }
        physical_address = STACK_BASE + offset;
    }else{
        printf("Invalid segment\n");
        return;
    }
    printf("Logical address: 0x%04X\n", logical_address);
    printf("Physical address: 0x%04X\n", physical_address);
}

int main(){//4000 logical address has 8800 as physical address
    unsigned int logical_address;
    printf("Enter a 16-bit logical address in hexadecimal format: ");
    //Input automatically gets converted to unsigned int
    while(1){
        if(scanf("%x", &logical_address) != 1){//scanf returns 0(failure) or 1(success) or EOF
            printf("Invalid input\n");
            while(getchar() != '\n');//Clearing the invalid input buffer until scanf moves past the bad input
            //Above we discard all characters until a new line is found
        }else{
            break;
        }
        printf("Enter a 16-bit logical address in hexadecimal format: ");
    }
    translateAddress(logical_address);
    return 0;
}