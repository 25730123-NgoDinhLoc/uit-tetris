#include "tetris.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;

// Constructor: Initialize the game state and SFML window
Tetris::Tetris() 
    : x(5), y(1), b(0), rot(0), next(0), score(0), lines(0), level(1), over(false),
      window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetris - SFML Edition") {
    
    srand(static_cast<unsigned int>(time(0)));
    
    // Configure window
    window.setFramerateLimit(60);
    
    // Initialize game data
    initBlocks();
    initBoard();
    b = rand() % P;
    next = rand() % P;
}

void Tetris::initBlocks() {
    // Clear all block patterns first
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
    // Initialize board with borders and empty spaces
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            board[i][j] = (i == 0 || i == H - 1 || j == 0 || j == W - 1) ? '#' : ' ';
}

void Tetris::spawn() {
    // Reset position and rotation for new piece
    x = 5;
    y = 1;
    rot = 0;
    b = next;
    next = rand() % P;
    
    // Game over if new piece collides immediately
    if (collides(b, rot, x, y))
        over = true;
}

char Tetris::getCell(int piece, int rotation, int r, int c) const {
    // Return the cell of `piece` at a given `rotation`.
    // On-the-fly 90-degree clockwise transformations:
    //   0: (r, c)         - no rotation
    //   1: (S-1-c, r)     - rotated 90 degrees clockwise
    //   2: (S-1-r, S-1-c) - rotated 180 degrees
    //   3: (c, S-1-r)     - rotated 270 degrees clockwise
    
    switch (rotation % 4) {
    case 0: return blocks[piece][r][c];
    case 1: return blocks[piece][S - 1 - c][r];
    case 2: return blocks[piece][S - 1 - r][S - 1 - c];
    case 3: return blocks[piece][c][S - 1 - r];
    }
    return ' ';
}

bool Tetris::collides(int piece, int rotation, int px, int py) const {
    // Check if piece at (px, py) with given rotation collides with board
    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < S; ++j) {
            if (getCell(piece, rotation, i, j) != ' ') {
                int xx = px + j;
                int yy = py + i;
                
                // Check boundaries and occupied cells
                if (xx < 0 || xx >= W || yy < 0 || yy >= H || board[yy][xx] != ' ')
                    return true;
            }
        }
    }
    return false;
}

bool Tetris::canMove(int dx, int dy) const {
    // Check if current piece can move by (dx, dy)
    return !collides(b, rot, x + dx, y + dy);
}

void Tetris::rotate() {
    // Try to rotate piece; only rotate if no collision
    int nextRot = (rot + 1) % 4;
    if (!collides(b, nextRot, x, y))
        rot = nextRot;
}

void Tetris::lock() {
    // Place current piece on the board permanently
    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < S; ++j) {
            char cell = getCell(b, rot, i, j);
            if (cell != ' ')
                board[y + i][x + j] = cell;
        }
    }
}

void Tetris::clearLines() {
    // Find and clear complete rows
    int cleared = 0;
    
    for (int row = H - 2; row > 0; --row) {
        // Check if row is complete
        bool full = true;
        for (int col = 1; col < W - 1; ++col) {
            if (board[row][col] == ' ') {
                full = false;
                break;
            }
        }

        if (full) {
            ++cleared;
            
            // Shift rows down
            for (int r = row; r > 1; --r) {
                for (int c = 1; c < W - 1; ++c)
                    board[r][c] = board[r - 1][c];
            }
            
            // Clear top row
            for (int c = 1; c < W - 1; ++c)
                board[1][c] = ' ';
            
            ++row; // re-check this row
        }
    }
    
    // Update score and level
    if (cleared) {
        lines += cleared;
        // Scoring: 40, 100, 300, 1200 points per line cleared
        int pts[] = { 0, 40, 100, 300, 1200 };
        score += pts[cleared] * level;
        level = 1 + lines / 10;
    }
}

sf::Color Tetris::getPieceColor(char piece) const {
    // Return SFML color for each piece type
    switch (piece) {
    case 'I': return sf::Color::Cyan;     // I piece
    case 'O': return sf::Color::Yellow;   // O piece
    case 'T': return sf::Color::Magenta;  // T piece
    case 'S': return sf::Color::Green;    // S piece
    case 'Z': return sf::Color::Red;      // Z piece
    case 'J': return sf::Color::Blue;     // J piece
    case 'L': return sf::Color::White;    // L piece
    case '#': return sf::Color::Black;    // Border
    default:  return sf::Color::Black;    // Empty
    }
}

void Tetris::drawBoard() {
    // Draw the game board grid
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            char cell = board[i][j];
            if (cell != ' ') {
                // Create rectangle for filled cell
                sf::RectangleShape cellRect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                cellRect.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                cellRect.setFillColor(getPieceColor(cell));
                window.draw(cellRect);
            } else {
                // Draw grid lines for empty cells
                sf::RectangleShape gridCell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                gridCell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                gridCell.setFillColor(sf::Color::Black);
                gridCell.setOutlineColor(sf::Color(50, 50, 50));
                gridCell.setOutlineThickness(1.0f);
                window.draw(gridCell);
            }
        }
    }
}

