#include "feed_forward.h"

int16_t ff_calc_result(ff_mod_t *ff){
	
	int16_t result = 0;
	result = ff->a_factor*(ff->ein - ff->last_ein)
		+ ff->b_factor*(ff->ein - 2*ff->last_ein + ff->last_last_ein);
	
	ff->last_last_ein = ff->last_ein;
	ff->last_ein = ff->ein;
	
	return result;
}