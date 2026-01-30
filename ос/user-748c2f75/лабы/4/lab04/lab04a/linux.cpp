//#include <iostream>
//#include <pthread.h>
//#include <unistd.h>    
//#include <sys/syscall.h> 
//#include <sys/types.h>  
//#include <cstdio>
//
//using namespace std;
//
//#define MAX_THREADS 2
//
//void* Handle(void* arg) {
//    int n = *((int*)arg);
//
//    const char array[12] = {
//        'u', 's', 'e', 'r',
//        '7', '4', '8', 'c',
//        '2', 'f', '7', '5'
//    };
//
//    for (int i = 0; i < n; i++) {
//        int index = i % 12;
//        printf("PID: %d  TID: %ld  process %d - %c\n",
//            getpid(),
//            syscall(SYS_gettid),
//            i,
//            array[index]);
//        usleep(300000);
//    }
//
//    return nullptr;
//}
//
//int main() {
//    cout << "PID of main process: " << getpid() << endl;
//
//    pthread_t threads[MAX_THREADS];
//    int parm1 = 50, parm2 = 125, parm3 = 100;
//
//    if (pthread_create(&threads[0], nullptr, Handle, &parm1)) {
//        cerr << "Thread 1: FAILURE\n";
//        return 1;
//    }
//
//    if (pthread_create(&threads[1], nullptr, Handle, &parm2)) {
//        cerr << "Thread 2: FAILURE\n";
//        return 1;
//    }
//
//    cout << "==== Main thread work =====" << endl;
//    Handle(&parm3);
//
//    cout << "\nWaiting for threads...." << endl;
//    for (int i = 0; i < MAX_THREADS; i++) {
//        pthread_join(threads[i], nullptr);
//    }
//
//    cout << "==== All threads finished =====" << endl;
//    return 0;
//}
