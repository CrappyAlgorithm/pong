#include "display.h"
#include "pong_field.h"
#include "font.h"

static const uint8_t numbers[NUMBER_COUNT][NUMBER_W] = {
    {0x7c, 0x82, 0x82, 0x82, 0x7c}, 
    {0x84, 0x84, 0xfe, 0x80, 0x80},
    {0xc4, 0xa2, 0x92, 0x8a, 0x84},
    {0x44, 0x82, 0x92, 0x92, 0x6c},
    {0x3e, 0x20, 0xf0, 0x20, 0x20},
    {0x9e, 0x92, 0x92, 0x92, 0x62},
    {0x7c, 0x92, 0x92, 0x92, 0x64},
    {0x02, 0x02, 0xf2, 0x0a, 0x06},
    {0x6c, 0x92, 0x92, 0x92, 0x6c},
    {0x4c, 0x92, 0x92, 0x92, 0x7c}
};

static Paddle player_left;
static Paddle player_right;

static Ball ball;
static Ball prev_ball;

static void write_paddle_to_framebuffer(Paddle paddle);
static void write_ball_to_framebuffer();
static void move_paddle_up(Direction direction);
static void move_paddle_down(Direction direction);

void init_pong() {
    player_left.row = GAME_H / 2 - PADDLE_PIXEL_RATIO / 2;
    player_left.direction = LEFT;
    player_left.score = 0;
    player_right.row = GAME_H / 2 - PADDLE_PIXEL_RATIO / 2; 
    player_right.direction = RIGHT;
    player_right.score = 0;
    ball.x = DISP_W / 2;
    ball.y = GAME_H / 2;
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
        if (player_left.row >= MOVE_PIXEL_RATIO) {
            player_left.row -= MOVE_PIXEL_RATIO;
        } else {
            player_left.row = 0;
        }
        break;
    case RIGHT:
        if (player_right.row >= MOVE_PIXEL_RATIO) {
            player_right.row -= MOVE_PIXEL_RATIO;
        } else {
            player_right.row = 0;
        }
        break;
    }
}

static void move_paddle_down(Direction direction) {
    switch (direction) {
    case LEFT:
        player_left.row += MOVE_PIXEL_RATIO;
        if (player_left.row + PADDLE_PIXEL_RATIO > GAME_H - 1) {
            player_left.row = GAME_H - 1 - PADDLE_PIXEL_RATIO;
        }
        break;
    case RIGHT:
        player_right.row += MOVE_PIXEL_RATIO;
        if (player_right.row + PADDLE_PIXEL_RATIO > GAME_H - 1) {
            player_right.row = GAME_H - 1 - PADDLE_PIXEL_RATIO;
        }
        break;
    }
}

void do_move() {
    ball.x += ball.direction;
    ball.y += ball.angle;
    if (ball.y == 0 && ball.angle == UP) {
        ball.angle = DOWN;
    } else if (ball.y == GAME_H - 1 && ball.angle == DOWN) {
        ball.angle = UP;
    }
    if (ball.x == 0) {
        ball.direction = RIGHT;
        if (ball.y >= player_left.row && ball.y <= player_left.row + PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle top
            ball.angle = UP;
        } else if (ball.y > player_left.row + PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_left.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle middle
            ball.angle = NONE;
        } else if (ball.y > player_left.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_left.row + PADDLE_PIXEL_RATIO) { // hit paddle bottom
            ball.angle = DOWN;
        } else { // miss paddle
            player_right.score = (player_right.score + 1) % MAX_SCORE;
        }
    } else if (ball.x == DISP_W - 1) {
        ball.direction = LEFT;
        if (ball.y >= player_right.row && ball.y <= player_right.row + PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle top
            ball.angle = UP;
        } else if (ball.y > player_right.row + PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_right.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO) { // hit paddle middle
            ball.angle = NONE;
        } else if (ball.y > player_right.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO && ball.y <= player_right.row + PADDLE_PIXEL_RATIO) { // hit paddle bottom
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
    uint8_t column_min = paddle.row;
    uint8_t column_max = paddle.row + PADDLE_PIXEL_RATIO;
    for (int i = 0; i < GAME_H; i++) {
        fb_set_pixel(row, i, 0);
    }
    for (int i = column_min; i < column_max; i++) {
        fb_set_pixel(row, i, 1);
    }
}


void add_score() {
    printf("Score: P1=%d, P2=%d\n", player_left.score, player_right.score);
    uint8_t left_major = player_left.score / NUMBER_COUNT;
    uint8_t left_minor = player_left.score % NUMBER_COUNT;
    uint8_t right_major = player_right.score / NUMBER_COUNT;
    uint8_t right_minor = player_right.score % NUMBER_COUNT;
    for (int i = 0; i < DISP_W; i++) {
        framebuffer[i][SCORE_ROW] = 0x0;
    }
    for (int i = 0; i < NUMBER_W; i++) {
        framebuffer[i][SCORE_ROW] = numbers[left_major][i];
        framebuffer[i + NUMBER_W + NUMBER_SPACING][SCORE_ROW] = numbers[left_minor][i];
        framebuffer[DISP_W - 1 - NUMBER_W + i][SCORE_ROW] = numbers[right_minor][i];
        framebuffer[DISP_W - 1 - 2 * NUMBER_W - NUMBER_SPACING + i][SCORE_ROW] = numbers[right_major][i];
    }
    /*
    for (int i = 0; i < 5; i++) {
        framebuffer[i][SCORE_ROW] = numbers[0][i];
        framebuffer[i + 7][SCORE_ROW] = numbers[1][i];
        framebuffer[i + 14][SCORE_ROW] = numbers[2][i];
        framebuffer[i + 21][SCORE_ROW] = numbers[3][i];
        framebuffer[i + 28][SCORE_ROW] = numbers[4][i];
        framebuffer[i + 35][SCORE_ROW] = numbers[5][i];
        framebuffer[i + 42][SCORE_ROW] = numbers[6][i];
        framebuffer[i + 49][SCORE_ROW] = numbers[7][i];
        framebuffer[i + 56][SCORE_ROW] = numbers[8][i];
        framebuffer[i + 63][SCORE_ROW] = numbers[9][i];
    }
    */
}
