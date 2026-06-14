#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "maze.h"

//premestih gi v maze.h

char grid[GH][GW]; //преименувах за consistency 
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

//main преместих в main.c