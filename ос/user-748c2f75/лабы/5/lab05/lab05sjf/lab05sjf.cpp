#include <stdio.h>
#include <limits.h>
#include <time.h>

struct process
{
    int pid;
    int arrive;
    int burst;
    int remain;
    int prio;
    int done;
    int wait;
    int turnaround;
};

void calc_times(struct process jobs[], int n)
{
    for (int i = 0; i < n; i++)
    {
        jobs[i].turnaround = jobs[i].done - jobs[i].arrive;
        jobs[i].wait = jobs[i].turnaround - jobs[i].burst;

        if (jobs[i].wait < 0)
            jobs[i].wait = 0;
    }
}

int main()
{
    const int N = 10;

    struct process jobs[N] =
    {
        {1, 0,   80, 80, 3, 0, 0, 0},
        {2, 20,  45, 45, 5, 0, 0, 0},
        {3, 30,  60, 60, 2, 0, 0, 0},
        {4, 50,  30, 30, 4, 0, 0, 0},
        {5, 70,  90, 90, 1, 0, 0, 0},
        {6, 100, 55, 55, 3, 0, 0, 0},
        {7, 120, 75, 75, 2, 0, 0, 0},
        {8, 150, 40, 40, 5, 0, 0, 0},
        {9, 180, 65, 65, 4, 0, 0, 0},
        {10,200, 85, 85, 1, 0, 0, 0}
    };

    printf("Run order: \n");

    int finished = 0;
    int tick = 0;
    int min_remain = INT_MAX;
    int cur_idx = 0;
    int found = 0;

    clock_t start = clock();

    while (finished != N)
    {
        for (int j = 0; j < N; j++)
        {
            if (jobs[j].arrive <= tick &&
                jobs[j].remain < min_remain &&
                jobs[j].remain > 0)
            {
                min_remain = jobs[j].remain;
                cur_idx = j;
                found = 1;
            }
        }

        if (!found)
        {
            tick++;
            continue;
        }

        jobs[cur_idx].remain--;
        min_remain = jobs[cur_idx].remain;

        if (min_remain == 0)
            min_remain = INT_MAX;

        if (jobs[cur_idx].remain == 0)
        {
            printf("%d -> ", jobs[cur_idx].pid);
            finished++;
            found = 0;
            jobs[cur_idx].done = tick + 1;
        }

        tick++;
    }
    clock_t end = clock();

    double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    double sec = (double)(end - start) / CLOCKS_PER_SEC;

    calc_times(jobs, N);

    double sum_wait = 0;
    double sum_turn = 0;
    double sum_burst = 0;
    double total_time = 0;

    for (int i = 0; i < N; i++)
    {
        sum_wait += jobs[i].wait;
        sum_turn += jobs[i].turnaround;
        sum_burst += jobs[i].burst;

        if (jobs[i].done > total_time)
            total_time = jobs[i].done;
    }

    printf("\nAveraga wait: %f units\n", sum_wait / N);
    printf("Averagag turnaround: %f units\n", sum_turn / N);
    printf("Averagag burst: %f units\n", sum_burst / N);
    printf("Total execution: %f units\n", total_time);
    printf("Tasks per unit: %f\n", N / total_time);

    printf("\nRuntime: %f ms (%f s)\n", ms, sec);

    return 0;
}