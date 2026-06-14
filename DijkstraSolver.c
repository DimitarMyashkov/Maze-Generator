#include <stdio.h>
#include <string.h>
#include "maze.h"

#define QUEUE_CAP (GW * GH)

typedef struct {
    int row, col;
} Pos;

static Pos queue[QUEUE_CAP];
static int q_head, q_tail;
static int came_from_row[GH][GW];
static int came_from_col[GH][GW];
static void enqueue(int row, int col) {
    queue[q_tail].row = row;
    queue[q_tail].col = col;
    q_tail = (q_tail + 1) % QUEUE_CAP;
}

static Pos dequeue(void) {
    Pos p = queue[q_head];
    q_head = (q_head + 1) % QUEUE_CAP;
    return p;
}

static int queue_empty(void) {
    return q_head == q_tail;
}

static void mark_solution(int end_row, int end_col) {
    int r = end_row, c = end_col;

    while (!(r == entrance_row && c == entrance_col)) {
        igrid[r][c] = STATE_SOLUTION;
        VisualizerStep();
        int pr = came_from_row[r][c];
        int pc = came_from_col[r][c];
        r = pr;
        c = pc;
    }

    igrid[entrance_row][entrance_col] = STATE_SOLUTION;
    VisualizerStep();
}

int SolveDijkstra(void) {
    const int dr[] = {0, 0, 1, -1};
    const int dc[] = {1, -1, 0, 0};
    memset(came_from_row, -1, sizeof(came_from_row));
    memset(came_from_col, -1, sizeof(came_from_col));
    q_head = q_tail = 0;
    enqueue(entrance_row, entrance_col);
    came_from_row[entrance_row][entrance_col] = entrance_row;
    came_from_col[entrance_row][entrance_col] = entrance_col;
    int prev_row = -1, prev_col = -1;

    while (!queue_empty()) {
        Pos cur = dequeue();
        int r = cur.row;
        int c = cur.col;
        if (prev_row != -1 && igrid[prev_row][prev_col] != STATE_ENTRANCE) {
            igrid[prev_row][prev_col] = STATE_VISITED;
        }
        if (igrid[r][c] != STATE_ENTRANCE && igrid[r][c] != STATE_EXIT) {
            igrid[r][c] = STATE_HEAD;
        }
        VisualizerStep();
        if (r == exit_row && c == exit_col) {
            mark_solution(r, c);
            return SOLVE_OK;
        }
        prev_row = r;
        prev_col = c;
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= GH || nc < 0 || nc >= GW) continue;
            if (came_from_row[nr][nc] != -1)              continue;
            int s = igrid[nr][nc];
            if (s == STATE_WALL) continue;
            came_from_row[nr][nc] = r;
            came_from_col[nr][nc] = c;
            enqueue(nr, nc);
        }
    }

    printf("Dijkstra: no path found.\n");
    return SOLVE_FAIL;
}