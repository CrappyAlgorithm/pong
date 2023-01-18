#include "display.h"
#include "pong_field.h"
#include "font.h"

static Paddle player_left;
static Paddle player_right;

static Ball ball;
static Ball prev_ball;

static void write_paddle_to_framebuffer(Paddle paddle);
static void write_ball_to_framebuffer();
static void move_paddle_up(Direction direction);
static void move_paddle_down(Direction direction);

void init_pong() {
    player_left.column = DISP_H / 2 - PADDLE_PIXEL_RATIO / 2;
    player_left.direction = LEFT;
    player_left.score = 0;
    player_right.column = DISP_H / 2 - PADDLE_PIXEL_RATIO / 2; 
    player_right.direction = RIGHT;
    player_right.score = 0;
    ball.x = DISP_W / 2;
    ball.y = DISP_H / 2;
    ball.direction = LEFT;
    ball.angle = NONE;
    prev_ball.x = ball.x;
    prev_ball.y = ball.y;
}

void move_paddle(color_t button) {
    switch (button) {
    case GREEN:
        move_paddle_up(LEFT);
        break;
    case BLUE:
        move_paddle_down(LEFT);
        break;
    case YELLOW:
        move_paddle_down(RIGHT);
        break;
    case RED:
        move_paddle_up(RIGHT);
        break;
    default:
        break;
    }
}

static void move_paddle_up(Direction direction) {
    switch (direction) {
    case LEFT:
        if (player_left.column >= MOVE_PIXEL_RATIO) {
            player_left.column -= MOVE_PIXEL_RATIO;
        } else {
            player_left.column = 0;
        }
        break;
    case RIGHT:
        if (player_right.column >= MOVE_PIXEL_RATIO) {
            player_right.column -= MOVE_PIXEL_RATIO;
        } else {
            player_right.column = 0;
        }
        break;
    }
}

static void move_paddle_down(Direction direction) {
    switch (direction) {
    case LEFT:
        player_left.column += MOVE_PIXEL_RATIO;
        if (player_left.column + PADDLE_PIXEL_RATIO > DISP_H - 1) {
            player_left.column = DISP_H - 1 - PADDLE_PIXEL_RATIO;
        }
        break;
    case RIGHT:
        player_right.column += MOVE_PIXEL_RATIO;
        if (player_right.column + PADDLE_PIXEL_RATIO > DISP_H - 1) {
            player_right.column = DISP_H - 1 - PADDLE_PIXEL_RATIO;
        }
        break;
    }
}

void do_move() {
    ball.x += ball.direction;
    ball.y += ball.angle;
    if (ball.y == 0 && ball.angle == UP) {
        ball.angle = DOWN;
    } else if (ball.y == DISP_H - 1 && ball.angle == DOWN) {
        ball.angle = UP;
    }
    if (ball.x == 0) {
        ball.direction = RIGHT;
        if (ball.y >= player_left.column && ball.y <= player_left.column + PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle top
            ball.angle = UP;
        } else if (ball.y > player_left.column + PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_left.column + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle middle
            ball.angle = NONE;
        } else if (ball.y > player_left.column + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_left.column + PADDLE_PIXEL_RATIO) { // hit paddle bottom
            ball.angle = DOWN;
        } else { // miss paddle
            player_right.score = (player_right.score + 1) % MAX_SCORE;
        }
    } else if (ball.x == DISP_W - 1) {
        ball.direction = LEFT;
        if (ball.y >= player_right.column && ball.y <= player_right.column + PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle top
            ball.angle = UP;
        } else if (ball.y > player_right.column + PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_right.column + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle middle
            ball.angle = NONE;
        } else if (ball.y > player_right.column + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_right.column + PADDLE_PIXEL_RATIO) { // hit paddle bottom
            ball.angle = DOWN;
        } else { // miss paddle
            player_left.score = (player_left.score + 1) % MAX_SCORE;
        }
    }
}

void write_field_to_framebuffer() {
    write_paddle_to_framebuffer(player_left);
    write_paddle_to_framebuffer(player_right);
    write_ball_to_framebuffer();
}

static void write_ball_to_framebuffer() {
    fb_set_pixel(prev_ball.x, prev_ball.y, 0);
    prev_ball.x = ball.x;
    prev_ball.y = ball.y;
    fb_set_pixel(ball.x, ball.y, 1);
}

static void write_paddle_to_framebuffer(Paddle paddle) {
    uint8_t row;
    if (paddle.direction == LEFT) {
        row = 0x0;
    } else {
        row = DISP_W - 1;
    }
    uint8_t column_min = paddle.column;
    uint8_t column_max = paddle.column + PADDLE_PIXEL_RATIO;
    for (int i = 0; i < DISP_H; i++) {
        fb_set_pixel(row, i, 0);
    }
    for (int i = column_min; i < column_max + 1; i++) {
        fb_set_pixel(row, i, 1);
    }
}

void add_score() {
    printf("Score: P1=%d, P2=%d\n", player_left.score, player_right.score);
}
