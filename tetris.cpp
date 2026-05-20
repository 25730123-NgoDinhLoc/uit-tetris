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

Tetris::Tetris() : pieceX(5), pieceY(1), currentPiece(0), currentRotation(0), nextPiece(0), score(0), lines(0), level(1), gameOver(false), isPaused(false) {
    srand(static_cast<unsigned int>(time(0)));
    initializePieces();
    initBlocks();
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
    // Clear all rotations first
    for (int p = 0; p < NUM_PIECES; ++p)
        for (int r = 0; r < NUM_ROTATIONS; ++r)
            for (int i = 0; i < GRID_SIZE; ++i)
                for (int j = 0; j < GRID_SIZE; ++j)
                    pieceRotations[p][r][i][j] = ' ';

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

void Tetris::spawnNewPiece() {
    pieceX = BOARD_WIDTH / 2 - 2;
    pieceY = 1;
    currentRotation = 0;
    currentPiece = nextPiece;
    nextPiece = rand() % NUM_PIECES;
    if (collides(currentPiece, currentRotation, pieceX, pieceY))
        gameOver = true;
}

void Tetris::spawn() {
    spawnNewPiece();
}

char Tetris::getCell(int piece, int pieceRotation, int row, int col) const {
    // Truy cập trực tiếp từ bảng tra pieceRotations thay vì tính on-the-fly.
    return pieceRotations[piece][pieceRotation % 4][row][col];
}

bool Tetris::checkCollision(int piece, int rotation, int posX, int posY) const {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (pieceRotations[piece][rotation][row][col] == ' ') continue;
            int boardX = posX + col;
            int boardY = posY + row;
            if (boardX < 0 || boardX >= BOARD_WIDTH) return true;
            if (boardY >= BOARD_HEIGHT) return true;
            if (boardY < 0) continue;
            if (board[boardY][boardX] != ' ') return true;
        }
    }
    return false;
}

bool Tetris::collides(int piece, int pieceRotation, int posX, int posY) const {
    return checkCollision(piece, pieceRotation, posX, posY);
}

bool Tetris::canMove(int deltaX, int deltaY) const {
    return !collides(currentPiece, currentRotation, pieceX + deltaX, pieceY + deltaY);
}

void Tetris::rotatePiece() {
    int nextRotation = (currentRotation + 1) % NUM_ROTATIONS;
    int horizontalShifts[] = {0, -1, 1, -2, 2};
    for (int shift : horizontalShifts) {
        if (!checkCollision(currentPiece, nextRotation, pieceX + shift, pieceY)) {
            currentRotation = nextRotation;
            pieceX += shift;
            return;
        }
    }
}

void Tetris::rotate() {
     rotatePiece();
}

void Tetris::lockPieceToBoard() {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (pieceRotations[currentPiece][currentRotation][row][col] == ' ') continue;
            int boardX = pieceX + col;
            int boardY = pieceY + row;
            if (boardY >= 0 && boardY < BOARD_HEIGHT && boardX >= 0 && boardX < BOARD_WIDTH) {
                board[boardY][boardX] = pieceRotations[currentPiece][currentRotation][row][col];
            }
        }
    }
}

bool Tetris::processPlayerInput(int key, chrono::steady_clock::time_point& lastDrop) {
    bool needRender = false;
	if (gameOver) return needRender;
    if (key == 'p' || key == 'P') {
        isPaused = !isPaused;
        needRender = true;
    } else if (!isPaused) {
        switch (key) {
            case KEY_LEFT:
            case 'a':
case 'A':  movePieceLeft();  needRender = true; break;
            case KEY_RIGHT:
            case 'd':
            case 'D':  movePieceRight(); needRender = true; break;
            case KEY_UP:
            case 'w':
            case 'W':  rotate();         needRender = true; break;
            case KEY_DOWN:
			case 's':
            case 'S':
                if (!tryMoveDownOneCell()) lockPieceAndSpawnNext();
				lastDrop = chrono::steady_clock::now();
                needRender = true;
                break;
            case ' ':
                int droppedCells = 0;
                while (canMove(0, 1)) { ++pieceY; ++droppedCells; }
                score += droppedCells * 2;
                lockPieceAndSpawnNext();
                needRender = true;
                break;
        }
    }
    return needRender;
}

