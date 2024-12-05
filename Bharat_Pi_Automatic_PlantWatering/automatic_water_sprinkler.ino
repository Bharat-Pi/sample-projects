#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust if I2C address is different

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int soilMoisturePin = 33; // Soil moisture sensor pin
const int relayPin = 18;        // Relay control pin
int moistureThreshold = 30;     // Threshold for soil moisture percentage
unsigned long sprinklerDuration = 10000; // Duration to keep the sprinkler on
bool sprinklerActive = false;   // Track if the sprinkler is active
unsigned long sprinklerStartTime = 0; // Track when the sprinkler was activated

// Wi-Fi credentials
const char* ssid = "Yogesh";
const char* password = "Yogi5244";

// ThingSpeak API settings
const char* server = "http://api.thingspeak.com";
const char* apiKey = "4LXWKQERLLY5FIAN"; // Your ThingSpeak Write API Key
//const int channelID = YOUR_THINGSPEAK_CHANNEL_ID; // Your Channel ID

void setup() {
  Serial.begin(115200);
  
  Wire.begin(21, 22); // SDA = 21, SCL = 22 for ESP32

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Water Sprinkler");

  dht.begin();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Turn off relay initially

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  Serial.println("System initialized. Monitoring soil moisture and climate...");
}

void loop() {
  // Read soil moisture
  int soilMoistureValue = analogRead(soilMoisturePin);
  int moisturePercent = map(soilMoistureValue, 1023, 0, 0, 100); // Convert to percentage

  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Output to Serial Monitor
  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  if (!isnan(temperature)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  } else {
    Serial.println("Temperature reading error.");
  }

  if (!isnan(humidity)) {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  } else {
    Serial.println("Humidity reading error.");
  }

  // Display readings on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moisturePercent);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C ");
  lcd.print(humidity);
  lcd.print("%");

  // Check if soil moisture is below threshold and control the sprinkler
  if (moisturePercent < moistureThreshold) {
    if (!sprinklerActive) {
      Serial.println("Soil is dry. Activating sprinkler...");
      digitalWrite(relayPin, LOW); // Activate relay to turn on sprinkler
      sprinklerActive = true;
      sprinklerStartTime = millis();
      lcd.setCursor(0, 1);
      lcd.print("Watering...     ");
    }
  } else {
    if (sprinklerActive && (millis() - sprinklerStartTime > sprinklerDuration)) {
      Serial.println("Soil is moist. Turning off sprinkler.");
      digitalWrite(relayPin, HIGH); // Turn off sprinkler
      sprinklerActive = false;
    }
  }

  // Send data to ThingSpeak every 20 seconds
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 20000) {
    sendToThingSpeak(moisturePercent, temperature, humidity);
    lastSendTime = millis();
  }

  delay(2000); // Delay for sensor stabilization
}

// Function to send data to ThingSpeak
void sendToThingSpeak(int moisture, float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the URL for ThingSpeak
    String url = String(server) + "/update?api_key=" + apiKey + 
                 "&field1=" + moisture +
                 "&field2=" + temperature +
                 "&field3=" + humidity;

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("Data sent to ThingSpeak. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
