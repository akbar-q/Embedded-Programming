/*
ESP32 Battery Monitor Web UI with Access Point, LED fading, and SPIFFS-hosted files.
Includes 5-cell voltage sensing, PWM LED control, 7-segment display, and web UI hosting with local fonts/images.

Thanks to https://www.youtube.com/watch?v=Qw5mQus-WwA

CTRL+SHIFT+P to Open Plugins to Upload Files

Search for LittleFS to Find the Upload to ESP32 Filesystem Plugin
*/

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// === Battery monitoring ===
const int cellPins[5] = { 36, 39, 34, 35, 32 }; // ADCs A0–A4 in an Array
const int ledPins[5]  = { 13, 14, 15, 16, 17 }; // PWM LEDs
const int emptyThreshold = 100; //To be Tested
const int fullThreshold  = 2300; // To be Tested

// === Touch and 7-segment ===
const int touchPin = T0; // GPIO4 // T0 Represents Touch Channel 0
const int segPins[8] = { 18, 19, 21, 22, 23, 25, 26, 27 }; // A-G, DP
const byte segDigits[10] = {
  B11111100, B01100000, B11011010, B11110010, B01100110,
  B10110110, B10111110, B11100000, B11111110, B11110110
};

// === LED fading state ===
int fadeLevel[5] = {0};
int fadeDir[5] = {1};
unsigned long lastFade = 0;

// === WiFi Access Point ===
const char* ssid = "ESP32-Monitor";
const char* password = "12345678";
WebServer server(80);
// Access Point IP: 192.168.4.1 I Believe this is the Default IP for ESP32 AP

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("AP IP: " + WiFi.softAPIP().toString());

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0);
  }

  for (int i = 0; i < 8; i++) pinMode(segPins[i], OUTPUT);

  server.on("/", []() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/style.css", []() {
    File file = LittleFS.open("/style.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });

  server.on("/fonts/inter.woff2", []() {
    File file = LittleFS.open("/fonts/inter.woff2", "r");
    server.streamFile(file, "font/woff2");
    file.close();
  });
  server.on("/fonts/oxanium.woff2", []() {
    File file = LittleFS.open("/fonts/oxanium.woff2", "r");
    server.streamFile(file, "font/woff2");
    file.close();
  });
  server.on("/fonts/montserratalternates.woff2", []() {
    File file = LittleFS.open("/fonts/montserratalternates.woff2", "r");
    server.streamFile(file, "font/woff2");
    file.close();
  });
  server.on("/logo.gif", []() {
    File file = LittleFS.open("/logo.gif", "r");
    server.streamFile(file, "image/gif");
    file.close();
  });

  server.on("/data", []() {
    String json = "{";
    for (int i = 0; i < 5; i++) {
      json += "\"cell" + String(i+1) + "\":" + String(analogRead(cellPins[i]));
      if (i < 4) json += ",";
    }
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/run/dino.html", []() {
    File file = LittleFS.open("/run/dino.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/run/dino.js", []() {
    File file = LittleFS.open("/run/dino.js", "r");
    server.streamFile(file, "application/javascript");
    file.close();
  });
  server.on("/run/dino.css", []() {
    File file = LittleFS.open("/run/dino.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });
  server.on("/run/index.js", []() {
    File file = LittleFS.open("/run/index.js", "r");
    server.streamFile(file, "application/javascript");
    file.close();
  });
  server.on("/run/index.css", []() {
    File file = LittleFS.open("/run/index.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });
  // ...and so on for any other files in /run/

  // 100% assets
  server.on("/run/assets/default_100_percent/100-disabled.png", []() {
    File file = LittleFS.open("/run/assets/default_100_percent/100-disabled.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });
  server.on("/run/assets/default_100_percent/100-error-offline.png", []() {
    File file = LittleFS.open("/run/assets/default_100_percent/100-error-offline.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });
  server.on("/run/assets/default_100_percent/100-offline-sprite.png", []() {
    File file = LittleFS.open("/run/assets/default_100_percent/100-offline-sprite.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });

  // 200% assets
  server.on("/run/assets/default_200_percent/200-disabled.png", []() {
    File file = LittleFS.open("/run/assets/default_200_percent/200-disabled.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });
  server.on("/run/assets/default_200_percent/200-error-offline.png", []() {
    File file = LittleFS.open("/run/assets/default_200_percent/200-error-offline.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });
  server.on("/run/assets/default_200_percent/200-offline-sprite.png", []() {
    File file = LittleFS.open("/run/assets/default_200_percent/200-offline-sprite.png", "r");
    server.streamFile(file, "image/png");
    file.close();
  });

  server.onNotFound([]() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.begin();
}

void loop() {
  int fullCount = 0; // Counter for fully charged cells
  // Read the touch sensor; true if pressed (threshold < 20)
  bool touchPressed = touchRead(touchPin) < 20;

  // Loop through each battery cell
  for (int i = 0; i < 5; i++) {
    int val = analogRead(cellPins[i]); // Read cell voltage (ADC value)
    bool isEmpty = val < emptyThreshold;   // Check if cell is empty
    bool isFull  = val > fullThreshold;    // Check if cell is full
    bool isCharging = !isEmpty && !isFull; // Cell is charging if not empty or full

    if (isFull) {
      // If cell is full, set LED to maximum brightness
      analogWrite(ledPins[i], 255);
      fullCount++; // Increment full cell counter
    } else if (isCharging) {
      // If cell is charging, fade LED in and out
      if (millis() - lastFade > 30) { // Update fade every 30ms
        fadeLevel[i] += fadeDir[i] * 5; // Change fade level
        // Reverse direction at bounds
        if (fadeLevel[i] >= 255 || fadeLevel[i] <= 0) fadeDir[i] *= -1;
      }
      analogWrite(ledPins[i], fadeLevel[i]); // Set LED to fade level
    } else {
      // If cell is empty, turn LED off or dim if touch pressed
      analogWrite(ledPins[i], touchPressed ? 20 : 0);
    }
  }
  lastFade = millis(); // Update last fade timestamp

  showDigit(fullCount); // Display number of full cells on 7-segment

  server.handleClient(); // Handle incoming web requests
}

// Display a single digit (0-9) on the 7-segment display
void showDigit(int digit) {
  if (digit < 0 || digit > 9) digit = 0; // Clamp to valid range
  byte pattern = segDigits[digit]; // Get segment pattern for digit
  // Set each segment pin according to pattern
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], bitRead(pattern, 7 - i));
}
