#include <DHT.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <LiquidCrystal_I2C.h>
#include <addons/TokenHelper.h>  
#include <addons/RTDBHelper.h>

// -------------------- Cấu hình WiFi --------------------
#define WIFI_SSID "DATN"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyApE5SkRt8Mk_B6I1vqIVwt53PutmLYK88"
#define DATABASE_URL "https://datn-chinh-default-rtdb.firebaseio.com/"

// -------------------- Cấu hình Firebase --------------------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -------------------- Cấu hình cảm biến --------------------
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ2PIN 32
#define FIRE_SENSOR_PIN 4

#define LED1_PIN 23
#define LED2_PIN 14
#define BUZZER_PIN 17

// -------------------- Cấu hình LCD --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C và kích thước màn hình LCD (16x2)

// Ngưỡng khí gas
#define GAS_THRESHOLD 1500

// Biến trạng thái
bool gasAlert = false;
bool fireAlert = false;
bool led1State = LOW;
bool led2State = LOW;
bool buzzerState = LOW;

// -------------------- Hàm kết nối WiFi --------------------
void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());
}

// -------------------- Cấu hình Firebase --------------------
void setupFirebase() {
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback;

    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase signup successful!");
    } else {
        Serial.printf("Signup error: %s\n", config.signer.signupError.message.c_str());
    }

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Firebase initialized.");
}

// -------------------- Gửi dữ liệu lên Firebase --------------------
void sendDataToFirebase(float temperature, float humidity, int gasValue, bool fireDetected) {
    if (!Firebase.ready()) {
        Serial.println("Firebase not ready!");
        return;
    }

    Firebase.setFloat(fbdo, "/sensors/temperature", temperature);
    Firebase.setFloat(fbdo, "/sensors/humidity", humidity);
    Firebase.setInt(fbdo, "/sensors/gasValue", gasValue);
    Firebase.setBool(fbdo, "/sensors/fireDetected", fireDetected);
    Firebase.setBool(fbdo, "/sensors/gasAlert", gasAlert);
    Firebase.setBool(fbdo, "/sensors/fireAlert", fireAlert);

    // Cập nhật trạng thái LED
    Firebase.setBool(fbdo, "/control/led1", led1State);
    Firebase.setBool(fbdo, "/control/led2", led2State);
}

// -------------------- Đọc trạng thái từ Firebase --------------------
void updateFromFirebase() {
    if (!Firebase.ready()) {
        Serial.println("Firebase not ready!");
        return;
    }

    // Đọc trạng thái LED1 từ Firebase
    if (Firebase.getBool(fbdo, "/control/led1")) {
        bool newLed1State = fbdo.boolData();
        if (newLed1State != led1State) {
            led1State = newLed1State;
            digitalWrite(LED1_PIN, led1State ? HIGH : LOW);
            Serial.printf("LED1 set to %s from Firebase\n", led1State ? "ON" : "OFF");
        }
    }

    // Đọc trạng thái LED2 từ Firebase
    if (Firebase.getBool(fbdo, "/control/led2")) {
        bool newLed2State = fbdo.boolData();
        if (newLed2State != led2State) {
            led2State = newLed2State;
            digitalWrite(LED2_PIN, led2State ? HIGH : LOW);
            Serial.printf("LED2 set to %s from Firebase\n", led2State ? "ON" : "OFF");
        }
    }
}

// -------------------- Hiển thị lên LCD --------------------
void displayOnLCD(float temperature, float humidity) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Humi: ");
    lcd.print(humidity);
    lcd.print("%");
}

// -------------------- Điều khiển còi --------------------
void controlBuzzer(bool enable) {
    if (enable && !buzzerState) {
        buzzerState = true;
        digitalWrite(BUZZER_PIN, LOW); // Bật còi
    } else if (!enable && buzzerState) {
        buzzerState = false;
        digitalWrite(BUZZER_PIN, HIGH); // Tắt còi
    }
}

// -------------------- Điều khiển LED --------------------
void controlLED(int pin, bool &state, bool newState) {
    if (state != newState) {
        state = newState;
        digitalWrite(pin, state ? HIGH : LOW);
    }
}

// -------------------- Cấu hình ban đầu --------------------
void setup() {
    Serial.begin(115200);
    dht.begin();
    lcd.begin();
    lcd.backlight();

    pinMode(MQ2PIN, INPUT);
    pinMode(FIRE_SENSOR_PIN, INPUT_PULLUP);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH); // Tắt còi ban đầu

    connectToWiFi();
    setupFirebase();
}

// -------------------- Vòng lặp chính --------------------
void loop() {
    // Đọc cảm biến
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int gasValue = analogRead(MQ2PIN);
    bool fireDetected = (digitalRead(FIRE_SENSOR_PIN) == LOW);

    // Hiển thị lên LCD
    displayOnLCD(temperature, humidity);

    // Xử lý khí gas
    if (gasValue > GAS_THRESHOLD) {
        gasAlert = true;
        controlLED(LED1_PIN, led1State, true);
    } else {
        gasAlert = false;
        controlLED(LED1_PIN, led1State, false);
    }

    // Xử lý lửa
    if (fireDetected) {
        fireAlert = true;
        controlLED(LED2_PIN, led2State, true);
    } else {
        fireAlert = false;
        controlLED(LED2_PIN, led2State, false);
    }

    // Điều khiển còi
    controlBuzzer(gasAlert || fireAlert);

    // Gửi dữ liệu lên Firebase
    sendDataToFirebase(temperature, humidity, gasValue, fireDetected);

    // Cập nhật trạng thái LED từ Firebase
    updateFromFirebase();

    // In thông tin ra Serial Monitor
    Serial.printf("Temp: %.2f°C, Humidity: %.2f%%, Gas: %d, Fire: %s, Buzzer: %s\n",
                  temperature, humidity, gasValue,
                  fireDetected ? "Yes" : "No",
                  buzzerState ? "On" : "Off");

    delay(2000); // Chờ 2 giây trước lần cập nhật tiếp theo
}
