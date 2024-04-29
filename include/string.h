#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>

// extern char * strerror(int errno);


// x86-32使用extern inline基本上就是把汇编展开了
// 对于RISC来说, 这样做会有膨胀代码量的风险, 没有必要
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int count);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, int count);
size_t strcmp(const char *cs, const char *ct);
size_t strncmp(const char *cs, const char *ct, int count);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
size_t strspn(const char *cs, const char *ct);
size_t strcspn(const char *cs, const char *ct);
char *strpbrk(const char *cs, const char *ct);
char *strstr(const char *cs, const char *ct);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
char *strtok(char *s, const char *ct);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
size_t memcmp(const void *cs, const void *ct, size_t count);
void *memchr(const void *s, int c, size_t n);
void *memset(void *s, int c, size_t count);

#endif