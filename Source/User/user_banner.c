#include "user_banner.h"
#include "usart_driver.h"

void user_banner_show(void){

	usart_printf(" \r\n ============== PMSM Version 0.01 ==============");
	
	usart_printf("\r\n      ____  __  ________ __  ___	");
	usart_printf("\r\n	   / __ \\/  |/  / ___//  |/  /	");
	usart_printf("\r\n	  / /_/ / /|_/ /\\__ \\/ /|_/ /	");
	usart_printf("\r\n	 / ____/ /  / /___/ / /  / /	");
	usart_printf("\r\n	/_/   /_/  /_//____/_/  /_/		");
	usart_printf("\r\n");
	usart_printf("\r\n    __                           __                          __");
	usart_printf("\r\n   / /_  ____  ____  ____ ______/ /_  ___  ____  ____ ____  / /__   _________  ____ ___");
	usart_printf("\r\n  / __ \\/ __ \\/ __ \\/ __ `/ ___/ __ \\/ _ \\/ __ \\/ __ `/ _ \\/ / _ \\ / ___/ __ \\/ __ `__ \\");
	usart_printf("\r\n / / / / /_/ / / / / /_/ (__  ) / / /  __/ / / / /_/ /  __/ /  __// /__/ /_/ / / / / / /");
	usart_printf("\r\n/_/ /_/\\____/_/ /_/\\__, /____/_/ /_/\\___/_/ /_/\\__, /\\___/_/\\___(_)___/\\____/_/ /_/ /_/");
	usart_printf("\r\n                  /____/                      /____/");
	usart_printf(" \r\n hongshengele.com");
	usart_printf(" \r\n ===============================================");
}