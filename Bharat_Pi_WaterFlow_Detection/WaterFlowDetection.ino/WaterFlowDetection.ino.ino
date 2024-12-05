#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the LCD library

#define SCREEN_COLS 16          // Number of columns on the LCD
#define SCREEN_ROWS 2           // Number of rows on the LCD

// Create an LCD object with I2C address 0x27 (default for most 16x2 LCDs)
LiquidCrystal_I2C lcd(0x27, SCREEN_COLS, SCREEN_ROWS);

String apiKey = "KBD1JSZTUKCXJ15V"; // Enter your Write API key from ThingSpeak
const char *ssid = "Hotspot";     // Replace with your WiFi ssid and WPA2 key
const char *pass = "password";
const char* server = "api.thingspeak.com";

#define LED_BUILTIN 12
#define SENSOR  32

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  lcd.begin(); // Initialize the LCD
  lcd.clear();
  lcd.setBacklight(1); // Turn on the LCD backlight

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop()
{
  currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Calculate flow rate in litres per minute
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Calculate flow in millilitres and litres
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);

    // Add the millilitres passed in this second to the total
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;

    // Print flow rate and total volume to the Serial monitor
    Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalLitres);
    Serial.println("L");

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);  // Line 1 of the LCD
    lcd.print("Water Flow Meter");

    lcd.setCursor(0, 1);  // Line 2 of the LCD
    lcd.print("Flow: ");
    lcd.print(float(flowRate));
    lcd.print(" L/M");

    lcd.setCursor(0, 0);  // Line 1 of the LCD (update with total volume)
    lcd.print("Volume: ");
    lcd.print(totalLitres);
    lcd.print(" L");
  }

  // Send data to ThingSpeak
  if (client.connect(server, 80))
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(float(flowRate));
    postStr += "&field2=";
    postStr += String(totalLitres);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();
}
