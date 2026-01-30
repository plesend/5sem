#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TASKS 10
#define QUANTUM 90

typedef struct {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int whenCompleted;
    int waitingTime;
    int fromArrivalToCompleted;
} Task;

typedef struct {
    int items[MAX_TASKS];
    int front;
    int rear;
} Queue;

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
}

int isEmpty(Queue* q) {
    return q->rear < q->front;
}

void enqueue(Queue* q, int value) {
    if (q->rear < MAX_TASKS - 1) {
        q->items[++(q->rear)] = value;
    }
}

int dequeue(Queue* q) {
    if (!isEmpty(q)) {
        return q->items[(q->front)++];
    }
    return -1;
}

int main() {
    Task tasks[MAX_TASKS] = {
        {1, 0, 80, 80, 3, 0, 0, 0},
        {2, 20, 45, 45, 5, 0, 0, 0},
        {3, 30, 60, 60, 2, 0, 0, 0},
        {4, 50, 30, 30, 4, 0, 0, 0},
        {5, 70, 90, 90, 1, 0, 0, 0},
        {6, 100, 55, 55, 3, 0, 0, 0},
        {7, 120, 75, 75, 2, 0, 0, 0},
        {8, 150, 40, 40, 5, 0, 0, 0},
        {9, 180, 65, 65, 4, 0, 0, 0},
        {10, 200, 85, 85, 1, 0, 0, 0}
    };

    printf("exec order: \n");

    int currentTime = 0;
    int doneAmount = 0;
    int nextTask = 0;

    Queue taskQueue;
    initQueue(&taskQueue);

    int inTaskQueue[MAX_TASKS] = { 0 };

    clock_t startClock = clock();

    while (doneAmount < MAX_TASKS) {
        while (nextTask < MAX_TASKS && tasks[nextTask].arrivalTime <= currentTime) {
            if (!inTaskQueue[nextTask]) {
                enqueue(&taskQueue, nextTask);
                inTaskQueue[nextTask] = 1;
            }
            nextTask++;
        }

        if (isEmpty(&taskQueue)) {
            if (nextTask < MAX_TASKS) {
                currentTime = tasks[nextTask].arrivalTime;
                enqueue(&taskQueue, nextTask);
                inTaskQueue[nextTask] = 1;
                nextTask++;
            }
        }

        int currentTaskIndex = dequeue(&taskQueue);

        if (tasks[currentTaskIndex].remainingTime <= QUANTUM) {
            currentTime += tasks[currentTaskIndex].remainingTime;

            tasks[currentTaskIndex].remainingTime = 0;
            tasks[currentTaskIndex].whenCompleted = currentTime;
            tasks[currentTaskIndex].fromArrivalToCompleted =
                tasks[currentTaskIndex].whenCompleted - tasks[currentTaskIndex].arrivalTime;

            tasks[currentTaskIndex].waitingTime =
                tasks[currentTaskIndex].fromArrivalToCompleted - tasks[currentTaskIndex].burstTime;

            doneAmount++;
            printf("%d -> ", tasks[currentTaskIndex].id);
        }
        else {
            currentTime += QUANTUM;
            tasks[currentTaskIndex].remainingTime -= QUANTUM;
            enqueue(&taskQueue, currentTaskIndex);
        }

        while (nextTask < MAX_TASKS && tasks[nextTask].arrivalTime <= currentTime) {
            if (!inTaskQueue[nextTask]) {
                enqueue(&taskQueue, nextTask);
                inTaskQueue[nextTask] = 1;
            }
            nextTask++;
        }
    }

    clock_t endClock = clock();

    double ms = (double)(endClock - startClock) / CLOCKS_PER_SEC * 1000.0;
    double sec = (double)(endClock - startClock) / CLOCKS_PER_SEC;

    double sum_wait = 0;
    double sum_turn = 0;
    double sum_burst = 0;
    double total_time = 0;

    for (int i = 0; i < MAX_TASKS; i++) {
        sum_wait += tasks[i].waitingTime;
        sum_turn += tasks[i].fromArrivalToCompleted;
        sum_burst += tasks[i].burstTime;

        if (tasks[i].whenCompleted > total_time)
            total_time = tasks[i].whenCompleted;
    }

    printf("\nAverage wait: %f units\n", sum_wait / MAX_TASKS);
    printf("Average turnaround: %f units\n", sum_turn / MAX_TASKS);
    printf("Average burst: %f units\n", sum_burst / MAX_TASKS);
    printf("Total execution: %f units\n", total_time);
    printf("Tasks per unit: %f\n", MAX_TASKS / total_time);

    printf("\nRuntime: %f ms (%f s)\n", ms, sec);

    return 0;
}