#include <windows.h>
#include <stdio.h>

void HeapInfo(HANDLE h)
{
    PROCESS_HEAP_ENTRY e;
    SIZE_T total = 0;

    e.lpData = NULL;
    while (HeapWalk(h, &e))
    {
        printf("addr=%p size=%llu ", e.lpData, (unsigned long long)e.cbData);

        if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY)
            printf("BUSY\n");
        else if (e.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE)
            printf("FREE\n");
        else if (e.wFlags & PROCESS_HEAP_REGION)
            printf("REGION\n");
        else
            printf("\n");

        total += e.cbData;
    }

    DWORD err = GetLastError();
    if (err != ERROR_NO_MORE_ITEMS)
        printf("HeapWalk error: %lu\n", err);

    printf("heap total size (sum of cbData)=%llu\n\n", (unsigned long long)total);
}



int main(void)
{
    HANDLE heap = HeapCreate(0, 1024 * 1024, 8 * 1024 * 1024);
    printf("heap created\n");
    HeapInfo(heap);
    system("pause & cls");

    void* blocks[10];

    for (int i = 0; i < 10; i++)
    {
        blocks[i] = HeapAlloc(heap, 0, 512 * 1024);
        printf("alloc %d\n", i);
        HeapInfo(heap);
        system("pause & cls");
    }

    for (int i = 0; i < 10; i++)
    {
        int* p = (int*)blocks[i];
        for (int j = 0; j < (512 * 1024) / sizeof(int); j++) p[j] = j;
    }

    for (int i = 0; i < 10; i++)
    {
        HeapFree(heap, 0, blocks[i]);
        printf("free %d\n", i);
        HeapInfo(heap);
        system("pause & cls");
    }

    HeapDestroy(heap);
    printf("heap destroyed\n");
    system("pause");

    return 0;
}
