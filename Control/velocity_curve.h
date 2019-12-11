#ifndef VELOCITY_CURE_H
#define VELOCITY_CURE_H
#include <stdint.h>

int16_t t_curve(int16_t a, int16_t t,int16_t p0,int16_t vm, int16_t v);
int16_t s_curve(int16_t a, int16_t t, int16_t spd);

#endif