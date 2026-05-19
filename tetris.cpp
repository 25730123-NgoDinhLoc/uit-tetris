#include "tetris.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

Tetris::Tetris() : pieceX(5), pieceY(1), currentPiece(0), currentRotation(0), nextPiece(0), score(0), lines(0), level(1), gameOver(false), isPaused(false), boardScreenRow(1), boardScreenCol(2) {
	{
	srand(static_cast<unsigned int>(time(0)));
	initBlocks();
	initBoard();
	b = rand() % P;
	next = rand() % P;
}

void Tetris::initBlocks() {
	// Clear all first
	for (int p = 0; p < P; ++p)
		for (int i = 0; i < S; ++i)
			for (int j = 0; j < S; ++j)
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
	for (int i = 0; i < H; ++i)
		for (int j = 0; j < W; ++j)
			board[i][j] = (i == 0 || i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

void Tetris::spawn() {
	x = 5;
	y = 1;
	rot = 0;
	b = next;
	next = rand() % P;
	if (collides(b, rot, x, y))
		over = true;
}

char Tetris::getCell(int piece, int rotation, int r, int c) const {
	// Return the cell of `piece` at a given `rotation`.
	// On-the-fly 90-degree clockwise transforms:
	//   0: (r, c)
	//   1: (S-1-c, r)
	//   2: (S-1-r, S-1-c)
	//   3: (c, S-1-r)
	switch (rotation % 4) {
	case 0: return blocks[piece][r][c];
	case 1: return blocks[piece][S - 1 - c][r];
	case 2: return blocks[piece][S - 1 - r][S - 1 - c];
	case 3: return blocks[piece][c][S - 1 - r];
	}
	return ' ';
}

bool Tetris::collides(int piece, int rotation, int px, int py) const {
	for (int i = 0; i < S; ++i)
		for (int j = 0; j < S; ++j)
			if (getCell(piece, rotation, i, j) != ' ') {
				int xx = px + j;
				int yy = py + i;
				if (xx < 0 || xx >= W || yy < 0 || yy >= H || board[yy][xx] != ' ')
					return true;
			}
	return false;
}

bool Tetris::canMove(int dx, int dy) const {
	return !collides(b, rot, x + dx, y + dy);
}

void Tetris::rotate() {
	int nextRot = (rot + 1) % 4;
	if (!collides(b, nextRot, x, y))
		rot = nextRot;
}

void Tetris::lock() {
	for (int i = 0; i < S; ++i)
		for (int j = 0; j < S; ++j)
			if (getCell(b, rot, i, j) != ' ')
				board[y + i][x + j] = getCell(b, rot, i, j);
}

void Tetris::clearLines() {
	int cleared = 0;
	for (int row = H - 2; row > 0; --row) {
		bool full = true;
		for (int col = 1; col < W - 1; ++col)
			if (board[row][col] == ' ') { full = false; break; }

		if (full) {
			++cleared;
			for (int r = row; r > 1; --r)
				for (int c = 1; c < W - 1; ++c)
					board[r][c] = board[r - 1][c];
			for (int c = 1; c < W - 1; ++c)
				board[1][c] = ' ';
			++row; // re-check this row
		}
	}
	if (cleared) {
		lines += cleared;
		// Simple scoring: 40, 100, 300, 1200 per level
		int pts[] = { 0, 40, 100, 300, 1200 };
		score += pts[cleared] * level;
		level = 1 + lines / 10;
	}
}

void Tetris::initializeColors() {
	if (has_colors()) {
		start_color();
		use_default_colors();
		init_pair(1, COLOR_CYAN, COLOR_BLACK);
		init_pair(2, COLOR_YELLOW, COLOR_BLACK);
		init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
		init_pair(5, COLOR_RED, COLOR_BLACK);
		init_pair(6, COLOR_BLUE, COLOR_BLACK);
		init_pair(7, COLOR_WHITE, COLOR_BLACK);
		init_pair(8, COLOR_WHITE, COLOR_BLACK);
		init_pair(9, COLOR_BLACK, COLOR_WHITE);
	}
}

void Tetris::drawBorder() {
	attron(COLOR_PAIR(8));
	int topRow = 0;
	int bottomRow = BOARD_HEIGHT - 1;
	int leftCol = 0;
	int rightCol = (BOARD_WIDTH - 1) * CELL_WIDTH;

	mvaddch(topRow, leftCol, ACS_ULCORNER);
	mvaddch(topRow, rightCol, ACS_URCORNER);
	mvaddch(bottomRow, leftCol, ACS_LLCORNER);
	mvaddch(bottomRow, rightCol, ACS_LRCORNER);

	for (int col = leftCol + 1; col < rightCol; ++col) {
		mvaddch(topRow, col, ACS_HLINE);
		mvaddch(bottomRow, col, ACS_HLINE);
	}
	for (int row = topRow + 1; row < bottomRow; ++row) {
		mvaddch(row, leftCol, ACS_VLINE);
		mvaddch(row, rightCol, ACS_VLINE);
	}
	attroff(COLOR_PAIR(8));
}

void Tetris::drawLockedBlocks() {
	for (int row = 1; row < BOARD_HEIGHT - 1; ++row) {
		for (int col = 1; col < BOARD_WIDTH - 1; ++col) {
			char cell = board[row][col];
			int screenRow = row;
			int screenCol = col * CELL_WIDTH;
			if (cell == ' ') {
				mvaddstr(screenRow, screenCol, " .");
			}
			else {
				int colorId = cell - 'A' + 1;
				attron(COLOR_PAIR(colorId) | A_REVERSE);
				mvaddstr(screenRow, screenCol, "  ");
				attroff(COLOR_PAIR(colorId) | A_REVERSE);
			}
		}
	}
}

void Tetris::drawSinglePiece(int piece, int rotation, int posX, int posY, bool isGhost) {
	for (int i = 0; i < PIECE_SIZE; ++i) {
		for (int j = 0; j < PIECE_SIZE; ++j) {
			char cell = getCell(piece, rotation, i, j);
			if (cell == ' ') continue;
			int boardCol = posX + j;
			int boardRow = posY + i;
			if (boardRow <= 0 || boardRow >= BOARD_HEIGHT - 1) continue;
			if (boardCol <= 0 || boardCol >= BOARD_WIDTH - 1) continue;
			int screenRow = boardRow;
			int screenCol = boardCol * CELL_WIDTH;
			int colorId = cell - 'A' + 1;
			if (isGhost) {
				attron(COLOR_PAIR(colorId));
				mvaddstr(screenRow, screenCol, "::");
				attroff(COLOR_PAIR(colorId));
			}
			else {
				attron(COLOR_PAIR(colorId) | A_REVERSE);
				mvaddstr(screenRow, screenCol, "  ");
				attroff(COLOR_PAIR(colorId) | A_REVERSE);
			}
		}
	}
}

int Tetris::calculateGhostPositionY() const {
	int ghostY = pieceY;
	while (!collides(currentPiece, currentRotation, pieceX, ghostY + 1))
		++ghostY;
	return ghostY;
}

void Tetris::drawNextPiecePreview() {
	int sidebarCol = BOARD_WIDTH * CELL_WIDTH + 4;
	int sidebarRow = 1;
	attron(COLOR_PAIR(8));
	mvprintw(sidebarRow, sidebarCol, "NEXT");
	attroff(COLOR_PAIR(8));
	for (int i = 0; i < PIECE_SIZE; ++i)
		for (int j = 0; j < PIECE_SIZE; ++j)
			mvaddstr(sidebarRow + 2 + i, sidebarCol + j * CELL_WIDTH, "  ");
	for (int i = 0; i < PIECE_SIZE; ++i) {
		for (int j = 0; j < PIECE_SIZE; ++j) {
			char cell = blocks[nextPiece][i][j];
			if (cell == ' ') continue;
			int colorId = cell - 'A' + 1;
			attron(COLOR_PAIR(colorId) | A_REVERSE);
			mvaddstr(sidebarRow + 2 + i, sidebarCol + j * CELL_WIDTH, "  ");
			attroff(COLOR_PAIR(colorId) | A_REVERSE);
		}
	}
}

void Tetris::drawSidebar() {
	int sidebarCol = BOARD_WIDTH * CELL_WIDTH + 4;
	int sidebarRow = 8;
	attron(COLOR_PAIR(8));
	mvprintw(sidebarRow, sidebarCol, "SCORE: %-6d", score);
	mvprintw(sidebarRow + 2, sidebarCol, "LEVEL: %-6d", level);
	mvprintw(sidebarRow + 4, sidebarCol, "LINES: %-6d", lines);
	mvprintw(sidebarRow + 7, sidebarCol, "CONTROLS");
	mvprintw(sidebarRow + 8, sidebarCol, "  <- / ->  move");
	mvprintw(sidebarRow + 9, sidebarCol, "  Up       rotate");
	mvprintw(sidebarRow + 10, sidebarCol, "  Down     soft drop");
	mvprintw(sidebarRow + 11, sidebarCol, "  Space    hard drop");
	mvprintw(sidebarRow + 12, sidebarCol, "  Q        quit");
	attroff(COLOR_PAIR(8));
}

void Tetris::drawGameOverOverlay() {
	int centerCol = (BOARD_WIDTH * CELL_WIDTH) / 2 - 5;
	int centerRow = BOARD_HEIGHT / 2;
	attron(COLOR_PAIR(9) | A_BOLD);
	mvprintw(centerRow, centerCol, "  GAME  OVER  ");
	mvprintw(centerRow + 1, centerCol, "  press  Q    ");
	attroff(COLOR_PAIR(9) | A_BOLD);
}

void Tetris::drawPauseOverlay() {
	int centerCol = (BOARD_WIDTH * CELL_WIDTH) / 2 - 4;
	int centerRow = BOARD_HEIGHT / 2;
	attron(COLOR_PAIR(9) | A_BOLD);
	mvprintw(centerRow, centerCol, "  PAUSED  ");
	attroff(COLOR_PAIR(9) | A_BOLD);
}

void Tetris::renderFrame() {
	erase();
	attron(COLOR_PAIR(8) | A_BOLD);
	mvprintw(0, 0, "TETRIS");
	attroff(COLOR_PAIR(8) | A_BOLD);
	drawBorder();
	drawLockedBlocks();
	drawSinglePiece(currentPiece, currentRotation, pieceX, calculateGhostPositionY(), true);
	drawSinglePiece(currentPiece, currentRotation, pieceX, pieceY, false);
	drawNextPiecePreview();
	drawSidebar();
	if (gameOver) drawGameOverOverlay();
	refresh();
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
	int dropMs = 500; // start speed

	while (!over) {
		int ch = getch();
		if (ch == 'q' || ch == 'Q') break;

		if (ch == 'a' || ch == KEY_LEFT) { if (canMove(-1, 0)) --x; }
		if (ch == 'd' || ch == KEY_RIGHT) { if (canMove(1, 0)) ++x; }
		if (ch == 'w' || ch == KEY_UP)    rotate();
		if (ch == 's' || ch == KEY_DOWN) { if (canMove(0, 1)) ++y; }
		if (ch == ' ') { // hard drop
			while (canMove(0, 1)) ++y;
			lock();
			clearLines();
			spawn();
		}

		auto now = chrono::steady_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(now - lastDrop).count() > dropMs) {
			if (canMove(0, 1))
				++y;
			else {
				lock();
				clearLines();
				spawn();
			}
			lastDrop = now;
			dropMs = max(50, 500 - (level - 1) * 50);
		}

		renderFrame();
		this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
	}

	// Wait for a key on game over before exit
	if (over) {
		nodelay(stdscr, FALSE);
		getch();
	}
	endwin();
}
