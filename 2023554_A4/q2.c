#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
//5 tracks, 100 sectors/track, 72RPM, seek time per track: 2ms, head initially at 100, SSTF algorithm
typedef struct{int sector; int track; int sector_num;}Req;

void seek_rotLatency_times(int disc_request_queue[], int num_requests){
    int head_posi = 100;
    double rotational_speed = 72 / 60.0; double rotLatency_per_sector = (1 / (double)rotational_speed) * 1000 / 100;

    Req requests[num_requests];
    for(int i = 0; i < num_requests; i++){requests[i].sector = disc_request_queue[i]; requests[i].track = disc_request_queue[i] / 100; requests[i].sector_num = disc_request_queue[i] % 100;}
    for(int i = 0; i < num_requests; i++){
        int min_seek_time = INT_MAX; int minimum_seek_ind = -1;
        for(int j = 0; j < num_requests; ++j){
            if(requests[j].sector != -1){
                int current_seek_time = abs(head_posi - requests[j].sector);
                if(current_seek_time < min_seek_time){min_seek_time = current_seek_time; minimum_seek_ind = j;}
            }
        }
        if(minimum_seek_ind == -1) break;

        int seek_time = abs(head_posi / 100 - requests[minimum_seek_ind].track) * 2;//seek time per track
        double rotLatency = abs(head_posi % 100 - requests[minimum_seek_ind].sector_num) * rotLatency_per_sector;

        printf("Seek Time %d: %d ms\n", i + 1, seek_time); printf("Rotational Latency %d: %.2f ms\n", i + 1, rotLatency);
        head_posi = requests[minimum_seek_ind].sector;
        requests[minimum_seek_ind].sector = -1;
    }
}

int main(){
    int a, b, c, d; printf("Enter 4 disc requests: "); scanf("%d %d %d %d", &a, &b, &c, &d);
    int disc_request_queue[] = {a, b, c, d}; int num_requests = 4;
    seek_rotLatency_times(disc_request_queue, num_requests);
    exit(EXIT_SUCCESS);
}