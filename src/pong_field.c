#include "display.h"
#include "pong_field.h"

static Paddle player_left;
static Paddle player_right;

static Ball ball;

static void write_paddle_to_framebuffer(Paddle paddle);

void init_pong() {
    player_left.column = DISP_H / 2;
    player_left.direction = LEFT;
    player_right.column = DISP_H / 2; 
    player_right.direction = RIGHT;
    ball.x = DISP_W / 2;
    ball.y = DISP_H /2;
    ball.direction = LEFT;
    ball.angle = 0;
}

uint8_t move_paddle_up(Direction direction) {
    uint8_t success = 0;
    switch (direction) {
    case LEFT:
        if (player_left.column - PADDLE_SIZE > 0 ) {
            player_left.column--;
            success = 1;
        }
        break;
    case RIGHT:
        if (player_right.column - PADDLE_SIZE > 0 ) {
            player_right.column--;
            success = 1;
        }
        break;
    }
    return success;
}

uint8_t move_paddle_down(Direction direction) {
    uint8_t success = 0;
    switch (direction) {
    case LEFT:
        if (player_left.column + PADDLE_SIZE < DISP_H - 1) {
            player_left.column++;
            success = 1;
        }
        break;
    case RIGHT:
        if (player_right.column + PADDLE_SIZE < DISP_H - 1) {
            player_right.column++;
            success = 1;
        }
        break;
    }
    return success;
}

void move_ball() {

}

void write_field_to_framebuffer() {
    write_paddle_to_framebuffer(player_left);
    write_paddle_to_framebuffer(player_right);
    fb_set_pixel(ball.x, ball.y, 1);
}

static void write_paddle_to_framebuffer(Paddle paddle) {
    uint8_t row;
    if (paddle.direction == LEFT) {
        row = 0x0;
    } else {
        row = DISP_W - 1;
    }
    uint8_t column_min = paddle.column - PADDLE_SIZE;
    uint8_t column_max = paddle.column + PADDLE_SIZE;
    for (int i = column_min; i < column_max + 1; i++) {
        fb_set_pixel(row, i, 1);
    }
}

