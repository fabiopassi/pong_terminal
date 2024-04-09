#ifndef __LIB_H__
#define __LIB_H__


/* Typedef */

typedef struct {
    int x,y;
} Block;


typedef struct {
    int x,y;
    int v_x, v_y;
} Ball;



/* Constants */

#define TABLE_DIM 36
#define BAR_DIM 7
#define ROW_PLAYER 2
#define SCORE_BUFFER_SIZE 128
#define INPUT_BUFFER_SIZE 4096
#define PORTAL_SIZE 7
#define SCORE_THRESHOLD_LV_1 500
#define SCORE_THRESHOLD_LV_2 1500
#define SCORE_THRESHOLD_LV_3 2500



/* Headers of functions */

void check_ball_player_contact(Ball* ball, Block* player, int* score);

void check_ball_ai_contact(Ball* ball, Block* ai);

void check_teleport(Ball* ball, Block* portal_1, Block* portal_2);

void move_ball(Ball* ball);

void move_player(Block* player, int dx);

void move_ai(Block* ai, Ball ball);

void move_ai_no_glitch(Block* ai, Ball ball);

void check_gameover(Ball ball, int* gameover);

void fill_board(char** board);

void draw_bar(char** board, Block* bar);

void draw_ball(char** board, Ball ball);

void draw_portal(char** board, Block* portal);

void print_board(char** board);

void check_input(Block* player, const int move, int* quit);

#endif

