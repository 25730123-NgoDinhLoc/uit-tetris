#ifndef TETRIS_H
#define TETRIS_H
#include <chrono>
// Simple Tetris game using ncurses.
// Stores one orientation per piece and rotates on-the-fly.
class Tetris {
public:
	Tetris();
	~Tetris() = default;
	void run();

private:
	static constexpr int W = 12;   // board width including borders
	static constexpr int H = 22;   // board height including borders
	static constexpr int P = 7;    // number of pieces
	static constexpr int NUM_ROTATIONS = 4;  // number of rotations
	static constexpr int S = 4;    // piece size (4x4)
	static constexpr int GRID_SIZE = 4;      // grid size (4x4)

	static constexpr int NUM_PIECES = 7;     // number of pieces

	static constexpr int BOARD_WIDTH = W;
	static constexpr int BOARD_HEIGHT = H;
	static constexpr int PIECE_SIZE = S;
	

	char board[H][W];
	char blocks[P][S][S];
	char pieceRotations[NUM_PIECES][NUM_ROTATIONS][GRID_SIZE][GRID_SIZE];

	int pieceX, pieceY;       // current piece position (top-left of 4x4 box)
	int currentPiece;         // current piece index
	int currentRotation;      // current rotation (0..3)
	int nextPiece;            // next piece index

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

	void initializePieces();
    void rotatePiece();
    bool checkCollision(int piece, int rotation, int posX, int posY) const;
    void lockPieceToBoard();
    void spawnNewPiece();

	void lock();
	void clearLines();
	bool isRowFull(int row) const;
    void deleteRowAndShiftDown(int row);
    int clearFullLines();
    int calculateScoreForLines(int numLines) const;
    int calculateGravityDelayMs() const;
	void draw() const;
	void movePieceLeft();
    void movePieceRight();
    bool tryMoveDownOneCell();
    bool processPlayerInput(int key, std::chrono::steady_clock::time_point& lastDrop);
    bool updateGravity(std::chrono::steady_clock::time_point& lastDrop);
    void lockPieceAndSpawnNext();
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
