#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>
#include <ArduinoJson.h> // FÃ¼r die JSON-Erstellung



// ==========================
// Pin-Definitionen
// ==========================
const int ledPin = LED_BUILTIN;
const int analogPin = A0; // EMF-Messung / ADC
#define PIN_ANTENNA A0
#define CHECK_DELAY 1000
#define lmillis() ((long)millis())
int currentEmfValue = 0; // Hier wird der aktuellste EMF-Wert gespeichert



// ==========================
// Display-Konfiguration
// ==========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int LCD_COLS = 16;
const int LCD_ROWS = 2;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // Achte ggf. auf 0x3F

// ==========================
// WLAN Access Point
// ==========================
const char* ap_ssid = "ESP_Hotspot";
const char* ap_password = "1234567890";

// ==========================
// Webserver
// ==========================
AsyncWebServer server(80);

// ==========================
// Setup Funktionen
// ==========================
void initLED() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  delay(1000);
  digitalWrite(ledPin, HIGH);
}

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Fehler beim Initialisieren des OLED-Displays"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Display gestartet");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LCD gestartet");
  lcd.setCursor(0, 1);
  lcd.print("... .. .");
  delay(2000);
  lcd.clear();
}

void initLittleFS() {
  Serial.println("Mounting LittleFS...");
  if (!LittleFS.begin()) {
    Serial.println("Fehler beim Mounten von LittleFS!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LittleFS Fehler!");
    display.display();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LittleFS Fehler!");
    delay(3000);
    display.clearDisplay();
    display.display();
    lcd.clear();
    return;
  }
  Serial.println("LittleFS erfolgreich gemountet.");
}

void initWiFiAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  IPAddress myIP = WiFi.softAPIP();

  Serial.println("AP gestartet:");
  Serial.println("SSID: " + String(ap_ssid));
  Serial.println("Passwort: " + String(ap_password));
  Serial.println("IP-Adresse: " + myIP.toString());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AP Modus aktiv");
  display.setCursor(0, 10);
  display.println("SSID: " + String(ap_ssid));
  display.setCursor(0, 20);
  display.println("IP: " + myIP.toString());
  display.display();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AP IP:");
  lcd.setCursor(0, 1);
  lcd.print(myIP.toString());
}

void initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/test.html", "text/html");
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "404: Not Found");
  });

  server.begin();
  Serial.println("Webserver gestartet");
}


void showReadings(int emfValue)
{
    display.clearDisplay();   
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(5,40);
    display.println("EMF");

    display.setCursor(70,40);
    display.println(emfValue);

    display.display();
}


// ==========================
// Setup & Loop
// ==========================
void setup() {
  Serial.begin(115200);
  Serial.println("\nESP startet...");
  pinMode(PIN_ANTENNA, INPUT);

  initLED();
  Wire.begin(D2, D1); // I2C-Pins
  initDisplay();
  initLCD();
  initLittleFS();
  initWiFiAP();
  initWebServer();


  display.clearDisplay();
}

void loop() {
  // Nicht-blockierendes Blinken der LED (Heartbeat)
  static unsigned long previousMillis = 0;
  const long interval = 500;
  static bool ledState = LOW;

  static int avgValue = 0, emfValue = 0;
    static long nextCheck = 0, emfSum = 0, iterations = 0;

    emfValue = constrain(analogRead(PIN_ANTENNA), 0, 1023);
    emfSum += emfValue;
    iterations++;

    if (lmillis() - nextCheck >= 0) {
        avgValue = emfSum / iterations;
        emfSum = 0;
        iterations = 0;
        showReadings(avgValue);
        nextCheck = lmillis() + CHECK_DELAY;
    }
    display.drawRoundRect(0, 5, 126, 30, 2, WHITE);
    display.fillRect(5, 10, 120, 23, BLACK);
    display.fillRect(5, 10, map(emfValue, 0, 1023, 0, 118), 20, WHITE);
    display.display();

if (emfValue > 80 && emfValue <150) {
  tone (12,100,500);
  }
if (emfValue > 151 && emfValue <250) {
  tone (12,500,500);
}
if (emfValue > 251 ) {
  tone (12,1000,500);
}

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}
