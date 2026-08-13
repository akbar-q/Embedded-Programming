#include <Arduino.h>
#include <stdlib.h>

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
const float pumpFlowRateMlPerSecond = 10.303;
const byte numberOfChannels = 4;
const unsigned long millisecondsPerDay = 86400000UL;

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

struct DailyUsageRecord
{
  unsigned long dayNumber;
  float usageMl[numberOfChannels];
  DailyUsageRecord *next;
};

// Water-use totals for the current day and a growing history of completed days.
// These values reset if the Arduino is switched off or reset.
float currentDayUsageMl[numberOfChannels] = {0, 0, 0, 0};
DailyUsageRecord *firstCompletedDay = NULL;
DailyUsageRecord *lastCompletedDay = NULL;
unsigned long currentDayNumber = 1;
unsigned long currentDayStartTime = 0;
bool dailyHistoryStorageFull = false;

void checkForNewDay();
void printUsageStatistics();
void printDayUsage(unsigned long dayNumber, const float usageMl[]);
void turnAllPumpsOff();

void setup()
{
  Serial.begin(9600);

  // Set relay pins as outputs.
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Make sure all pumps are OFF when the board starts.
  turnAllPumpsOff();

  Serial.println("========================================");
  Serial.println("4-Channel Plant Watering System Starting");
  Serial.println("Sensors: A0, A1, A2, A3");
  Serial.println("Relays : 8, 9, 10, 11");
  Serial.print("Moisture threshold: ");
  Serial.println(moistureThreshold);
  Serial.print("Pump timeout (ms): ");
  Serial.println(pumpRunTimeMs);
  Serial.print("Pump flow rate (mL/s): ");
  Serial.println(pumpFlowRateMlPerSecond, 3);
  Serial.println("Only one channel is checked at a time.");
  Serial.println("Only one pump is allowed to run at a time.");
  Serial.println("========================================");

  currentDayStartTime = millis();
}

void loop()
{
  checkForNewDay();

  // The code is split into 4 simple frames.
  // Each pass through loop() processes only one frame.
  switch (currentFrame)
  {
    case 0:
      Serial.println();
      Serial.println("----- FRAME 1: Checking Channel 1 -----");
      checkAndWaterChannel(1, sensorPin1, relayPin1, 0);
      currentFrame = 1;
      break;

    case 1:
      Serial.println();
      Serial.println("----- FRAME 2: Checking Channel 2 -----");
      checkAndWaterChannel(2, sensorPin2, relayPin2, 1);
      currentFrame = 2;
      break;

    case 2:
      Serial.println();
      Serial.println("----- FRAME 3: Checking Channel 3 -----");
      checkAndWaterChannel(3, sensorPin3, relayPin3, 2);
      currentFrame = 3;
      break;

    case 3:
      Serial.println();
      Serial.println("----- FRAME 4: Checking Channel 4 -----");
      checkAndWaterChannel(4, sensorPin4, relayPin4, 3);
      currentFrame = 0;
      break;
  }

  // Small gap before moving to the next frame.
  delay(gapBetweenFramesMs);
}

