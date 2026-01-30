#include <windows.h>
#include <stdio.h>

int main(void)
{
    HANDLE timer;
    LARGE_INTEGER dueTime;

    unsigned long long it = 0;
    int secondsPassed = 0;

    timer = CreateWaitableTimer(NULL, FALSE, NULL);

    dueTime.QuadPart = -30000000LL;

    SetWaitableTimer(timer, &dueTime, 3000, NULL, NULL, FALSE);

    for (;;)
    {
        it++;

        if (WaitForSingleObject(timer, 0) == WAIT_OBJECT_0)
        {
            secondsPassed += 3;
            printf("%d seconds: iterations = %llu\n", secondsPassed, it);

            if (secondsPassed >= 15)
                break;
        }
    }

    CloseHandle(timer);
    return 0;
}
