#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TASKS 10

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

typedef struct Node {
    Task* task;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

void enqueue(Queue* q, Task* task) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->task = task;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    }
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

Task* dequeue(Queue* q) {
    if (q->front == NULL) return NULL;

    Node* temp = q->front;
    Task* task = temp->task;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return task;
}

int isEmpty(Queue* q) {
    return q->front == NULL;
}

Task* peek(Queue* q) {
    if (q->front == NULL) return NULL;
    return q->front->task;
}

Task* findHighestPriority(Queue* q) {
    if (q->front == NULL) return NULL;

    Node* current = q->front;
    Task* highest = current->task;

    while (current != NULL) {
        if (current->task->priority < highest->priority ||
            (current->task->priority == highest->priority &&
                current->task->arrivalTime < highest->arrivalTime)) {
            highest = current->task;
        }
        current = current->next;
    }
    return highest;
}

void removeTask(Queue* q, Task* task) {
    if (q->front == NULL) return;

    if (q->front->task == task) {
        Node* temp = q->front;
        q->front = q->front->next;
        if (q->front == NULL) q->rear = NULL;
        free(temp);
        return;
    }

    Node* current = q->front;
    while (current->next != NULL) {
        if (current->next->task == task) {
            Node* temp = current->next;
            current->next = current->next->next;
            if (current->next == NULL) q->rear = current;
            free(temp);
            return;
        }
        current = current->next;
    }
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

    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].remainingTime = tasks[i].burstTime;
    }

    clock_t startClock = clock();

    Queue readyQueue;
    initQueue(&readyQueue);

    int executionOrder[100];
    int orderSize = 0;

    int currentTime = 0;
    int completedTasks = 0;
    Task* currentTask = NULL;

    while (completedTasks < MAX_TASKS) {
        for (int i = 0; i < MAX_TASKS; i++) {
            if (tasks[i].arrivalTime == currentTime) {
                enqueue(&readyQueue, &tasks[i]);
            }
        }

        Task* nextTask = findHighestPriority(&readyQueue);

        if (nextTask != NULL && currentTask != NULL &&
            nextTask->priority < currentTask->priority) {
            enqueue(&readyQueue, currentTask);
            currentTask = NULL;
        }

        if (currentTask == NULL && !isEmpty(&readyQueue)) {
            currentTask = findHighestPriority(&readyQueue);
            removeTask(&readyQueue, currentTask);

            if (orderSize == 0 || executionOrder[orderSize - 1] != currentTask->id) {
                executionOrder[orderSize++] = currentTask->id;
            }
        }

        if (currentTask != NULL) {
            currentTask->remainingTime--;

            for (int i = 0; i < MAX_TASKS; i++) {
                if (&tasks[i] != currentTask &&
                    tasks[i].arrivalTime <= currentTime &&
                    tasks[i].remainingTime > 0) {
                    tasks[i].waitingTime++;
                }
            }

            if (currentTask->remainingTime == 0) {
                currentTask->whenCompleted = currentTime + 1;
                currentTask->fromArrivalToCompleted =
                    currentTask->whenCompleted - currentTask->arrivalTime;
                completedTasks++;
                currentTask = NULL;
            }
        }

        currentTime++;
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

        if (tasks[i].whenCompleted > total_time) {
            total_time = tasks[i].whenCompleted;
        }
    }

    printf("exec order: ");
    for (int i = 0; i < orderSize; i++) {
        printf("%d", executionOrder[i]);
        if (i != orderSize - 1)
            printf(" -> ");
    }
    printf("\n");

    printf("\nAverage wait: %f units\n", sum_wait / MAX_TASKS);
    printf("Average turnaround: %f units\n", sum_turn / MAX_TASKS);
    printf("Average burst: %f units\n", sum_burst / MAX_TASKS);
    printf("Total execution: %f units\n", total_time);
    printf("Tasks per unit: %f\n", MAX_TASKS / total_time);

    printf("\nRuntime: %f ms (%f s)\n", ms, sec);

    return 0;
}