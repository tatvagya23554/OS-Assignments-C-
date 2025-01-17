#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef struct page{
    uint8_t page_number;
    bool use_bit;
    bool dirty_bit;
}Page;

int checkPage(Page frames[], int num_frames, uint8_t page_number){
    for(int i = 0; i < num_frames; i++){
        if(frames[i].page_number == page_number){
            return i;
        }
    }
    return -1;
}

void clockPageReplacement(Page frames[], int num_frames, uint8_t page_number, int* clock_hand){//FIFO variant of Second Chance Algorithm
    while(1){
        if(!frames[*clock_hand].use_bit){
            frames[*clock_hand].page_number = page_number;
            frames[*clock_hand].use_bit = true;
            frames[*clock_hand].dirty_bit = false;//The new page has not been modified
            return;
        }else{
            frames[*clock_hand].use_bit = false;
            *clock_hand = (*clock_hand + 1) % num_frames;//To keep clock_hand < num_frames
        }
    }
}

int main(){
    int num_frames;
    printf("Enter number of frames: ");
    scanf("%d", &num_frames);
    Page* frames = (Page*)malloc(num_frames * sizeof(Page));//Simulate main memory
    for(int i = 0; i < num_frames; i++){
        frames[i].page_number = -1;//Main memory is empty -- put i in place of -1 for sample runs
        frames[i].use_bit = false;
        frames[i].dirty_bit = false;//None of the pages modified
    }
    char input[100];//Will help take around 50 pages as input
    printf("Enter page requests as CSVs(without spaces): ");
    scanf("%s", input);//A char array -- a string
    char* token = strtok(input, ",");
    uint8_t page_requests[55];
    int num_requests = 0;
    while(token != NULL){//Parsing input
        page_requests[num_requests] = atoi(token);
        num_requests++;
        token = strtok(NULL, ",");
    }
    int page_faults = 0;
    int page_hits = 0;
    int clock_hand = 0;//Pointing to the first frame
    for(int i = 0; i < num_requests; i++){
        uint8_t current_page = page_requests[i];
        int page_index = checkPage(frames, num_frames, current_page);
        if(page_index != -1){
            frames[page_index].use_bit = true;
            frames[page_index].dirty_bit = true;//Pre-existent page altered
            page_hits++;
        }else{
            clockPageReplacement(frames, num_frames, current_page, &clock_hand);
            page_faults++;
        }
    }
    printf("\nTotal page faults: %d\n", page_faults);
    printf("Total page hits: %d\n", page_hits);
    printf("Hit rate: %.2f%%\n",(page_hits / (float)num_requests) * 100);
    return 0;
}