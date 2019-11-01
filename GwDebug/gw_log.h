#ifndef GW_LOG_H	
#define GW_LOG_H	

#include <stdio.h>

extern int printf(const char *, ...);
#define fprintf(fmt, args...) printf( ##args)

#if LOG_OUTPUT
#if (DEBUG_DEBUG)
#define Log_d(tag, format, ...) fprintf (tag, "Debug:<Func:%s @line:%d> "format"\n", __func__, __LINE__, ##__VA_ARGS__)
#else 
#define Log_d(tag, format, ...)
#endif
#if (DEBUG_INFO)
#define Log_i(tag, format, ...) fprintf (tag, "Info:<Func:%s> "format"\n", __func__, ## __VA_ARGS__)
#else 
#define Log_i(tag, format, ...)
#endif
#if (DEBUG_KEY)
#define Log_k(tag, format, ...) fprintf (tag, "Key:<Func:%s @line:%d> "format"\n", __func__, __LINE__, ##__VA_ARGS__)
#else 
#define Log_k(tag, format, ...)
#endif
#if (DEBUG_WARRING)
#define Log_w(tag, format, ...) fprintf (tag, "Warring:<Func:%s @line:%d> "format"\n", __func__, __LINE__, ##__VA_ARGS__)
#else 
#define Log_w(tag, format, ...)
#endif
#if (DEBUG_ERROR)
#define Log_e(tag, format, ...) fprintf (tag, "Error:<Func:%s @Line:%d @File:%s> "format"\n", __func__, __LINE__, __FILE__, ##__VA_ARGS__)
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
