#pragma once

#include "kernel.h"

unsigned int    ft_strlen(const char *str);
int             ft_strcmp(const char *s1, const char *s2);
void            strcat(char *dest, const char *src);
char            *strcpy(char *dest, const char *src);
void            *memset (void *s, int c, uint32_t n);
void            *memcpy(void *dest, const void *src, uint32_t n);