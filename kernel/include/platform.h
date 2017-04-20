/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <stdint.h>

__attribute__((__noreturn__))
void Internal_KernelPanic(const char* file, int line, const char* message, ...);
/*
 * The first vararg should always be the format string, but the preprocessor is slightly rubbish so we have to do
 * this to allow the case where it's just the message.
 */
#define KERNEL_PANIC(...) Internal_KernelPanic(__FILE__, __LINE__, __VA_ARGS__);

void InitPlatform();

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

struct registers;
typedef void (*interrupt_handler_t)(struct registers);
void RegisterInterruptHandler(uint8_t interruptNum, interrupt_handler_t handler);
