/*
 * ═══════════════════════════════════════════════════════════════════════
 * Tên dự án: XE ROBOT - 2 CHẾ ĐỘ
 * ═══════════════════════════════════════════════════════════════════════
 */

// --- THƯ VIỆN ---
#include <Servo.h>

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 1: CẤU HÌNH CHÂN KẾT NỐI
// ═══════════════════════════════════════════════════════════════════════

// --- ĐỘNG CƠ (L298N Motor Driver) ---
#define ENA 6         // PWM Motor A (bánh trái)
#define ENB 11        // PWM Motor B (bánh phải)
#define IN1 2         // Chiều Motor A
#define IN2 3         // Chiều Motor A
#define IN3 4         // Chiều Motor B
#define IN4 5         // Chiều Motor B

// --- CẢM BIẾN SIÊU ÂM (HC-SR04) ---
#define TRIG_PIN A0   // Chân TRIG
#define ECHO_PIN A1   // Chân ECHO

// --- CẢM BIẾN HỒNG NGOẠI (MH SENSOR) ---
#define IR_LEFT A2    // Cảm biến IR bên trái
#define IR_RIGHT A3   // Cảm biến IR bên phải

// --- SERVO (SG90) ---
#define SERVO_PIN 13  // Chân tín hiệu của Servo

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 2: HẰNG SỐ CẤU HÌNH
// ═══════════════════════════════════════════════════════════════════════
const int SPEED_NORMAL = 150;
const int SPEED_MAX = 255;
const int SPEED_TURN = 200;
const int AVOID_DISTANCE = 20;    // Khoảng cách siêu âm phát hiện vật cản (20cm)
const int BACKWARD_DURATION = 400; // Thời gian lùi (400ms)
const int TURN_DURATION = 400;     // Thời gian rẽ (400ms)
const int SCAN_DELAY = 300;        // Thời gian chờ servo quay
const int SERVO_CENTER = 90;
const int SERVO_LEFT = 150;
const int SERVO_RIGHT = 30;

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 3: BIẾN TOÀN CỤC
// ═══════════════════════════════════════════════════════════════════════
int speed = SPEED_NORMAL;
int currentMode = 1; // 1 = Điều khiển BT, 2 = Tự động
Servo myServo;

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 4: HÀM SETUP - KHỞI TẠO HỆ THỐNG
// ═══════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(9600);
  
  // Cấu hình chân động cơ
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Cấu hình chân cảm biến siêu âm
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Cấu hình chân cảm biến hồng ngoại
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // Gắn Servo vào chân đã định nghĩa
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_CENTER); // Đưa servo về giữa khi khởi động
  delay(500); // Chờ servo ổn định
  
  Serial.println(F("Xe da san sang. Dang cho lenh..."));
  Serial.println(F("Che do mac dinh: DIEU KHIEN (1)"));
  
  stopCar();
}

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 5: HÀM LOOP - VÒNG LẶP CHÍNH
// ═══════════════════════════════════════════════════════════════════════
void loop() {
  if (currentMode == 1) {
    handleBluetoothControl();
  } else {
    autonomousMode();
  }
}

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 6: XỬ LÝ LỆNH BLUETOOTH
// ═══════════════════════════════════════════════════════════════════════
void handleBluetoothControl() {
  if (Serial.available()) {
    char c = Serial.read();

    Serial.print(F("Nhan lenh: "));
    Serial.println(c);

    switch (c) {
      case '1':
        currentMode = 1;
        stopCar();
        Serial.println(F("  -> Da chuyen sang CHE DO DIEU KHIEN"));
        break;
        
      case '2':
        currentMode = 2;
        speed = SPEED_NORMAL; // **Giảm tốc độ khi vào chế độ tự động**
        myServo.write(SERVO_CENTER); // Đảm bảo servo về giữa
        stopCar();
        Serial.println(F("  -> Da chuyen sang CHE DO TU DONG (Toc do 150)"));
        break;

      case 'F': forward(); Serial.println(F("  -> Trang thai: TIEN")); break;
      case 'B': backward(); Serial.println(F("  -> Trang thai: LUI")); break;
      case 'L': turnLeft(); Serial.println(F("  -> Trang thai: RE TRAI")); break;
      case 'R': turnRight(); Serial.println(F("  -> Trang thai: RE PHAI")); break;
      case 'S': stopCar(); Serial.println(F("  -> Trang thai: DUNG")); break;
        
      case '3':
        speed = SPEED_NORMAL;
        Serial.println(F("  -> Toc do: VUA (150)"));
        break;
        
      case '4':
        speed = SPEED_MAX;
        Serial.println(F("  -> Toc do: TOI DA (255)"));
        break;
        
      default:
        Serial.println(F("  -> Lenh khong hop le."));
        break;
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 7: CHẾ ĐỘ LÁI TỰ ĐỘNG (LOGIC MỚI VỚI IR)
// ═══════════════════════════════════════════════════════════════════════
void autonomousMode() {
  // 1. Kiểm tra ngắt khẩn cấp (nếu người dùng gửi '1')
  if (checkInterrupt(0)) return;

  // 2. ƯU TIÊN 1: Kiểm tra Cảm biến IR (vật cản cực gần ở góc)
  // (Giả định: LOW = có vật cản, HIGH = không có)
  bool left_IR_blocked = (digitalRead(IR_LEFT) == LOW);
  bool right_IR_blocked = (digitalRead(IR_RIGHT) == LOW);

  if (left_IR_blocked || right_IR_blocked) {
    Serial.println(F("  -> (IR) Vat can GOC! Dung lai và lui..."));
    stopCar();
    backward();
    if (checkInterrupt(BACKWARD_DURATION)) return; // Lùi (cho phép ngắt)

    if (left_IR_blocked) {
      // Trái vướng -> Rẽ phải
      Serial.println(F("  -> (IR) Goc trai vuong, re PHAI..."));
      turnRight();
      if (checkInterrupt(TURN_DURATION)) return;
    } else {
      // Phải vướng -> Rẽ trái
      Serial.println(F("  -> (IR) Goc phai vuong, re TRAI..."));
      turnLeft();
      if (checkInterrupt(TURN_DURATION)) return;
    }
    stopCar();
    return; // Kết thúc vòng lặp này, bắt đầu lại
  }

  // 3. ƯU TIÊN 2: Kiểm tra Cảm biến Siêu âm (vật cản phía trước)
  // (Chỉ chạy nếu 2 cảm biến IR đều thoáng)
  
  int distance_front = getDistance();
  Serial.print(F("  -> (US) Phia truoc: "));
  Serial.print(distance_front);
  Serial.println(F(" cm"));

  if (distance_front < AVOID_DISTANCE) { // Phát hiện vật cản dưới 20cm
    Serial.println(F("  -> (US) Vat can phia truoc! Dung lai và lui..."));
    stopCar();
    backward();
    if (checkInterrupt(BACKWARD_DURATION)) return; // Lùi (cho phép ngắt)
    
    stopCar();
    Serial.println(F("  -> (US) Dang quet..."));

    // Bắt đầu quét
    myServo.write(SERVO_RIGHT);
    if (checkInterrupt(SCAN_DELAY)) return;
    int distance_right = getDistance();
    Serial.print(F("  -> (US) Khoang cach PHAI: ")); Serial.println(distance_right);

    myServo.write(SERVO_LEFT);
    if (checkInterrupt(SCAN_DELAY * 2)) return;
    int distance_left = getDistance();
    Serial.print(F("  -> (US) Khoang cach TRAI: ")); Serial.println(distance_left);

    myServo.write(SERVO_CENTER);
    if (checkInterrupt(SCAN_DELAY)) return;

    // Ra quyết định
    bool left_scan_blocked = (distance_left < AVOID_DISTANCE);
    bool right_scan_blocked = (distance_right < AVOID_DISTANCE);

    if (left_scan_blocked && right_scan_blocked) {
      Serial.println(F("  -> (US) BI DUONG! Lui va re trai..."));
      backward();
      if (checkInterrupt(BACKWARD_DURATION)) return;
      turnLeft();
      if (checkInterrupt(TURN_DURATION)) return;
      
    } else if (left_scan_blocked) {
      Serial.println(F("  -> (US) Trai vuong, re PHAI..."));
      turnRight();
      if (checkInterrupt(TURN_DURATION)) return;
      
    } else if (right_scan_blocked) {
      Serial.println(F("  -> (US) Phai vuong, re TRAI..."));
      turnLeft();
      if (checkInterrupt(TURN_DURATION)) return;
      
    } else {
      Serial.println(F("  -> (US) Thoang. Uu tien re TRAI..."));
      turnLeft();
      if (checkInterrupt(TURN_DURATION)) return;
    }

  } else {
    // Đường trống -> Cứ đi thẳng
    forward();
  }
  
  // Thêm một khoảng trễ nhỏ để không spam Serial
  if (checkInterrupt(50)) return;
}

// Hàm phụ để kiểm tra ngắt từ Bluetooth
bool checkInterrupt(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '1') {
        currentMode = 1; 
        stopCar();
        myServo.write(SERVO_CENTER);
        Serial.println(F("  -> DA NGAT! Chuyen ve CHE DO DIEU KHIEN"));
        return true;
      }
    }
  }
  return false;
}


// ═══════════════════════════════════════════════════════════════════════
// PHẦN 8: CÁC HÀM ĐIỀU KHIỂN ĐỘNG CƠ
// ═══════════════════════════════════════════════════════════════════════

void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, SPEED_NORMAL); 
  analogWrite(ENB, SPEED_NORMAL);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, SPEED_TURN);
  analogWrite(ENB, SPEED_TURN);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, SPEED_TURN);
  analogWrite(ENB, SPEED_TURN);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ═══════════════════════════════════════════════════════════════════════
// PHẦN 9: HÀM LẤY KHOẢNG CÁCH (HC-SR04)
// ═══════════════════════════════════════════════════════════════════════
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 

  int distance = duration * 0.034 / 2;

  if (distance == 0) {
    return 999; // Lỗi hoặc quá xa
  }
  
  return distance;
}