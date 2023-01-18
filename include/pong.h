#pragma once

#include "inttypes.h"

#define REG32(P) (*(volatile uint32_t *) (P))

#define GPIO_BASE 0x10012000
/*

#define GPIO_INPUT_EN GPIO_BASE + 0x4
#define GPIO_OUTPUT_EN GPIO_BASE + 0x8
#define GPIO_INPUT_VAL GPIO_BASE + 0x0
#define GPIO_
#define GPIO_PUE GPIO_BASE + 0x10
#define GPIO_FALL_IE GPIO_BASE + 0x20
#define GPIO_RISE_IE GPIO_BASE + 0x18
#define GPIO_RISE_IP GPIO_BASE + 0x1c
*/

#define PLIC_BASE   0x0C000000
#define PLIC_ENABLE 0x2000
#define PLIC_THRESH 0x200000
#define PLIC_CLAIM  0x200004

#define COLOR_COUNT 4

typedef enum {GREEN, BLUE, YELLOW, RED} color_t;

typedef struct {
    color_t color;
    uint32_t pin;
} pin_mapping;


