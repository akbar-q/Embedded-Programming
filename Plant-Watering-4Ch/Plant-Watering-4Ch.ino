#include <Arduino.h>

/*
  4-Channel Plant Watering System
  --------------------------------
  - Soil moisture sensors on A0, A1, A2, A3
  - Relay outputs on pins 8, 9, 10, 11
  - Only one channel is checked at a time
  - Only one pump is allowed to run at a time
  - Each pump has a 5 second maximum run time

  Notes:
  - This sketch is written to stay simple and easy to edit.
  - Many relay boards are ACTIVE LOW. That means writing LOW turns the relay ON.
  - If your relay board works the opposite way, swap RELAY_ON and RELAY_OFF below.
  - The moisture threshold may need adjusting for your sensors.
*/

// -----------------------------
// User settings
// -----------------------------
const int moistureThreshold = 700;
const unsigned long pumpRunTimeMs = 5000;
const unsigned long gapBetweenFramesMs = 1000;

// Change these two lines if your relay module logic is reversed.
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// Soil moisture sensor pins
const int sensorPin1 = A0;
const int sensorPin2 = A1;
const int sensorPin3 = A2;
const int sensorPin4 = A3;

// Relay pins for the pumps
const int relayPin1 = 8;
const int relayPin2 = 9;
const int relayPin3 = 10;
const int relayPin4 = 11;

// This variable keeps track of which frame runs next.
// 0 = Channel 1, 1 = Channel 2, 2 = Channel 3, 3 = Channel 4
int currentFrame = 0;

void setup()
{
  Serial.begin(9600);

  // Set relay pins as outputs.
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Make sure all pumps are OFF when the board starts.
  digitalWrite(relayPin1, RELAY_OFF);
  digitalWrite(relayPin2, RELAY_OFF);
  digitalWrite(relayPin3, RELAY_OFF);
  digitalWrite(relayPin4, RELAY_OFF);

  Serial.println("========================================");
  Serial.println("4-Channel Plant Watering System Starting");
  Serial.println("Sensors: A0, A1, A2, A3");
  Serial.println("Relays : 8, 9, 10, 11");
  Serial.print("Moisture threshold: ");
  Serial.println(moistureThreshold);
  Serial.print("Pump timeout (ms): ");
  Serial.println(pumpRunTimeMs);
  Serial.println("Only one channel is checked at a time.");
  Serial.println("Only one pump is allowed to run at a time.");
  Serial.println("========================================");
}

void loop()
{
  // The code is split into 4 simple frames.
  // Each pass through loop() processes only one frame.
  switch (currentFrame)
  {
    case 0:
      Serial.println();
      Serial.println("----- FRAME 1: Checking Channel 1 -----");
      checkAndWaterChannel(1, sensorPin1, relayPin1);
      currentFrame = 1;
      break;

    case 1:
      Serial.println();
      Serial.println("----- FRAME 2: Checking Channel 2 -----");
      checkAndWaterChannel(2, sensorPin2, relayPin2);
      currentFrame = 2;
      break;

    case 2:
      Serial.println();
      Serial.println("----- FRAME 3: Checking Channel 3 -----");
      checkAndWaterChannel(3, sensorPin3, relayPin3);
      currentFrame = 3;
      break;

    case 3:
      Serial.println();
      Serial.println("----- FRAME 4: Checking Channel 4 -----");
      checkAndWaterChannel(4, sensorPin4, relayPin4);
      currentFrame = 0;
      break;
  }

  // Small gap before moving to the next frame.
  delay(gapBetweenFramesMs);
}

void checkAndWaterChannel(int channelNumber, int sensorPin, int relayPin)
{
  int sensorValue = analogRead(sensorPin);

  Serial.print("Channel ");
  Serial.print(channelNumber);
  Serial.print(" sensor reading: ");
  Serial.println(sensorValue);

  Serial.print("Channel ");
  Serial.print(channelNumber);
  Serial.print(" threshold check: reading ");
  Serial.print(sensorValue);
  Serial.print(" > ");
  Serial.println(moistureThreshold);

  // The user asked for watering when moisture is low.
  // This sketch currently assumes a higher analog value means the soil is drier.
  // If your sensor behaves the opposite way, change > to < below.
  if (sensorValue > moistureThreshold)
  {
    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.println(" is DRY. Pump ON.");

    digitalWrite(relayPin, RELAY_ON);

    unsigned long pumpStartTime = millis();
    while (millis() - pumpStartTime < pumpRunTimeMs)
    {
      unsigned long elapsedSeconds = (millis() - pumpStartTime) / 1000;

      Serial.print("Channel ");
      Serial.print(channelNumber);
      Serial.print(" watering... ");
      Serial.print(elapsedSeconds);
      Serial.println(" second(s) elapsed");

      delay(1000);
    }

    digitalWrite(relayPin, RELAY_OFF);

    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.println(" pump timeout reached. Pump OFF.");
    Serial.println("Moving to next channel.");
  }
  else
  {
    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.println(" moisture is OK. Pump stays OFF.");
    Serial.println("Moving to next channel.");
  }
}