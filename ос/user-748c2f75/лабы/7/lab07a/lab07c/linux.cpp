#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>

int main(void)
{
    timer_t timer;
    struct sigevent sev;
    struct itimerspec its;

    unsigned long long it = 0;
    int secondsPassed = 0;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;

    timer_create(CLOCK_MONOTONIC, &sev, &timer);

    its.it_value.tv_sec = 3;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 3;
    its.it_interval.tv_nsec = 0;

    timer_settime(timer, 0, &its, NULL);

    for (;;)
    {
        it++;

        struct timespec ts = { 0, 0 };
        int sig = sigtimedwait(&mask, NULL, &ts);

        if (sig == SIGRTMIN)
        {
            secondsPassed += 3;
            printf("%d seconds: iterations = %llu\n", secondsPassed, it);

            if (secondsPassed >= 15)
                break;
        }
    }

    timer_delete(timer);
    return 0;
}
