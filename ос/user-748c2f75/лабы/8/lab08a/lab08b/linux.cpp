#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void wait_enter(void) {
    while (getchar() != '\n');
    printf("press enter to continue\n");
}

int main(void) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t total_pages = 256;
    size_t half_pages = 128;

    void* mem = mmap(NULL, total_pages * page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) { printf("reserve failed\n"); return 1; }
    printf("reserve: %p\n", mem);
    wait_enter();

    void* half = (char*)mem + half_pages * page_size;
    if (mprotect(half, half_pages * page_size, PROT_READ | PROT_WRITE) != 0) { printf("commit failed\n"); return 1; }
    printf("commit second half\n");
    wait_enter();

    int* p = (int*)half;
    for (size_t i = 0; i < (half_pages * page_size) / sizeof(int); i++) p[i] = (int)i;
    printf("filled second half\n");
    wait_enter();

    if (mprotect(half, half_pages * page_size, PROT_READ) != 0) { printf("readonly failed\n"); return 1; }
    printf("readonly\n");
    wait_enter();

    if (munmap(half, half_pages * page_size) != 0) { printf("decommit failed\n"); return 1; }
    printf("decommit second half\n");
    wait_enter();

    if (munmap(mem, half_pages * page_size) != 0) { printf("release failed\n"); return 1; }
    printf("release first half\n");
    wait_enter();

    return 0;
}
