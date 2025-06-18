# 🔥 Hệ thống Cảnh báo Cháy và Khí Gas sử dụng ESP32 + Firebase + LCD

## 📌 Giới thiệu

Dự án này là một **hệ thống giám sát môi trường thời gian thực** sử dụng vi điều khiển **ESP32**, với khả năng:

- Đo **nhiệt độ** và **độ ẩm** qua cảm biến **DHT11**  
- Phát hiện **khí gas** bằng cảm biến **MQ2**
- Nhận biết **ngọn lửa** qua cảm biến **lửa**
- Cảnh báo bằng **LED**, **còi buzzer**, và **LCD**
- Gửi dữ liệu lên **Firebase Realtime Database**
- Điều khiển LED từ Firebase (từ xa qua Internet)

---

## 🧠 Chức năng chính

- ✅ **Hiển thị nhiệt độ & độ ẩm** trên màn hình LCD I2C (16x2)
- ✅ **Cảnh báo khẩn cấp** khi phát hiện gas vượt ngưỡng hoặc có lửa
- ✅ **Bật tắt LED từ Firebase**
- ✅ **Ghi dữ liệu cảm biến lên Firebase liên tục**
- ✅ **Phản hồi trạng thái buzzer, LED, cảm biến về app/web**

---

## 🛠️ Phần cứng sử dụng

| Tên linh kiện         | Vai trò                        |
|------------------------|--------------------------------|
| ESP32 Dev Module       | Vi điều khiển chính             |
| Cảm biến DHT11         | Đo nhiệt độ và độ ẩm           |
| Cảm biến MQ2           | Phát hiện khí gas              |
| Cảm biến lửa           | Phát hiện ngọn lửa             |
| LCD 16x2 I2C           | Hiển thị thông tin tại chỗ     |
| Buzzer                 | Cảnh báo âm thanh              |
| 2 LED (LED1 & LED2)    | Cảnh báo trực quan             |
| Điện trở, dây nối, Breadboard, Nguồn |

---

## 🌐 Kết nối

- **WiFi SSID**: `DATN`  
- **Mật khẩu WiFi**: `12345678`
- **Firebase Realtime Database**:
  - `API_KEY`: `AIzaSyApE...` (đã có trong code)
  - `DATABASE_URL`: `https://datn-chinh-default-rtdb.firebaseio.com/`

---

## 🧩 Cấu trúc Firebase

```json
{
  "sensors": {
    "temperature": 30.5,
    "humidity": 70.2,
    "gasValue": 1024,
    "fireDetected": true,
    "gasAlert": true,
    "fireAlert": false
  },
  "control": {
    "led1": true,
    "led2": false
  }
}
🖥️ Cài đặt và sử dụng
1. Thư viện Arduino cần cài:
DHT sensor library

Firebase ESP32 by Mobizt

LiquidCrystal_I2C (phiên bản phù hợp)

2. Upload code
Mở bằng Arduino IDE

Chọn board ESP32 Dev Module

Cắm ESP32 và upload

🔔 Điều khiển từ Firebase
Bạn có thể:

Bật/Tắt LED từ xa bằng cách chỉnh control/led1 hoặc control/led2 trên Firebase Console.

Giám sát dữ liệu thời gian thực (nhiệt độ, độ ẩm, gas, lửa, còi) qua đường dẫn sensors/.


📖 Ghi chú thêm
Buzzer dùng logic LOW để kêu, HIGH để tắt (đã xử lý trong code).

Firebase được tự động kết nối lại khi mất WiFi (Firebase.reconnectWiFi(true)).

LCD sẽ liên tục hiển thị nhiệt độ và độ ẩm.

Bạn có thể tích hợp thêm với ứng dụng web hoặc Android để điều khiển tiện lợi hơn.