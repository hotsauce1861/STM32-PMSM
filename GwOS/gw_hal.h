#ifndef GW_HAL_H
#define GW_HAL_H

#include "stm32f10x.h"
#include "core_cm3.h"

#define HAL_ENABLE_IRQ 		__enable_irq
#define HAL_DISABLE_IRQ 	__disable_irq
#define HAL_SLEEP			__WFI()
#define HAL_WAKEUP			__WFE()	

#endif