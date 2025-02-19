#ifndef LIBK__H
#define LIBK__H

#include "kernel.h"

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)

unsigned int    ft_strlen(const char *str);
int             ft_strcmp(const char *s1, const char *s2);
void            ft_strcat(char *dest, const char *src);
char            *ft_strcpy(char *dest, const char *src);
void            *ft_memset (void *s, int c, uint32_t n);
void            *ft_memcpy(void *dest, const void *src, uint32_t n);
void            ft_printk(const char *fmt, ...);
char            *ft_itoa(int nb);


#endif