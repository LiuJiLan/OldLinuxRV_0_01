#include <string.h>

char *strcpy(char *dest, const char *src) {
    char *tmp = dest;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

char *strncpy(char *dest, const char *src, int count) {
    char *tmp = dest;
    while (count-- && (*dest++ = *src++) != '\0');
    while (count-- > 0) *dest++ = '\0';
    return tmp;
}

char *strcat(char *dest, const char *src) {
    char *tmp = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

char *strncat(char *dest, const char *src, int count) {
    char *tmp = dest;
    while (*dest) dest++;
    while (count-- && (*dest++ = *src++) != '\0');
    *dest = '\0';
    return tmp;
}

size_t strcmp(const char *cs, const char *ct) {
    while (*cs && *cs == *ct) {
        cs++;
        ct++;
    }
    return (*(unsigned char *)cs) - (*(unsigned char *)ct);
}

size_t strncmp(const char *cs, const char *ct, int count) {
    if (!count) return 0;
    while (--count && *cs && *cs == *ct) {
        cs++;
        ct++;
    }
    return (*(unsigned char *)cs) - (*(unsigned char *)ct);
}

char *strchr(const char *s, int c) {
    while (*s != (char)c) {
        if (!*s++) return NULL;
    }
    return (char *)s;
}


char *strrchr(const char *s, int c) {
    const char *last = NULL;
    do {
        if (*s == (char)c) last = s;
    } while (*s++);
    return (char *)last;
}

size_t strspn(const char *cs, const char *ct) {
    const char *p, *start = cs;
    for (; *cs; ++cs) {
        for (p = ct; *p && *p != *cs; ++p);
        if (!*p) break;
    }
    return cs - start;
}

size_t strcspn(const char *cs, const char *ct) {
    const char *p, *start = cs;
    for (; *cs; ++cs) {
        for (p = ct; *p; ++p) {
            if (*p == *cs) return cs - start;
        }
    }
    return cs - start;
}

char *strpbrk(const char *cs, const char *ct) {
    for (; *cs; ++cs) {
        for (const char *p = ct; *p; ++p) {
            if (*p == *cs) return (char *)cs;
        }
    }
    return NULL;
}

char *strstr(const char *cs, const char *ct) {
    if (!*ct) return (char *)cs;
    for (; *cs; ++cs) {
        const char *p1 = cs, *p2 = ct;
        while (*p2 && *p1 == *p2) {
            p1++;
            p2++;
        }
        if (!*p2) return (char *)cs;
    }
    return NULL;
}

size_t strlen(const char *s) {
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc);
    return sc - s;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') {
        len++;
    }
    return len;
}

char *strtok(char *s, const char *ct) {
    static char *olds;
    if (!s && !(s = olds)) return NULL;
    s += strspn(s, ct);
    if (!*s) return olds = NULL;
    char *token = s;
    s = strpbrk(token, ct);
    if (s) *s++ = '\0';
    olds = s;
    return token;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *dp = dest;
    const char *sp = src;
    while (n--) *dp++ = *sp++;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    char *dp = dest;
    const char *sp = src;
    if (dp < sp) {
        while (n--) *dp++ = *sp++;
    } else {
        dp += n;
        sp += n;
        while (n--) *--dp = *--sp;
    }
    return dest;
}

size_t memcmp(const void *cs, const void *ct, size_t count) {
    const unsigned char *p1 = cs, *p2 = ct;
    while (count-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++, p2++;
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = s;
    while (n-- > 0) {
        if (*p == (unsigned char)c) {
            return (void *)p;
        }
        p++;
    }
    return NULL;
}

void *memset(void *s, int c, size_t count) {
    char *xs = (char *)s;
    while (count-- > 0) {
        *xs++ = (char)c;
    }
    return s;
}

