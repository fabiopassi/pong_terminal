#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "lib.h"


void check_ball_player_contact(Ball* ball, Block* player, int* score){
    /* NOTE: When the ball hits the bar, the velocity along x picks a random value: -2, -1, 1 or 2 */

    for(int i = 0; i < BAR_DIM; i++) {
        /* If the ball is above the bar and directing towards it, bounce it back */
        if ( ball->x == player[i].x && ball->y == player[i].y + 1 && ball->v_y < 0 ) {
            ball->v_y *= -1;
            (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
            if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
            printf("\a");       /* Bell sound */
            *score += 100;
        }
    }

    /* If the ball bounces back against an edge (diagonally) */
    if ( ball->x == player[0].x - 1 && ball->y == player[0].y + 1 && ball->v_y < 0 ) {
        ball->v_y *= -1;
        (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
        if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
        printf("\a");
        *score += 100;
    }

    if ( ball->x == player[BAR_DIM-1].x + 1 && ball->y == player[BAR_DIM-1].y + 1 && ball->v_y < 0 ) {
        ball->v_y *= -1;
        (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
        if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
        printf("\a");
        *score += 100;
    }
}



void check_ball_ai_contact(Ball* ball, Block* ai){
    /* Same rules as for player */

    for(int i = 0; i < BAR_DIM; i++) {
        /* If the ball is above the bar */
        if ( ball->x == ai[i].x && ball->y == ai[i].y - 1 && ball->v_y > 0 ) {
            ball->v_y *= -1;
            (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
            if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
        }
    }

    /* If the ball bounces back against an edge */
    if ( ball->x == ai[0].x - 1 && ball->y == ai[0].y - 1 && ball->v_y > 0 ) {
        ball->v_y *= -1;
        (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
        if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
    }

    if ( ball->x == ai[BAR_DIM-1].x + 1 && ball->y == ai[BAR_DIM-1].y - 1 && ball->v_y > 0 ) {
        ball->v_y *= -1;
        (rand() < RAND_MAX/2) ? (ball->v_x = -1) : (ball->v_x = 1);
        if (rand() < RAND_MAX/2) { ball->v_x *= 2; }
    }
}



void check_teleport(Ball* ball, Block* portal_1, Block* portal_2){

    /* If ball is on portal_1, teleport it to portal_2 and viceversa */
    for(int i = 0; i < PORTAL_SIZE; i++) {

        if (ball->x == portal_1[i].x && ball->y == portal_1[i].y) {
            ball->x = portal_2[i].x;
            ball->y = portal_2[i].y;
            break;
        }

        if (ball->x == portal_2[i].x && ball->y == portal_2[i].y) {
            ball->x = portal_1[i].x;
            ball->y = portal_1[i].y;
            break;
        }
    }


}



void move_ball(Ball* ball){

    ball->x += ball->v_x;
    ball->y += ball->v_y;

    /* Check if the ball is against the wall or behind it. Reflect the motion and bring it back if it is outside the region */
    if (ball->x >= TABLE_DIM - 2) {
        ball->x = (TABLE_DIM - 2) - ball->x % (TABLE_DIM - 2);
        ball->v_x *= -1;
    }

    if (ball->x <= 1) {
        ball->x = (1) + (1 - ball->x);
        ball->v_x *= -1;
    }
}



void move_player(Block* player, int dx){

    /* If the player is more than 3 blocks away from the border, move it by 3 blocks. Otherwise, just reach the border without overcoming it */

    /* Move right */
    if ( dx == 3 && player[BAR_DIM-1].x < TABLE_DIM-4 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x += dx;
        }
    }
    if ( dx == 3 && player[BAR_DIM-1].x == TABLE_DIM-4 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x += 2;
        }
    }
    if ( dx == 3 && player[BAR_DIM-1].x == TABLE_DIM-3 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x += 1;
        }
    }
    /* Move left */
    if ( dx == -3 && player[0].x > 3 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x += dx;
        }
    }
    if ( dx == -3 && player[0].x == 3 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x -= 2;
        }
    }
    if ( dx == -3 && player[0].x == 2 ) {
        for (int i = 0; i < BAR_DIM; i++) {
            player[i].x -= 1;
        }
    }
}



