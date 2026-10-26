#include <stdio.h>

typedef double f64;
typedef long long i64;

f64 q_rsqrt(double x) {
    f64 xhalf = 0.5 * x;
    long long i;

    i = *(i64*)&x;
    i = 0x5fe6ec85e7de30aLL - (i >> 1);
    x = *(double*)&i;
    x = x * (1.5 - xhalf * x * x);

    return x;
}