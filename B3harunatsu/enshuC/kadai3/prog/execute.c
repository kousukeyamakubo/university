#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_TRIALS 100

double get_execution_time(const char* command) {
    clock_t start, end;
    start = clock();
    system(command);
    end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

int main(int argc , char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int number = atoi(argv[1]);
    double total_time_h2c = 0.0;
    double total_time_mc = 0.0;
    double times_h2c[NUM_TRIALS];
    double times_mc[NUM_TRIALS];

    char command_h2c[256];

    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        snprintf(command_h2c, sizeof(command_h2c), "./h %d", number);
        times_h2c[trial] = get_execution_time(command_h2c);
        total_time_h2c += times_h2c[trial];

        times_mc[trial] = get_execution_time("./k");
        total_time_mc += times_mc[trial];
    }

    double mean_time_h2c = total_time_h2c / NUM_TRIALS;
    double mean_time_mc = total_time_mc / NUM_TRIALS;

    double variance_h2c = 0.0;
    double variance_mc = 0.0;

    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        variance_h2c += pow(times_h2c[trial] - mean_time_h2c, 2);
        variance_mc += pow(times_mc[trial] - mean_time_mc, 2);
    }

    variance_h2c /= NUM_TRIALS;
    variance_mc /= NUM_TRIALS;

    printf("hの平均: %e seconds\n", mean_time_h2c);
    printf("Variance of execution time for h2c: %e seconds^2\n", variance_h2c);
    printf("kの平均: %e seconds\n", mean_time_mc);
    printf("Variance of execution time for mc: %e seconds^2\n", variance_mc);

    double absolute_mean_difference = fabs(mean_time_h2c - mean_time_mc);
    printf("Absolute mean difference between h2c and mc: %e seconds\n", absolute_mean_difference);

    return 0;
}
