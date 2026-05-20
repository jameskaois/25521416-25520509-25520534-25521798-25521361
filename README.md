# Tetris Console C++ - Nhóm 10 (UIT)

## Thành viên nhóm (Đồ án Kỹ năng Nghề nghiệp SS004.Q27)

- 25521416: Cao Hoàng Phúc
- 25520509: Phan Huy Hào
- 25520534: Hồ Hoàng Hiệp
- 25521798: Trần Hoàng Thông
- 25521361: Lê Viết Phát

## Giới thiệu

Đây là tựa game **Tetris (Xếp gạch)** cổ điển được phát triển bằng ngôn ngữ **C++**, chạy trực tiếp trên môi trường Console của Windows. Game mang lại trải nghiệm mượt mà với nhiều tính năng hiện đại được tích hợp sẵn, giao diện màu sắc rõ ràng và hiệu ứng âm thanh sinh động.

Mục tiêu của bạn là sinh tồn lâu nhất có thể, dọn dẹp các hàng gạch và ghi được điểm số kỷ lục!

## Tính năng nổi bật

- **Bóng mờ (Ghost Piece):** Hiển thị trước vị trí rơi của khối gạch dưới đáy, giúp người chơi căn chỉnh và thả gạch chính xác.
- **Cơ chế Giữ (Hold Block):** Cho phép "cất" một khối gạch đang rơi vào kho chứa để dành sử dụng cho những tình huống nguy cấp (bấm phím `C`).
- **Hệ thống Điểm Chuỗi & Cấp độ:** Áp dụng luật tính điểm chuẩn, khuyến khích dọn dẹp nhiều hàng cùng lúc. Tự động tăng độ khó (Level) sau mỗi 10 hàng xóa được.
- **Lưu Kỷ Lục (High Score):** Điểm số cao nhất (Best Score) được ghi vào file `bestscore.txt` và lưu giữ qua những lần mở game sau.
- **3 Chế độ chơi đa dạng:** - _Easy / Normal:_ Phù hợp để giải trí và luyện tập.
  - _Hard:_ Thử thách thực sự với tốc độ rơi "chóng mặt" và cơ chế **Gạch rác (Garbage Lines)** liên tục đùn lên từ dưới đáy.
- **Âm thanh sống động:** Tích hợp hiệu ứng âm thanh khi xoay gạch, chạm đáy và ăn điểm.

## Hướng dẫn Cài đặt & Chạy game

**1. Clone mã nguồn:**
Mở Command Prompt và chạy lệnh sau:

```bash
git clone https://github.com/jameskaois/tetris-uit-nhom-10.git
cd tetris-uit-nhom-10
```

**2. Yêu cầu môi trường:**

- Máy tính chạy hệ điều hành **Windows**.
- Đã cài đặt trình biên dịch **G++** (thông qua MinGW).

**3. Biên dịch và Thực thi:**
Mở Command Prompt tại thư mục project vừa clone. **Bắt buộc** thêm cờ `-lwinmm` vào lệnh biên dịch để liên kết thư viện âm thanh của Windows:

```bash
g++ main.cpp Piece.cpp -o tetris.exe -lwinmm
```

Khởi chạy game bằng lệnh:

```bash
tetris.exe
```

## Phím điều khiển

| Phím      | Chức năng                                 |
| :-------- | :---------------------------------------- |
| `A` / `←` | Di chuyển khối gạch sang trái             |
| `D` / `→` | Di chuyển khối gạch sang phải             |
| `W` / `↑` | Xoay khối gạch 90 độ                      |
| `S` / `↓` | Rơi nhanh dần (Soft Drop)                 |
| `Space`   | Thả rơi chạm đáy ngay lập tức (Hard Drop) |
| `C`       | Giữ / Đổi khối gạch (Hold Block)          |
| `P`       | Tạm dừng game (Pause)                     |
| `Q`       | Thoát game (Quit)                         |

## Hệ thống Tính điểm & Level

Điểm số của bạn sẽ được nhân với hệ số **Level** hiện tại. Sống sót càng lâu, điểm thưởng càng lớn:

- Xóa 1 hàng: `100 x Level`
- Xóa 2 hàng: `300 x Level`
- Xóa 3 hàng: `500 x Level`
- Xóa 4 hàng: `800 x Level`

_(Cứ mỗi 10 hàng được xóa, bạn sẽ tăng 1 Level và gạch sẽ rơi nhanh hơn)._

## Cấu trúc thư mục (Lưu ý về Âm thanh)

Để game phát được âm thanh, hãy đảm bảo thư mục `sounds` (chứa các file `.wav`) luôn nằm cùng một chỗ với file thực thi `tetris.exe`. Cấu trúc chuẩn sẽ trông như sau:

```text
/tetris-uit-nhom-10
├── tetris.exe           <-- Chạy file này
├── bestscore.txt        <-- File tự sinh ra để lưu kỷ lục
└── sounds/              <-- Thư mục âm thanh (không được đổi tên)
    ├── rotate.wav
    ├── land.wav
    └── clear_line.wav
```
