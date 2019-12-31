#include "velocity_curve.h"

int16_t t_curve(int16_t a, int16_t t,int16_t p0,int16_t vm, int16_t v){

	static uint8_t step = 0;

	switch (step)
	{
		case 0:
			if(v>=vm){
				step = 1;
			}
			break;
		case 1:
			break;
		case 2:
			break;
	}
	return 0;
}

int16_t s_curve(int16_t a, int16_t t, int16_t spd){

	return 0;
}




