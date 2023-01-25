#pragma once

#include "inttypes.h"

#define REG32(P) (*(volatile uint32_t *) (P))
#define REG64(P) (*(volatile uint64_t *) (P))

#define GPIO_BASE 0x10012000

#define PLIC_BASE   0x0C000000
#define PLIC_ENABLE 0x2000
#define PLIC_THRESH 0x200000
#define PLIC_CLAIM  0x200004

#define COLOR_COUNT 4u
#define INIT_DELAY 2000u

void handle_trap_button(void);
