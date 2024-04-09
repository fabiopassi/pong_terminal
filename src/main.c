#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lib.h"

/* 
	Table hockey game

	-> Level 0: Simple ping pong game 						( 0 				  <= score < SCORE_THRESHOLD_LV_1 )
	-> Level 1: 2 portals to teleport the ball 				(SCORE_THRESHOLD_LV_1 <= score < SCORE_THRESHOLD_LV_2 ) 
	-> Level 2: 4 portals to teleport the ball 				(SCORE_THRESHOLD_LV_2 <= score < SCORE_THRESHOLD_LV_3 )
	-> Level 3: 4 portals + faster ball 					(SCORE_THRESHOLD_LV_3 <= score )
	-> Level 4: 4 portals randomized + faster ball (to implement)

*/

int main (int argc, char **argv) {

	srand(time(NULL));

	/* Hide cursor */
	printf("\e[?25l");

	/* Remove echo */
	struct termios old_t, new_t;
	tcgetattr(STDIN_FILENO, &old_t);
	new_t = old_t;
	new_t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
	
	/* Variables */
	int quit = 0;
	int gameover = 0;
	int score = 0;
	int len;
	char score_buffer[SCORE_BUFFER_SIZE];
	char input_buffer[INPUT_BUFFER_SIZE];
	Block ai[BAR_DIM];
	Block player[BAR_DIM];
	Ball ball;

	Block portal_1a[PORTAL_SIZE];
	Block portal_1b[PORTAL_SIZE];
	Block portal_2a[PORTAL_SIZE];
	Block portal_2b[PORTAL_SIZE];

	char** board;
	board = malloc(TABLE_DIM * sizeof(char*));
	for (int i = 0; i < TABLE_DIM; i++) {
		board[i] = malloc(TABLE_DIM * sizeof(char));
	}


	/* Initialize player */
	for(int i = 0; i < BAR_DIM; i++) {
		player[i].x = i + TABLE_DIM / 2 - BAR_DIM/2;
		player[i].y = ROW_PLAYER;
	}

	/* Initialize AI */
	for(int i = 0; i < BAR_DIM; i++) {
		ai[i].x = i + TABLE_DIM / 2 - BAR_DIM/2;
		ai[i].y = TABLE_DIM - 1 - ROW_PLAYER;
	}

	/* Initialize ball */
	ball.x = TABLE_DIM/2;
	ball.y = TABLE_DIM/2;
	ball.v_x = 0;
	ball.v_y = 1;

	/* Initialize portals */
	for (int i = 0; i < PORTAL_SIZE; i++) {

		portal_2a[i].x = i + (TABLE_DIM / 5) - (PORTAL_SIZE / 2) - 2;
		portal_2a[i].y = TABLE_DIM / 2;

		portal_1a[i].x = i + (2 * TABLE_DIM / 5) - (PORTAL_SIZE / 2) - 1;
		portal_1a[i].y = TABLE_DIM / 2;

		portal_1b[i].x = i + (3 * TABLE_DIM / 5) - (PORTAL_SIZE / 2) + 1;
		portal_1b[i].y = TABLE_DIM / 2;

		portal_2b[i].x = i + (4 * TABLE_DIM / 5) - (PORTAL_SIZE / 2) + 2;
		portal_2b[i].y = TABLE_DIM / 2;
	}

	/* Clear terminal and move cursor in position (1,1) - i.e. top left corner - on the terminal */
	printf("\x1b[%dJ", 2);
    printf("\x1b[%d;%df", 1, 1);

	/* Print commands */
	printf("Commands:\n\n");
	printf("\t-> f : move left\n");
	printf("\t-> h : move right\n");
	printf("\t-> q : quit\n\n");


	/* GAME */

	while( ! quit && ! gameover ) {

		/* Check if ball against player */
		check_ball_player_contact(&ball, player, &score);

		/* Check if ball against ai */
		check_ball_ai_contact(&ball, ai);

		/* If level 1 is active, check if the ball hits portals 1 */
		if (score >= SCORE_THRESHOLD_LV_1) {
			check_teleport(&ball, portal_1a, portal_1b);
		}

		/* If level 2 is active, check if the ball hits portals 2 */
		if (score >= SCORE_THRESHOLD_LV_2) {
			check_teleport(&ball, portal_2a, portal_2b);
		}

		/* Move ball (considering reflection of walls) */
		move_ball(&ball);

		/* Move AI */
		move_ai_no_glitch(ai, ball);

		/* Check if gameover */
		check_gameover(ball, &gameover);

		/* Fill board */
		fill_board(board);

		/* Place player */
		draw_bar(board, player);

		/* Place AI */
		draw_bar(board, ai);

		/* Place ball */
		draw_ball(board, ball);

		/* Place portals (if present) */
		if (score >= SCORE_THRESHOLD_LV_1) {
			draw_portal(board, portal_1a);
			draw_portal(board, portal_1b);
		}
		if (score >= SCORE_THRESHOLD_LV_2) {
			draw_portal(board, portal_2a);
			draw_portal(board, portal_2b);
		}

		/* Print board */
		print_board(board);

		/* Print the score */
		snprintf(score_buffer, SCORE_BUFFER_SIZE, "\nScore : %d", score);
		puts(score_buffer);

		/* Pause time */
		int pause;
		(score < SCORE_THRESHOLD_LV_3) ? (pause = 4.35 * 1000000 / 60) : (pause = 3.5 * 1000000 / 60);
		usleep(pause);

		/* Make a move */
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &fds)) {
			len = read(STDIN_FILENO, input_buffer, INPUT_BUFFER_SIZE);
			int move = input_buffer[len - 1];
			check_input(player, move, &quit);
		}

		/* Move terminal to beginning and clean screen */
		if (! quit && ! gameover) {
			fflush(stdout);
			printf("\x1b[%dD", (int)strlen(score_buffer));
			printf("\x1b[%dA", TABLE_DIM + 2);
			printf("\x1b[%dJ", 0);
		}
		
	}


	/* If gameover, print message and score */
	if (gameover == 1) {
		printf("\nGameover!\n\nFinal Score : %d\n\n", score);
	}

	/* If quit, thank the player! */
	if (quit == 1) {
		printf("\nThank you for having tried this Atari-like pong game!\n\nLet me know if you have ideas to make it better if you got bored!\n\n");
	}

	/* Free board */
	for (int i = 0; i < TABLE_DIM; i++) {
		free(board[i]);
	}
	free(board);
	
	/* Show cursor */
	printf("\e[?25h");

	/* Reintroduce echo */
	tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
	
	return 0;
}

