#include "utils/string.h"

#include <mm/heap.h>

// Copying

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while ((*dest++ = *src++))
        ;

    return ret;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    char *ret = dest;

    while (n && (*dest++ = *src++) != '\0')
        n--;
    while (n--)
        *dest++ = '\0';

    return ret;
}

// Concatenation

char *strcat(char *dest, const char *src)
{
    char *ret = dest;

    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;

    return ret;
}

// Comparison

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && (*s1 == *s2))
        s1++, s2++, n--;

    if (n == 0)
        return 0;
    else
        return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Searching

char *strchr(const char *s, char c)
{
    while (*s)
    {
        if (*s == c)
            return (char *)s;
        s++;
    }

    return NULL;
}

char *strrchr(const char *s, int c)
{
    char *ret = NULL;

    while (*s)
    {
        if (*s == c)
            ret = (char *)s;
        s++;
    }

    return ret;
}

char *strstr(const char *s1, const char *s2)
{
    if (!*s2)
        return (char *)s1;

    for (; *s1; s1++)
    {
        const char *h = s1, *n = s2;

        while (*h && *n && *h == *n)
            h++, n++;

        if (!*n)
            return (char *)s1;
    }

    return NULL;
}

// Other

size_t strlen(const char *s)
{
    size_t len = 0;

    while (*s++)
        len++;

    return len;
}

char *strdup(const char *s)
{
    if (!s)
        return NULL;

    char *dup = heap_alloc(strlen(s) + 1);
    if (!dup)
        return NULL;
    strcpy(dup, s);

    return dup;
}
