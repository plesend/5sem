#include <stdio.h>
#include <unistd.h>      
#include <pthread.h>     

void Handle(int n) {
    const char array[12] = {
        'u', 's', 'e', 'r',
        '7', '4', '8', 'c',
        '2', 'f', '7', '5'
    };

    for (int i = 0; i < n; i++) {
        int index = i % 12;
        printf("PID: %d  TID: %lu  process %d - %c\n",
            getpid(),
            (unsigned long)pthread_self(),
            i,
            array[index]);

        usleep(300000); 
    }
}
