#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

int main() {
    struct sysinfo info;
    if(sysinfo(&info) != 1){//successfully setting up the properties in info
        unsigned long uptime = info.uptime;
        //unsigned int days = uptime / (24 * 3600);
        unsigned int hours = (uptime) / 3600;// % (24 * 3600)
        unsigned int minutes = (uptime % 3600) / 60;
        unsigned int seconds = uptime % 60;//Remove the number of complete minutes that have passed and we are left with seconds

        printf("Uptime: %ld\n", uptime);
        printf("Uptime: %u hours %u minutes %u seconds\n", hours, minutes, seconds);//%u days , days
    }else{
        perror("sysinfo");
        exit(1);
    }
    return 0;
}