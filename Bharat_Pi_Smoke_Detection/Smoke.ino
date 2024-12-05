#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DO_PIN 13  // ESP32's pin GPIO14 connected to DO pin of the MQ2 sensor

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  // initialize the ESP32's pin as an input
  pinMode(DO_PIN, INPUT);

  // initialize the OLED display
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x64

  display.display(); // Show initial display buffer contents on the screen
  delay(2000);       // Pause for 2 seconds
  display.clearDisplay(); // Clear the buffer

  Serial.println("Warming up the MQ2 sensor");
  delay(20000);  // wait for the MQ2 to warm up
}

void loop() {
  int gasState = digitalRead(DO_PIN);

  display.clearDisplay();      // Clear the buffer
  display.setTextSize(2);       // Set text size
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 10);    // Set cursor position

  if (gasState == HIGH) {
    Serial.println("The gas is NOT present");
    display.print("No Gas");
  } else {
    Serial.println("The gas is present");
    display.clearDisplay();      // Clear the buffer
    display.setTextSize(2); 
    display.setCursor(0,16); 
    display.print("Gas ");     // Clear the buffer
    display.setTextSize(2); 
    display.setCursor(0,32);
    display.print("Detected");
  }

  display.display(); // Display the content on the screen
  delay(1000);       // Update the display every 1 second
}
