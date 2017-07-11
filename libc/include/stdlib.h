/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, unsigned char, size_t);
__attribute__((__noreturn__)) void abort(void);
