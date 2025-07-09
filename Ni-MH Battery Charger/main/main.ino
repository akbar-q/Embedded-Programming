/*
ESP32 Battery Monitor Web UI with Access Point, LED fading, and SPIFFS-hosted files.
Includes 5-cell voltage sensing, PWM LED control, 7-segment display, and web UI hosting with local fonts/images.

Thanks to https://www.youtube.com/watch?v=Qw5mQus-WwA

CTRL+SHIFT+P to Open Plugins to Upload Files

Search for LittleFS to Find the Upload to ESP32 Filesystem Plugin

*/

// ================== Arduino Sketch (ESP32) ==================

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// === Battery monitoring ===
const int cellPins[5] = { 36, 39, 34, 35, 32 }; // ADCs A0–A4
const int ledPins[5]  = { 13, 14, 15, 16, 17 }; // PWM LEDs
const int emptyThreshold = 100;
const int fullThreshold  = 2300;

// === Touch and 7-segment ===
const int touchPin = T0; // GPIO4
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
  int fullCount = 0;
  bool touchPressed = touchRead(touchPin) < 20;

  for (int i = 0; i < 5; i++) {
    int val = analogRead(cellPins[i]);
    bool isEmpty = val < emptyThreshold;
    bool isFull  = val > fullThreshold;
    bool isCharging = !isEmpty && !isFull;

    if (isFull) {
      analogWrite(ledPins[i], 255);
      fullCount++;
    } else if (isCharging) {
      if (millis() - lastFade > 30) {
        fadeLevel[i] += fadeDir[i] * 5;
        if (fadeLevel[i] >= 255 || fadeLevel[i] <= 0) fadeDir[i] *= -1;
      }
      analogWrite(ledPins[i], fadeLevel[i]);
    } else {
      analogWrite(ledPins[i], touchPressed ? 20 : 0);
    }
  }
  lastFade = millis();
  showDigit(fullCount);
  server.handleClient();
}

void showDigit(int digit) {
  if (digit < 0 || digit > 9) digit = 0;
  byte pattern = segDigits[digit];
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], bitRead(pattern, 7 - i));
}

// ================== /data Folder Structure ==================

/*
/data
  |-- index.html
  |-- style.css
  |-- fonts/
        |-- inter.woff2
        |-- oxanium.woff2
        |-- montserratalternates.woff2
*/