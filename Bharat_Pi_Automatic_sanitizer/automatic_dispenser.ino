#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define IR_SENSOR_PIN 18
#define SERVO_PIN 33
#define LED_PIN 13
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSED_ANGLE 0
#define DEBOUNCE_DELAY 1000
#define WIFI_SSID "Yogesh" // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "Yogi5244" // Replace with your Wi-Fi password
#define BASE_URL "https://api.refillbot.in/api/webhook/high/device"

const String orgID = "9f031939-4071-4fb7-a08e-b97b24c9b987"; // Organization ID
const String sensorName = "FL1"; // Sensor name
const String deviceId = ""; // Replace with your actual device ID

Servo testServo;
unsigned long lastMotionTime = 0;

void setup() {
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  testServo.attach(SERVO_PIN);
  testServo.write(SERVO_CLOSED_ANGLE);

  Serial.begin(115200);
  Serial.println("IR Sensor and Servo Test with Extended Debounce and LED");

  connectToWiFi();
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void sendJsonData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(BASE_URL);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);

    // Create JSON object
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["orgID"] = orgID;
    jsonDoc["sensorName"] = sensorName;
    jsonDoc["motionDetected"] = true;
    jsonDoc["timestamp"] = millis();
    jsonDoc["deviceId"] = deviceId; // Use the defined device ID

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    Serial.println("Sending JSON: " + jsonString);

    int retryCount = 3;
    int httpResponseCode;
    while (retryCount-- > 0) {
      httpResponseCode = http.POST(jsonString);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        Serial.println("Response: " + response);
        break; // Exit loop on success
      } else {
        Serial.printf("Error sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
        Serial.println("Retrying...");
        delay(2000);
      }
    }
    http.end();
  } else {
    Serial.println("WiFi not connected, attempting to reconnect...");
    connectToWiFi();
  }
}

void loop() {
  bool motionDetected = digitalRead(IR_SENSOR_PIN);

  if (motionDetected == LOW) { // Assuming LOW means motion detected
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Motion Detected!");

    if (millis() - lastMotionTime > DEBOUNCE_DELAY) {
      Serial.println("Moving Servo...");
      lastMotionTime = millis();
      testServo.write(SERVO_OPEN_ANGLE);
      delay(1000);
      testServo.write(SERVO_CLOSED_ANGLE);
      delay(1000);
      sendJsonData();
      Serial.println("Servo Test Complete - Waiting for Next Detection");
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("No Motion Detected.");
  }

  delay(500); // Small delay for readability in the Serial Monitor
}
