/********************************************************
File: SLOPE_FILTER.h
********************************************************/
#ifndef SLOPE_FILTER_H
#define SLOPE_FILTER_H

#include <stdint.h>

void slope_lpf(float *data_in, int frame_length, float *state, const float *slope_b,  const float *slope_a);
void slope_hpf(float *data_in, int frame_length, float *state, const  float *slope_b, const float *slope_a);

#endif /* SLOPE_FILTER_H */
