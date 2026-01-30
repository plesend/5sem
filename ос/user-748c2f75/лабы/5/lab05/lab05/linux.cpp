//#include <iostream>
//#include <unistd.h>
//#include <sched.h>
//#include <sys/syscall.h>
//
//using namespace std;
//
//string toBinary(cpu_set_t& set, int maxCpu) {
//    string s;
//    for (int i = maxCpu - 1; i >= 0; --i) {
//        s += CPU_ISSET(i, &set) ? '1' : '0';
//    }
//    return s;
//}
//
//int main() {
//    cpu_set_t set;
//    CPU_ZERO(&set);
//    sched_getaffinity(0, sizeof(set), &set);
//    int maxCpu = sysconf(_SC_NPROCESSORS_CONF);
//
//    int policy = sched_getscheduler(0); 
//    sched_param param;
//    sched_getparam(0, &param); 
//
//    cout << "PID: " << getpid()
//        << " TID: " << syscall(SYS_gettid)
//        << "\nPriorproc: " << policy
//        << "\nPriorthread: " << param.sched_priority
//        << "\nAffinMask: " << toBinary(set, maxCpu)
//        << "\nProcNum: " << sched_getcpu() << endl;
//
//    return 0;
//}
