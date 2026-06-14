#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CW 11
#define CH 4
#define W (CW*2+1)
#define H (CH*2+1)

char grid[H][W];
int visited[CH][CW];

int dx[] = {1, -1, 0,  0};
int dy[] = {0,  0, 1, -1};

void carve(int cx, int cy) {
    visited[cy][cx] = 1;
    grid[cy*2+1][cx*2+1] = '0';

    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = tmp;
    }

    for (int i = 0; i < 4; i++) {
        int d  = dirs[i];
        int nx = cx + dx[d];
        int ny = cy + dy[d];
        if (nx < 0 || nx >= CW || ny < 0 || ny >= CH) continue;
        if (visited[ny][nx]) continue;

        grid[cy*2+1 + dy[d]][cx*2+1 + dx[d]] = '0';
        carve(nx, ny);
    }
}

void print_maze(int ex, int ey, int xx, int xy) {
    if      (ex == 0)    { grid[ey*2+1][0]   = 'E'; grid[ey*2+1][1] = '0'; }
    else if (ex == CW-1) { grid[ey*2+1][W-1] = 'E'; grid[ey*2+1][W-2] = '0'; }
    else if (ey == 0)    { grid[0][ex*2+1]   = 'E'; grid[1][ex*2+1] = '0'; }
    else                 { grid[H-1][ex*2+1] = 'E'; grid[H-2][ex*2+1] = '0'; }

    if      (xx == 0)    { grid[xy*2+1][0]   = 'X'; grid[xy*2+1][1] = '0'; }
    else if (xx == CW-1) { grid[xy*2+1][W-1] = 'X'; grid[xy*2+1][W-2] = '0'; }
    else if (xy == 0)    { grid[0][xx*2+1]   = 'X'; grid[1][xx*2+1] = '0'; }
    else                 { grid[H-1][xx*2+1] = 'X'; grid[H-2][xx*2+1] = '0'; }

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) putchar(grid[y][x]);
        putchar('\n');
    }
}

int main(void) {
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            grid[y][x] = '1';

    carve(0, 0);

    printf("Maze: columns 0..%d, rows 0..%d\n", CW-1, CH-1);
    printf("Border cells only recommended for entrance/exit.\n\n");

    int ex, ey, xx, xy;
    printf("Entrance (col row): ");
    if (scanf("%d %d", &ex, &ey) != 2) { puts("bad input"); return 1; }
    printf("Exit     (col row): ");
    if (scanf("%d %d", &xx, &xy) != 2) { puts("bad input"); return 1; }

    if (ex < 0 || ex >= CW || ey < 0 || ey >= CH ||
        xx < 0 || xx >= CW || xy < 0 || xy >= CH) {
        puts("Coordinates out of range.");
        return 1;
    }

    printf("\n");
    print_maze(ex, ey, xx, xy);
    return 0;
}
