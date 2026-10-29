#include <stdio.h>
#include <math.h>
#include "qrsqrt.c"
#include <time.h>
#include <unistd.h>

#define STEP 1e-7
#define START 0
#define END 10.0

void run_math() {
    for(double i = START+STEP; i <= END; i += STEP) {
        volatile double a = 1/sqrt(i);
    }
    return;
}

void run_fast() {
    for(double i = START+STEP; i <= END; i += STEP) {
        volatile double a = q_rsqrt(i);
    }
    return;
}

void run_err() {
    double avg = 0, max = 0, min = 0xFFFF;
    for(double i = START+STEP; i <= END; i += STEP) {
        double err = 1/sqrt(i)-q_rsqrt(i);
        //printf("%lf\n",err);
        avg += err;
        if (err > max && err != INFINITY)
            max = err;
        if (err < min)
            min = err;
    }
    avg /= ((END-START)/STEP);

    printf("max err: %lf\n",max);
    printf("min err: %lf\n",min);
    printf("avg err: %lf\n",avg);
    return;
}


int main() {
    
    printf("Benchmark 1/sqrt(x): ");
    clock_t start = clock();
    run_math();
    clock_t end = clock();
    double gap = (double)(end-start)/CLOCKS_PER_SEC;
    printf("%lf\n",gap);

    printf("Benchmark q_rsqrt(x): ");
    start = clock();
    run_fast();
    end = clock();
    gap = (double)(end-start)/CLOCKS_PER_SEC;
    printf("%lf\n",gap);

    
    run_err();

    return 0;
}