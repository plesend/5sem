//#include <stdio.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <semaphore.h>
//#include <sys/wait.h>
//
//#define SEMAPHORE_NAME "/semaphore"
//
//void mainProc(const char* procName)
//{
//    sem_t* sem = sem_open(SEMAPHORE_NAME, 0);
//    if (sem == SEM_FAILED) {
//        perror("cant open semaphore");
//        return;
//    }
//
//    const char array[13] = {
//        'u', 's', 'e', 'r', '-',
//        '7', '4', '8', 'c',
//        '2', 'f', '7', '5'
//    };
//
//    for (int i = 0; i <= 90; i++) {
//        int lock = (i >= 30 && i <= 60);
//
//        if (lock)
//            sem_wait(sem);
//
//        int index = i % 13;
//        char symb = array[index];
//        printf("[%s] iteration %d char: %c\n", procName, i, symb);
//        fflush(stdout);
//
//        if (lock)
//            sem_post(sem);
//
//        usleep(100000);
//    }
//
//    sem_close(sem);
//}
//
//int main()
//{
//    sem_t* sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
//    if (sem == SEM_FAILED) {
//        perror("sem_open");
//        return 1;
//    }
//
//    pid_t pidA = fork();
//    if (pidA == 0) {
//        mainProc("A");
//        return 0;
//    }
//
//    pid_t pidB = fork();
//    if (pidB == 0) {
//        mainProc("B");
//        return 0;
//    }
//
//    mainProc("MAIN");
//
//    waitpid(pidA, NULL, 0);
//    waitpid(pidB, NULL, 0);
//
//    sem_close(sem);
//    sem_unlink(SEMAPHORE_NAME);
//
//    return 0;
//}
