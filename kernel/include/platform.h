/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

__attribute__((__noreturn__))
void Internal_KernelPanic(const char* file, int line, const char* message, ...);
/*
 * The first vararg should always be the format string, but the preprocessor is slightly rubbish so we have to do
 * this to allow the case where it's just the message.
 */
#define KERNEL_PANIC(...) Internal_KernelPanic(__FILE__, __LINE__, __VA_ARGS__);
