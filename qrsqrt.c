#include <memory.h>
typedef double f64;
typedef long long i64;
char dummy;
f64 q_rsqrt(f64 x) {
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