#ifndef FEED_FORWARD_H
#define FEED_FORWARD_H
#include <stdint.h>

//https://www.cnblogs.com/foxclever/p/9311124.html
struct ff_mod{
	int16_t ein;
	int16_t last_ein;
	int16_t last_last_ein;	
	
	int16_t result;
	//Gff(s) = as^2 + bs
	int16_t a_factor;
	int16_t b_factor;
};

typedef struct ff_mod ff_mod_t;

int16_t ff_calc_result(ff_mod_t *ff);

#endif