void Tetris::drawActivePiece() {
    // Draw the current falling piece
    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < S; ++j) {
            char cell = getCell(b, rot, i, j);
            if (cell != ' ') {
                int boardX = x + j;
                int boardY = y + i;
                
                sf::RectangleShape cellRect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                cellRect.setPosition(boardX * CELL_SIZE, boardY * CELL_SIZE);
                cellRect.setFillColor(getPieceColor(cell));
                window.draw(cellRect);
            }
        }
    }
}

void Tetris::drawSidebar() {
    // Draw game information on the right side
    int sidebarX = W * CELL_SIZE + 20;
    
    // Create simple text labels using ASCII (or actual sf::Text if font is available)
    // For now, we'll use simple colored rectangles and text
    
    sf::Text scoreText;
    scoreText.setPosition(sidebarX, 20);
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: " + to_string(score));
    window.draw(scoreText);
    
    sf::Text linesText;
    linesText.setPosition(sidebarX, 50);
    linesText.setCharacterSize(16);
    linesText.setFillColor(sf::Color::White);
    linesText.setString("Lines: " + to_string(lines));
    window.draw(linesText);
    
    sf::Text levelText;
    levelText.setPosition(sidebarX, 80);
    levelText.setCharacterSize(16);
    levelText.setFillColor(sf::Color::White);
    levelText.setString("Level: " + to_string(level));
    window.draw(levelText);
    
    // Draw "Next Piece" label
    sf::Text nextLabel;
    nextLabel.setPosition(sidebarX, 130);
    nextLabel.setCharacterSize(14);
    nextLabel.setFillColor(sf::Color::Yellow);
    nextLabel.setString("Next:");
    window.draw(nextLabel);
    
    // Draw next piece preview
    for (int i = 0; i < S; ++i) {
        for (int j = 0; j < S; ++j) {
            char cell = blocks[next][i][j];
            if (cell != ' ') {
                sf::RectangleShape cellRect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                cellRect.setPosition(sidebarX + j * CELL_SIZE, 150 + i * CELL_SIZE);
                cellRect.setFillColor(getPieceColor(cell));
                window.draw(cellRect);
            }
        }
    }
    
    // Draw controls
    sf::Text controlsLabel;
    controlsLabel.setPosition(sidebarX, 320);
    controlsLabel.setCharacterSize(14);
    controlsLabel.setFillColor(sf::Color::Cyan);
    controlsLabel.setString("Controls:");
    window.draw(controlsLabel);
    
    vector<string> controls = {
        "A/D    : Move",
        "W      : Rotate",
        "S      : Drop",
        "Space  : Hard Drop",
        "Q      : Quit"
    };
    
    for (size_t i = 0; i < controls.size(); ++i) {
        sf::Text control;
        control.setPosition(sidebarX, 350 + i * 25);
        control.setCharacterSize(12);
        control.setFillColor(sf::Color::White);
        control.setString(controls[i]);
        window.draw(control);
    }
    
    // Draw game over message
    if (over) {
        sf::RectangleShape gameOverBg(sf::Vector2f(200, 100));
        gameOverBg.setPosition(sidebarX - 100, WINDOW_HEIGHT / 2 - 50);
        gameOverBg.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(gameOverBg);
        
        sf::Text gameOverText;
        gameOverText.setPosition(sidebarX - 80, WINDOW_HEIGHT / 2 - 20);
        gameOverText.setCharacterSize(24);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER");
        window.draw(gameOverText);
    }
}

void Tetris::drawGame() {
    // Clear window with black background
    window.clear(sf::Color::Black);
    
    // Draw all game components
    drawBoard();
    drawActivePiece();
    drawSidebar();
    
    // Display rendered frame
    window.display();
}

void Tetris::run() {
    // Main game loop
    auto lastDrop = chrono::steady_clock::now();
    int dropMs = 500; // Initial drop speed in milliseconds
    
    while (window.isOpen() && !over) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Q:
                    window.close();
                    return;
                    
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    // Move left
                    if (canMove(-1, 0))
                        --x;
                    break;
                    
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    // Move right
                    if (canMove(1, 0))
                        ++x;
                    break;
                    
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    // Rotate piece
                    rotate();
                    break;
                    
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    // Soft drop (move down)
                    if (canMove(0, 1))
                        ++y;
                    break;
                    
                case sf::Keyboard::Space:
                    // Hard drop (move to bottom)
                    while (canMove(0, 1))
                        ++y;
                    lock();
                    clearLines();
                    spawn();
                    break;
                    
                default:
                    break;
                }
            }
        }
        
        // Automatic drop based on level speed
        auto now = chrono::steady_clock::now();
        int elapsedMs = chrono::duration_cast<chrono::milliseconds>(now - lastDrop).count();
        
        if (elapsedMs > dropMs) {
            if (canMove(0, 1)) {
                // Move piece down
                ++y;
            } else {
                // Piece can't move down - lock it and spawn new piece
                lock();
                clearLines();
                spawn();
            }
            
            lastDrop = now;
            // Increase speed with level (minimum 50ms)
            dropMs = max(50, 500 - (level - 1) * 50);
        }
        
        // Render the game
        drawGame();
    }
    
    // Window closed or game over - exit the loop
}

