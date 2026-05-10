// Tetris entry point.
// Creates the game object (which initializes ncurses) and starts the main loop.
// When run() returns (player pressed Q), the destructor restores the terminal.

#include "tetris.h"

int main() {
    Tetris game;
    game.run();
    return 0;
}