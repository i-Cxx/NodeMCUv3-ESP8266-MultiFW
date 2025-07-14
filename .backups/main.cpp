#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPI.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h>




// Pin-Definitionen
const int ledPin = LED_BUILTIN;    // LED auf PIN = LED_BUILTIN
const int ledPin2 = D4;            // LED auf PIN = D4



// Der einzige ADC-Pin des NodeMCU ESP8266
const int analogPin = A0; 

// Antennen-Pin für EMF-Messung
#define PIN_ANTENNA A0
#define CHECK_DELAY 1000
#define lmillis() ((long)millis())



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
const char* ssid = "Vodafone-Test";      // Ersetze dies durch deinen WLAN-Namen
const char* password = "1234567890"; // Ersetze dies durch dein WLAN-Passwort




// Variablen für nicht-blockierendes Blinken
unsigned long previousMillis = 0;
const long blinkInterval = 250; // Intervall für das Blinken in Millisekunden
bool ledState = LOW;




// Webserver auf Port 80 (Standard HTTP)
AsyncWebServer server(80);









// EMF Display Funktionen

// Funktion zum Anzeigen der EMF-Werte auf dem Display
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






// Funktion zum Senden eines Strings an das Display
void writeDisplay(String text) {
  display.clearDisplay();                // Löscht den gesamten Puffer des Displays
  display.setTextSize(1);                // Setzt die Textgröße (1 ist Standard, 2 wäre doppelt so groß etc.)
  display.setTextColor(SSD1306_WHITE);   // Setzt die Textfarbe (SSD1306_WHITE oder SSD1306_BLACK)
  display.setCursor(0,0);                // Setzt den Cursor an die obere linke Ecke (0,0)

  display.println(text);                 // Gibt den String auf dem Display aus
  display.display();                    // Aktualisiert das physische Display mit dem Inhalt des Puffers
}

// Funktion zum Senden eines Strings an das LCD
void writeLCD(String text) {
  lcd.clear();                            // Löscht den gesamten Puffer des LCDs
  lcd.setCursor(0,0);                     // Setzt den Cursor an die obere linke Ecke (0,0)
  lcd.print(text);                        // Gibt den String auf dem LCD aus
}




// Funktion zum Löschen des LCDs
void clearLCD() {
  Serial.println("Console > Clearing LCD ! ... .. .");              // Erklärt, dass das LCD gelöscht wird
  lcd.clear();                                                      // Löscht den Inhalt des LCDs
  lcd.setCursor(0,0);                                               // Setzt den Cursor auf die erste Zeile, erste Spalte
  Serial.println("Console > LCD cleared! ... .. .");                // LCD wurde gelöscht
}


// Funktion zum Löschen des Displays
void clearDisplay() {
  Serial.println("Console > Clearing Display ! ... .. .");          // Erklärt, dass das Display gelöscht wird
  display.clearDisplay();                                           // Löscht den Inhalt des Displays
  display.setCursor(0,0);                                           // Setzt den Cursor auf die erste Zeile, erste Spalte
  display.display();                                                // Aktualisiert das Display
  Serial.println("Console > Display cleared! ... .. .");            // Display wurde gelöscht
}






// Initialisiere OLED SSD1306 Display
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Console > SSD1306 konnte nicht initialisiert werden! ... .. ."));
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

  Serial.println("Console > Display initialisiert! ... .. .");

  delay(3000);

  display.clearDisplay();      // Display löschen
  display.display();           // Anzeige aktualisieren
  display.setCursor(0, 0);     // Cursor zurücksetzen
}


// LCD 1602 LiquidCrystal Display initialisieren
void initLCD() {
  lcd.init();                            // Initialisiert das LCD
  lcd.backlight();                       // Schaltet die Hintergrundbeleuchtung ein

  lcd.setCursor(0,0);                    // Setzt den Cursor auf die erste Zeile, erste Spalte
  lcd.print("Console > Started !");      // Gibt Text auf dem LCD aus
  lcd.setCursor(0,1);                    // Setzt den Cursor auf die zweite Zeile, erste Spalte
  lcd.print("... .. .");                 // Gibt Text auf der zweiten Zeile aus

  Serial.println("Console > LCD initialisiert! ... .. .");    // Initialisierung abgeschlossen
  delay(3000);                                                // Kurze Pause, um die Anzeige zu stabilisieren
  lcd.clear();                                                // Löscht den Inhalt des LCDs
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
  Serial.println("Console > Mounting LittleFS ! ... .. .");

  if(!LittleFS.begin()){
    Serial.println("Console > An Error has occurred while mounting LittleFS ! ... .. .");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("LittleFS Error!");
    display.display();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Console > ");
    lcd.setCursor(0,1);
    lcd.print("LittleFS Error!");
    delay(2000); // Kurze Pause, um die Anzeige zu stabilisieren
    display.clearDisplay();
    display.display();
    lcd.clear(); // Löscht den LCD-Inhalt
    return;
  }

  Serial.println("Console > LittleFS mounted successfully !!! ... .. .");
}


// Funktion zum Initialisieren der WLAN-Verbindung
void initWIFI() {
  Serial.print("Console > Verbinde mit WLAN: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Console > Connecting to WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }

  Serial.println("\n");
  Serial.println("Console > WLAN verbunden!");
  Serial.print("Console > IP-Adresse: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Console > WiFi Connected!");
  display.display();
  display.setCursor(0,8);
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
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

  // Fallback-Route, wenn eine Datei nicht gefunden wird.
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not Found");
  });


  // 8. Webserver starten
  server.begin();
  Serial.println("Webserver gestartet!");

  // 9. Anzeigen der IP-Adresse auf dem Display und LCD
  Serial.println("Console > Anzeigen der IP-Adresse auf dem Display und LCD ... .. .");
  clearDisplay(); // Display leeren
  writeDisplay("Console >");
  display.setCursor(0, 8);
  display.println("IP: " + WiFi.localIP().toString());
  display.display();

  clearLCD();     // LCD leeren
  lcd.clear(); // LCD leeren
  lcd.setCursor(0,0);
  writeLCD("WiFi IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP().toString());

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
