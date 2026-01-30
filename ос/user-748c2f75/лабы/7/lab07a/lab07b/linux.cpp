#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

static inline long long ms_diff(struct timespec a, struct timespec b)
{
    return (b.tv_sec - a.tv_sec) * 1000LL +
        (b.tv_nsec - a.tv_nsec) / 1000000LL;
}

int main()
{
    struct timespec cpuStart, cpuNow;
    struct timespec realStart, realEnd;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuStart);
    clock_gettime(CLOCK_MONOTONIC, &realStart);

    unsigned long long it = 0;
    int five = 0, ten = 0;

    for (;;)
    {
        it++;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuNow);
        long long cpuElapsed = ms_diff(cpuStart, cpuNow);

        if (cpuElapsed >= 5000 && !five)
        {
            printf("5 seconds: iterations = %llu\n", it);
            five = 1;
        }

        if (cpuElapsed >= 10000 && !ten)
        {
            printf("10 seconds: iterations = %llu\n", it);
            ten = 1;
        }

        if (cpuElapsed >= 15000)
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &realEnd);

    long long realElapsed = ms_diff(realStart, realEnd);

    printf("THE END: iterations = %llu\n", it);
    printf("Real time: %lld мс\n", realElapsed);

    return 0;
}
