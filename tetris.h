#ifndef TETRIS_H
#define TETRIS_H

#include <chrono>

/**
 * Tetris game engine dùng ncurses để vẽ lên terminal.
 *
 * Kiến trúc:
 *   - Bảng chơi (GameBoard): lưới 2D, giá trị 0 => ô trống, giá trị khác 0 => id màu của khối đã khóa.
 *   - Các khối (Piece): 7 tetromino, mỗi khối có 4 trạng thái xoay lưu trong lưới 4x4.
 *   - Vòng lặp chính (run()): đọc phím không chặn (non-blocking) + tự động rơi theo thời gian.
 */

// ---------------------------------------------------------------------------
// Cấu trúc dữ liệu cơ bản
// ---------------------------------------------------------------------------

// Lưới 4x4 mô tả 1 trạng thái xoay của khối Tetris.
// Giá trị 0 = ô trống, giá trị khác 0 = id màu (dùng cho COLOR_PAIR của ncurses).
struct RotationGrid {
    int cells[4][4];
};

// Định nghĩa 1 khối Tetris gồm 4 trạng thái xoay và màu sắc.
struct PieceDefinition {
    RotationGrid allRotations[4];  // 4 góc xoay: 0°, 90°, 180°, 270°
    int colorPairId;               // id cặp màu dùng khi vẽ
};

// Bảng chơi chính: lưới các ô đã khóa.
struct GameBoard {
    static constexpr int WIDTH = 10;   // Chiều rộng chuẩn của Tetris
    static constexpr int HEIGHT = 20;  // Chiều cao chuẩn của Tetris
    int cells[HEIGHT][WIDTH];          // 0 = trống, khác 0 = id màu khối đã khóa

    void clearAll();
    bool isInsideBoard(int col, int row) const;
    bool isCellOccupied(int col, int row) const;
};

// Thông tin khối đang rơi (active piece).
struct FallingPiece {
    int pieceIndex;    // Chỉ số khối trong bảng tra pieceDefinitions
    int rotation;      // 0..3
    int posX;          // Cột trái của hộp bao 4x4
    int posY;          // Hàng trên của hộp bao 4x4
};

// Trạng thái tổng quát của trò chơi.
struct GameStatus {
    int score = 0;            // Điểm số hiện tại
    int level = 1;            // Cấp độ hiện tại
    int totalLinesCleared = 0; // Tổng số hàng đã xóa
    bool isGameOver = false;
    bool isPaused = false;
};

// ---------------------------------------------------------------------------
// Class chính
// ---------------------------------------------------------------------------

class Tetris {
public:
    static constexpr int NUM_PIECES = 7;       // I, O, T, S, Z, J, L
    static constexpr int NUM_ROTATIONS = 4;    // 4 góc xoay
    static constexpr int GRID_SIZE = 4;        // Kích thước hộp bao 4x4
    static constexpr int CELL_WIDTH = 2;       // Mỗi ô vẽ rộng 2 ký tự để hình vuông

    Tetris();
    ~Tetris();

    // Vòng lặp chính. Chờ ngườichơi nhấn Q để thoát.
    void run();

private:
    // Bảng tra 7 khối Tetris.
    PieceDefinition pieceDefinitions[NUM_PIECES];

    GameBoard board;
    FallingPiece falling;
    int nextPieceIndex;       // Khối tiếp theo sẽ xuất hiện
    int holdPieceIndex;       // Khối đang được giữ (-1 nếu chưa có)
    bool hasHeldThisTurn;     // Đã hold trong lượt này chưa (chỉ 1 lần/lượt)
    GameStatus status;

    // Tọa độ trên màn hình terminal để vẽ bảng chơi (hàng, cột)
    int boardScreenRow;
    int boardScreenCol;

    // --- Logic trò chơi ---
    void initializePieces();           // Khởi tạo bảng tra pieceDefinitions
    void initializeColors();           // Thiết lập cặp màu ncurses
    void resetBoard();                 // Xóa trống bảng chơi
    void spawnNewPiece();              // Đưa khối next lên current, tạo next mới
    bool checkCollision(int pieceIndex, int rotation, int posX, int posY) const;
    void lockPieceToBoard();           // Ghi khối đang rơi vào bảng (khóa lại)
    int clearFullLines();              // Xóa các hàng đầy, trả về số hàng đã xóa
    void rotatePiece();                // Xoay khối + xử lý va chạm tường (wall kick)
    void movePieceLeft();
    void movePieceRight();
    bool tryMoveDownOneCell();         // Rơi xuống 1 ô, false nếu bị chặn
    int dropToBottom();                // Rơi thẳng xuống đáy, trả về số ô đã rơi
    int calculateGhostPositionY() const; // Tính vị trí Y của ghost piece (bóng đổ) nếu khối rơi thẳng xuống đáy
    int calculateScoreForLines(int numLines) const;
    bool isRowFull(int row) const;     // Kiểm tra 1 hàng đã đầy chưa
    void deleteRowAndShiftDown(int deletedRow); // Xóa hàng và dồn các hàng trên xuống
    void holdCurrentPiece();           // Lưu khối hiện tại, đổi lấy khối hold (nếu có)

    // --- Xử lý vòng lặp ---
    void processPlayerInput(int keyPressed, std::chrono::steady_clock::time_point& lastGravityDrop);
    bool updateGravity(std::chrono::steady_clock::time_point& lastGravityDrop);
    void lockPieceAndSpawnNext();      // Khóa khối, xóa hàng, sinh khối mới

    // --- Vẽ (Rendering) ---
    void renderFrame();                // Vẽ lại toàn bộ khung hình
    void drawBorder();                 // Viền quanh bảng chơi
    void drawLockedBlocks();           // Các khối đã khóa trên bảng
    void drawSinglePiece(int pieceIndex, int rotation, int posX, int posY, bool isGhost); // isGhost=true vẽ bóng đổ (::), false vẽ khối đặc (A_REVERSE)
    void drawNextPiecePreview();       // Khối tiếp theo ở sidebar
    void drawHoldPiece();              // Khối đang giữ ở sidebar
    void drawSidebar();                // Điểm, cấp độ, hướng dẫn điều khiển
    void drawGameOverOverlay();
    void drawPauseOverlay();

    // --- Thời gian ---
    int calculateGravityDelayMs() const; // Thờigian chờ giữa các lần tự rơi (ms). Công thức: max(50, 600 - (level-1)*50)
};

#endif
