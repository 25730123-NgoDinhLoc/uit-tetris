#include "tetris.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

namespace {
constexpr int INDEX_I = 0;
constexpr int INDEX_O = 1;
constexpr int INDEX_T = 2;
constexpr int INDEX_S = 3;
constexpr int INDEX_Z = 4;
constexpr int INDEX_J = 5;
constexpr int INDEX_L = 6;
} // namespace

Tetris::Tetris() : pieceX(5), pieceY(1), currentPiece(0), currentRotation(0), nextPiece(0), score(0), lines(0), level(1), gameOver(false) {
    srand(static_cast<unsigned int>(time(0)));
    initializePieces();
    initBoard();
    currentPiece = rand() % NUM_PIECES;
    nextPiece = rand() % NUM_PIECES;
}

void Tetris::initBlocks() {
    // Clear all first
    for (int p = 0; p < NUM_PIECES; ++p)
        for (int i = 0; i < PIECE_SIZE; ++i)
            for (int j = 0; j < PIECE_SIZE; ++j)
                blocks[p][i][j] = ' ';

    // I piece (cyan)
    blocks[0][1][0] = blocks[0][1][1] = blocks[0][1][2] = blocks[0][1][3] = 'I';

    // O piece (yellow)
    blocks[1][1][1] = blocks[1][1][2] = blocks[1][2][1] = blocks[1][2][2] = 'O';

    // T piece (magenta)
    blocks[2][1][0] = blocks[2][1][1] = blocks[2][1][2] = blocks[2][2][1] = 'T';

    // S piece (green)
    blocks[3][1][1] = blocks[3][1][2] = blocks[3][2][0] = blocks[3][2][1] = 'S';

    // Z piece (red)
    blocks[4][1][0] = blocks[4][1][1] = blocks[4][2][1] = blocks[4][2][2] = 'Z';

    // J piece (blue)
    blocks[5][0][0] = blocks[5][1][0] = blocks[5][1][1] = blocks[5][1][2] = 'J';

    // L piece (white)
    blocks[6][0][2] = blocks[6][1][0] = blocks[6][1][1] = blocks[6][1][2] = 'L';
}

void Tetris::initBoard() {
    for (int i = 0; i < BOARD_HEIGHT; ++i)
        for (int j = 0; j < BOARD_WIDTH; ++j)
            board[i][j] = (i == 0 || i == BOARD_HEIGHT - 1 || j == 0 || j == BOARD_WIDTH - 1) ? '#' : ' ';
}

