#ifndef MAZE_H
#define MAZE_H
 
#define CW 11          /* logical columns */
#define CH 11          /* logical rows    */
#define GW (CW*2+1)    /* full grid width  (23) */
#define GH (CH*2+1)    /* full grid height (23) */
/* Cell states stored in the grid that main.c and the solvers share
 The visualizer colors each state differently. I hope its the most optimal logic ;)*/
#define STATE_WALL      1   // black – wall//
#define STATE_OPEN      0   // dark grey – walkable path//
#define STATE_VISITED   2   // blue – solver has been here//
#define STATE_HEAD      3   // yellow – current position//
#define STATE_SOLUTION  4   // green – final shortest path//
#define STATE_ENTRANCE  5   // cyan – entrance marker//
#define STATE_EXIT      6   // red  – exit marker//
 
// The integer grid shared by main, solvers, and visualizer //
extern int  igrid[GH][GW];
// Entrance / exit positions in the full (igrid) coordinate space //
extern int  entrance_row, entrance_col;
extern int  exit_row,     exit_col;
 
#define SOLVE_OK     0
#define SOLVE_FAIL  -1
 
int SolveDijkstra(void);
int SolveRightWall(void);
void VisualizerStep(void);
 
#endif 
 