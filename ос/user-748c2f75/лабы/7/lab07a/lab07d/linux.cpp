#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    if (!fork()) {
        execl("lab07x", "lab07x", "60", NULL);
        return 1;
    }

    if (!fork()) {
        execl("lab07x", "lab07x", "120", NULL);
        return 1;
    }

    printf("started both processes\n");

    wait(NULL);
    wait(NULL);

    printf("processes finished\n");

    return 0;
}
