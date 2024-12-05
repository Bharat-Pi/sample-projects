#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
//#include <AutoConnect.h>

#define IR_SENSOR_PIN 18
#define SERVO_PIN 33
#define LED_PIN 13
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSED_ANGLE 0
#define DEBOUNCE_DELAY 1000
#define WIFI_SSID "Yogesh" // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "Yogi5244" // Replace with your Wi-Fi password
#define BASE_URL "https://api.refillbot.in/api/webhook/high/device"

const char* rootCACertificate = 
  "-----BEGIN CERTIFICATE-----\n"
  "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\n"
  "-----END CERTIFICATE-----\n";

String orgID = "27fb34a9-7ae2-421a-8163-a368b015997b"; // Organization ID
String sensorName = "FL1"; // Sensor name
String secID = ""; // Replace with your secret key

Servo testServo;
unsigned long lastMotionTime = 0;
String devID;

void setup() {
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  testServo.attach(SERVO_PIN);
  testServo.write(SERVO_CLOSED_ANGLE);

  Serial.begin(115200);
  Serial.println("IR Sensor and Servo Test with Extended Debounce and LED");

  connectToWiFi();
  devID = WiFi.macAddress(); // Set device ID
  Serial.print("Device ID set to: ");
  Serial.println(devID);
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void sendJsonData(const String& dataTobePushed) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(BASE_URL, rootCACertificate);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);

    // Build JSON data manually
    String sensorData = "{\"did\":\"" + devID + "\",\"oid\":\"" + orgID + "\",\"data\":[{\"data\":\"" + dataTobePushed + "\",\"sid\":\"" + sensorName + "\"}],\"secret\":\"" + secID + "\"}";

    Serial.println("Sending JSON: " + sensorData);

    int retryCount = 3;
    int httpResponseCode;
    while (retryCount-- > 0) {
      httpResponseCode = http.POST(sensorData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        Serial.println("Response: " + response);
        break; // Exit loop on success
      } else {
        Serial.printf("Failed to send data. HTTP Response code: %d\n", httpResponseCode);
      }
    }
    http.end();
  } else {
    Serial.println("Wi-Fi disconnected; reconnecting...");
    connectToWiFi();
  }
}

void loop() {
  if (digitalRead(IR_SENSOR_PIN) == HIGH) {
    if (millis() - lastMotionTime > DEBOUNCE_DELAY) {
      lastMotionTime = millis();

      Serial.println("Motion Detected!");
      digitalWrite(LED_PIN, HIGH);

      // Open and close servo
      testServo.write(SERVO_OPEN_ANGLE);
      delay(1000);
      testServo.write(SERVO_CLOSED_ANGLE);

      // Replace with actual data to be pushed
      String dataTobePushed = "your_sensor_data"; 
      sendJsonData(dataTobePushed);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
