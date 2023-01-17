#pragma once

#include "inttypes.h"
#include "framebuffer.h"
#include "display.h"

#define PADDLE_SIZE 5

typedef enum {LEFT, RIGHT} Direction;

typedef struct paddle {
    Direction direction;
    uint32_t column;
} Paddle;

typedef struct {
    uint32_t x;
    uint32_t y;
    Direction direction;
    uint32_t angle;
} Ball;


void init_pong();
uint8_t move_paddle_up(Direction direction);
uint8_t move_paddle_down(Direction direction);
void move_ball();
void write_field_to_framebuffer();
