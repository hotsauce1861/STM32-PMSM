#ifndef USER_FLASH_H
#define USER_FLASH_H
#include <stdint.h>

#define FLASH_PAGE_SIZE    		((uint16_t)0x400)

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x0801E000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x08020000)

#define PID_CUR_KP_ADDR			((uint16_t)0x0004)
#define PID_CUR_KI_ADDR			((uint16_t)0x0008)
#define PID_CUR_KD_ADDR			((uint16_t)0x000C)

#define PID_SPD_KP_ADDR			((uint16_t)0x0010)
#define PID_SPD_KI_ADDR			((uint16_t)0x0014)
#define PID_SPD_KD_ADDR			((uint16_t)0x0018)

#define PID_POS_KP_ADDR			((uint16_t)0x001C)
#define PID_POS_KI_ADDR			((uint16_t)0x0020)
#define PID_POS_KD_ADDR			((uint16_t)0x0024)

#define FF_FACTOR_A				((uint16_t)0x0028)
#define FF_FACTOR_B				((uint16_t)0x002C)

void user_flash_clear_all_data(void);
void user_flash_earse_page(uint16_t index);


#endif
