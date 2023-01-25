#include "display.h"
#include "pong_field.h"

typedef enum {LEFT = -1, RIGHT = 1} Direction;

typedef enum {NONE = 0, UP = -1, DOWN = 1} Angle;

typedef struct {
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

static Paddle player_left;
static Paddle player_right;

static Ball ball;
static Ball prev_ball;

static void write_paddle_to_framebuffer(Paddle paddle);
static void write_ball_to_framebuffer(void);
static void move_paddle_up(Direction direction);
static void move_paddle_down(Direction direction);

void init_pong(void) {
    player_left.row = (((uint32_t)GAME_H / 2u) - ((uint32_t)PADDLE_PIXEL_RATIO / 2u));
    player_left.direction = LEFT;
    player_left.score = 0u;
    player_right.row = ((GAME_H / 2u) - ((uint32_t)PADDLE_PIXEL_RATIO / 2u)); 
    player_right.direction = RIGHT;
    player_right.score = 0u;
    ball.x = ((uint32_t)DISP_W / 2u);
    ball.y = (GAME_H / 2u);
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
            player_left.row = 0u;
        }
        break;
    case RIGHT:
        if (player_right.row >= MOVE_PIXEL_RATIO) {
            player_right.row -= MOVE_PIXEL_RATIO;
        } else {
            player_right.row = 0u;
        }
        break;
    default:
        // useless misra block!
        break;
    }
}

static void move_paddle_down(Direction direction) {
    switch (direction) {
    case LEFT:
        player_left.row += MOVE_PIXEL_RATIO;
        if ((player_left.row + PADDLE_PIXEL_RATIO) > (GAME_H - 1u)) {
            player_left.row = (GAME_H - 1u - PADDLE_PIXEL_RATIO);
        }
        break;
    case RIGHT:
        player_right.row += MOVE_PIXEL_RATIO;
        if ((player_right.row + PADDLE_PIXEL_RATIO) > (GAME_H - 1u)) {
            player_right.row = (GAME_H - 1u - PADDLE_PIXEL_RATIO);
        }
        break;
    default:
        // useless misra block!
        break;
    }
}

void do_move(void) {
    if (ball.direction == LEFT) {
        ball.x -= 1u;
    } else {
        ball.x += 1u;
    }
    if (ball.angle == UP) {
        ball.y -= 1u;
    } else if (ball.angle == DOWN) {
        ball.y += 1u;
    } else {
        // useless misra block!
    }
    if ((ball.y == 0u) && (ball.angle == UP)) {
        ball.angle = DOWN;
    } else if ((ball.y == (GAME_H - 1u)) && (ball.angle == DOWN)) {
        ball.angle = UP;
    } else {
        // useless misra block!
    }
    if (ball.x == 0u) {
        ball.direction = RIGHT;
        if ((ball.y >= player_left.row) && (ball.y <= (player_left.row + PADDLE_ANGLE_CHANGE_RATIO))) { // hit paddle top
            ball.angle = UP;
        } else if ((ball.y > (player_left.row + PADDLE_ANGLE_CHANGE_RATIO)) && (ball.y <= (player_left.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO))) { // hit paddle middle
            ball.angle = NONE;
        } else if ((ball.y > (player_left.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO)) && (ball.y <= (player_left.row + PADDLE_PIXEL_RATIO))) { // hit paddle bottom
            ball.angle = DOWN;
        } else { // miss paddle
            player_right.score = ((player_right.score + 1u) % MAX_SCORE);
        }
    } else if (ball.x == (DISP_W - 1u)) {
        ball.direction = LEFT;
        if ((ball.y >= player_right.row) && (ball.y <= (player_right.row + PADDLE_ANGLE_CHANGE_RATIO))) { // hit paddle top
            ball.angle = UP;
        } else if ((ball.y > (player_right.row + PADDLE_ANGLE_CHANGE_RATIO)) && (ball.y <= (player_right.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO))) { // hit paddle middle
            ball.angle = NONE;
        } else if ((ball.y > (player_right.row + PADDLE_PIXEL_RATIO - PADDLE_ANGLE_CHANGE_RATIO)) && (ball.y <= (player_right.row + PADDLE_PIXEL_RATIO))) { // hit paddle bottom
            ball.angle = DOWN;
        } else { // miss paddle
            player_left.score = ((player_left.score + 1u) % MAX_SCORE);
        }
    } else {
        // useless misra block!
    }
}

void write_field_to_framebuffer(void) {
    write_paddle_to_framebuffer(player_left);
    write_paddle_to_framebuffer(player_right);
    write_ball_to_framebuffer();
}

static void write_ball_to_framebuffer(void) {
    fb_set_pixel(prev_ball.x, prev_ball.y, 0u);
    prev_ball.x = ball.x;
    prev_ball.y = ball.y;
    fb_set_pixel(ball.x, ball.y, 1);
}

static void write_paddle_to_framebuffer(Paddle paddle) {
    uint8_t row;
    if (paddle.direction == LEFT) {
        row = 0x0u;
    } else {
        row = DISP_W - 1u;
    }
    uint8_t column_min = paddle.row;
    uint8_t column_max = paddle.row + PADDLE_PIXEL_RATIO;
    for (uint32_t i = 0; i < GAME_H; i++) {
        fb_set_pixel(row, i, 0u);
    }
    for (uint32_t i = column_min; i < column_max; i++) {
        fb_set_pixel(row, i, 1u);
    }
}


void add_score(void) {
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
    uint8_t left_major = player_left.score / NUMBER_COUNT;
    uint8_t left_minor = player_left.score % NUMBER_COUNT;
    uint8_t right_major = player_right.score / NUMBER_COUNT;
    uint8_t right_minor = player_right.score % NUMBER_COUNT;
    for (int i = 0; i < (int) DISP_W; i++) {
        framebuffer[i][SCORE_ROW] = 0x0u;
    }
    for (uint32_t i = 0; i < NUMBER_W; i++) {
        framebuffer[i][SCORE_ROW] = numbers[left_major][i];
        framebuffer[i + ((uint32_t)NUMBER_W + NUMBER_SPACING)][SCORE_ROW] = numbers[left_minor][i];
        framebuffer[i + ((uint32_t)DISP_W - 1u - NUMBER_W)][SCORE_ROW] = numbers[right_minor][i];
        framebuffer[i + ((uint32_t)DISP_W - 1u - ((uint32_t)2u * NUMBER_W) - NUMBER_SPACING)][SCORE_ROW] = numbers[right_major][i];
    }
}
