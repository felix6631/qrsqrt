// build: clang -O2 main.c -o bench -lm     (qrsqrt.c should be in same folder.)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "qrsqrt.c"          // double q_rsqrt(double) 제공


#define START 1.0
#define END   10.0
#define N     20000000LL     // 샘플 개수
#define STEP  ((END - START) / (double)N)
#define REPS  7              // 반복 측정 횟수 (최솟값 사용)

typedef double (*rsqrt_fn)(double);

static double ref_rsqrt(double x) { return 1.0 / sqrt(x); }

typedef struct { const char *name; rsqrt_fn fn; } entry_t;

static const entry_t TABLE[] = {
    { "1/sqrt(x)  (기준)",   ref_rsqrt },
    { "q_rsqrt",            q_rsqrt   },
#if defined(__aarch64__)
    { "frsqrte only",       hw_rsqrt_0 },
    { "frsqrte + NR x1",    hw_rsqrt_1 },
    { "frsqrte + NR x2",    hw_rsqrt_2 },
    { "frsqrte + NR x3",    hw_rsqrt_3 },
#endif
};

#define NFUNC (sizeof(TABLE) / sizeof(TABLE[0]))

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

// 누산기 4개로 덧셈 의존성 체인을 끊어서 latency가 아닌 throughput 위주로 측정.
__attribute__((noinline))
static double bench_once(rsqrt_fn fn) {
    double s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (long long k = 1; k + 3 <= N; k += 4) {
        s0 += fn(START + (k    ) * STEP);
        s1 += fn(START + (k + 1) * STEP);
        s2 += fn(START + (k + 2) * STEP);
        s3 += fn(START + (k + 3) * STEP);
    }
    return s0 + s1 + s2 + s3;   // 반환해서 컴파일러가 루프를 지우지 못하게 함
}

// 최소 시간(초)을 반환. sink에 결과 합을 넘겨서 출력에 사용.
static double bench(rsqrt_fn fn, double *sink) {
    double best = 1e30;
    bench_once(fn);                          // warm-up
    for (int r = 0; r < REPS; r++) {
        double t0 = now();
        *sink = bench_once(fn);
        double dt = now() - t0;
        if (dt < best) best = dt;
    }
    return best;
}

// 기준 함수 대비 상대 오차 |f-ref|/ref 의 최대/평균
static void rel_error(rsqrt_fn fn, double *max_e, double *avg_e) {
    double mx = 0, sum = 0;
    long long cnt = 0;
    for (long long k = 1; k <= N; k++) {
        double x = START + k * STEP;
        double ref = ref_rsqrt(x);
        double e = fabs((fn(x) - ref) / ref);
        if (!isfinite(e)) continue;
        if (e > mx) mx = e;
        sum += e;
        cnt++;
    }
    *max_e = mx;
    *avg_e = cnt ? sum / cnt : 0;
}

int main(void) {
    double base_time = 0;

    printf("범위 [%.1f, %.1f], 샘플 %lld개, %d회 중 최소\n\n", START, END, N, REPS);
    printf("%-22s %10s %8s %14s %14s   %s\n",
           "함수", "시간(s)", "배속", "최대 상대오차", "평균 상대오차", "checksum");

    for (size_t i = 0; i < NFUNC; i++) {
        double sink, mx, av;
        double t = bench(TABLE[i].fn, &sink);
        rel_error(TABLE[i].fn, &mx, &av);
        if (i == 0) base_time = t;
        printf("%-22s %10.4f %7.2fx %14.3e %14.3e   %.3f\n",
               TABLE[i].name, t, base_time / t, mx, av, sink);
    }
    return 0;
}