void Tetris::initializePieces() {
    // Khối I - cyan
    pieceRotations[INDEX_I][0][1][0] = pieceRotations[INDEX_I][0][1][1] = pieceRotations[INDEX_I][0][1][2] = pieceRotations[INDEX_I][0][1][3] = 'I';
    pieceRotations[INDEX_I][1][0][1] = pieceRotations[INDEX_I][1][1][1] = pieceRotations[INDEX_I][1][2][1] = pieceRotations[INDEX_I][1][3][1] = 'I';
    pieceRotations[INDEX_I][2][2][0] = pieceRotations[INDEX_I][2][2][1] = pieceRotations[INDEX_I][2][2][2] = pieceRotations[INDEX_I][2][2][3] = 'I';
    pieceRotations[INDEX_I][3][0][2] = pieceRotations[INDEX_I][3][1][2] = pieceRotations[INDEX_I][3][2][2] = pieceRotations[INDEX_I][3][3][2] = 'I';

    // Khối O - yellow (giống nhau ở cả 4 trạng thái)
    for (int r = 0; r < NUM_ROTATIONS; ++r) {
        pieceRotations[INDEX_O][r][1][1] = pieceRotations[INDEX_O][r][1][2] = pieceRotations[INDEX_O][r][2][1] = pieceRotations[INDEX_O][r][2][2] = 'O';
    }

    // Khối T - magenta
    pieceRotations[INDEX_T][0][1][0] = pieceRotations[INDEX_T][0][1][1] = pieceRotations[INDEX_T][0][1][2] = pieceRotations[INDEX_T][0][2][1] = 'T';
    pieceRotations[INDEX_T][1][0][1] = pieceRotations[INDEX_T][1][1][1] = pieceRotations[INDEX_T][1][1][2] = pieceRotations[INDEX_T][1][2][1] = 'T';
    pieceRotations[INDEX_T][2][1][0] = pieceRotations[INDEX_T][2][1][1] = pieceRotations[INDEX_T][2][1][2] = pieceRotations[INDEX_T][2][0][1] = 'T';
    pieceRotations[INDEX_T][3][0][1] = pieceRotations[INDEX_T][3][1][0] = pieceRotations[INDEX_T][3][1][1] = pieceRotations[INDEX_T][3][2][1] = 'T';

      // Khối S - green
    pieceRotations[INDEX_S][0][1][1] = pieceRotations[INDEX_S][0][1][2] = pieceRotations[INDEX_S][0][2][0] = pieceRotations[INDEX_S][0][2][1] = 'S';
    pieceRotations[INDEX_S][1][0][1] = pieceRotations[INDEX_S][1][1][1] = pieceRotations[INDEX_S][1][1][2] = pieceRotations[INDEX_S][1][2][2] = 'S';
    pieceRotations[INDEX_S][2][0][1] = pieceRotations[INDEX_S][2][0][2] = pieceRotations[INDEX_S][2][1][0] = pieceRotations[INDEX_S][2][1][1] = 'S';
    pieceRotations[INDEX_S][3][0][0] = pieceRotations[INDEX_S][3][1][0] = pieceRotations[INDEX_S][3][1][1] = pieceRotations[INDEX_S][3][2][1] = 'S';

    // Khối Z - red
    pieceRotations[INDEX_Z][0][1][0] = pieceRotations[INDEX_Z][0][1][1] = pieceRotations[INDEX_Z][0][2][1] = pieceRotations[INDEX_Z][0][2][2] = 'Z';
    pieceRotations[INDEX_Z][1][0][2] = pieceRotations[INDEX_Z][1][1][1] = pieceRotations[INDEX_Z][1][1][2] = pieceRotations[INDEX_Z][1][2][1] = 'Z';
    pieceRotations[INDEX_Z][2][0][0] = pieceRotations[INDEX_Z][2][0][1] = pieceRotations[INDEX_Z][2][1][1] = pieceRotations[INDEX_Z][2][1][2] = 'Z';
    pieceRotations[INDEX_Z][3][0][1] = pieceRotations[INDEX_Z][3][1][0] = pieceRotations[INDEX_Z][3][1][1] = pieceRotations[INDEX_Z][3][2][0] = 'Z';

     // Khối J - blue
    pieceRotations[INDEX_J][0][0][0] = pieceRotations[INDEX_J][0][1][0] = pieceRotations[INDEX_J][0][1][1] = pieceRotations[INDEX_J][0][1][2] = 'J';
    pieceRotations[INDEX_J][1][0][1] = pieceRotations[INDEX_J][1][0][2] = pieceRotations[INDEX_J][1][1][1] = pieceRotations[INDEX_J][1][2][1] = 'J';
    pieceRotations[INDEX_J][2][1][0] = pieceRotations[INDEX_J][2][1][1] = pieceRotations[INDEX_J][2][1][2] = pieceRotations[INDEX_J][2][2][2] = 'J';
    pieceRotations[INDEX_J][3][0][1] = pieceRotations[INDEX_J][3][1][1] = pieceRotations[INDEX_J][3][2][0] = pieceRotations[INDEX_J][3][2][1] = 'J';

    // Khối L - white
    pieceRotations[INDEX_L][0][0][2] = pieceRotations[INDEX_L][0][1][0] = pieceRotations[INDEX_L][0][1][1] = pieceRotations[INDEX_L][0][1][2] = 'L';
    pieceRotations[INDEX_L][1][0][1] = pieceRotations[INDEX_L][1][1][1] = pieceRotations[INDEX_L][1][2][1] = pieceRotations[INDEX_L][1][2][2] = 'L';
    pieceRotations[INDEX_L][2][1][0] = pieceRotations[INDEX_L][2][1][1] = pieceRotations[INDEX_L][2][1][2] = pieceRotations[INDEX_L][2][2][0] = 'L';
    pieceRotations[INDEX_L][3][0][0] = pieceRotations[INDEX_L][3][0][1] = pieceRotations[INDEX_L][3][1][1] = pieceRotations[INDEX_L][3][2][1] = 'L';
}

void Tetris::spawn() {
    pieceX = 5;
    pieceY = 1;
    currentRotation = 0;
    currentPiece = nextPiece;
    nextPiece = rand() % NUM_PIECES;
    if (collides(currentPiece, currentRotation, pieceX, pieceY))
        gameOver = true;
}

char Tetris::getCell(int piece, int pieceRotation, int row, int col) const {
    // Return the cell of `piece` at a given `pieceRotation`.
    // On-the-fly 90-degree clockwise transforms:
    //   0: (row, col)
    //   1: (PIECE_SIZE-1-col, row)
    //   2: (PIECE_SIZE-1-row, PIECE_SIZE-1-col)
    //   3: (col, PIECE_SIZE-1-row)
    switch (pieceRotation % 4) {
    case 0: return blocks[piece][row][col];
    case 1: return blocks[piece][PIECE_SIZE - 1 - col][row];
    case 2: return blocks[piece][PIECE_SIZE - 1 - row][PIECE_SIZE - 1 - col];
    case 3: return blocks[piece][col][PIECE_SIZE - 1 - row];
    }
    return ' ';
}

