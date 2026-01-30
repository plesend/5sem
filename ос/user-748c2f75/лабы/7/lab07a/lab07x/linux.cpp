//#define _POSIX_C_SOURCE 199309L
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//
//int isPrime(unsigned long long n)
//{
//    if (n < 2)
//        return 0;
//
//    for (unsigned long long i = 2; i * i <= n; i++)
//        if (n % i == 0)
//            return 0;
//
//    return 1;
//}
//
//unsigned long long funzione()
//{
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
//}
//
//int main(int argc, char* argv[])
//{
//    int seconds = (argc < 2) ? 10 : atoi(argv[1]);
//    if (seconds <= 0) seconds = 10;
//
//    unsigned long long startTime = funzione();
//    unsigned long long endTime;
//
//    unsigned long long number = 2;
//    unsigned long long count = 0;
//
//    for (;;)
//    {
//        if (isPrime(number))
//        {
//            count++;
//            printf("%llu: %llu\n", count, number);
//        }
//        number++;
//
//        if (funzione() - startTime >= (unsigned long long)seconds * 1000)
//            break;
//    }
//
//    endTime = funzione();
//    printf("\ntime: %llu ms\n", endTime - startTime);
//
//    return 0;
//}
