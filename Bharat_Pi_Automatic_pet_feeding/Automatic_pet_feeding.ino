// Blynk Template and Auth Details
#define BLYNK_TEMPLATE_ID "TMPL34tWTqG4R"
#define BLYNK_TEMPLATE_NAME "esp32 pet feeder"
#define BLYNK_AUTH_TOKEN "iFPvi_ws_CJmRrZaFRNeLgiUdw5xtIlS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

Servo servo1;

const int servoPin = 27;  // Define the pin for the servo motor

// Your WiFi credentials
char ssid[] = "Hotspot";
char pass[] = "password";

BLYNK_WRITE(V0)
{
    int s0 = param.asInt();
    if (s0 == 1) {
        Serial.println("Feed! my pet");
        for (int i = 0; i < 3; i++) {
            servo1.write(0);   // Move servo to 0 degrees
            delay(500);
            servo1.write(90);  // Move servo to 90 degrees
            delay(1000);
        }
        servo1.write(0);  // Reset the servo to 0 degrees
        delay(500);
    }
}

void setup()
{
    Serial.begin(115200);  // Start serial communication
    Serial.println("Pet Feeder Code Running...");  // Message to indicate the code is running

    // Initialize Blynk with Auth Token, WiFi SSID, and Password
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    servo1.attach(servoPin);  // Attach the servo to the specified pin
}

void loop()
{
    Blynk.run();  // This keeps the Blynk connection alive
}
