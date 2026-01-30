#include <windows.h>
#include <stdio.h>

void wait_enter(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("enter to continue :kekw:\n");
    getchar(); 
}

int main(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    size_t page = si.dwPageSize;
    size_t size256 = 256 * page;
    size_t size128 = 128 * page;

    void* mem = VirtualAlloc(NULL, size256, MEM_RESERVE, PAGE_READWRITE);
    printf("reserve: %p\n", mem);
    wait_enter();

    void* half = (char*)mem + size128;
    VirtualAlloc(half, size128, MEM_COMMIT, PAGE_READWRITE);
    printf("commit\n");
    wait_enter();

    int* p = (int*)half;
    for (size_t i = 0; i < size128 / sizeof(int); i++) p[i] = (int)i;
    printf("fill\n");
    wait_enter();

    DWORD old;
    VirtualProtect(half, size128, PAGE_READONLY, &old);
    printf("readonly\n");
    wait_enter();

    VirtualFree(half, size128, MEM_DECOMMIT);
    printf("decommit\n");
    wait_enter();

    VirtualFree(mem, 0, MEM_RELEASE);
    printf("release\n");
    wait_enter();

    return 0;
}
