//#include <iostream>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <sys/types.h>
//#include <sched.h>
//#include <sys/resource.h>
//#include <cstdlib>
//#include <cstring>
//#include <sys/syscall.h>
//
//using namespace std;
//
//int main(int argc, char** argv) {
//    if (argc < 4) {
//        cout << "Usage: lab05b <P1_mask> <P2_priority> <P3_priority>\n";
//        return 1;
//    }
//
//    unsigned long long affinityMask = strtoull(argv[1], nullptr, 10);
//    int priority1 = atoi(argv[2]);
//    int priority2 = atoi(argv[3]);
//
//    cout << "Affinity mask (P1): " << affinityMask << endl;
//    cout << "Child 1 priority (P2): " << priority1 << endl;
//    cout << "Child 2 priority (P3): " << priority2 << endl;
//
//    pid_t pids[2];
//
//    for (int i = 0; i < 2; i++) {
//        cout << (i + 1) << " child\n";
//        pid_t pid = fork();
//
//        if (pid < 0) {
//            cerr << "fork failed: " << strerror(errno) << endl;
//            return 1;
//        }
//
//        if (pid == 0) {
//            // Настройка маски процессоров
//            cpu_set_t cpuset;
//            CPU_ZERO(&cpuset);
//            int nproc = sysconf(_SC_NPROCESSORS_CONF);
//            for (int j = 0; j < nproc; j++) {
//                if (affinityMask & (1ULL << j))
//                    CPU_SET(j, &cpuset);
//            }
//
//            if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
//                cerr << "sched_setaffinity failed: " << strerror(errno) << endl;
//                _exit(1);
//            }
//
//            // Установка приоритета
//            int prio = (i == 0) ? priority1 : priority2;
//            if (setpriority(PRIO_PROCESS, 0, prio) != 0) {
//                cerr << "setpriority failed: " << strerror(errno) << endl;
//                _exit(1);
//            }
//
//            cout << "Child " << (i + 1) << " PID: " << getpid()
//                << " TID: " << syscall(SYS_gettid) << " running...\n";
//
//            // Цикл на 1 миллион итераций с задержкой каждые 1000
//            const long totalIterations = 1'000'000;
//            const int printInterval = 1000;
//            const useconds_t sleepMicroSec = 200'000; // 200 мс
//
//            for (long iter = 1; iter <= totalIterations; ++iter) {
//                if (iter % printInterval == 0) {
//                    cout << "Child " << (i + 1) << " iteration: " << iter << endl;
//                    usleep(sleepMicroSec);
//                }
//            }
//
//            cout << "Child " << (i + 1) << " finished all iterations.\n";
//            _exit(0);
//        }
//
//        pids[i] = pid;
//    }
//
//    // Ожидание дочерних процессов
//    for (int i = 0; i < 2; i++) {
//        int status;
//        waitpid(pids[i], &status, 0);
//    }
//
//    cout << "Finished all children.\n";
//
//    return 0;
//}