void move_ai(Block* ai, Ball ball) {

    /* If the ball is further than BAR_DIM / 2 from the border: move all the bar under the ball */
    if (ball.x > BAR_DIM/2 && ball.x < TABLE_DIM - 1 - BAR_DIM/2) {
        for (int i = 0; i < BAR_DIM; i++) {
            ai[i].x = ball.x - BAR_DIM/2 + i;
        }
    }

    /* If not, the ball is close to the border, hence I move all the AI close to the border */
    /* Left border */
    if (ball.x < BAR_DIM/2) {
        for (int i = 0; i < BAR_DIM; i++) {
            ai[i].x = i + 1;
        }
    }
    /* Right border */
    if (ball.x > TABLE_DIM - 1 - BAR_DIM/2) {
        for (int i = 0; i < BAR_DIM; i++) {
            ai[i].x = TABLE_DIM - BAR_DIM - 1 + i;
        }
    }
}



void move_ai_no_glitch(Block* ai, Ball ball){

    /* 
       If the ball is closer than (or as far as) to 2 blocks from the center of the ai bar, then simply move the bar under the ball.
       If the distance is greater, then move the ai of 3 blocks towards the ball.
       This prevents the ai from glitching when the ball teleports in portals.
    */

    if (abs( ai[BAR_DIM/2].x - ball.x ) <= 2 ) {
        move_ai(ai, ball);
    } else {
        int dx;
        (ai[BAR_DIM/2].x - ball.x < 0) ? (dx = +3) : (dx = -3);
        move_player(ai, dx); 
    }
}



void check_gameover(Ball ball, int* gameover){

    if ( ball.y == 0 || ball.y == TABLE_DIM - 1 ) {
        *gameover = 1;
    }
}



void fill_board(char** board){

    for (int i = 0; i < TABLE_DIM; i++) {
        for (int j = 0; j < TABLE_DIM; j++) {
            board[i][j] = ' ';
            if (j == 0 || j == TABLE_DIM - 1) { board[i][j] = '|'; }
            if (i == 0 || i == TABLE_DIM - 1) { board[i][j] = '-'; }
        }
    }
    
}



void draw_bar(char** board, Block* bar){

    for (int i = 0; i < BAR_DIM; i++) {
        board[bar[i].y][bar[i].x] = '#';
    }
}



void draw_ball(char** board, Ball ball){
    board[ball.y][ball.x] = 'o';
}



void draw_portal(char** board, Block* portal){

    for (int i = 0; i < PORTAL_SIZE; i++) {
        board[portal[i].y][portal[i].x] = '~';
    }
}



void print_board(char** board) {

    for(int i = 0; i < TABLE_DIM; i++) {
        for (int j = 0; j<TABLE_DIM; j++) {

            switch (board[i][j]) {
            case '-' :
                printf("\x1b[41m%c", ' ');
                printf("\x1b[0m");
                break;

            case '|' :
                printf("\x1b[46m%c", ' ');
                printf("\x1b[0m");
                break;

            case '#' :
                printf("\x1b[42m%c", ' ');
                printf("\x1b[0m");
                break;

            case '~' :
                printf("\x1b[45m%c", ' ');
                printf("\x1b[0m");
                break;
            
            default:
                putchar(board[i][j]);
                break;
            }

        }
        putchar('\n');
    }
}


void check_input(Block* player, const int move, int* quit){
    switch(move) {
			case 'q' : 
				*quit = 1; 
				break;
			case 'f' :
				move_player(player, -3);
				break;
			case 'h' :
				move_player(player, +3);
				break;
		}
}