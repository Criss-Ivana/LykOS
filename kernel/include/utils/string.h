#pragma once

#include <stddef.h>

// Copying

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, size_t n);

// Concatenation

char *strcat(char *dest, const char *src);

// Searching

char *strchr(const char *s, char c);

char *strrchr(const char *s, int c);

char *strstr(const char *s1, const char *s2);

char *strtok_r(char *str, const char *delim, char **saveptr);

// Comparison

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

// Other

size_t strlen(const char *s);

char *strdup(const char *s);
