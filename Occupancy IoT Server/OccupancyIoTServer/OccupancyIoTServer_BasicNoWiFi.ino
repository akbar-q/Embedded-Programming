/*
  OccupancyIoTServer_BasicNoWiFi.ino
  ------------------------------------------------------------
  PURPOSE (BEGINNER VERSION):
  - No Wi-Fi
  - No web server
  - Only core occupancy logic using a PIR motion sensor
  - Controls 4 outputs: Lights, Fan, TV, Air Con

  This file is intentionally full of comments for first-time coders.

  HOW THE LOGIC WORKS:
  1) PIR detects movement.
  2) If movement happened recently, we say "room is occupied".
  3) If occupied:
       - Lights ON
       - TV ON
       - Fan ON
  4) If PIR triggers happen frequently (more activity):
       - Air Con ON
     Otherwise:
       - Air Con OFF

  BOARD TARGET:
  - ESP32-C3 (safe default pins used below)

  IMPORTANT HARDWARE NOTE:
  - Many relay modules are ACTIVE-LOW.
    That means writing LOW turns relay ON.
  - This code supports both active-low and active-high modules.
*/

#include <Arduino.h>

// -------------------- PIN SETUP --------------------
// Input pin from PIR sensor output.
const int PIR_PIN = 2;

// Output pins (ESP32-C3 safer defaults, avoids GPIO20/GPIO21 USB/JTAG area).
const int LIGHT_PIN = 3;
const int AC_PIN = 4;
const int TV_PIN = 5;
const int FAN_PIN = 6;

// Relay polarity switch:
// true  = active-low relay board (common)
// false = active-high output device
const bool RELAY_ACTIVE_LOW = true;

// -------------------- TIMING / LOGIC SETTINGS --------------------
// If last motion trigger was within this time, room = occupied.
const unsigned long OCCUPIED_TIMEOUT_MS = 60UL * 1000UL; // 60 seconds

// Count how many PIR triggers happened in this rolling time window.
const unsigned long TRIGGER_WINDOW_MS = 30UL * 1000UL; // 30 seconds

// If trigger count in window >= this threshold, AC turns ON.
const uint8_t AC_TRIGGER_THRESHOLD = 3;

// Small gap to prevent counting one long PIR pulse many times.
const unsigned long MIN_TRIGGER_GAP_MS = 800UL;

// -------------------- INTERNAL STATE VARIABLES --------------------
// Remembers the PIR level from previous loop iteration.
bool previousPirLevel = false;

// Timestamp of most recent accepted trigger.
unsigned long lastTriggerTimeMs = 0;

// Timestamp of trigger accepted for debounce/gap control.
unsigned long lastAcceptedTriggerTimeMs = 0;

// Circular buffer to store recent trigger times.
const uint8_t MAX_STORED_TRIGGERS = 32;
unsigned long triggerTimestampsMs[MAX_STORED_TRIGGERS] = {0};
uint8_t triggerWritePosition = 0;

// Device state variables (for clarity and serial debugging).
bool lightsOn = false;
bool airConOn = false;
bool tvOn = false;
bool fanOn = false;

// Print status every 1 second (so serial monitor is readable).
unsigned long lastStatusPrintMs = 0;
const unsigned long STATUS_PRINT_INTERVAL_MS = 1000UL;

// -------------------- HELPER FUNCTION: WRITE OUTPUT --------------------
// This function writes an ON/OFF command to a pin, respecting relay polarity.
void writeOutputPin(int pinNumber, bool shouldBeOn) {
  int outputLevel;

  if (RELAY_ACTIVE_LOW) {
    // Active-low means LOW = ON, HIGH = OFF.
    outputLevel = shouldBeOn ? LOW : HIGH;
  } else {
    // Active-high means HIGH = ON, LOW = OFF.
    outputLevel = shouldBeOn ? HIGH : LOW;
  }

  digitalWrite(pinNumber, outputLevel);
}

// -------------------- HELPER FUNCTION: COUNT RECENT TRIGGERS --------------------
// Counts how many stored trigger timestamps are inside TRIGGER_WINDOW_MS.
uint8_t countRecentTriggers(unsigned long nowMs) {
  uint8_t triggerCount = 0;

  for (uint8_t index = 0; index < MAX_STORED_TRIGGERS; index++) {
    unsigned long savedTimestamp = triggerTimestampsMs[index];

    // 0 means slot is empty (never used yet).
    if (savedTimestamp == 0) {
      continue;
    }

    // If timestamp is recent enough, count it.
    if ((nowMs - savedTimestamp) <= TRIGGER_WINDOW_MS) {
      triggerCount++;
    }
  }

  return triggerCount;
}

