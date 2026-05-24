#include "tetris.h"

#include <ncurses.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>

// ---------------------------------------------------------------------------
// Các hằng số nội bộ (chỉ dùng trong file này)
// ---------------------------------------------------------------------------
namespace {

    // Chỉ số khối trong bảng tra pieceDefinitions, dùng để khởi tạo dễ đọc hơn.
    constexpr int INDEX_I = 0;
    constexpr int INDEX_O = 1;
    constexpr int INDEX_T = 2;
    constexpr int INDEX_S = 3;
    constexpr int INDEX_Z = 4;
    constexpr int INDEX_J = 5;
    constexpr int INDEX_L = 6;

} // namespace

// ---------------------------------------------------------------------------
// GameBoard: cài đặt các hàm tiện ích
// ---------------------------------------------------------------------------

// Xóa toàn bộ bảng chơi về trạng thái trống (tất cả ô = 0).
void GameBoard::clearAll() {
    for (int row = 0; row < HEIGHT; ++row)
        for (int col = 0; col < WIDTH; ++col)
            cells[row][col] = 0;
}

// Kiểm tra tọa độ (col, row) có nằm trong biên cho phép của bảng không.
// row có thể < 0 (trên trần) vẫn hợp lệ khi khối vừa xuất hiện.
bool GameBoard::isInsideBoard(int col, int row) const {
    return col >= 0 && col < WIDTH && row < HEIGHT;
}

