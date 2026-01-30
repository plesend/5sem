//#include <iostream>
//#include <unistd.h>
//#include <sys/syscall.h>
//#include <sched.h>
//#include <sys/time.h>
//#include <sys/resource.h>
//#include <chrono>
//
//using namespace std;
//using namespace std::chrono;
//
//long get_tid() {
//    return syscall(SYS_gettid);
//}
//
//int main() {
//    const long totalIterations = 1000000;
//    const int printInterval = 1000;
//    const useconds_t sleepMicroSec = 200000; 
//
//    auto startTime = high_resolution_clock::now();
//
//    pid_t pid = getpid();
//    int niceValue = getpriority(PRIO_PROCESS, pid);
//
//    for (long i = 1; i <= totalIterations; ++i) {
//        if (i % printInterval == 0) {
//            int cpuNum = sched_getcpu();
//            cout << "Iteration: " << i
//                << " PID: " << pid
//                << " TID: " << get_tid()
//                << " Nice: " << niceValue
//                << " CPU: " << cpuNum
//                << endl;
//
//            usleep(sleepMicroSec); 
//        }
//    }
//
//    auto endTime = high_resolution_clock::now();
//    duration<double> elapsed = endTime - startTime;
//
//    cout << "\ntime: " << elapsed.count() << " seconds (" << elapsed.count() * 1000 << " ms)" << endl;
//
//    return 0;
//}
