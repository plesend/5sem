//#include <windows.h>
//#include <stdio.h>
//
//void wait_enter(void) {
//    while (getchar() != '\n');
//    printf("press enter to continue\n");
//}
//
//int main(void) {
//    HANDLE heap = HeapCreate(0, 1024 * 1024, 8 * 1024 * 1024);
//    if (!heap) { printf("heap create failed\n"); return 1; }
//    printf("heap created\n");
//    wait_enter();
//
//    void* blocks[10];
//    for (int i = 0; i < 10; i++) {
//        blocks[i] = HeapAlloc(heap, 0, 512 * 1024);
//        if (!blocks[i]) { printf("alloc %d failed\n", i); return 1; }
//        printf("alloc %d\n", i);
//        wait_enter();
//    }
//
//    for (int i = 0; i < 10; i++) {
//        int* p = (int*)blocks[i];
//        for (int j = 0; j < 512 * 1024 / sizeof(int); j++) p[j] = j;
//    }
//    printf("filled blocks\n");
//    wait_enter();
//
//    for (int i = 0; i < 10; i++) {
//        HeapFree(heap, 0, blocks[i]);
//        printf("free %d\n", i);
//        wait_enter();
//    }
//
//    HeapDestroy(heap);
//    printf("heap destroyed\n");
//    wait_enter();
//
//    return 0;
//}
