#pragma once

#include "types.hpp"

void *memcpy(void* dest, const void* src, size_t len);
void *memset(void* dest, int byte, size_t len);
size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
char *strcpy(char* dest, const char* src);
char *strchr(const char *p, int ch);
char *strcat(char *dst, const char *src);
long atol(const char* str);
void *memmove(void* dst, const void* src, size_t n);
char *safestrcpy(char* s, const char* t, int n);
