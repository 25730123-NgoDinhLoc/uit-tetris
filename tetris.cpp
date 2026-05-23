#include "tetris.h"

#include <ncurses.h>
#include <cstdlib>
#include <ctime>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

Tetris::Tetris()
    : nextPieceIndex(0), holdPieceIndex(-1), hasHeldThisTurn(false),
      boardScreenRow(1), boardScreenCol(2) {
    srand(static_cast<unsigned int>(time(nullptr)));
    initializePieces();
    resetBoard();
    (void)boardScreenRow;
    (void)boardScreenCol;
}

Tetris::~Tetris() = default;

// ---------------------------------------------------------------------------
// Bảng chơi
// ---------------------------------------------------------------------------

void Tetris::resetBoard() {
    board.clearAll();
    falling = FallingPiece{};
    nextPieceIndex = rand() % NUM_PIECES;
    holdPieceIndex = -1;
    hasHeldThisTurn = false;
    status = GameStatus{};
}

// ---------------------------------------------------------------------------
// Khởi tạo các khối Tetris
// ---------------------------------------------------------------------------

void Tetris::initializePieces() {
    for (int p = 0; p < NUM_PIECES; ++p) {
        for (int r = 0; r < NUM_ROTATIONS; ++r) {
            for (int i = 0; i < GRID_SIZE; ++i) {
                for (int j = 0; j < GRID_SIZE; ++j) {
                    pieceDefinitions[p].allRotations[r].cells[i][j] = 0;
                }
            }
        }
        pieceDefinitions[p].colorPairId = 0;
    }
}

// ---------------------------------------------------------------------------
// Sinh khối mới
// ---------------------------------------------------------------------------

void Tetris::spawnNewPiece() {}

// ---------------------------------------------------------------------------
// Va chạm
// ---------------------------------------------------------------------------

bool Tetris::checkCollision(int pieceIndex, int rotation, int posX, int posY) const {
    (void)pieceIndex;
    (void)rotation;
    (void)posX;
    (void)posY;
    return true;
}

// ---------------------------------------------------------------------------
// Khóa khối vào bảng
// ---------------------------------------------------------------------------

void Tetris::lockPieceToBoard() {}

// ---------------------------------------------------------------------------
// Xóa hàng đầy
// ---------------------------------------------------------------------------

bool Tetris::isRowFull(int row) const {
    (void)row;
    return false;
}

void Tetris::deleteRowAndShiftDown(int deletedRow) {
    (void)deletedRow;
}

int Tetris::clearFullLines() {
    return 0;
}

// ---------------------------------------------------------------------------
// Xoay khối
// ---------------------------------------------------------------------------

void Tetris::rotatePiece() {}

// ---------------------------------------------------------------------------
// Di chuyển
// ---------------------------------------------------------------------------

void Tetris::movePieceLeft() {}

void Tetris::movePieceRight() {}

bool Tetris::tryMoveDownOneCell() {
    return false;
}

// ---------------------------------------------------------------------------
// Ghost piece & Tính điểm
// ---------------------------------------------------------------------------

// TODO: implement ghost calculation
int Tetris::calculateGhostPositionY() const {
    return 0;
}

int Tetris::calculateScoreForLines(int numLines) const {
    (void)numLines;
    return 0;
}

// ---------------------------------------------------------------------------
// Hold piece
// ---------------------------------------------------------------------------

void Tetris::holdCurrentPiece() {}

// ---------------------------------------------------------------------------
// Lock piece + spawn next
// ---------------------------------------------------------------------------

void Tetris::lockPieceAndSpawnNext() {}

// ---------------------------------------------------------------------------
// Thờigian
// ---------------------------------------------------------------------------

int Tetris::calculateGravityDelayMs() const {
    return 600;
}

// ---------------------------------------------------------------------------
// Vòng lặp chính
// ---------------------------------------------------------------------------

void Tetris::run() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    initializeColors();

    endwin();
}

// ---------------------------------------------------------------------------
// Xử lý phím bấm
// ---------------------------------------------------------------------------

void Tetris::processPlayerInput(int keyPressed, std::chrono::steady_clock::time_point& lastGravityDrop) {
    (void)keyPressed;
    (void)lastGravityDrop;
}

// ---------------------------------------------------------------------------
// Trọng lực tự động
// ---------------------------------------------------------------------------

bool Tetris::updateGravity(std::chrono::steady_clock::time_point& lastGravityDrop) {
    (void)lastGravityDrop;
    return false;
}

// ---------------------------------------------------------------------------
// Màu sắc
// ---------------------------------------------------------------------------

void Tetris::initializeColors() {}

// ---------------------------------------------------------------------------
// Vẽ (Rendering)
// ---------------------------------------------------------------------------

void Tetris::renderFrame() {}

void Tetris::drawBorder() {}

void Tetris::drawLockedBlocks() {}

void Tetris::drawSinglePiece(int pieceIndex, int rotation, int posX, int posY, bool isGhost) {
    (void)pieceIndex;
    (void)rotation;
    (void)posX;
    (void)posY;
    (void)isGhost;
}

void Tetris::drawNextPiecePreview() {}

void Tetris::drawHoldPiece() {}

void Tetris::drawSidebar() {}

void Tetris::drawGameOverOverlay() {}

void Tetris::drawPauseOverlay() {}

// ---------------------------------------------------------------------------
// Rơi thẳng xuống đáy
// ---------------------------------------------------------------------------

int Tetris::dropToBottom() {
    return 0;
}

// ---------------------------------------------------------------------------
// GameBoard helpers
// ---------------------------------------------------------------------------

void GameBoard::clearAll() {
    for (int r = 0; r < HEIGHT; ++r) {
        for (int c = 0; c < WIDTH; ++c) {
            cells[r][c] = 0;
        }
    }
}

// row có thể < 0 (trên trần) vẫn hợp lệ khi khối vừa xuất hiện.
bool GameBoard::isInsideBoard(int col, int row) const {
    return col >= 0 && col < WIDTH && row < HEIGHT;
}

bool GameBoard::isCellOccupied(int col, int row) const {
    if (row < 0) return false; // Phía trên trần coi như trống.
    return cells[row][col] != 0;
}
