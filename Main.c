#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maze.h"

extern char grid[GH][GW];
void carve(int cx, int cy);
int igrid[GH][GW];
int entrance_row, entrance_col;
int exit_row, exit_col;
#define CELL_SIZE 28
#define TIMER_ID 1
#define STEP_MS 18
#define WM_SOLVER_STEP (WM_APP + 1)

static HWND g_hwnd = NULL;
static int g_solve_done = 0;

void VisualizerStep(void) {
    if (!g_hwnd) return;
    InvalidateRect(g_hwnd, NULL, FALSE);
    UpdateWindow(g_hwnd);
    Sleep(STEP_MS);
}

static COLORREF StateColor(int state) {
    switch (state) {
        case STATE_OPEN: return RGB(10, 10, 10);
        case STATE_WALL: return RGB(50, 50, 50);
        case STATE_VISITED: return RGB(30, 100, 220);
        case STATE_HEAD: return RGB(255, 220, 0);
        case STATE_SOLUTION: return RGB(0,   200,  80);
        case STATE_ENTRANCE: return RGB(0,   220, 220);
        case STATE_EXIT: return RGB(220, 40, 40);
        default: return RGB(128, 0, 128);
    }
}

static void DrawCell(HDC hdc, int row, int col) {
    int state = igrid[row][col];
    int left = col * CELL_SIZE;
    int top = row * CELL_SIZE;
    int right = left + CELL_SIZE;
    int bottom = top  + CELL_SIZE;
    HBRUSH brush = CreateSolidBrush(StateColor(state));
    RECT rect = { left, top, right, bottom };
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
}

static void DrawGrid(HDC hdc) {
    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            DrawCell(hdc, r, c);
}

static int g_solver_choice = 0;
static DWORD WINAPI SolverThread(LPVOID param) {
    (void)param;
    int result;
    if (g_solver_choice == 1)
        result = SolveDijkstra();
    else
        result = SolveRightWall();

    if (result == SOLVE_FAIL) {
        MessageBoxA(g_hwnd, "Solver could not find a path!", "Result", MB_OK | MB_ICONWARNING);
    }
    g_solve_done = 1;
    return 0;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        g_hwnd = hwnd;
        HANDLE hThread = CreateThread(NULL, 0, SolverThread, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawGrid(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

static void ConvertGrid(void) {
    entrance_row = entrance_col = -1;
    exit_row = exit_col = -1;

    for (int r = 0; r < GH; r++) {
        for (int c = 0; c < GW; c++) {
            char ch = grid[r][c];
            if (ch == '1') {
                igrid[r][c] = STATE_WALL;
            } else if (ch == '0') {
                igrid[r][c] = STATE_OPEN;
            } else if (ch == 'E') {
                igrid[r][c] = STATE_ENTRANCE;
                entrance_row = r;
                entrance_col = c;
            } else if (ch == 'X') {
                igrid[r][c] = STATE_EXIT;
                exit_row = r;
                exit_col = c;
            }
        }
    }
}

static int IsSolvable(void) {
    int visited[GH][GW];
    memset(visited, 0, sizeof(visited));
    typedef struct { int r, c; } P;
    P queue[GH * GW];
    int head = 0, tail = 0;
    queue[tail].r = entrance_row;
    queue[tail].c = entrance_col;
    tail++;
    visited[entrance_row][entrance_col] = 1;
    const int dr[] = { 0, 0, 1, -1 };
    const int dc[] = { 1, -1, 0, 0 };

    while (head < tail) {
        P cur = queue[head++];
        if (cur.r == exit_row && cur.c == exit_col)
            return 1;

        for (int d = 0; d < 4; d++) {
            int nr = cur.r + dr[d];
            int nc = cur.c + dc[d];
            if (nr < 0 || nr >= GH || nc < 0 || nc >= GW) continue;
            if (visited[nr][nc]) continue;
            if (igrid[nr][nc] == STATE_WALL) continue;
            visited[nr][nc] = 1;
            queue[tail].r = nr;
            queue[tail].c = nc;
            tail++;
        }
    }
    return 0;
}

static void PlaceMarker(char marker, int lx, int ly) {
    if (lx == 0) {
        grid[ly * 2 + 1][0] = marker;
        grid[ly * 2 + 1][1] = '0';
    } else if (lx == CW-1) {
        grid[ly*2+1][GW-1] = marker;
        grid[ly*2+1][GW-2] = '0';
    } else if (ly == 0) {
        grid[0][lx*2+1]   = marker;
        grid[1][lx*2+1]   = '0';
    } else {
        grid[GH-1][lx*2+1] = marker;
        grid[GH-2][lx*2+1] = '0';
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    printf("=== Maze Solver ===\n");
    printf("Grid: %d logical columns (0..%d), %d logical rows (0..%d)\n",
           CW, CW-1, CH, CH-1);
    printf("Note: entrance and exit must be on the border.\n\n");
    printf("Fast test example: entrance 0 1, exit %d 1\n\n", CW - 1);
    int ex, ey, xx, xy;

    while (1) {
        printf("Entrance (col row, e.g. 0 1): ");
        if (scanf("%d %d", &ex, &ey) != 2) { puts("Bad input."); continue; }
        if (ex < 0 || ex >= CW || ey < 0 || ey >= CH) {
            printf("Out of range (col 0..%d, row 0..%d).\n", CW-1, CH-1);
            continue;
        }
        if (ex != 0 && ex != CW-1 && ey != 0 && ey != CH-1) {
            puts("Must be a border cell (col=0, col=max, row=0, or row=max).");
            continue;
        }
        break;
    }

    while (1) {
        printf("Exit     (col row, e.g. %d 1): ", CW - 1);
        if (scanf("%d %d", &xx, &xy) != 2) { puts("Bad input."); continue; }
        if (xx < 0 || xx >= CW || xy < 0 || xy >= CH) {
            printf("Out of range (col 0..%d, row 0..%d).\n", CW-1, CH-1);
            continue;
        }
        if (xx != 0 && xx != CW-1 && xy != 0 && xy != CH-1) {
            puts("Must be a border cell.");
            continue;
        }
        if (xx == ex && xy == ey) {
            puts("Entrance and exit cannot be the same cell.");
            continue;
        }
        break;
    }

    printf("\nChoose solver:\n");
    printf("  1 - Dijkstra (shortest path)\n");
    printf("  2 - Right-Wall Follower\n");
    printf("Choice: ");
    while (scanf("%d", &g_solver_choice) != 1 ||
           (g_solver_choice != 1 && g_solver_choice != 2)) {
        puts("Please enter 1 or 2.");
        printf("Choice: ");
    }

    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            grid[r][c] = '1';

    carve(0, 0);
    PlaceMarker('E', ex, ey);
    PlaceMarker('X', xx, xy);
    ConvertGrid();

    if (entrance_row < 0 || exit_row < 0) {
        puts("ERROR: could not locate entrance or exit in the grid.");
        return 1;
    }

    printf("\nChecking solvability... ");
    if (!IsSolvable()) {
        puts("FAILED - the maze has no path from entrance to exit.\n"
             "This should not happen with a perfect maze; please try again.");
        return 1;
    }
    puts("OK - path exists.\n");
    printf("Opening visualizer...\n");
    const char className[] = "MazeSolverWindow";
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClassA(&wc)) {
        puts("RegisterClass failed.");
        return 1;
    }
    RECT wr = { 0, 0, GW * CELL_SIZE, GH * CELL_SIZE };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExA(
        0, className, "Maze Solver Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right  - wr.left,
        wr.bottom - wr.top,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!hwnd) {
        puts("CreateWindow failed.");
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}