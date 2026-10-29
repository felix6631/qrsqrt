#include <memory.h>

#if defined(__aarch64__)
#include <arm_neon.h>
#endif

typedef double f64;
typedef long long i64;
char dummy;
static inline f64 q_rsqrt(f64 x) {
    f64 xhalf = 0.5 * x;
    long long i;

    //i = *(i64*)&x;
    memcpy(&i, &x, sizeof i);
    i = 0x5fe6eb50c7b537a9LL - (i >> 1);

    //x = *(double*)&i;
    memcpy(&x, &i, sizeof x);
    x = x * (1.5 - xhalf * x * x);
    return x;
}

#if defined(__aarch64__)
static double hw_rsqrt_0(double x) { return vrsqrted_f64(x); }

static double hw_rsqrt_1(double x) {
    double y = vrsqrted_f64(x);
    y *= vrsqrtsd_f64(x * y, y);
    return y;
}

static double hw_rsqrt_2(double x) {          // frsqrte + 뉴턴 2회
    double y = vrsqrted_f64(x);
    y *= vrsqrtsd_f64(x * y, y);
    y *= vrsqrtsd_f64(x * y, y);
    return y;
}
static double hw_rsqrt_3(double x) {          // frsqrte + 뉴턴 3회
    double y = vrsqrted_f64(x);
    y *= vrsqrtsd_f64(x * y, y);
    y *= vrsqrtsd_f64(x * y, y);
    y *= vrsqrtsd_f64(x * y, y);
    return y;
}


#endif