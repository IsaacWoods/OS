/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <sys/cdefs.h>
#include <stdarg.h>

#define EOF (-1)

int vprintf(const char* __restrict, va_list);
int printf(const char* __restrict, ...);
char putchar(char);
int puts(const char*);
