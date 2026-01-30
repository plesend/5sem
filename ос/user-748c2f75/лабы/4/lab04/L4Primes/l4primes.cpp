#include <windows.h>
#include <stdio.h>
#include <string>

#define MAX_THREADS 8
#define MAX_PRIMES 10000000
#define MAX_PRIMES_PER_THREAD 5000000

DWORD tlsInd = TLS_OUT_OF_INDEXES;
int primes[MAX_PRIMES];       
int primesCount = 0;          

struct Range {
    int start;
    int end;
};

DWORD WINAPI Func(LPVOID lpParam) {
    auto isPrime = [](int n) -> bool {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (int i = 3; i * i <= n; i += 2)
            if (n % i == 0) return false;
        return true;
        };

    Range* range = reinterpret_cast<Range*>(lpParam);
    if (!range) return 1;

    int* tlsBuf = new int[MAX_PRIMES_PER_THREAD];
    int localCount = 0;

    TlsSetValue(tlsInd, tlsBuf);

    for (int i = range->start; i <= range->end; i++) {
        if (isPrime(i)) {
            if (localCount < MAX_PRIMES_PER_THREAD)
                tlsBuf[localCount++] = i;
        }
    }

    int* get = reinterpret_cast<int*>(TlsGetValue(tlsInd));
    if (get) {
        for (int i = 0; i < localCount; i++)
            primes[primesCount++] = get[i];
        delete[] get;
        TlsSetValue(tlsInd, nullptr);
    }

    delete range;
    return 0;
}


int main(int argc, char** argv)
{
    if (argc < 4) {
        printf("usage: l4primes.exe {threadCount} {start} {end}\n");
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

    tlsInd = TlsAlloc();
    HANDLE threads[MAX_THREADS];
    int currStart = start;

    int totalNumbers = end - start + 1;
    int numbersPerThread = totalNumbers / threadCount;
    int remainder = totalNumbers % threadCount;

    for (int i = 0; i < threadCount; i++) {
        int currEnd = currStart + numbersPerThread - 1;
        if (i == threadCount - 1)
            currEnd += remainder;

        Range* r = new Range{ currStart, currEnd };

        threads[i] = CreateThread(NULL, 0, Func, r, 0, NULL);
        if (!threads[i]) {
            printf("Error creating thread %d\n", i);
            delete r;
        }

        currStart = currEnd + 1;
    }

    WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);

    for (int i = 0; i < threadCount; i++) CloseHandle(threads[i]);
    TlsFree(tlsInd);

    for (int i = 0; i < primesCount; i++) {
        printf("%d ", primes[i]);
    }
    printf("\n");

    return 0;
}
