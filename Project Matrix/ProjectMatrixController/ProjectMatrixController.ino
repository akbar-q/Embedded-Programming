#include <Arduino.h>

constexpr uint8_t BUZZER_PIN = 22;
constexpr uint8_t LED_RED_PIN = 4;
constexpr uint8_t LED_GREEN_PIN = 16;
constexpr uint8_t LED_BLUE_PIN = 17;
constexpr uint8_t MOTOR_ENABLE_PIN = 27;
constexpr uint8_t MOTOR_IN4_PIN = 26;
constexpr uint8_t MOTOR_IN3_PIN = 25;
constexpr uint8_t ULTRASONIC_ECHO_PIN = 12;
constexpr uint8_t ULTRASONIC_TRIGGER_PIN = 14;
constexpr uint8_t LIMIT_SWITCH_PIN = 15;

constexpr uint8_t MOTOR_SPEED = 190;
constexpr float MID_DESCENT_DISTANCE_CM = 110.0F;
constexpr float FINAL_DESCENT_DISTANCE_CM = 200.0F;
constexpr uint16_t ULTRASONIC_INTERVAL_MS = 100;
constexpr uint32_t ULTRASONIC_TIMEOUT_US = 30000;
constexpr uint32_t LIMIT_SWITCH_WAIT_MS = 11000;
constexpr uint32_t MID_DESCENT_WAIT_MS = 2000;
constexpr uint8_t SENSOR_FAILURE_LIMIT = 3;

enum class ClimberState {
  ASCENDING,
  LIMIT_SWITCH_WAIT,
  DESCENDING_TO_MIDPOINT,
  MID_DESCENT_WAIT,
  DESCENDING_TO_FINAL,
  COMPLETE,
  SENSOR_FAULT
};

ClimberState climberState = ClimberState::ASCENDING;
uint32_t stateStartTime = 0;
uint32_t lastUltrasonicReadTime = 0;
uint32_t lastAlertTime = 0;
float distanceCm = 0.0F;
bool hasDistanceReading = false;
uint8_t consecutiveSensorFailures = 0;

void setStatusLed(bool red, bool green, bool blue) {
  digitalWrite(LED_RED_PIN, red ? LOW : HIGH);
  digitalWrite(LED_GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(LED_BLUE_PIN, blue ? LOW : HIGH);
}

void stopMotor() {
  analogWrite(MOTOR_ENABLE_PIN, 0);
  digitalWrite(MOTOR_IN3_PIN, LOW);
  digitalWrite(MOTOR_IN4_PIN, LOW);
}

void climbUp() {
  digitalWrite(MOTOR_IN3_PIN, HIGH);
  digitalWrite(MOTOR_IN4_PIN, LOW);
  analogWrite(MOTOR_ENABLE_PIN, MOTOR_SPEED);
}

void climbDown() {
  digitalWrite(MOTOR_IN3_PIN, LOW);
  digitalWrite(MOTOR_IN4_PIN, HIGH);
  analogWrite(MOTOR_ENABLE_PIN, MOTOR_SPEED);
}

void setClimberState(ClimberState newState) {
  climberState = newState;
  stateStartTime = millis();

  if (climberState == ClimberState::ASCENDING) {
    climbUp();
    setStatusLed(false, true, false);
  } else if (climberState == ClimberState::DESCENDING_TO_MIDPOINT ||
             climberState == ClimberState::DESCENDING_TO_FINAL) {
    climbDown();
    setStatusLed(false, false, true);
  } else if (climberState == ClimberState::LIMIT_SWITCH_WAIT ||
             climberState == ClimberState::MID_DESCENT_WAIT) {
    stopMotor();
    setStatusLed(true, true, false);
  } else if (climberState == ClimberState::COMPLETE) {
    stopMotor();
    setStatusLed(true, false, false);
  } else if (climberState == ClimberState::SENSOR_FAULT) {
    stopMotor();
    setStatusLed(true, true, false);
  }
}

bool readDistanceCm(float &measuredDistanceCm) {
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

  uint32_t echoDurationUs = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRASONIC_TIMEOUT_US);
  if (echoDurationUs == 0) {
    return false;
  }

  measuredDistanceCm = echoDurationUs * 0.0343F / 2.0F;
  return measuredDistanceCm > 0.0F;
}

void updateDistance() {
  if (millis() - lastUltrasonicReadTime < ULTRASONIC_INTERVAL_MS) {
    return;
  }

  lastUltrasonicReadTime = millis();
  float measuredDistanceCm = 0.0F;
  if (readDistanceCm(measuredDistanceCm)) {
    distanceCm = measuredDistanceCm;
    hasDistanceReading = true;
    consecutiveSensorFailures = 0;
  } else {
    consecutiveSensorFailures++;
  }
}

void updateController() {
  if ((climberState == ClimberState::DESCENDING_TO_MIDPOINT ||
       climberState == ClimberState::DESCENDING_TO_FINAL) &&
      consecutiveSensorFailures >= SENSOR_FAILURE_LIMIT) {
    setClimberState(ClimberState::SENSOR_FAULT);
    return;
  }

  if (climberState == ClimberState::ASCENDING && digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    setClimberState(ClimberState::LIMIT_SWITCH_WAIT);
  } else if (climberState == ClimberState::LIMIT_SWITCH_WAIT &&
             millis() - stateStartTime >= LIMIT_SWITCH_WAIT_MS) {
    setClimberState(ClimberState::DESCENDING_TO_MIDPOINT);
  } else if (climberState == ClimberState::DESCENDING_TO_MIDPOINT && hasDistanceReading &&
             distanceCm >= MID_DESCENT_DISTANCE_CM) {
    setClimberState(ClimberState::MID_DESCENT_WAIT);
  } else if (climberState == ClimberState::MID_DESCENT_WAIT &&
             millis() - stateStartTime >= MID_DESCENT_WAIT_MS) {
    setClimberState(ClimberState::DESCENDING_TO_FINAL);
  } else if (climberState == ClimberState::DESCENDING_TO_FINAL && hasDistanceReading &&
             distanceCm >= FINAL_DESCENT_DISTANCE_CM) {
    setClimberState(ClimberState::COMPLETE);
  }
}

void updateBuzzer() {
  uint32_t currentTime = millis();
  uint16_t alertIntervalMs = 0;
  uint16_t toneDurationMs = 0;

  if (climberState == ClimberState::COMPLETE) {
    alertIntervalMs = 1000;
    toneDurationMs = 140;
  } else if (climberState == ClimberState::SENSOR_FAULT) {
    alertIntervalMs = 400;
    toneDurationMs = 100;
  }

  if (alertIntervalMs > 0 && currentTime - lastAlertTime >= alertIntervalMs) {
    lastAlertTime = currentTime;
    tone(BUZZER_PIN, climberState == ClimberState::COMPLETE ? 1200 : 500, toneDurationMs);
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_IN3_PIN, OUTPUT);
  pinMode(MOTOR_IN4_PIN, OUTPUT);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  setClimberState(ClimberState::ASCENDING);
}

void loop() {
  updateDistance();
  updateController();
  updateBuzzer();
}