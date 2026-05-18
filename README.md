## Source code game Tetris của nhóm 10 - SS004.Q27 - UIT

### Thành viên trong nhóm

- 25521416: Cao Hoàng Phúc
- 25520509: Phan Huy Hào
- 25520534: Hồ Hoàng Hiệp
- 25521798: Trần Hoàng Thông
- 25521361: Lê Viết Phát

---

## Giới thiệu game

Đây là game **Tetris** được viết bằng ngôn ngữ **C++** và chạy trên màn hình console.

Trong game, các khối gạch sẽ rơi từ trên xuống. Người chơi cần di chuyển và xoay các khối gạch sao cho chúng xếp kín thành một hàng ngang. Khi một hàng được lấp đầy, hàng đó sẽ biến mất.

Mục tiêu của người chơi là cố gắng sống càng lâu càng tốt và không để các khối gạch chạm lên tới đỉnh màn hình.

---

## Hướng dẫn cài đặt và chạy

**1. Clone project:**

```bash
git clone https://github.com/jameskaois/25521416-25520509-25520534-25521798-25521361.git
cd 25521416-25520509-25520534-25521798-25521361
```

**2. Cài đặt môi trường:**

Đảm bảo máy tính của bạn chạy hệ điều hành Windows và đã cài đặt trình biên dịch C++ (ví dụ: MinGW để dùng lệnh g++).

**3. Biên dịch và thực thi:**

Mở terminal tại thư mục vừa clone về và chạy lệnh biên dịch (lưu ý phải có cờ `-lwinmm` để liên kết thư viện âm thanh):

```bash
g++ main.cpp Piece.cpp -o Tetris.exe -lwinmm
./Tetris.exe
```

---

## Cách chơi

Hãy tưởng tượng game giống như xếp gạch.

Các khối gạch sẽ rơi xuống từ phía trên.  
Bạn cần điều khiển chúng sang trái, sang phải, xoay khối gạch và cho chúng rơi xuống đúng vị trí.

Khi bạn xếp kín một hàng ngang, hàng đó sẽ được xóa.

Nếu gạch chất quá cao và không còn chỗ cho khối mới xuất hiện, game sẽ kết thúc.

---

## Phím điều khiển

| Phím                        | Chức năng                                         |
| --------------------------- | ------------------------------------------------- |
| `A` hoặc mũi tên trái       | Di chuyển khối gạch sang trái                     |
| `D` hoặc mũi tên phải       | Di chuyển khối gạch sang phải                     |
| `W` hoặc mũi tên lên        | Xoay khối gạch                                    |
| `S`, `X` hoặc mũi tên xuống | Cho khối gạch rơi nhanh hơn                       |
| `Space`                     | Thả khối gạch rơi xuống ngay lập tức              |
| `P`                         | Tạm dừng game, sau đó bấm phím bất kỳ để tiếp tục |
| `Q`                         | Thoát game                                        |

---

## Chế độ chơi

Game có 3 chế độ:

### Easy Mode

Chế độ dễ.  
Khối gạch rơi chậm hơn, phù hợp với người mới chơi.

### Normal Mode

Chế độ bình thường.  
Tốc độ vừa phải, phù hợp để chơi cơ bản.

### Hard Mode

Chế độ khó.  
Khối gạch rơi nhanh hơn. Ngoài ra, trong lúc chơi có thể xuất hiện thêm các hàng gạch rác ở phía dưới để làm game khó hơn.

---

## Hiệu ứng âm thanh

Game có hiệu ứng âm thanh cho một số hành động:

- Khi xoay khối gạch
- Khi khối gạch chạm đáy
- Khi người chơi xóa được hàng

Game có thư mục `sounds`, hãy đặt thư mục này cùng chỗ với file `.exe` để âm thanh hoạt động đúng.

Ví dụ:

```text
tetris.exe
sounds/
  rotate.wav
  land.wav
  clear_line.wav
```
