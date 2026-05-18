#ifndef TETRIS_H
#define TETRIS_H

// Simple Tetris game using ncurses.
// Stores one orientation per piece and rotates on-the-fly.
class Tetris {
public:
	Tetris();
	void run();

private:
	static constexpr int W = 12;   // board width including borders
	static constexpr int H = 22;   // board height including borders
	static constexpr int P = 7;    // number of pieces
	static constexpr int S = 4;    // piece size (4x4)
	static constexpr int CELL_WIDTH = 2;     // each cell draws 2 chars wide

	char board[H][W];
	char blocks[P][S][S];

	int x, y;       // current piece position (top-left of 4x4 box)
	int b;          // current piece index
	int rot;        // current rotation (0..3)
	int next;       // next piece index

	int score;
	int lines;
	int level;
	bool gameOver;
	bool isPaused;
	int boardScreenRow;
	int boardScreenCol;

	void initBlocks();
	void initBoard();
	void spawn();

	char getCell(int piece, int rotation, int r, int c) const;
	bool collides(int piece, int rotation, int px, int py) const;
	bool canMove(int dx, int dy) const;
	void rotate();

	void lock();
	void clearLines();
	void draw() const;

	void initializeColors();
	void drawBorder();
	void drawLockedBlocks();
	void drawSinglePiece(int piece, int rotation, int posX, int posY, bool isGhost);
	int calculateGhostPositionY() const;
	void drawNextPiecePreview();
	void drawSidebar();
	void drawGameOverOverlay();
	void drawPauseOverlay();
	void renderFrame();
};

#endif