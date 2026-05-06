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
}

// Khởi tạo bảng blocks[P][S][S] chứa 7 tetromino (mỗi khối 4x4).
void Tetris::initBlocks() {
	// TODO: gán ký tự cho từng khối I, O, T, S, Z, J, L trong mảng blocks
}

// Tạo bàn chơi với viền '#' và nội thất là khoảng trắng.
void Tetris::initBoard() {
	// TODO: điền board[H][W] với viền xung quanh và khoảng trắng bên trong
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
bool Tetris::collides(int piece, int rotation, int px, int py) const {
	// TODO: duyệt 4x4, nếu ô không trống thì kiểm tra va chạm với board hoặc tường
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
	// TODO: duyệt các hàng, nếu đầy thì xóa và dồn xuống
	// TODO: cập nhật score, lines, level
}

// Vẽ toàn bộ màn hình: bàn chơi, khối đang rơi, sidebar, điểm số.
void Tetris::draw() const {
	// TODO: dùng ncurses để vẽ board, khối active, next piece, score, controls
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
