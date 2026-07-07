#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define TRIG_PIN 6
#define ECHO_PIN 7
#define SERVO_PIN 8

const long DETECT_DIST_CM = 20;
const unsigned long OPEN_DELAY_MS = 100;
const unsigned long CLOSE_DELAY_MS = 3000;
const unsigned long SENSOR_TIMEOUT_US = 30000;
const long NO_ECHO_DISTANCE_CM = 300;
const unsigned long LOOP_DELAY_MS = 100;

const int GATE_CLOSED_ANGLE = 0;
const int GATE_OPEN_ANGLE = 90;

Servo gateServo;

bool gateOpen = false;
bool waitingToOpen = false;
bool objectPreviouslyDetected = false;

unsigned long detectStartTimeMs = 0;
unsigned long lastDetectedTimeMs = 0;

int carCount = 0;
int displayedCarCount = -1;
bool displayedGateOpen = false;

long getDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long durationUs = pulseIn(ECHO_PIN, HIGH, SENSOR_TIMEOUT_US);
  if (durationUs == 0) {
    return NO_ECHO_DISTANCE_CM;
  }

  return static_cast<long>(durationUs * 0.0343f / 2.0f);
}

void updateDisplay() {
  if (displayedCarCount == carCount && displayedGateOpen == gateOpen) {
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Cars: ");
  lcd.print(carCount);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  if (gateOpen) {
    lcd.print("Gate: OPEN   ");
  } else {
    lcd.print("Gate: CLOSED ");
  }

  displayedCarCount = carCount;
  displayedGateOpen = gateOpen;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(GATE_CLOSED_ANGLE);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Parking System");
  delay(1500);
  lcd.clear();

  updateDisplay();
}

void loop() {
  const long distanceCm = getDistanceCm();
  const bool objectDetected = distanceCm < DETECT_DIST_CM;

  Serial.print("Distance: ");
  Serial.println(distanceCm);

  if (objectDetected && !objectPreviouslyDetected) {
    carCount++;
  }
  objectPreviouslyDetected = objectDetected;

  if (objectDetected) {
    lastDetectedTimeMs = millis();

    if (!waitingToOpen && !gateOpen) {
      detectStartTimeMs = millis();
      waitingToOpen = true;
    }
  }

  if (waitingToOpen && !gateOpen && millis() - detectStartTimeMs > OPEN_DELAY_MS) {
    gateServo.write(GATE_OPEN_ANGLE);
    gateOpen = true;
    waitingToOpen = false;
  }

  if (!objectDetected && !gateOpen) {
    waitingToOpen = false;
  }

  if (gateOpen && millis() - lastDetectedTimeMs > CLOSE_DELAY_MS) {
    gateServo.write(GATE_CLOSED_ANGLE);
    gateOpen = false;
  }

  updateDisplay();
  delay(LOOP_DELAY_MS);
}