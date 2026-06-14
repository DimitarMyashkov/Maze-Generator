#include <stdio.h>
#include "maze.h"


#define MAX_STEPS (GW * GH * 16)
#define DIR_RIGHT 0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_UP    3

static const int dr[] = {0, 1, 0, -1};
static const int dc[] = {1, 0, -1, 0};
static int turn_right(int dir) { return (dir + 1) % 4; }
static int turn_left(int dir) { return (dir + 3) % 4; }
static int turn_around(int dir) { return (dir + 2) % 4; }
static int can_enter(int r, int c) {
    if (r < 0 || r >= GH || c < 0 || c >= GW) return 0;
    return igrid[r][c] != STATE_WALL;
}
typedef struct { int row, col; } PathCell;
static PathCell path_walked[MAX_STEPS];
static int path_len = 0;

int SolveRightWall(void) {
    int cur_r = entrance_row;
    int cur_c  = entrance_col;
    int facing;
    if      (entrance_row == 0)    facing = DIR_DOWN;
    else if (entrance_row == GH-1) facing = DIR_UP;
    else if (entrance_col == 0)     facing = DIR_RIGHT;
    else                            facing = DIR_LEFT;
    path_len = 0;
    path_walked[path_len].row = cur_r;
    path_walked[path_len].col = cur_c;
    path_len++;
    if (igrid[cur_r][cur_c] == STATE_OPEN) {
        igrid[cur_r][cur_c] = STATE_HEAD;
    }
    VisualizerStep();
    int steps = 0;
    while (steps < MAX_STEPS) {
        if (cur_r == exit_row && cur_c == exit_col) {
            for (int i = 0; i < path_len; i++) {
                int r = path_walked[i].row;
                int c = path_walked[i].col;
                if (igrid[r][c] != STATE_ENTRANCE && igrid[r][c] != STATE_EXIT) {
                    igrid[r][c] = STATE_SOLUTION;
                }
                VisualizerStep();
            }
            return SOLVE_OK;
        }

        int try_dirs[4] = {
            turn_right(facing),
            facing,
            turn_left(facing),
            turn_around(facing)
        };

        int moved = 0;
        for (int i = 0; i < 4; i++) {
            int d = try_dirs[i];
            int nr = cur_r + dr[d];
            int nc = cur_c + dc[d];

            if (!can_enter(nr, nc)) continue;
            if (igrid[cur_r][cur_c] != STATE_ENTRANCE &&
                igrid[cur_r][cur_c] != STATE_EXIT) {
                igrid[cur_r][cur_c] = STATE_VISITED;
            }
            facing = d;
            cur_r = nr;
            cur_c = nc;
            if (path_len < MAX_STEPS) {
                path_walked[path_len].row = cur_r;
                path_walked[path_len].col = cur_c;
                path_len++;
            }

            if (igrid[cur_r][cur_c] != STATE_ENTRANCE &&
                igrid[cur_r][cur_c] != STATE_EXIT) {
                igrid[cur_r][cur_c] = STATE_HEAD;
            }
            VisualizerStep();
            moved = 1;
            break;
        }

        if (!moved) {
            printf("RightWall: solver is completely boxed in at (%d,%d).\n",
                   cur_r, cur_c);
            return SOLVE_FAIL;
        }
        steps++;
    }

    printf("RightWall: exceeded %d steps. Is this a perfect maze?\n", MAX_STEPS);
    return SOLVE_FAIL;
}