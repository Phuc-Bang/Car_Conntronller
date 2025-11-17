# XE ROBOT - 2 CHẾ ĐỘ

Dự án xe robot điều khiển bằng Arduino với 2 chế độ: **Điều khiển Bluetooth** và **Tự động tránh vật cản**.

## 📋 Mục lục

- [Tổng quan](#tổng-quan)
- [Phần cứng cần thiết](#phần-cứng-cần-thiết)
- [Sơ đồ kết nối](#sơ-đồ-kết-nối)
- [Cài đặt](#cài-đặt)
- [Hướng dẫn sử dụng](#hướng-dẫn-sử-dụng)
- [Các chế độ hoạt động](#các-chế-độ-hoạt-động)
- [Lệnh điều khiển](#lệnh-điều-khiển)
- [Cấu hình](#cấu-hình)
- [Giải thích code](#giải-thích-code)

## 🎯 Tổng quan

Xe robot này được thiết kế với 2 chế độ hoạt động chính:

1. **Chế độ 1 - Điều khiển Bluetooth**: Người dùng điều khiển xe qua Bluetooth bằng các lệnh đơn giản
2. **Chế độ 2 - Tự động**: Xe tự động di chuyển và tránh vật cản sử dụng cảm biến siêu âm và cảm biến hồng ngoại

## 🔧 Phần cứng cần thiết

### Linh kiện chính:

- **Arduino Uno** (hoặc tương thích)
- **Module L298N** - Driver điều khiển động cơ DC
- **2 x Động cơ DC** - Động cơ bánh xe
- **Cảm biến siêu âm HC-SR04** - Phát hiện vật cản phía trước
- **2 x Cảm biến hồng ngoại (MH Sensor)** - Phát hiện vật cản ở góc
- **Servo SG90** - Quay cảm biến siêu âm để quét môi trường
- **Module Bluetooth HC-05/HC-06** - Kết nối không dây
- **Nguồn cấp** - Pin hoặc adapter 5V-12V

### Phụ kiện:

- Dây nối, breadboard
- Khung xe robot
- Bánh xe và bánh đỡ

## 🔌 Sơ đồ kết nối

### Động cơ (L298N):

```
ENA  → Pin 6  (PWM - Bánh trái)
ENB  → Pin 11 (PWM - Bánh phải)
IN1  → Pin 2  (Chiều Motor A)
IN2  → Pin 3  (Chiều Motor A)
IN3  → Pin 4  (Chiều Motor B)
IN4  → Pin 5  (Chiều Motor B)
```

### Cảm biến siêu âm (HC-SR04):

```
TRIG → Pin A0
ECHO → Pin A1
VCC  → 5V
GND  → GND
```

### Cảm biến hồng ngoại:

```
IR_LEFT  → Pin A2
IR_RIGHT → Pin A3
VCC      → 5V
GND      → GND
```

### Servo (SG90):

```
Signal → Pin 13
VCC    → 5V
GND    → GND
```

### Bluetooth:

```
TX → Pin 0 (RX của Arduino)
RX → Pin 1 (TX của Arduino)
VCC → 5V
GND → GND
```

## 📥 Cài đặt

1. **Cài đặt Arduino IDE**

   - Tải và cài đặt [Arduino IDE](https://www.arduino.cc/en/software)

2. **Cài đặt thư viện**

   - Mở Arduino IDE
   - Vào `Sketch` → `Include Library` → `Manage Libraries`
   - Tìm và cài đặt thư viện `Servo`

3. **Nạp code**
   - Kết nối Arduino với máy tính qua cáp USB
   - Mở file `v1.ino` trong Arduino IDE
   - Chọn board: `Tools` → `Board` → `Arduino Uno`
   - Chọn cổng COM: `Tools` → `Port` → Chọn cổng của Arduino
   - Nhấn nút Upload (→) để nạp code

## 🎮 Hướng dẫn sử dụng

### Bước 1: Kết nối phần cứng

- Kết nối tất cả linh kiện theo sơ đồ ở trên
- Kiểm tra lại các kết nối trước khi bật nguồn

### Bước 2: Kết nối Bluetooth

- Bật Bluetooth trên điện thoại/máy tính
- Kết nối với module Bluetooth (mặc định: HC-05/HC-06)
- Mở ứng dụng Serial Bluetooth Terminal hoặc ứng dụng điều khiển tương tự

### Bước 3: Điều khiển xe

- Xe khởi động ở **Chế độ 1 (Điều khiển Bluetooth)**
- Gửi các lệnh qua Bluetooth để điều khiển xe (xem phần [Lệnh điều khiển](#lệnh-điều-khiển))

## 🚗 Các chế độ hoạt động

### Chế độ 1: Điều khiển Bluetooth

- Người dùng điều khiển xe thủ công qua Bluetooth
- Hỗ trợ các lệnh: Tiến, Lùi, Rẽ trái, Rẽ phải, Dừng
- Có thể điều chỉnh tốc độ (Vừa/Tối đa)

### Chế độ 2: Tự động tránh vật cản

- Xe tự động di chuyển và tránh vật cản
- Sử dụng cảm biến hồng ngoại để phát hiện vật cản ở góc (ưu tiên cao)
- Sử dụng cảm biến siêu âm để phát hiện vật cản phía trước
- Servo quay để quét môi trường và quyết định hướng di chuyển
- Tốc độ tự động được giảm xuống 150 để an toàn
- Có thể ngắt khẩn cấp bằng cách gửi lệnh '1' qua Bluetooth

## ⌨️ Lệnh điều khiển

### Chuyển đổi chế độ:

- `1` - Chuyển sang **Chế độ Điều khiển Bluetooth**
- `2` - Chuyển sang **Chế độ Tự động**

### Điều khiển di chuyển (Chế độ 1):

- `F` - **Tiến** (Forward)
- `B` - **Lùi** (Backward)
- `L` - **Rẽ trái** (Turn Left)
- `R` - **Rẽ phải** (Turn Right)
- `S` - **Dừng** (Stop)

### Điều chỉnh tốc độ (Chế độ 1):

- `3` - Tốc độ **Vừa** (150)
- `4` - Tốc độ **Tối đa** (255)

## ⚙️ Cấu hình

Các thông số có thể điều chỉnh trong code:

```cpp
const int SPEED_NORMAL = 150;        // Tốc độ bình thường
const int SPEED_MAX = 255;           // Tốc độ tối đa
const int SPEED_TURN = 200;          // Tốc độ khi rẽ
const int AVOID_DISTANCE = 20;       // Khoảng cách phát hiện vật cản (cm)
const int BACKWARD_DURATION = 400;   // Thời gian lùi (ms)
const int TURN_DURATION = 400;       // Thời gian rẽ (ms)
const int SCAN_DELAY = 300;          // Thời gian chờ servo quay (ms)
const int SERVO_CENTER = 90;         // Góc giữa của servo
const int SERVO_LEFT = 150;          // Góc trái của servo
const int SERVO_RIGHT = 30;          // Góc phải của servo
```

## 📖 Giải thích code

### Cấu trúc chương trình:

1. **Phần 1: Cấu hình chân kết nối**

   - Định nghĩa các chân kết nối cho động cơ, cảm biến, servo

2. **Phần 2: Hằng số cấu hình**

   - Các thông số tốc độ, thời gian, khoảng cách

3. **Phần 3: Biến toàn cục**

   - Biến lưu trạng thái tốc độ, chế độ hiện tại

4. **Phần 4: Hàm setup()**

   - Khởi tạo các chân I/O, servo, Serial

5. **Phần 5: Hàm loop()**

   - Vòng lặp chính, chuyển đổi giữa 2 chế độ

6. **Phần 6: Xử lý lệnh Bluetooth**

   - Nhận và xử lý lệnh từ Bluetooth

7. **Phần 7: Chế độ tự động**

   - Logic tránh vật cản với ưu tiên:
     - **Ưu tiên 1**: Cảm biến IR (vật cản góc)
     - **Ưu tiên 2**: Cảm biến siêu âm (vật cản phía trước)
   - Quét môi trường bằng servo
   - Quyết định hướng di chuyển

8. **Phần 8: Hàm điều khiển động cơ**

   - `forward()` - Tiến
   - `backward()` - Lùi
   - `turnLeft()` - Rẽ trái
   - `turnRight()` - Rẽ phải
   - `stopCar()` - Dừng

9. **Phần 9: Hàm lấy khoảng cách**
   - `getDistance()` - Đọc giá trị từ cảm biến siêu âm HC-SR04

### Logic tránh vật cản:

1. **Kiểm tra cảm biến IR** (ưu tiên cao):

   - Nếu phát hiện vật cản → Dừng → Lùi → Rẽ theo hướng an toàn

2. **Kiểm tra cảm biến siêu âm**:

   - Nếu phát hiện vật cản < 20cm → Dừng → Lùi
   - Quét trái/phải bằng servo
   - So sánh khoảng cách và chọn hướng an toàn nhất
   - Nếu cả 2 bên đều bị chặn → Lùi và rẽ trái

3. **Nếu đường trống**:
   - Tiến thẳng

## ⚠️ Lưu ý

- Đảm bảo nguồn cung cấp đủ công suất cho động cơ
- Kiểm tra kết nối trước khi sử dụng
- Trong chế độ tự động, có thể ngắt khẩn cấp bằng lệnh '1'
- Cảm biến IR: `LOW` = có vật cản, `HIGH` = không có vật cản
- Điều chỉnh `AVOID_DISTANCE` phù hợp với môi trường sử dụng

## 🐛 Xử lý lỗi

- **Xe không di chuyển**: Kiểm tra kết nối động cơ và nguồn
- **Bluetooth không kết nối**: Kiểm tra module Bluetooth và cổng Serial
- **Cảm biến không hoạt động**: Kiểm tra kết nối và nguồn cấp
- **Servo không quay**: Kiểm tra kết nối và nguồn 5V

## 📝 License

Dự án mã nguồn mở, tự do sử dụng và chỉnh sửa.

## 👤 Tác giả

Dự án được phát triển cho mục đích học tập và nghiên cứu.

---

**Chúc bạn thành công với dự án!** 🚀
