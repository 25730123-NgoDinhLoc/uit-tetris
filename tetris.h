#ifndef TETRIS_H
#define TETRIS_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// Tetris game using SFML graphics library.
// Displays the game board and pieces as colored rectangles.
// Stores one orientation per piece and rotates on-the-fly.
class Tetris {
public:
    Tetris();
    void run();

private:
    // Game board dimensions
    static constexpr int W = 12;   // board width including borders
    static constexpr int H = 22;   // board height including borders
    static constexpr int P = 7;    // number of pieces
    static constexpr int S = 4;    // piece size (4x4)

    // Display settings
    static constexpr int CELL_SIZE = 30;  // pixel size of each cell
    static constexpr int WINDOW_WIDTH = 600;   // window width in pixels
    static constexpr int WINDOW_HEIGHT = 720;  // window height in pixels

    // Game state
    char board[H][W];
    char blocks[P][S][S];

    int x, y;       // current piece position (top-left of 4x4 box)
    int b;          // current piece index
    int rot;        // current rotation (0..3)
    int next;       // next piece index

    int score;
    int lines;
    int level;
    bool over;

    // SFML components
    sf::RenderWindow window;
    sf::Font font;

    // Helper methods
    void initBlocks();
    void initBoard();
    void spawn();

    char getCell(int piece, int rotation, int r, int c) const;
    bool collides(int piece, int rotation, int px, int py) const;
    bool canMove(int dx, int dy) const;
    void rotate();

    void lock();
    void clearLines();

    // Graphics methods
    void drawGame();
    void drawBoard();
    void drawActivePiece();
    void drawSidebar();
    sf::Color getPieceColor(char piece) const;
};

#endif
