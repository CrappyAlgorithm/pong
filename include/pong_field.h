#pragma once

#include "inttypes.h"
#include "framebuffer.h"
#include "display.h"
#include "pong.h"

#define PADDLE_PIXEL_RATIO 16u
#define PADDLE_ANGLE_CHANGE_RATIO 4u
#define MOVE_PIXEL_RATIO 8u

#define MAX_SCORE 100u
#define GAME_H ((uint32_t)DISP_H - 8u)
#define SCORE_ROW (DISP_H / 8u) - 1u
#define NUMBER_COUNT 10u
#define NUMBER_W 5u
#define NUMBER_SPACING 2u

typedef enum {GREEN, BLUE, YELLOW, RED} color_t;

void init_pong( void );
void move_paddle(color_t button);
void do_move( void );
void write_field_to_framebuffer( void );
void add_score( void );
