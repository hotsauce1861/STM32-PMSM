#ifndef __DISPLAY_DRIVER_H 
#define __DISPLAY_DRIVER_H 

#include <stdint.h>

struct dirver_74hc595 {
	uint8_t sck;	//serial clock
	uint8_t rck;
	uint8_t sdi; 	//serial_data_input	
	// hardware spec
	// pin out interface
	void (*init)(struct dirver_74hc595*);
	
	void (*write)(struct dirver_74hc595*, uint8_t);
	void (*output_sck)(uint8_t);
	void (*output_rck)(uint8_t);
	void (*output_sdi)(uint8_t);
};

void hc595_init(struct dirver_74hc595* pdev);
void hc595_write(struct dirver_74hc595* pdev, uint8_t data);
void hc595_output(struct dirver_74hc595* pdev);

#endif