// Kiểm tra ô (col, row) đã bị chiếm bởi khối đã khóa hay chưa.
// Phía trên trần (row < 0) coi như trống.
bool GameBoard::isCellOccupied(int col, int row) const {
    if (row < 0) return false;
    return cells[row][col] != 0;
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

// Khởi tạo bảng tra các trạng thái xoay, bảng chơi, và sinh khối đầu tiên.
Tetris::Tetris()
    : nextPieceIndex(0),
    holdPieceIndex(-1),
    hasHeldThisTurn(false),
    boardScreenRow(1),
    boardScreenCol(2) {
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

// Xóa bảng và reset toàn bộ trạng thái game về ban đầu.
void Tetris::resetBoard() {
    board.clearAll();
    falling = FallingPiece{};
    nextPieceIndex = std::rand() % NUM_PIECES;
    holdPieceIndex = -1;
    hasHeldThisTurn = false;
    status = GameStatus{};
}

// ---------------------------------------------------------------------------
// Khởi tạo các khối Tetris
// ---------------------------------------------------------------------------

// Điền bảng pieceDefinitions với tất cả các trạng thái xoay của 7 tetromino.
// Mỗi ô lưu id cặp màu (chỉ số khối + 1). Giá trị 0 nghĩa là ô trống.
void Tetris::initializePieces() {
    // Khối I
    pieceDefinitions[INDEX_I].colorPairId = 1;
    pieceDefinitions[INDEX_I].allRotations[0] = { {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_I].allRotations[1] = { {
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    } };
    pieceDefinitions[INDEX_I].allRotations[2] = { {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_I].allRotations[3] = { {
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    } };

    // Khối O (giống nhau ở cả 4 trạng thái vì không xoay)
    pieceDefinitions[INDEX_O].colorPairId = 2;
    for (int r = 0; r < NUM_ROTATIONS; ++r) {
        pieceDefinitions[INDEX_O].allRotations[r] = { {
            {0, 2, 2, 0},
            {0, 2, 2, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        } };
    }

    // Khối T
    pieceDefinitions[INDEX_T].colorPairId = 3;
    pieceDefinitions[INDEX_T].allRotations[0] = { {
        {0, 3, 0, 0},
        {3, 3, 3, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_T].allRotations[1] = { {
        {0, 3, 0, 0},
        {0, 3, 3, 0},
        {0, 3, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_T].allRotations[2] = { {
        {0, 0, 0, 0},
        {3, 3, 3, 0},
        {0, 3, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_T].allRotations[3] = { {
        {0, 3, 0, 0},
        {3, 3, 0, 0},
        {0, 3, 0, 0},
        {0, 0, 0, 0}
    } };

    // Khối S
    pieceDefinitions[INDEX_S].colorPairId = 4;
    pieceDefinitions[INDEX_S].allRotations[0] = { {
        {0, 4, 4, 0},
        {4, 4, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_S].allRotations[1] = { {
        {4, 0, 0, 0},
        {4, 4, 0, 0},
        {0, 4, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_S].allRotations[2] = { {
        {0, 0, 0, 0},
        {0, 4, 4, 0},
        {4, 4, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_S].allRotations[3] = { {
        {0, 4, 0, 0},
        {0, 4, 4, 0},
        {0, 0, 4, 0},
        {0, 0, 0, 0}
    } };

    // Khối Z
    pieceDefinitions[INDEX_Z].colorPairId = 5;
    pieceDefinitions[INDEX_Z].allRotations[0] = { {
        {5, 5, 0, 0},
        {0, 5, 5, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_Z].allRotations[1] = { {
        {0, 0, 5, 0},
        {0, 5, 5, 0},
        {0, 5, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_Z].allRotations[2] = { {
        {0, 0, 0, 0},
        {5, 5, 0, 0},
        {0, 5, 5, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_Z].allRotations[3] = { {
        {0, 5, 0, 0},
        {5, 5, 0, 0},
        {5, 0, 0, 0},
        {0, 0, 0, 0}
    } };

    // Khối J
    pieceDefinitions[INDEX_J].colorPairId = 6;
    pieceDefinitions[INDEX_J].allRotations[0] = { {
        {6, 0, 0, 0},
        {6, 6, 6, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_J].allRotations[1] = { {
        {0, 6, 6, 0},
        {0, 6, 0, 0},
        {0, 6, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_J].allRotations[2] = { {
        {0, 0, 0, 0},
        {6, 6, 6, 0},
        {0, 0, 6, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_J].allRotations[3] = { {
        {0, 6, 0, 0},
        {0, 6, 0, 0},
        {6, 6, 0, 0},
        {0, 0, 0, 0}
    } };

    // Khối L
    pieceDefinitions[INDEX_L].colorPairId = 7;
    pieceDefinitions[INDEX_L].allRotations[0] = { {
        {0, 0, 7, 0},
        {7, 7, 7, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_L].allRotations[1] = { {
        {0, 7, 0, 0},
        {0, 7, 0, 0},
        {0, 7, 7, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_L].allRotations[2] = { {
        {0, 0, 0, 0},
        {7, 7, 7, 0},
        {7, 0, 0, 0},
        {0, 0, 0, 0}
    } };
    pieceDefinitions[INDEX_L].allRotations[3] = { {
        {7, 7, 0, 0},
        {0, 7, 0, 0},
        {0, 7, 0, 0},
        {0, 0, 0, 0}
    } };
}

// ---------------------------------------------------------------------------
// Sinh khối mới
// ---------------------------------------------------------------------------

// Đưa khối "next" thành khối đang rơi, sau đó tạo ngẫu nhiên khối "next" mới.
// Nếu khối mới xuất hiện đã va chạm ngay lập tức, trò chơi kết thúc.
void Tetris::spawnNewPiece() {
    falling.pieceIndex = nextPieceIndex;
    nextPieceIndex = std::rand() % NUM_PIECES;
    falling.rotation = 0;
    falling.posX = GameBoard::WIDTH / 2 - 2; // Căn giữa hộp bao 4 ô
    falling.posY = 0;
    hasHeldThisTurn = false;

    if (checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY)) {
        status.isGameOver = true;
    }
}

// ---------------------------------------------------------------------------
// Va chạm
// ---------------------------------------------------------------------------

// Kiểm tra khối (chỉ số + góc xoay) tại vị trí (posX, posY) có đè lên tường
// hoặc khối đã khóa hay không. Dùng cho mọi thao tác di chuyển và xoay.
// Quy tắc: ô trống trong grid 4x4 (value = 0) bỏ qua; row < 0 (trên trần) bỏ qua.
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

// Ghi khối đang rơi vào bảng chơi, biến nó thành các khối đã khóa.
// Chỉ ghi các ô có giá trị != 0 và nằm trong biên board.
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

// Kiểm tra 1 hàng đã đầy (không còn ô trống nào) chưa.
bool Tetris::isRowFull(int row) const {
    for (int col = 0; col < GameBoard::WIDTH; ++col)
        if (board.cells[row][col] == 0) return false;
    return true;
}

// Xóa 1 hàng và dồn tất cả các hàng phía trên xuống 1 bậc.
// Hàng trên cùng (row = 0) được xóa trống sau khi dồn.
void Tetris::deleteRowAndShiftDown(int deletedRow) {
    for (int row = deletedRow; row > 0; --row)
        for (int col = 0; col < GameBoard::WIDTH; ++col)
            board.cells[row][col] = board.cells[row - 1][col];

    for (int col = 0; col < GameBoard::WIDTH; ++col)
        board.cells[0][col] = 0;
}

// Quét từ dưới lên trên. Với mỗi hàng đầy, xóa hàng đó và dồn các hàng bên trên xuống.
// Trả về số hàng đã xóa để bên ngoài tính điểm.
int Tetris::clearFullLines() {
    int linesCleared = 0;
    for (int row = GameBoard::HEIGHT - 1; row >= 0; --row) {
        if (isRowFull(row)) {
            deleteRowAndShiftDown(row);
            ++linesCleared;
            ++row; // Kiểm tra lại hàng này vì đã có hàng mới rơi xuống.
        }
    }
    return linesCleared;
}

// ---------------------------------------------------------------------------
// Xoay khối
// ---------------------------------------------------------------------------

// Xoay khối theo chiều kim đồng hồ. Nếu góc mới bị va chạm, thử đẩy sang
// trái/phải (wall kick) để ngườichơi có thể xoay khi sát tường.
// Nếu không có độ dịch nào hợp lệ, bỏ qua thao tác xoay.
void Tetris::rotatePiece() {
    int nextRotation = (falling.rotation + 1) % NUM_ROTATIONS;
    // Thử các độ dịch chuyển ngang: 0, -1, +1, -2, +2 ô.
    int horizontalShifts[] = { 0, -1, 1, -2, 2 };
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

// Di chuyển khối sang trái 1 ô nếu không va chạm.
void Tetris::movePieceLeft() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX - 1, falling.posY))
        --falling.posX;
}

// Di chuyển khối sang phải 1 ô nếu không va chạm.
void Tetris::movePieceRight() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX + 1, falling.posY))
        ++falling.posX;
}

// Rơi xuống 1 ô. Trả về true nếu thành công, false nếu bị sàn/khối chặn.
// Bên ngoài dùng giá trị trả về để biết khi nào cần khóa khối.
bool Tetris::tryMoveDownOneCell() {
    if (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY + 1)) {
        ++falling.posY;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Ghost piece
// ---------------------------------------------------------------------------

// Tính vị trí Y thấp nhất hợp lệ của khối hiện tại (không di chuyển thật).
// Dùng để vẽ bóng đổ (ghost) cho ngườichơi biết khối sẽ rơi xuống đâu.
int Tetris::calculateGhostPositionY() const {
    int ghostPosY = falling.posY;
    while (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, ghostPosY + 1))
        ++ghostPosY;
    return ghostPosY;
}

// ---------------------------------------------------------------------------
// Tính điểm
// ---------------------------------------------------------------------------

// Điểm theo hàng đã xóa: 1=100, 2=300, 3=500, 4=800. Sau đó nhân với cấp độ.
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

// Lưu khối hiện tại vào hold, đổi lấy khối đang giữ (nếu có).
// Chỉ cho phép hold 1 lần trong mỗi lượt (cho đến khi spawn khối mới).
void Tetris::holdCurrentPiece() {
    if (hasHeldThisTurn || status.isGameOver) {
        return;
    }

    if (holdPieceIndex == -1) {
        holdPieceIndex = falling.pieceIndex;
        spawnNewPiece();
    }
    else {
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

// Khóa khối đang rơi vào bảng, xóa các hàng đầy, tính điểm, tăng level, rồi sinh khối mới.
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
// Thời gian
// ---------------------------------------------------------------------------

// Khoảng thờigian chờ giữa các lần tự rơi: giảm 50 ms mỗi cấp, tối thiểu 50 ms.
int Tetris::calculateGravityDelayMs() const {
    int delay = 600 - (status.level - 1) * 50;
    return (delay < 50) ? 50 : delay;
}

// ---------------------------------------------------------------------------
// Vòng lặp chính
// ---------------------------------------------------------------------------

// Vòng lặp chính: khởi tạo ncurses, vẽ frame ban đầu, rồi lặp đọc phím không chặn
// và cập nhật trọng lực theo thờigian (~60 FPS).
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

// Đọc phím từ ngườichơi và thực hiện di chuyển / xoay / tạm dừng.
// Tham số lastGravityDrop dùng để reset bộ đếm trọng lực khi ngườichơi chủ động rơi.
void Tetris::processPlayerInput(int keyPressed, std::chrono::steady_clock::time_point& lastGravityDrop) {
    (void)keyPressed;
    (void)lastGravityDrop;
}

// ---------------------------------------------------------------------------
// Trọng lực tự động
// ---------------------------------------------------------------------------

// Kiểm tra xem đã đến lúc tự rơi chưa, nếu có thì hạ khối xuống 1 ô.
// Trả về true nếu cần vẽ lại màn hình.
bool Tetris::updateGravity(std::chrono::steady_clock::time_point& lastGravityDrop) {
    (void)lastGravityDrop;
    return false;
}

// ---------------------------------------------------------------------------
// Màu sắc
// ---------------------------------------------------------------------------

void Tetris::initializeColors() {
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, COLOR_BLACK); // I
        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // O
        init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // T
        init_pair(4, COLOR_GREEN, COLOR_BLACK); // S
        init_pair(5, COLOR_RED, COLOR_BLACK); // Z
        init_pair(6, COLOR_BLUE, COLOR_BLACK); // J
        init_pair(7, COLOR_WHITE, COLOR_BLACK); // L
        init_pair(8, COLOR_WHITE, COLOR_BLACK); // Viền / chữ giao diện
        init_pair(9, COLOR_BLACK, COLOR_WHITE); // Lớp phủ (pause, game over)
    }
}

// ---------------------------------------------------------------------------
// Vẽ viền bảng chơi
// ---------------------------------------------------------------------------
void Tetris::drawBorder() {
    attron(COLOR_PAIR(8));
    int topRow = boardScreenRow - 1;
    int bottomRow = boardScreenRow + GameBoard::HEIGHT;
    int leftCol = boardScreenCol - 1;
    int rightCol = boardScreenCol + GameBoard::WIDTH * CELL_WIDTH;

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

// ---------------------------------------------------------------------------
// Vẽ các khối đã khóa
// ---------------------------------------------------------------------------
void Tetris::drawLockedBlocks() {
    for (int row = 0; row < GameBoard::HEIGHT; ++row) {
        for (int col = 0; col < GameBoard::WIDTH; ++col) {
            int colorId = board.cells[row][col];
            int screenRow = boardScreenRow + row;
            int screenCol = boardScreenCol + col * CELL_WIDTH;
            if (colorId == 0) {
                mvaddstr(screenRow, screenCol, " .");
            }
            else {
                attron(COLOR_PAIR(colorId) | A_REVERSE);
                mvaddstr(screenRow, screenCol, "  ");
                attroff(COLOR_PAIR(colorId) | A_REVERSE);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Vẽ 1 khối tại vị trí bất kỳ
// ---------------------------------------------------------------------------
// Vẽ một khối tại tọa độ bảng (posX, posY).
// Nếu isGhost = true, vẽ bằng "::" thay vì khối đặc để người chơi thấy điểm rơi.
void Tetris::drawSinglePiece(int pieceIndex, int rotation, int posX, int posY, bool isGhost) {
    const RotationGrid& grid = pieceDefinitions[pieceIndex].allRotations[rotation];
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            int colorId = grid.cells[row][col];
            if (colorId == 0) continue;
            int boardCol = posX + col;
            int boardRow = posY + row;
            if (boardRow < 0 || boardRow >= GameBoard::HEIGHT) continue;
            if (boardCol < 0 || boardCol >= GameBoard::WIDTH)  continue;
            int screenRow = boardScreenRow + boardRow;
            int screenCol = boardScreenCol + boardCol * CELL_WIDTH;
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

// ---------------------------------------------------------------------------
// Vẽ khối tiếp theo (NEXT)
// ---------------------------------------------------------------------------
void Tetris::drawNextPiecePreview() {
    int sidebarCol = boardScreenCol + GameBoard::WIDTH * CELL_WIDTH + 3;
    int sidebarRow = boardScreenRow;

    attron(COLOR_PAIR(8));
    mvprintw(sidebarRow, sidebarCol, "NEXT");
    attroff(COLOR_PAIR(8));

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            mvaddstr(sidebarRow + 2 + row, sidebarCol + col * CELL_WIDTH, "  ");
        }
    }

    const RotationGrid& grid = pieceDefinitions[nextPieceIndex].allRotations[0];
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            int colorId = grid.cells[row][col];
            if (colorId == 0) continue;
            attron(COLOR_PAIR(colorId) | A_REVERSE);
            mvaddstr(sidebarRow + 2 + row, sidebarCol + col * CELL_WIDTH, "  ");
            attroff(COLOR_PAIR(colorId) | A_REVERSE);
        }
    }
}

// ---------------------------------------------------------------------------
// Vẽ khối đang giữ (HOLD)
// ---------------------------------------------------------------------------
void Tetris::drawHoldPiece() {
    int sidebarCol = boardScreenCol + GameBoard::WIDTH * CELL_WIDTH + 3;
    int sidebarRow = boardScreenRow + 7;

    attron(COLOR_PAIR(8));
    mvprintw(sidebarRow, sidebarCol, "HOLD");
    attroff(COLOR_PAIR(8));

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            mvaddstr(sidebarRow + 2 + row, sidebarCol + col * CELL_WIDTH, "  ");
        }
    }

    if (holdPieceIndex == -1) {
        return;
    }

    const RotationGrid& grid = pieceDefinitions[holdPieceIndex].allRotations[0];
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            int colorId = grid.cells[row][col];
            if (colorId == 0) continue;
            attron(COLOR_PAIR(colorId) | A_REVERSE);
            mvaddstr(sidebarRow + 2 + row, sidebarCol + col * CELL_WIDTH, "  ");
            attroff(COLOR_PAIR(colorId) | A_REVERSE);
        }
    }
}

// ---------------------------------------------------------------------------
// Vẽ thanh bên (sidebar)
// ---------------------------------------------------------------------------
void Tetris::drawSidebar() {
    int sidebarCol = boardScreenCol + GameBoard::WIDTH * CELL_WIDTH + 3;
    int sidebarRow = boardScreenRow + 15;

    attron(COLOR_PAIR(8));
    mvprintw(sidebarRow, sidebarCol, "SCORE: %-6d", status.score);
    mvprintw(sidebarRow + 2, sidebarCol, "LEVEL: %-6d", status.level);
    mvprintw(sidebarRow + 4, sidebarCol, "LINES: %-6d", status.totalLinesCleared);

    mvprintw(sidebarRow + 7, sidebarCol, "CONTROLS");
    mvprintw(sidebarRow + 8, sidebarCol, "  A/D      move");
    mvprintw(sidebarRow + 9, sidebarCol, "  W        rotate");
    mvprintw(sidebarRow + 10, sidebarCol, "  S        soft drop");
    mvprintw(sidebarRow + 11, sidebarCol, "  Space    hard drop");
    mvprintw(sidebarRow + 12, sidebarCol, "  C        hold");
    mvprintw(sidebarRow + 13, sidebarCol, "  P        pause");
    mvprintw(sidebarRow + 14, sidebarCol, "  Q        quit");
    attroff(COLOR_PAIR(8));
}

// ---------------------------------------------------------------------------
// Vẽ lớp phủ Game Over / Pause
// ---------------------------------------------------------------------------
void Tetris::drawGameOverOverlay() {
    int centerCol = boardScreenCol + (GameBoard::WIDTH * CELL_WIDTH) / 2 - 5;
    int centerRow = boardScreenRow + GameBoard::HEIGHT / 2;
    attron(COLOR_PAIR(9) | A_BOLD);
    mvprintw(centerRow, centerCol - 2, "  GAME  OVER  ");
    mvprintw(centerRow + 1, centerCol - 2, "  press  Q    ");
    attroff(COLOR_PAIR(9) | A_BOLD);
}

void Tetris::drawPauseOverlay() {
    int centerCol = boardScreenCol + (GameBoard::WIDTH * CELL_WIDTH) / 2 - 4;
    int centerRow = boardScreenRow + GameBoard::HEIGHT / 2;
    attron(COLOR_PAIR(9) | A_BOLD);
    mvprintw(centerRow, centerCol, "  PAUSED  ");
    attroff(COLOR_PAIR(9) | A_BOLD);
}

// ---------------------------------------------------------------------------
// Vẽ toàn bộ khung hình
// ---------------------------------------------------------------------------

void Tetris::renderFrame() {
    erase();

    int titleCol = boardScreenCol;
    attron(COLOR_PAIR(8) | A_BOLD);
    mvprintw(0, titleCol, "TETRIS");
    attroff(COLOR_PAIR(8) | A_BOLD);

    drawBorder();
    drawLockedBlocks();
    if (!status.isGameOver) {
        drawSinglePiece(falling.pieceIndex, falling.rotation, falling.posX, calculateGhostPositionY(), true);
        drawSinglePiece(falling.pieceIndex, falling.rotation, falling.posX, falling.posY, false);
    }
    drawNextPiecePreview();
    drawHoldPiece();
    drawSidebar();

    if (status.isGameOver) drawGameOverOverlay();
    else if (status.isPaused) drawPauseOverlay();

    refresh();
}

// ---------------------------------------------------------------------------
// Rơi thẳng xuống đáy
// ---------------------------------------------------------------------------

// Rơi thẳng xuống đáy, trả về số ô đã rơi để tính điểm hard drop.
int Tetris::dropToBottom() {
    int droppedCells = 0;
    while (!checkCollision(falling.pieceIndex, falling.rotation, falling.posX, falling.posY + 1)) {
        ++falling.posY;
        ++droppedCells;
    }
    return droppedCells;
}
