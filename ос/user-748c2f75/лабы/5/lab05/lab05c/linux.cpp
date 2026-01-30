//#include <iostream>
//#include <unistd.h>
//#include <sched.h>
//#include <sys/syscall.h>
//#include <sys/resource.h>
//#include <thread>
//#include <chrono>
//#include <vector>
//
//using namespace std;
//using namespace std::chrono;
//
//string toBinary(cpu_set_t& set, int maxCpu) {
//    string s;
//    for (int i = maxCpu - 1; i >= 0; --i) {
//        s += CPU_ISSET(i, &set) ? '1' : '0';
//    }
//    return s;
//}
//
//void threadFunction(int id, int niceValue, int cpuMask) {
//    int maxCpu = sysconf(_SC_NPROCESSORS_CONF);
//
//    setpriority(PRIO_PROCESS, 0, niceValue);
//
//    cpu_set_t cpuset;
//    CPU_ZERO(&cpuset);
//    for (int i = 0; i < maxCpu; ++i) {
//        if (cpuMask & (1 << i)) CPU_SET(i, &cpuset);
//    }
//    sched_setaffinity(0, sizeof(cpuset), &cpuset);
//
//    const int printInterval = 1000;
//    const chrono::milliseconds sleepDuration(200);
//    for (long i = 1; i <= 100000; ++i) {
//        if (i % printInterval == 0) {
//            int currNice = getpriority(PRIO_PROCESS, 0);
//            cout << "Thread " << id << " Iter: " << i
//                << " PID: " << getpid()
//                << " TID: " << syscall(SYS_gettid)
//                << " nice: " << currNice
//                << " CPU: " << sched_getcpu()
//                << endl;
//            this_thread::sleep_for(sleepDuration);
//        }
//    }
//}
//
//int main(int argc, char** argv) {
//    if (argc != 4) {
//        cout << "Usage: " << argv[0] << " P1 P2 P3\n";
//        return 1;
//    }
//
//    int P1 = stoi(argv[1]); 
//    int P2 = stoi(argv[2]); 
//    int P3 = stoi(argv[3]); 
//
//    cout << "Program parameters: P1 (CPU mask) = " << P1
//        << ", P2 (Thread1 priority) = " << P2
//        << ", P3 (Thread2 priority) = " << P3 << endl;
//
//    thread t1(threadFunction, 1, P2, P1);
//    thread t2(threadFunction, 2, P3, P1);
//
//    t1.join();
//    t2.join();
//
//    cout << "Threads finished." << endl;
//    return 0;
//}
