/**
************************************************************************************************
 * @file    : gw_log.h
 * @brief   : Interface to manage events
 * @details : None
 * @date    : 11-09-2018
 * @version : v1.0.0.0
 * @author  : UncleMac
 * @email   : zhaojunhui1861@163.com
 *
 *      @license    : GNU General Public License, version 3 (GPL-3.0)
 *
 *      Copyright (C)2019 UncleMac.
 *
 *      This code is open source: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This code is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.If not, see < https://www.gnu.org/licenses/>.
*************************************************************************************************
*/
#ifndef GW_LOG_H	
#define GW_LOG_H	

#include <stdio.h>

/* define compiler specific symbols */
#if defined ( __CC_ARM   )
#if USE_PRINTF
extern int printf(const char *, ...);
#define fprintf(fmt, args...) printf( ##args)
#else
extern int usart_printf(const char *, ...);
#define fprintf(fmt, args...) usart_printf( ##args)
#endif
#elif defined   (  __GNUC__  )
	
#if USE_PRINTF
extern int printf(const char *, ...);
#define fprintf(fmt, args...) printf( ##args)
#else
extern int usart_printf(const char *, ...);
#define fprintf(fmt, args...)         usart_printf(##args)
#endif

#endif //endif of __CC_ARM  __GNUC__


#define LOG_OUTPUT 		1

#define LOG_DEBUG		1
#define LOG_INFO		2
#define LOG_KEY			3
#define DEBUG_WARRING	4
#define DEBUG_ERROR		5


#if LOG_OUTPUT
#if (LOG_OUTPUT == LOG_DEBUG)
#define Log_d(tag, format, ...) fprintf (tag " Debug:<%s @%d @%s> "format"\r\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
#else 
#define Log_d(tag, format, ...)
#endif
#if (LOG_OUTPUT == LOG_INFO)
#define Log_i(tag, format, ...) fprintf (tag " Info:<%s @%d @%s> "format"\r\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
#else 
#define Log_i(tag, format, ...)
#endif
#if (LOG_OUTPUT == LOG_KEY)
#define Log_k(tag, format, ...) fprintf (tag " Key:<%s @%d @%s> "format"\r\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
#else 
#define Log_k(tag, format, ...)
#endif
#if (LOG_OUTPUT == DEBUG_WARRING)
#define Log_w(tag, format, ...) fprintf (tag " Warring:<%s @%d @%s> "format"\r\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
#else 
#define Log_w(tag, format, ...)
#endif
#if (LOG_OUTPUT == DEBUG_ERROR)
#define Log_e(tag, format, ...) fprintf (tag " Error:<%s @%d @%s> "format"\r\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
#else 
#define Log_e(tag, format, ...)
#endif
#else /* LOG_OUTPUT */
#define Log_d(tag, format, ...)
#define Log_i(tag, format, ...)
#define Log_k(tag, format, ...)
#define Log_w(tag, format, ...)
#define Log_e(tag, format, ...)

#endif 

#endif //endif for GW_LOG_H