void checkAndWaterChannel(int channelNumber, int sensorPin, int relayPin, byte channelIndex)
{
  // This prevents any previously selected relay from staying on.
  turnAllPumpsOff();

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
    Serial.println(" is DRY. Pump ON for 5 seconds.");

    digitalWrite(relayPin, RELAY_ON);

    unsigned long pumpStartTime = millis();

    // Keep this selected pump on for the complete 5-second watering cycle.
    // The sensor is not checked again until the next visit to this channel.
    while (millis() - pumpStartTime < pumpRunTimeMs)
    {
      unsigned long elapsedMs = millis() - pumpStartTime;
      unsigned long remainingPumpTimeMs = pumpRunTimeMs - elapsedMs;
      unsigned long waitTimeMs = 1000;

      // Do not let the final status delay extend the 5-second run time.
      if (waitTimeMs > remainingPumpTimeMs)
      {
        waitTimeMs = remainingPumpTimeMs;
      }

      Serial.print("Channel ");
      Serial.print(channelNumber);
      Serial.print(" watering... elapsed: ");
      Serial.print(elapsedMs);
      Serial.println(" ms");

      delay(waitTimeMs);
    }

    digitalWrite(relayPin, RELAY_OFF);

    unsigned long actualPumpRunTimeMs = millis() - pumpStartTime;
    float waterUsedMl = (actualPumpRunTimeMs / 1000.0) * pumpFlowRateMlPerSecond;
    currentDayUsageMl[channelIndex] += waterUsedMl;

    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.print(" pump run time: ");
    Serial.print(actualPumpRunTimeMs);
    Serial.println(" ms");

    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.print(" water used this run: ");
    Serial.print(waterUsedMl, 2);
    Serial.println(" mL");

    Serial.print("Channel ");
    Serial.print(channelNumber);
    Serial.println(" completed its 5 second watering cycle. Pump OFF.");

    printUsageStatistics();
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

void turnAllPumpsOff()
{
  digitalWrite(relayPin1, RELAY_OFF);
  digitalWrite(relayPin2, RELAY_OFF);
  digitalWrite(relayPin3, RELAY_OFF);
  digitalWrite(relayPin4, RELAY_OFF);
}

void checkForNewDay()
{
  if (millis() - currentDayStartTime < millisecondsPerDay)
  {
    return;
  }

  // Preserve each completed day until the Arduino runs out of available RAM.
  if (!dailyHistoryStorageFull)
  {
    DailyUsageRecord *newDay = (DailyUsageRecord *)malloc(sizeof(DailyUsageRecord));

    if (newDay == NULL)
    {
      dailyHistoryStorageFull = true;
      Serial.println("WARNING: Daily history memory is full.");
      Serial.println("New daily totals will still be shown, but cannot be saved for later reports.");
    }
    else
    {
      newDay->dayNumber = currentDayNumber;
      newDay->next = NULL;

      for (byte channelIndex = 0; channelIndex < numberOfChannels; channelIndex++)
      {
        newDay->usageMl[channelIndex] = currentDayUsageMl[channelIndex];
      }

      if (firstCompletedDay == NULL)
      {
        firstCompletedDay = newDay;
      }
      else
      {
        lastCompletedDay->next = newDay;
      }

      lastCompletedDay = newDay;
    }
  }

  for (byte channelIndex = 0; channelIndex < numberOfChannels; channelIndex++)
  {
    currentDayUsageMl[channelIndex] = 0;
  }

  currentDayStartTime = millis();
  currentDayNumber++;
  Serial.println();
  Serial.println("========== NEW DAY: USAGE REPORT ==========");
  printUsageStatistics();
}

void printUsageStatistics()
{
  Serial.println("---------- WATER USAGE STATISTICS ----------");

  DailyUsageRecord *storedDay = firstCompletedDay;
  while (storedDay != NULL)
  {
    printDayUsage(storedDay->dayNumber, storedDay->usageMl);
    storedDay = storedDay->next;
  }

  printDayUsage(currentDayNumber, currentDayUsageMl);
  Serial.println("--------------------------------------------");
}

void printDayUsage(unsigned long dayNumber, const float usageMl[])
{
  float totalUsageMl = 0;

  Serial.print("Day ");
  Serial.print(dayNumber);
  Serial.print(": ");

  for (byte channelIndex = 0; channelIndex < numberOfChannels; channelIndex++)
  {
    Serial.print("Plant ");
    Serial.print(channelIndex + 1);
    Serial.print(" = ");
    Serial.print(usageMl[channelIndex], 2);
    Serial.print(" mL; ");
    totalUsageMl += usageMl[channelIndex];
  }

  Serial.print("Total = ");
  Serial.print(totalUsageMl, 2);
  Serial.println(" mL");
}