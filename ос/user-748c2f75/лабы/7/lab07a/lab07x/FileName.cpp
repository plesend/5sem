#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int isPrime(unsigned long long n)
{
    if (n < 2)
        return 0;

    for (unsigned long long i = 2; i * i <= n; i++)
        if (n % i == 0)
            return 0;

    return 1;
}

int main(int argc, char* argv[])
{

    int seconds;
    if (argc < 2) seconds = 10;
    else seconds = atoi(argv[1]);

    ULONGLONG startTime = GetTickCount64();
    ULONGLONG endTime;

    unsigned long long number = 2;
    unsigned long long count = 0;

    for (;;)
    {
        if (isPrime(number))
        {
            count++;
            printf("%llu: %llu\n", count, number);
        }
        number++;

        if (GetTickCount64() - startTime >= (ULONGLONG)seconds * 1000)
            break;
    }

    endTime = GetTickCount64();

    printf("\ntime: %llu ms\n", endTime - startTime);

    system("pause");

    return 0;
}
