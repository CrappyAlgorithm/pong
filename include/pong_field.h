#pragma once

#include "inttypes.h"
#include "framebuffer.h"
#include "display.h"
#include "pong.h"

#define PADDLE_PIXEL_RATIO 16
#define PADDLE_ANGLE_CHANGE_RATIO 4
#define MOVE_PIXEL_RATIO 8

#define MAX_SCORE 100
#define GAME_H (DISP_H - 8)
#define SCORE_ROW (DISP_H / 8) - 1
#define NUMBER_COUNT 10
#define NUMBER_W 5
#define NUMBER_SPACING 2

typedef enum {LEFT = -1, RIGHT = 1} Direction;

typedef enum {NONE = 0, UP = -1, DOWN = 1} Angle;

typedef struct paddle {
    Direction direction;
    uint32_t row;
    uint32_t score;
} Paddle;

typedef struct {
    uint32_t x;
    uint32_t y;
    Direction direction;
    Angle angle;
} Ball;


void init_pong();
void move_paddle(color_t button);
void do_move();
void write_field_to_framebuffer();
void add_score();