// -------------------- HELPER FUNCTION: OCCUPANCY DECISION --------------------
// Returns true if last trigger is recent.
bool isRoomOccupied(unsigned long nowMs) {
  // If we never got a trigger, room is not occupied.
  if (lastTriggerTimeMs == 0) {
    return false;
  }

  // Occupied if last trigger is within timeout.
  return ((nowMs - lastTriggerTimeMs) <= OCCUPIED_TIMEOUT_MS);
}

// -------------------- HELPER FUNCTION: APPLY AUTOMATION --------------------
void applyDeviceLogic(unsigned long nowMs) {
  bool occupied = isRoomOccupied(nowMs);
  uint8_t recentTriggerCount = countRecentTriggers(nowMs);

  // AC rule: only ON if occupied AND activity is high enough.
  bool shouldUseAirCon = occupied && (recentTriggerCount >= AC_TRIGGER_THRESHOLD);

  // Base occupied behavior.
  lightsOn = occupied;
  tvOn = occupied;
  fanOn = occupied;
  airConOn = shouldUseAirCon;

  // Write final states to output pins.
  writeOutputPin(LIGHT_PIN, lightsOn);
  writeOutputPin(TV_PIN, tvOn);
  writeOutputPin(FAN_PIN, fanOn);
  writeOutputPin(AC_PIN, airConOn);
}

// -------------------- HELPER FUNCTION: SERIAL STATUS --------------------
void printStatus(unsigned long nowMs) {
  bool occupied = isRoomOccupied(nowMs);
  uint8_t recentTriggerCount = countRecentTriggers(nowMs);
  bool pirLevelNow = (digitalRead(PIR_PIN) == HIGH);

  Serial.println("---------------- STATUS ----------------");
  Serial.print("PIR Level: ");
  Serial.println(pirLevelNow ? "HIGH (motion signal)" : "LOW (no motion signal)");

  Serial.print("Occupied: ");
  Serial.println(occupied ? "YES" : "NO");

  Serial.print("Recent triggers: ");
  Serial.println(recentTriggerCount);

  Serial.print("Last trigger age (ms): ");
  if (lastTriggerTimeMs == 0) {
    Serial.println("No trigger yet");
  } else {
    Serial.println(nowMs - lastTriggerTimeMs);
  }

  Serial.print("Lights: "); Serial.println(lightsOn ? "ON" : "OFF");
  Serial.print("Fan: ");    Serial.println(fanOn ? "ON" : "OFF");
  Serial.print("TV: ");     Serial.println(tvOn ? "ON" : "OFF");
  Serial.print("Air Con: ");Serial.println(airConOn ? "ON" : "OFF");
}

// -------------------- ARDUINO SETUP --------------------
void setup() {
  // Start serial monitor output for debugging and learning.
  Serial.begin(115200);
  delay(300);

  Serial.println("Starting Basic Occupancy Logic (No Wi-Fi, No Server)...");
  Serial.println("Target board: ESP32-C3");

  // Configure PIR input pin.
  pinMode(PIR_PIN, INPUT);

  // Configure output pins.
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(AC_PIN, OUTPUT);
  pinMode(TV_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Start safely with everything OFF.
  writeOutputPin(LIGHT_PIN, false);
  writeOutputPin(AC_PIN, false);
  writeOutputPin(TV_PIN, false);
  writeOutputPin(FAN_PIN, false);

  Serial.println("Setup complete. Waiting for PIR motion triggers...");
}

// -------------------- ARDUINO LOOP --------------------
void loop() {
  unsigned long nowMs = millis();

  // Read current PIR signal.
  bool currentPirLevel = (digitalRead(PIR_PIN) == HIGH);

  // Detect rising edge (LOW -> HIGH), which means a new motion trigger event.
  bool isRisingEdge = (currentPirLevel && !previousPirLevel);

  if (isRisingEdge) {
    // Apply minimum time gap so one long pulse is not counted repeatedly.
    if ((nowMs - lastAcceptedTriggerTimeMs) >= MIN_TRIGGER_GAP_MS) {
      lastAcceptedTriggerTimeMs = nowMs;
      lastTriggerTimeMs = nowMs;

      // Save this trigger time in circular buffer.
      triggerTimestampsMs[triggerWritePosition] = nowMs;
      triggerWritePosition = (triggerWritePosition + 1) % MAX_STORED_TRIGGERS;

      Serial.println("Motion trigger accepted.");
    }
  }

  // Save PIR state for next loop's edge detection.
  previousPirLevel = currentPirLevel;

  // Apply occupancy + device control logic.
  applyDeviceLogic(nowMs);

  // Print status every 1 second.
  if ((nowMs - lastStatusPrintMs) >= STATUS_PRINT_INTERVAL_MS) {
    lastStatusPrintMs = nowMs;
    printStatus(nowMs);
  }

  // Short delay to keep loop stable and reduce CPU churn.
  delay(10);
}
