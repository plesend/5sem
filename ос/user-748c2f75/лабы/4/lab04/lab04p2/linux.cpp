#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>

#define MAX_THREADS 8
#define MAX_PRIMES 10000000
#define MAX_PRIMES_PER_THREAD 5000000

int primes[MAX_PRIMES];
int primesCount = 0;
pthread_mutex_t primesMutex = PTHREAD_MUTEX_INITIALIZER;

__thread int tlsBuf[MAX_PRIMES_PER_THREAD];
__thread int localCount = 0;

using namespace std;

struct Range {
    int start;
    int end;
};

int* BubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
    return arr;
}

void* Func(void* lpParam) {
    auto isPrime = [](int n) -> bool {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (int i = 3; i * i <= n; i += 2)
            if (n % i == 0) return false;
        return true;
        };

    Range* range = reinterpret_cast<Range*>(lpParam);
    if (!range) return nullptr;

    localCount = 0;

    for (int i = range->start; i <= range->end; i++) {
        if (isPrime(i)) {
            if (localCount < MAX_PRIMES_PER_THREAD)
                tlsBuf[localCount++] = i;
        }
    }

    for (int i = 0; i < localCount; i++)
        primes[primesCount++] = tlsBuf[i];

    delete range;
    return nullptr;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("usage: ./l4primes {threadCount} {start} {end}\n");
        return 1;
    }

    int threadCount = atoi(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);

    if (threadCount <= 0 || start <= 0 || end <= 0 || start >= end) {
        printf("threadCount: %d start: %d end: %d\n", threadCount, start, end);
        printf("values should be > 0, start < end\n");
        return 1;
    }

    pthread_t threads[MAX_THREADS];
    int currStart = start;

    int totalNumbers = end - start + 1;
    int numbersPerThread = totalNumbers / threadCount;
    int remainder = totalNumbers % threadCount;

    for (int i = 0; i < threadCount; i++) {
        int currEnd = currStart + numbersPerThread - 1;
        if (i == threadCount - 1)
            currEnd += remainder;

        Range* r = new Range{ currStart, currEnd };
        if (pthread_create(&threads[i], nullptr, Func, r) != 0) {
            printf("Error creating thread %d\n", i);
            delete r;
        }

        currStart = currEnd + 1;
    }

    for (int i = 0; i < threadCount; i++)
        pthread_join(threads[i], nullptr);

    BubbleSort(primes, primesCount);

    for (int i = 0; i < primesCount; i++)
        printf("%d ", primes[i]);
    printf("\n");

    return 0;
}
