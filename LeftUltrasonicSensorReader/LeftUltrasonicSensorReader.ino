#include <Arduino.h>

#define ENA 5
#define ENB 10
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9

#define LEFT_TRIG 2
#define LEFT_ECHO 3

const int MOTOR_SPEED = 220;
const long STOP_DISTANCE_CM = 15;
const long SECOND_STOP_DISTANCE_CM = 25;
const unsigned long SENSOR_TIMEOUT_US = 30000;
const long NO_ECHO_DISTANCE_CM = 300;
const long MAX_VALID_DISTANCE_CM = 250;
const unsigned long READ_INTERVAL_MS = 20;
const unsigned long SENSOR_READ_INTERVAL_MS = 60;
const unsigned long SECOND_FORWARD_DURATION_MS = 200;
const unsigned long CLOCKWISE_DURATION_MS = 400;
const unsigned long THIRD_FORWARD_DURATION_MS = 600;
const unsigned long ANTICLOCKWISE_DURATION_MS = 400;

const bool LEFT_MOTOR_FORWARD_HIGH = false;
const bool RIGHT_MOTOR_FORWARD_HIGH = false;

enum ParkingState {
  INITIAL_APPROACH,
  SECOND_FORWARD,
  CLOCKWISE_ROTATION,
  THIRD_FORWARD,
  ANTICLOCKWISE_ROTATION,
  FINAL_STOP
};

ParkingState currentState = INITIAL_APPROACH;
unsigned long stateStartTimeMs = 0;
unsigned long lastEchoDurationUs = 0;
unsigned long lastSensorReadTimeMs = 0;
long lastLeftDistanceCm = NO_ECHO_DISTANCE_CM;

void setLeftMotor(bool forward) {
  const bool driveHigh = forward ? LEFT_MOTOR_FORWARD_HIGH : !LEFT_MOTOR_FORWARD_HIGH;
  digitalWrite(IN1, driveHigh ? LOW : HIGH);
  digitalWrite(IN2, driveHigh ? HIGH : LOW);
}

void setRightMotor(bool forward) {
  const bool driveHigh = forward ? RIGHT_MOTOR_FORWARD_HIGH : !RIGHT_MOTOR_FORWARD_HIGH;
  digitalWrite(IN3, driveHigh ? LOW : HIGH);
  digitalWrite(IN4, driveHigh ? HIGH : LOW);
}

void moveForward() {
  setLeftMotor(true);
  setRightMotor(true);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
}

void rotateClockwise() {
  setLeftMotor(false);
  setRightMotor(true);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
}

void rotateAnticlockwise() {
  setLeftMotor(true);
  setRightMotor(false);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

long readLeftDistanceCm() {
  digitalWrite(LEFT_TRIG, LOW);
  delayMicroseconds(10);
  digitalWrite(LEFT_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(LEFT_TRIG, LOW);

  lastEchoDurationUs = pulseIn(LEFT_ECHO, HIGH, SENSOR_TIMEOUT_US);
  if (lastEchoDurationUs == 0) {
    return NO_ECHO_DISTANCE_CM;
  }

  return static_cast<long>(lastEchoDurationUs * 0.0343f / 2.0f);
}

long getLeftDistanceCm() {
  const unsigned long now = millis();
  if (now - lastSensorReadTimeMs >= SENSOR_READ_INTERVAL_MS) {
    lastLeftDistanceCm = readLeftDistanceCm();
    lastSensorReadTimeMs = now;
  }

  return lastLeftDistanceCm;
}

bool isObstacleDetected(long distanceCm) {
  if (distanceCm <= 0) {
    return true;
  }

  if (distanceCm >= MAX_VALID_DISTANCE_CM || distanceCm == NO_ECHO_DISTANCE_CM) {
    return false;
  }

  return distanceCm <= STOP_DISTANCE_CM;
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  stopMotors();
  digitalWrite(LEFT_TRIG, LOW);
  stateStartTimeMs = millis();
  lastSensorReadTimeMs = 0;
  lastLeftDistanceCm = readLeftDistanceCm();

  delay(2000);
  Serial.println("Parking sequence ready.");
}

void loop() {
  const long leftDistanceCm = getLeftDistanceCm();

  switch (currentState) {
    case INITIAL_APPROACH:
      if (!isObstacleDetected(leftDistanceCm)) {
        moveForward();
        Serial.print("Initial approach, left distance: ");
        Serial.print(leftDistanceCm);
        Serial.println(" cm");
      } else {
        stopMotors();
        currentState = SECOND_FORWARD;
        stateStartTimeMs = millis();
        Serial.print("Initial stop at ");
        Serial.print(leftDistanceCm);
        Serial.println(" cm");
      }
      break;

    case SECOND_FORWARD:
      if (leftDistanceCm > SECOND_STOP_DISTANCE_CM || millis() - stateStartTimeMs >= SECOND_FORWARD_DURATION_MS) {
        stopMotors();
        currentState = CLOCKWISE_ROTATION;
        stateStartTimeMs = millis();
        Serial.print("Second stop at ");
        Serial.print(leftDistanceCm);
        Serial.println(" cm");
      } else {
        moveForward();
        Serial.print("Second forward step, left distance: ");
        Serial.print(leftDistanceCm);
        Serial.println(" cm");
      }
      break;

    case CLOCKWISE_ROTATION:
      if (millis() - stateStartTimeMs >= CLOCKWISE_DURATION_MS) {
        stopMotors();
        currentState = THIRD_FORWARD;
        stateStartTimeMs = millis();
        Serial.println("Clockwise rotation complete.");
      } else {
        rotateClockwise();
        Serial.println("Rotating clockwise.");
      }
      break;

    case THIRD_FORWARD:
      if (millis() - stateStartTimeMs >= THIRD_FORWARD_DURATION_MS) {
        stopMotors();
        currentState = ANTICLOCKWISE_ROTATION;
        stateStartTimeMs = millis();
        Serial.println("Third forward step complete.");
      } else {
        moveForward();
        Serial.println("Third forward step.");
      }
      break;

    case ANTICLOCKWISE_ROTATION:
      if (millis() - stateStartTimeMs >= ANTICLOCKWISE_DURATION_MS) {
        stopMotors();
        currentState = FINAL_STOP;
        Serial.println("Anticlockwise rotation complete.");
      } else {
        rotateAnticlockwise();
        Serial.println("Rotating anticlockwise.");
      }
      break;

    case FINAL_STOP:
      stopMotors();
      Serial.print("Final stop, left distance: ");
      Serial.print(leftDistanceCm);
      Serial.println(" cm");
      break;
  }

  delay(READ_INTERVAL_MS);
}