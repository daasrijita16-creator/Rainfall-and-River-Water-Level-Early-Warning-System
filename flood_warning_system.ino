// Flood / Water Level Warning System - ESP32 + Blynk
// Credentials live in secrets.h (not committed). See secrets.h.example.

#include "secrets.h"            // defines BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

// --- Pin Definitions ---
#define TRIG_PIN     5
#define ECHO_PIN     18   // use a 1k/2k voltage divider (HC-SR04 echo is 5V, ESP32 is 3.3V)
#define RAIN_PIN     19

#define LED_SAFE     2    // Green LED
#define LED_WARN     4    // Yellow LED
#define LED_DANGER   15   // Red LED
#define BUZZER_PIN   21   // Buzzer - change to your actual wired pin

// --- Tank Dimensions (cm) ---
const float TANK_DEPTH_CM    = 8.0;  // Total height of the container
const float SENSOR_OFFSET_CM = 1.0;  // Gap between sensor and the top of the tank

// --- Water level thresholds (percentage of tank depth) ---
// NOTE: calibrate these for your own container.
const int WARN_THRESHOLD   = 4;
const int DANGER_THRESHOLD = 6;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);

  pinMode(LED_SAFE, OUTPUT);
  pinMode(LED_WARN, OUTPUT);
  pinMode(LED_DANGER, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_SAFE, LOW);
  digitalWrite(LED_WARN, LOW);
  digitalWrite(LED_DANGER, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Run sendData() every 1000 ms
  timer.setInterval(1000L, sendData);
}

// Measures distance in centimetres using the HC-SR04
float measureDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30 ms timeout
  if (duration == 0) return -1.0;                  // no echo

  return (duration * 0.0343) / 2.0;
}

void sendData() {
  // 1. Measure distance and convert to water level (%)
  float distance = measureDistanceCM();
  int waterPercentage = 0;
  String statusStr = "SAFE";

  if (distance > 0) {
    float waterHeight = (TANK_DEPTH_CM + SENSOR_OFFSET_CM) - distance;           // cm
    waterPercentage = constrain((waterHeight / TANK_DEPTH_CM) * 100.0, 0, 100);  // %
  }

  // 2. Read rain / moisture sensor (LOW = wet)
  bool isWet = (digitalRead(RAIN_PIN) == LOW);

  // 3. Determine state, drive LEDs and buzzer
  if (waterPercentage >= DANGER_THRESHOLD && isWet) {
    statusStr = "DANGER";
    digitalWrite(LED_SAFE, LOW);
    digitalWrite(LED_WARN, LOW);
    digitalWrite(LED_DANGER, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.logEvent("danger_alert", "Water level DANGER!");

  } else if (waterPercentage >= WARN_THRESHOLD && isWet) {
    statusStr = "WARN";
    digitalWrite(LED_SAFE, LOW);
    digitalWrite(LED_WARN, HIGH);
    digitalWrite(LED_DANGER, LOW);
    digitalWrite(BUZZER_PIN, LOW);

  } else {
    statusStr = "SAFE";
    digitalWrite(LED_SAFE, HIGH);
    digitalWrite(LED_WARN, LOW);
    digitalWrite(LED_DANGER, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 4. Serial debug output
  Serial.printf("Dist: %.1f cm | Level: %d%% | Status: %s | Wet: %s\n",
                distance, waterPercentage, statusStr.c_str(), isWet ? "YES" : "NO");

  // 5. Send data to Blynk
  Blynk.virtualWrite(V0, waterPercentage);   // Gauge widget
  Blynk.virtualWrite(V1, isWet ? 1 : 0);     // LED widget
  Blynk.virtualWrite(V2, statusStr);         // Label widget
}

void loop() {
  Blynk.run();
  timer.run();
}
