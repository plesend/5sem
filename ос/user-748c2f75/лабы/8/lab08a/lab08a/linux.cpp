#include <stdio.h>

int globalCoolLevel = 5;
int globalNotInitializedCool;
static int globalStaticCool = 10;
static int globalStaticNotInitialized;

void katusha_cool(int level)
{
    printf("Checking katusha cool level...\n");
    if (level < 3)
        printf("katusha is not cool yet\n");
    else if (level < 7)
        printf("katusha is cool\n");
    else
        printf("katusha is SUPER COOL\n");
}

int main(int argc, char** argv)
{
    int localCool = 5;
    int localNotInitialized;
    static int localStaticCool = 1;
    static int localStaticNotInitialized;

    printf("Function addresses:\n");
    printf("katusha_cool: %p\n", (void*)katusha_cool);
    printf("main: %p\n", (void*)main);

    printf("\nGlobal variables:\n");
    printf("globalCoolLevel: %p\n", (void*)&globalCoolLevel);
    printf("globalNotInitializedCool: %p\n", (void*)&globalNotInitializedCool);
    printf("globalStaticCool: %p\n", (void*)&globalStaticCool);
    printf("globalStaticNotInitialized: %p\n", (void*)&globalStaticNotInitialized);

    printf("\nLocal variables:\n");
    printf("localCool: %p\n", (void*)&localCool);
    printf("localNotInitialized: %p\n", (void*)&localNotInitialized);
    printf("localStaticCool: %p\n", (void*)&localStaticCool);
    printf("localStaticNotInitialized: %p\n", (void*)&localStaticNotInitialized);

    printf("\nmain arguments\n");
    printf("argc: %p\n", (void*)&argc);
    printf("argv: %p\n", (void*)&argv);

    printf("\nwaiting for char\n");
    getchar();  

    return 0;
}