bool Tetris::collides(int piece, int pieceRotation, int posX, int posY) const {
    for (int i = 0; i < PIECE_SIZE; ++i)
        for (int j = 0; j < PIECE_SIZE; ++j)
            if (getCell(piece, pieceRotation, i, j) != ' ') {
                int boardX = posX + j;
                int boardY = posY + i;
                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY < 0 || boardY >= BOARD_HEIGHT || board[boardY][boardX] != ' ')
                    return true;
            }
    return false;
}

bool Tetris::canMove(int deltaX, int deltaY) const {
    return !collides(currentPiece, currentRotation, pieceX + deltaX, pieceY + deltaY);
}

void Tetris::rotate() {
    int nextRotation = (currentRotation + 1) % 4;
    if (!collides(currentPiece, nextRotation, pieceX, pieceY))
        currentRotation = nextRotation;
}

void Tetris::lock() {
    for (int i = 0; i < PIECE_SIZE; ++i)
        for (int j = 0; j < PIECE_SIZE; ++j)
            if (getCell(currentPiece, currentRotation, i, j) != ' ')
                board[pieceY + i][pieceX + j] = getCell(currentPiece, currentRotation, i, j);
}

void Tetris::clearLines() {
    int cleared = 0;
    for (int row = BOARD_HEIGHT - 2; row > 0; --row) {
        bool full = true;
        for (int col = 1; col < BOARD_WIDTH - 1; ++col)
            if (board[row][col] == ' ') { full = false; break; }

        if (full) {
            ++cleared;
            for (int r = row; r > 1; --r)
                for (int c = 1; c < BOARD_WIDTH - 1; ++c)
                    board[r][c] = board[r - 1][c];
            for (int c = 1; c < BOARD_WIDTH - 1; ++c)
                board[1][c] = ' ';
            ++row; // re-check this row
        }
    }
    if (cleared) {
        lines += cleared;
        // Simple scoring: 40, 100, 300, 1200 per level
        int pointsTable[] = { 0, 40, 100, 300, 1200 };
        score += pointsTable[cleared] * level;
        level = 1 + lines / 10;
    }
}

void Tetris::draw() const {
    erase();

    // Draw board
    for (int i = 0; i < BOARD_HEIGHT; ++i)
        for (int j = 0; j < BOARD_WIDTH; ++j)
            mvaddch(i, j * 2, board[i][j]);

    // Draw active piece
    for (int i = 0; i < PIECE_SIZE; ++i)
        for (int j = 0; j < PIECE_SIZE; ++j)
            if (getCell(currentPiece, currentRotation, i, j) != ' ')
                mvaddch(pieceY + i, (pieceX + j) * 2, getCell(currentPiece, currentRotation, i, j));

    // Sidebar info
    int col = BOARD_WIDTH * 2 + 4;
    mvprintw(1, col, "Score: %d", score);
    mvprintw(2, col, "Lines: %d", lines);
    mvprintw(3, col, "Level: %d", level);

    mvprintw(5, col, "Next:");
    for (int i = 0; i < PIECE_SIZE; ++i)
        for (int j = 0; j < PIECE_SIZE; ++j)
            mvaddch(6 + i, col + j * 2, blocks[nextPiece][i][j]);

    mvprintw(12, col, "Controls:");
    mvprintw(13, col, "A / D  Move");
    mvprintw(14, col, "W      Rotate");
    mvprintw(15, col, "S      Soft Drop");
    mvprintw(16, col, "Space  Hard Drop");
    mvprintw(17, col, "Q      Quit");

    if (gameOver)
        mvprintw(BOARD_HEIGHT / 2, BOARD_WIDTH, " GAME OVER ");

    refresh();
}

void Tetris::run() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    auto lastDrop = chrono::steady_clock::now();
    int dropMs = 500; // start speed

    while (!gameOver) {
        int key = getch();
        if (key == 'q' || key == 'Q') break;

        if (key == 'a' || key == KEY_LEFT) { if (canMove(-1, 0)) --pieceX; }
        if (key == 'd' || key == KEY_RIGHT) { if (canMove(1, 0)) ++pieceX; }
        if (key == 'w' || key == KEY_UP)    rotate();
        if (key == 's' || key == KEY_DOWN) { if (canMove(0, 1)) ++pieceY; }
        if (key == ' ') { // hard drop
            while (canMove(0, 1)) ++pieceY;
            lock();
            clearLines();
            spawn();
        }

        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(now - lastDrop).count() > dropMs) {
            if (canMove(0, 1))
                ++pieceY;
            else {
                lock();
                clearLines();
                spawn();
            }
            lastDrop = now;
            dropMs = max(50, 500 - (level - 1) * 50);
        }

        draw();
        this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
    }

    // Wait for a key on game over before exit
    if (gameOver) {
        nodelay(stdscr, FALSE);
        getch();
    }
    endwin();
}