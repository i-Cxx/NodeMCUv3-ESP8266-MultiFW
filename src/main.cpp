#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h>



// Pin-Definitionen
const int ledPin = LED_BUILTIN;    // LED auf PIN = LED_BUILTIN
const int ledPin2 = D4;            // LED auf PIN = D4




// Display-Konfiguration

// Display-Auflösung
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I2C Adresse des SSD1306 (häufig 0x3C)
#define OLED_ADDRESS 0x3C

// Erstelle Display-Objekt
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


// LCD Definitionen
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
// Prüfe deine LCD I2C-Adresse! 0x27 oder 0x3F sind am häufigsten.
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); 






// WLAN Zugangsdaten
const char* ssid = "Vodafone-2F64";      // Ersetze dies durch deinen WLAN-Namen
const char* password = "GedK4saacbP4ckTX"; // Ersetze dies durch dein WLAN-Passwort




// Variablen für nicht-blockierendes Blinken
unsigned long previousMillis = 0;
const long blinkInterval = 250; // Intervall für das Blinken in Millisekunden
bool ledState = LOW;




// Webserver auf Port 80 (Standard HTTP)
AsyncWebServer server(80);






// Initialisiere OLED SSD1306 Display
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 konnte nicht initialisiert werden!"));
    while (true); // Anhalten, wenn das Display nicht gefunden wird
  }

  // Bildschirm löschen
  display.clearDisplay();

  // Text-Eigenschaften setzen
  display.setTextSize(1);      // Textgröße
  display.setTextColor(SSD1306_WHITE); // Textfarbe
  display.setCursor(0, 0);     // Startposition

  // Text ausgeben
  display.println(F("Hello, NodeMCU!"));
  display.display();           // Anzeige aktualisieren

  Serial.println("Display initialisiert.");

  delay(2000);

}




// LCD initialisieren
void initLCD() {
  lcd.init();      // Initialisiert das LCD
  lcd.backlight(); // Schaltet die Hintergrundbeleuchtung ein

  lcd.setCursor(0,0);
  lcd.print("LCD OK!");
  lcd.setCursor(0,1);
  lcd.print("NodeMCU");

  Serial.println("LCD initialisiert.");
}




// LED Pin initialisieren
void initLED() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED aus
  delay(1000); // Kurze Pause, um den Start zu stabilisieren
  digitalWrite(ledPin, HIGH); // LED an
}




// 2. Initialisiere DateiSystem (LittleFS)
void initFS() {
  Serial.println("Mounting LittleFS...");

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("LittleFS Error!");
    display.display();
    return;
  }
  Serial.println("LittleFS mounted successfully.");
}




void initWIFI() {
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n");
  Serial.println("WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.println(WiFi.localIP());
  display.display();

}


// Setup-Funktion, die einmal beim Start aufgerufen wird
void setup() {

  // 1. Serielle Kommunikation starten
  Serial.begin(115200);
  Serial.println("\nStarting NodeMCU...");


  // 2. Initialisiere LED
  initLED();
  
  
  // 3. I2C initialisieren
  Wire.begin(D2, D1); // SDA, SCL


  

  // 4. Initialisiere Displays
  initDisplay();   //  4.1 Initialisiere OLED SSD1306 Display
  initLCD();       //  4.2 Initialisiere LCD


  


  // 5. Initialisiere DateiSystem (LittleFS)
  initFS();


  // 6. WLAN verbinden
  initWIFI();



  // 7. Webserver-Routen definieren
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // Fallback-Route, wenn eine Datei nicht gefunden wird.
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not Found");
  });


  // 8. Webserver starten
  server.begin();
  Serial.println("Webserver gestartet!");

}


void loop() {
  // Nicht-blockierendes Blinken der LED.
  // Dies stellt sicher, dass der Rest des Codes (z.B. der Webserver)
  // ohne Unterbrechung weiterlaufen kann.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    ledState = !ledState; // Zustand umschalten (AN/AUS)
    digitalWrite(ledPin, ledState);
  }
}
