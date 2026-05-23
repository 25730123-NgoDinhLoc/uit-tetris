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
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    nextPieceIndex = std::rand() % NUM_PIECES;
    spawnNewPiece();
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

void Tetris::spawnNewPiece() {
    falling.pieceIndex = nextPieceIndex;
    nextPieceIndex     = std::rand() % NUM_PIECES;
    falling.rotation   = 0;
    falling.posX       = GameBoard::WIDTH / 2 - 2; // Căn giữa hộp bao 4 ô
    falling.posY       = 0;
    hasHeldThisTurn    = false;

    if (checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY)) {
        status.isGameOver = true;
    }
}

// ---------------------------------------------------------------------------
// Va chạm
// ---------------------------------------------------------------------------

bool Tetris::checkCollision(int pieceIndex, int rotation, int posX, int posY) const {
    const RotationGrid& grid = pieceDefinitions[pieceIndex].allRotations[rotation];
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (grid.cells[row][col] == 0) continue; // Ô trống trong hộp bao
            int boardCol = posX + col;
            int boardRow = posY + row;
            if (boardCol < 0 || boardCol >= GameBoard::WIDTH) return true;  // Va tường ngang
            if (boardRow >= GameBoard::HEIGHT)          return true;  // Va sàn
            if (boardRow < 0)                           continue;     // Trên trần là bình thường
            if (board.cells[boardRow][boardCol] != 0)   return true;  // Va khối đã khóa
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Khóa khối vào bảng
// ---------------------------------------------------------------------------

void Tetris::lockPieceToBoard() {
    const RotationGrid& grid = pieceDefinitions[falling.pieceIndex].allRotations[falling.rotation];
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (grid.cells[row][col] == 0) continue;
            int boardCol = falling.posX + col;
            int boardRow = falling.posY + row;
            if (boardRow >= 0 && boardRow < GameBoard::HEIGHT && boardCol >= 0 && boardCol < GameBoard::WIDTH) {
                board.cells[boardRow][boardCol] = grid.cells[row][col];
            }
        }
    }
}


// ---------------------------------------------------------------------------
// Xóa hàng đầy
// ---------------------------------------------------------------------------

bool Tetris::isRowFull(int row) const {
    for (int col = 0; col < GameBoard::WIDTH; ++col)
        if (board.cells[row][col] == 0) return false;
    return true;
}

void Tetris::deleteRowAndShiftDown(int deletedRow) {
    for (int row = deletedRow; row > 0; --row)
        for (int col = 0; col < GameBoard::WIDTH; ++col)
            board.cells[row][col] = board.cells[row - 1][col];

    for (int col = 0; col < GameBoard::WIDTH; ++col)
        board.cells[0][col] = 0;
}


int Tetris::clearFullLines() {
    int linesCleared = 0;
    for (int row = GameBoard::HEIGHT - 1; row >= 0; --row) {
        if (isRowFull(row)) {
            deleteRowAndShiftDown(row);
            linesCleared++;
            row++; // Kiểm tra lại hàng đã xóa
        }
    }
    return linesCleared;
}

// ---------------------------------------------------------------------------
// Xoay khối
// ---------------------------------------------------------------------------

void Tetris::rotatePiece() {
     int nextRotation = (falling.rotation + 1) % NUM_ROTATIONS;
    // Thử các độ dịch chuyển ngang: 0, -1, +1, -2, +2 ô.
    int horizontalShifts[] = {0, -1, 1, -2, 2};
    for (int shift : horizontalShifts) {
        if (!checkCollision(falling.pieceIndex, nextRotation, falling.posX + shift, falling.posY)) {
            falling.rotation = nextRotation;
            falling.posX += shift;
            return;
        }
    }
}

// ---------------------------------------------------------------------------
// Di chuyển
// ---------------------------------------------------------------------------

void Tetris::movePieceLeft() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX - 1, falling.posY))
        --falling.posX;
}

void Tetris::movePieceRight() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX + 1, falling.posY))
        ++falling.posX;
}

bool Tetris::tryMoveDownOneCell() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY + 1)) {
        ++falling.posY;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Ghost piece & Tính điểm
// ---------------------------------------------------------------------------

// TODO: implement ghost calculation
int Tetris::calculateGhostPositionY() const {
   int ghostPosY = falling.posY;
    while (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, ghostPosY + 1))
        ++ghostPosY;
    return ghostPosY;
}


int Tetris::calculateScoreForLines(int numLines) const {
    switch (numLines) {
        case 1: return 100;
        case 2: return 300;
        case 3: return 500;
        case 4: return 800;
        default: return 0;
    }
}

// ---------------------------------------------------------------------------
// Hold piece
// ---------------------------------------------------------------------------

void Tetris::holdCurrentPiece() {
    if (hasHeldThisTurn || status.isGameOver) {
        return;
    }

    if (holdPieceIndex == -1) {
        holdPieceIndex = falling.pieceIndex;
        spawnNewPiece();
    } else {
        int temp = falling.pieceIndex;
        falling.pieceIndex = holdPieceIndex;
        holdPieceIndex = temp;
        falling.posX = GameBoard::WIDTH / 2 - 2;
        falling.posY = 0;
        falling.rotation = 0;
        if (checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY)) {
            status.isGameOver = true;
        }
    }
    hasHeldThisTurn = true;

}

// ---------------------------------------------------------------------------
// Lock piece + spawn next
// ---------------------------------------------------------------------------

void Tetris::lockPieceAndSpawnNext() {
     lockPieceToBoard();
    int linesJustCleared = clearFullLines();
    if (linesJustCleared > 0) {
        status.totalLinesCleared += linesJustCleared;
        status.score += calculateScoreForLines(linesJustCleared) * status.level;
        status.level = 1 + status.totalLinesCleared / 10;
    }
    spawnNewPiece();
}

// ---------------------------------------------------------------------------
// Thờigian
// ---------------------------------------------------------------------------

int Tetris::calculateGravityDelayMs() const {
    int delay = 600 - (status.level - 1) * 50;
    return (delay < 50) ? 50 : delay;
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
    int droppedCells = 0;
    while (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY + 1)) {
        ++falling.posY;
        ++droppedCells;
    }
    return droppedCells;
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
