#ifndef TETRIS_H
#define TETRIS_H

// Simple Tetris game using ncurses.
// Stores one orientation per piece and rotates on-the-fly.
class Tetris {
public:
    Tetris();
    void run();

private:
    static constexpr int BOARD_WIDTH = 12;   // board width including borders
    static constexpr int BOARD_HEIGHT = 22;  // board height including borders
    static constexpr int NUM_PIECES = 7;     // number of pieces
    static constexpr int PIECE_SIZE = 4;     // piece size (4x4)

    char board[BOARD_HEIGHT][BOARD_WIDTH];
    char blocks[NUM_PIECES][PIECE_SIZE][PIECE_SIZE];

    int pieceX, pieceY;      // current piece position (top-left of 4x4 box)
    int currentPiece;        // current piece index
    int currentRotation;     // current rotation (0..3)
    int nextPiece;           // next piece index

    int score;
    int lines;
    int level;
    bool gameOver;

    void initBlocks();
    void initBoard();
    void spawn();

    char getCell(int piece, int pieceRotation, int row, int col) const;
    bool collides(int piece, int pieceRotation, int posX, int posY) const;
    bool canMove(int deltaX, int deltaY) const;
    void rotate();

    void lock();
    void clearLines();
    void draw() const;
};

#endif