void Tetris::movePieceLeft() {
    if (!collides(currentPiece, currentRotation, pieceX - 1, pieceY))
        --pieceX;
}

void Tetris::movePieceRight() {
    if (!collides(currentPiece, currentRotation, pieceX + 1, pieceY))
        ++pieceX;
}

bool Tetris::tryMoveDownOneCell() {
    if (!collides(currentPiece, currentRotation, pieceX, pieceY + 1)) {
        ++pieceY;
        return true; 
    }
    return false;
}

void Tetris::lock() {
   lockPieceToBoard();            
}

void Tetris::lockPieceAndSpawnNext() {
    lock();
    int linesJustCleared = clearFullLines();
    if (linesJustCleared > 0) {
        lines += linesJustCleared;
        score += calculateScoreForLines(linesJustCleared) * level;
        level = 1 + lines / 10;
    }
    spawn();
}

bool Tetris::isRowFull(int row) const {
    for (int col = 1; col < BOARD_WIDTH - 1; ++col)
        if (board[row][col] == ' ') return false;
    return true;
}

void Tetris::deleteRowAndShiftDown(int deletedRow) {
    for (int r = deletedRow; r > 1; --r)
        for (int c = 1; c < BOARD_WIDTH - 1; ++c)
            board[r][c] = board[r - 1][c];
    for (int c = 1; c < BOARD_WIDTH - 1; ++c)
        board[1][c] = ' ';
}

int Tetris::clearFullLines() {
    int linesCleared = 0;
    for (int row = BOARD_HEIGHT - 2; row > 0; --row) {
        if (isRowFull(row)) {
            deleteRowAndShiftDown(row);
            ++linesCleared;
            ++row;
        }
    }

    return linesCleared;
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

int Tetris::calculateGravityDelayMs() const {
    int delay = 600 - (level - 1) * 50;
    return (delay < 50) ? 50 : delay;
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

bool Tetris::updateGravity(chrono::steady_clock::time_point& lastDrop) {
    if (gameOver || isPaused) return false;
    auto now = chrono::steady_clock::now();
    int dropMs = max(50, 500 - (level - 1) * 50);
    if (chrono::duration_cast<chrono::milliseconds>(now - lastDrop).count() > dropMs) {
        if (!tryMoveDownOneCell())
            lockPieceAndSpawnNext();
        lastDrop = now;
        return true;
    }
    return false;
}

void Tetris::run() {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);
	initializeColors();

	auto lastDrop = chrono::steady_clock::now();

	while (!gameOver) {
		int ch = getch();
		if (ch == 'q' || ch == 'Q') break;

		if (ch == 'a' || ch == KEY_LEFT) { if (canMove(-1, 0)) --pieceX; }
		if (ch == 'd' || ch == KEY_RIGHT) { if (canMove(1, 0)) ++pieceX; }
		if (ch == 'w' || ch == KEY_UP)    rotate();
		if (ch == 's' || ch == KEY_DOWN) {
            if (canMove(0, 1)) { ++pieceY; score += 1; }
            else { lockPieceAndSpawnNext(); }
            lastDrop = chrono::steady_clock::now();
        }
		if (ch == ' ') { // hard drop
			int droppedCells = 0;
            while (canMove(0, 1)) { ++pieceY; ++droppedCells; }
            score += droppedCells * 2;
            lockPieceAndSpawnNext();
			lastDrop = chrono::steady_clock::now();
		}

		auto now = chrono::steady_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(now - lastDrop).count() > calculateGravityDelayMs()) {
			if (canMove(0, 1))
				++pieceY;
			else {
				lockPieceAndSpawnNext();
			}
			lastDrop = now;

		}

		renderFrame();
		this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
	}

	// Wait for a key on game over before exit
	if (gameOver) {
		nodelay(stdscr, FALSE);
		getch();
	}
	endwin();
}

void Tetris::initializeColors() {}

void Tetris::renderFrame() {
	draw();
}
