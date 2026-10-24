#include <Arduino.h>
#include <DHT.h>

// SMART HOME / WOKWI: ESP32 DevKit, Arduino-ESP32 3.x
// MQ-2 su dung DO (tin hieu so) de demo trang thai canh bao on dinh.
// Day la logic MO PHONG, khong phai thiet bi bao ro ri gas dat chuan an toan.
#define PIR_PIN     4
#define DHT_PIN    15
#define DHT_TYPE DHT22   // Wokwi: DHT22. Phan cung DHT11: doi thanh DHT11.
#define LED_MOTION  2
#define LED_TEMP   27
#define LED_LIGHT  25
#define GAS_AO_PIN 35    // MQ-2 DO -> GPIO35 (KHONG dung ADC34 trong ban demo)
#define BUZZER_PIN 26
#define LDR_PIN    32
#define POT_PIN    33
#define LED_LDR 14// GPIO14 PWM (LED tuong ung voi LDR)

constexpr float TEMP_THRESHOLD_C = 35.0f;
constexpr uint32_t REPORT_INTERVAL_MS = 2000;
constexpr int PWM_FREQ = 5000;
constexpr int PWM_RESOLUTION = 8;
int gasThresholdAdc = 0;
bool gasThresholdValid = false;
DHT dht(DHT_PIN, DHT_TYPE);
bool autoMode = true;
bool gasAlarm = false;
uint32_t previousReport = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(GAS_AO_PIN, INPUT);  // GPIO35 input only; MQ-2 Wokwi DO can drive it
  pinMode(LDR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(LED_MOTION, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_MOTION, LOW);
  digitalWrite(LED_TEMP, LOW);
  pinMode(LED_LDR, OUTPUT);
digitalWrite(LED_LDR, LOW); //đèn tối
  noTone(BUZZER_PIN);
  analogReadResolution(12);
  
  // Hieu chuan tai muc 400ppm ban dau cua Wokwi
  delay(500);

  long gasSum = 0;

  for (int i = 0; i < 20; i++) {
    gasSum += analogRead(GAS_AO_PIN);
    delay(10);
  }

  gasThresholdAdc = gasSum / 20;
  gasThresholdValid =
      gasThresholdAdc > 0 && gasThresholdAdc < 4095;

  Serial.print("NGUONG GAS ADC (400ppm): ");
  Serial.println(gasThresholdAdc);

  if (!gasThresholdValid) {
    Serial.println("LOI: ADC GAS BAO HOA / KHONG HOP LE");
  }
//
  if (!ledcAttach(LED_LIGHT, PWM_FREQ, PWM_RESOLUTION)) {
    Serial.println("LOI: Khong khoi tao duoc PWM GPIO25");
  }
  dht.begin();
  Serial.println("\n=== SMART HOME ESP32 / WOKWI ===");
  Serial.println("MQ-2 DO GPIO35: 1=Binh thuong, 0=Canh bao");
  Serial.println("Nhap a: AUTO (LDR), m: MANUAL (bien tro)");
  Serial.println("Ban demo: khong dung de canh bao gas an toan thuc te.");
}

void loop() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'a' || ch == 'A') {
      autoMode = true;
      Serial.println("MODE -> AUTO / LDR");
    } else if (ch == 'm' || ch == 'M') {
      autoMode = false;
      Serial.println("MODE -> MANUAL / POT");
    }
  }

  const bool motion = digitalRead(PIR_PIN) == HIGH;
  digitalWrite(LED_MOTION, motion ? HIGH : LOW);

  // Read the actual digital level on every loop. No reset-dependent state or debounce.
  const uint32_t now = millis();
  
  // ========== CAM BIEN GAS MQ-2 ==========
  const int gasAdc = analogRead(GAS_AO_PIN);

  // So sanh voi ADC tai muc 400ppm da hieu chuan
  const bool sensorAlarm =
      gasThresholdValid && (gasAdc >= gasThresholdAdc);

  if (sensorAlarm != gasAlarm) {
    gasAlarm = sensorAlarm;

    if (gasAlarm) {
      tone(BUZZER_PIN, 1500);
      Serial.println("CANH BAO GAS - COI BAT");
    } else {
      noTone(BUZZER_PIN);
      Serial.println("GAS DUOI NGUONG - COI TAT");
    }
  }


  const int ldrAdc = analogRead(LDR_PIN);
  const int potAdc = analogRead(POT_PIN);
  // Wokwi LDR: ADC tang khi troi toi
constexpr int LDR_DARK_THRESHOLD = 2000;

bool isDark = ldrAdc > LDR_DARK_THRESHOLD;

// LED xanh duong bat khi troi toi
digitalWrite(LED_LDR, isDark ? HIGH : LOW);
  // LDR Wokwi: AO thuong tang khi toi. Neu module that nguoc chieu, dao map.
  const int brightness = constrain(autoMode
      ? map(ldrAdc, 0, 4095, 0, 255)
      : map(potAdc, 0, 4095, 0, 255), 0, 255);
  ledcWrite(LED_LIGHT, brightness);

  if (now - previousReport >= REPORT_INTERVAL_MS) {
    previousReport = now;
    const float temperature = dht.readTemperature();
    const float humidity = dht.readHumidity();

    Serial.println("-----------------------------------");
    Serial.print("PIR       : "); Serial.println(motion ? "CO CHUYEN DONG" : "KHONG CO CHUYEN DONG");
    Serial.print("GAS ADC   : ");
Serial.println(gasAdc);

Serial.print("NGUONG ADC: ");
Serial.println(gasThresholdAdc);

Serial.print("GAS       : ");
Serial.println(
    !gasThresholdValid ? "LOI CAM BIEN" :
    gasAlarm ? "CANH BAO (>=400PPM MO PHONG)"
             : "DUOI NGUONG"
);

Serial.print("COI       : ");
Serial.println(gasAlarm ? "BAT" : "TAT");
    Serial.print("NGUON BAO : "); Serial.println(sensorAlarm ? "MQ-2" : "KHONG CO");
    Serial.print("GAS       : "); Serial.println(gasAlarm ? "CANH BAO" : "BINH THUONG");
    Serial.print("LDR ADC   : "); Serial.println(ldrAdc);
    Serial.print("POT ADC   : "); Serial.println(potAdc);
    Serial.print("PWM LED   : "); Serial.println(brightness);
    Serial.print("CHE DO    : "); Serial.println(autoMode ? "AUTO" : "MANUAL");
    Serial.print("ANH SANG  : ");
    Serial.println(isDark ? "TROI TOI" : "TROI SANG");
    Serial.print("LED LDR   : ");
    Serial.println(isDark ? "BAT" : "TAT");
    if (isnan(temperature) || isnan(humidity)) {
      digitalWrite(LED_TEMP, LOW);
      Serial.println("DHT: KHONG DOC DUOC!");
    } else {
      const bool hot = temperature > TEMP_THRESHOLD_C;
      digitalWrite(LED_TEMP, hot ? HIGH : LOW);
      Serial.print("NHIET DO  : "); Serial.print(temperature, 1); Serial.println(" C");
      Serial.print("DO AM     : "); Serial.print(humidity, 1); Serial.println(" %");
      Serial.print("NHIET DO  : "); Serial.println(hot ? "CANH BAO (>35C)" : "BINH THUONG");
    }
    Serial.println("-----------------------------------");
  }
  delay(10);
}
