# Build:   make
# Clean:   make clean
# Run:    .\maze_solver.exe

CC      = gcc
TARGET  = maze_solver.exe

SRCS    = Main.c MazeGenerator.c DijkstraSolver.c RightWallSolver.c

CFLAGS  = -std=c11 -Wall -Wextra -O2 -mconsole
LDFLAGS = -lgdi32 -luser32 -lkernel32

$(TARGET): $(SRCS) maze.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	del /Q $(TARGET) 2>nul || rm -f $(TARGET)