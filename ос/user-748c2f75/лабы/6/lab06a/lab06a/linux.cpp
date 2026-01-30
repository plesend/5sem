//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//#include <unistd.h>
//
//pthread_mutex_t mutex;
//
//const char array[13] = {
//    'u', 's', 'e', 'r', '-',
//    '7', '4', '8', 'c',
//    '2', 'f', '7', '5'
//};
//
//void ThreadFunc(const char* threadName)
//{
//    for (int i = 0; i <= 90; i++)
//    {
//        int lock = (i >= 30 && i <= 60);
//        if (lock)
//            pthread_mutex_lock(&mutex);
//
//        int index = i % 13;
//        char symb = array[index];
//
//        printf("[%s] iter %d char: %c\n", threadName, i, symb);
//        fflush(stdout);
//
//        if (lock)
//            pthread_mutex_unlock(&mutex);
//
//        usleep(100000);
//    }
//}
//
//void* AFunc(void* arg)
//{
//    ThreadFunc("A");
//    return NULL;
//}
//
//void* BFunc(void* arg)
//{
//    ThreadFunc("B");
//    return NULL;
//}
//
//int main()
//{
//    pthread_t A, B;
//
//    pthread_mutex_init(&mutex, NULL);
//
//    pthread_create(&A, NULL, AFunc, NULL);
//    pthread_create(&B, NULL, BFunc, NULL);
//
//    ThreadFunc("M");
//
//    pthread_join(A, NULL);
//    pthread_join(B, NULL);
//
//    pthread_mutex_destroy(&mutex);
//
//    return 0;
//}
