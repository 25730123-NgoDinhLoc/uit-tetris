#include "tetris.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

// Khởi tạo trò chơi: thiết lập RNG, tạo bàn chơi và sinh khối đầu tiên.
Tetris::Tetris() : x(5), y(1), b(0), rot(0), next(0), score(0), lines(0), level(1), over(false) {
	// TODO: khởi tạo srand, gọi initBlocks(), initBoard(), sinh khối đầu tiên
	Tetris::Tetris() : x(5), y(1), b(0), rot(0), next(0), score(0), lines(0), level(1), over(false) {
    srand(static_cast<unsigned int>(time(0)));
    initBlocks();
    initBoard();
    b = rand() % P;
    next = rand() % P;
}

// Khởi tạo bảng blocks[P][S][S] chứa 7 tetromino (mỗi khối 4x4).
void Tetris::initBlocks() {
	// TODO: gán ký tự cho từng khối I, O, T, S, Z, J, L trong mảng blocks
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

// Tạo bàn chơi với viền '#' và nội thất là khoảng trắng.
void Tetris::initBoard() {
	// TODO: điền board[H][W] với viền xung quanh và khoảng trắng bên trong
	for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            board[i][j] = (i == 0 || i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

// Đưa khối tiếp theo (next) thành khối hiện tại, sau đó random khối mới cho next.
void Tetris::spawn() {
	// TODO: reset vị trí (x, y) về giữa trên cùng, rot = 0, đổi next -> b, random next mới
	// TODO: kiểm tra va chạm ngay khi sinh -> nếu va chạm thì over = true
}

// Trả về ký tự của khối `piece` tại ô (r, c) sau khi xoay `rotation` lần 90°.
char Tetris::getCell(int piece, int rotation, int r, int c) const {
	// TODO: tính toạ độ xoay và trả về blocks[piece][...][...]
	return ' ';
}

// Kiểm tra xem khối `piece` ở vị trí (px, py) và xoay `rotation` có va chạm tường hoặc
// khối đã khóa trên bàn chơi hay không.
// TODO: duyệt 4x4, nếu ô không trống thì kiểm tra va chạm với board hoặc tường
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

// Kiểm tra xem khối hiện tại có thể di chuyển (dx, dy) không.
bool Tetris::canMove(int dx, int dy) const {
	// TODO: gọi collides với vị trí x+dx, y+dy
	return false;
}

// Xoay khối hiện tại 90° theo chiều kim đồng hồ nếu không va chạm.
void Tetris::rotate() {
	// TODO: thử rot mới = (rot + 1) % 4, nếu không collides thì cập nhật rot
}

// Khóa khối hiện tại vào bàn chơi (merge vào board).
void Tetris::lock() {
	// TODO: duyệt 4x4, nếu ô khối không trống thì ghi ký tự vào board[y+i][x+j]
}

// Quét từ dưới lên, xóa các hàng đầy, tính điểm và tăng level.
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

// Vẽ toàn bộ màn hình: bàn chơi, khối đang rơi, sidebar, điểm số.
void Tetris::draw() const {
	// TODO: dùng ncurses để vẽ board, khối active, next piece, score, controls
	erase();

    // Draw board
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            mvaddch(i, j * 2, board[i][j]);

    // Draw active piece
    for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            if (getCell(b, rot, i, j) != ' ')
                mvaddch(y + i, (x + j) * 2, getCell(b, rot, i, j));

    // Sidebar info
    int col = W * 2 + 4;
    mvprintw(1, col, "Score: %d", score);
    mvprintw(2, col, "Lines: %d", lines);
    mvprintw(3, col, "Level: %d", level);

    mvprintw(5, col, "Next:");
    for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            mvaddch(6 + i, col + j * 2, blocks[next][i][j]);

    mvprintw(12, col, "Controls:");
    mvprintw(13, col, "A / D  Move");
    mvprintw(14, col, "W      Rotate");
    mvprintw(15, col, "S      Soft Drop");
    mvprintw(16, col, "Space  Hard Drop");
    mvprintw(17, col, "Q      Quit");

    if (over)
        mvprintw(H / 2, W, " GAME OVER ");

    refresh();
}

// Vòng lặp chính: nhận phím, xử lý gravity tự động, gọi draw().

// TODO: khởi tạo ncurses (initscr, cbreak, noecho, nodelay, keypad, curs_set)
void Tetris::run() {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);

	auto lastDrop = chrono::steady_clock::now();
	int dropMs = 500; // start speed

	// TODO: vòng lặp while (!over): đọc phím, xử lý di chuyển/xoay/drop, gravity tự động
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

		draw();
		this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
	}
	// TODO: dọn dẹp ncurses khi thoát
	 // Wait for a key on game over before exit
	if (over) {
		nodelay(stdscr, FALSE);
		getch();
	}
	endwin();
}
