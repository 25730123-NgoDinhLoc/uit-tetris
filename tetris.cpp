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
	}

	// Vòng lặp chính: nhận phím, xử lý gravity tự động, gọi draw().
	void Tetris::run() {
		// TODO: khởi tạo ncurses (initscr, cbreak, noecho, nodelay, keypad, curs_set)
		// TODO: vòng lặp while (!over): đọc phím, xử lý di chuyển/xoay/drop, gravity tự động
		// TODO: dọn dẹp ncurses khi thoát
	}
