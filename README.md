# uit-tetris

## Yêu cầu hệ thống

- Trình biên dịch C++ hỗ trợ C++17 (ví dụ: `g++`)
- Thư viện `ncurses` (dùng để vẽ giao diện terminal)

### Cài đặt ncurses

**macOS:**
```bash
brew install ncurses
```

**Windows (MSYS2):**
1. Cài đặt MSYS2 từ [https://www.msys2.org](https://www.msys2.org)
2. Mở MSYS2 UCRT64 terminal và chạy:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ncurses make
```

3. Biên dịch và chạy trong terminal MSYS2:
```bash
make
./tetris.exe
```

## Biên dịch và chạy

```bash
make        # Biên dịch chương trình
make run    # Biên dịch và chạy game
make clean  # Xóa file biên dịch
```

Hoặc biên dịch thủ công:
```bash
g++ -std=c++17 -Wall -Wextra -O2 -o tetris main.cpp tetris.cpp -lncurses
./tetris
```

## Điều khiển

| Phím | Chức năng |
|------|-----------|
| `A` / `←` | Di chuyển trái |
| `D` / `→` | Di chuyển phải |
| `W` / `↑` | Xoay khối |
| `S` / `↓` | Rơi nhanh (Soft Drop) |
| `Space` | Rơi tức thì (Hard Drop) |
| `Q` | Thoát game |
