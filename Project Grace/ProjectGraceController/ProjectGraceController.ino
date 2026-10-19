#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <esp_system.h>
#include <math.h>

constexpr uint8_t BUTTON_PIN = 21;
constexpr uint8_t MOTOR_IN1_PIN = 3;
constexpr uint8_t MOTOR_IN2_PIN = 2;
constexpr uint8_t LED_RING_PIN = 4;
constexpr uint8_t BUZZER_PIN = 5;
constexpr uint8_t SERVO_PIN = 6;
constexpr uint8_t LED_COUNT = 12;
constexpr uint8_t SERVO_START_ANGLE = 170;
constexpr uint8_t SERVO_FINAL_ANGLE = 10;

constexpr uint16_t IDLE_FRAME_INTERVAL_MS = 90;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 25;
constexpr uint32_t FORWARD_DURATION_MS = 9500;
constexpr uint32_t STOP_DURATION_MS = 10900;
constexpr uint32_t REVERSE_DURATION_MS = 2750;
constexpr uint32_t FINAL_SERVO_HOLD_MS = 500;
constexpr uint32_t FINAL_SERVO_DESCENT_DURATION_MS = 3750;
constexpr uint16_t STOP_BEEP_INTERVAL_MS = 1000;
constexpr uint16_t STOP_BEEP_DURATION_MS = 160;
constexpr uint16_t IDLE_NOTE_GAP_MS = 45;

Adafruit_NeoPixel ledRing(LED_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
Servo armServo;

enum class MotorState {
  IDLE,
  FORWARD,
  STOPPED,
  REVERSE,
  FINAL_SERVO_HOLD,
  FINAL_SERVO_DESCENT
};

constexpr uint16_t IDLE_MELODY[] = {523, 659, 784, 659, 587, 698, 880, 698};
constexpr uint16_t IDLE_NOTE_DURATIONS_MS[] = {180, 180, 260, 180, 180, 180, 300, 360};
constexpr uint8_t IDLE_MELODY_LENGTH = sizeof(IDLE_MELODY) / sizeof(IDLE_MELODY[0]);
constexpr uint16_t IDLE_NEON_PALETTE[] = {0, 9500, 21845, 32768, 43690, 54613};
constexpr uint8_t IDLE_NEON_COLOUR_COUNT = sizeof(IDLE_NEON_PALETTE) / sizeof(IDLE_NEON_PALETTE[0]);

MotorState motorState = MotorState::IDLE;
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
uint32_t lastDebounceTime = 0;
uint32_t lastIdleFrameTime = 0;
uint32_t motorStateStartTime = 0;
uint32_t lastStopBeepTime = 0;
uint32_t idleNoteStartTime = 0;
uint8_t idleNoteIndex = 0;

void setMotorState(MotorState newState) {
  motorState = newState;
  motorStateStartTime = millis();

  if (motorState == MotorState::FORWARD) {
    armServo.write(SERVO_START_ANGLE);
  } else if (motorState == MotorState::FINAL_SERVO_HOLD) {
    armServo.write(SERVO_FINAL_ANGLE);
  }

  if (motorState == MotorState::FORWARD) {
    digitalWrite(MOTOR_IN1_PIN, HIGH);
    digitalWrite(MOTOR_IN2_PIN, LOW);
  } else if (motorState == MotorState::REVERSE || motorState == MotorState::FINAL_SERVO_DESCENT) {
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, HIGH);
  } else {
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);
  }
}

void updateServoDescent() {
  if (motorState != MotorState::FINAL_SERVO_DESCENT) {
    return;
  }

  uint32_t elapsedTime = millis() - motorStateStartTime;
  int16_t angleRange = static_cast<int16_t>(SERVO_START_ANGLE) - SERVO_FINAL_ANGLE;
  uint8_t servoAngle = SERVO_FINAL_ANGLE + angleRange * elapsedTime / FINAL_SERVO_DESCENT_DURATION_MS;
  armServo.write(servoAngle);
}

void showMotorRunning() {
  ledRing.fill(ledRing.Color(0, 255, 0));
  ledRing.show();
}

void showMotorStopped() {
  ledRing.fill(ledRing.Color(255, 0, 0));
  ledRing.show();
}

void showIdleAnimation() {
  static uint8_t paletteOffset = 0;
  static uint8_t pulse = 0;

  for (uint8_t pixel = 0; pixel < LED_COUNT; pixel++) {
    uint16_t hue = IDLE_NEON_PALETTE[(pixel + paletteOffset) % IDLE_NEON_COLOUR_COUNT];
    float angle = (pixel * 21 + pulse) * TWO_PI / 256.0F;
    uint8_t brightness = 18 + (sin(angle) + 1.0F) * 24;
    ledRing.setPixelColor(pixel, ledRing.gamma32(ledRing.ColorHSV(hue, 255, brightness)));
  }

  for (uint8_t powerUp = 0; powerUp < 2; powerUp++) {
    uint8_t pixel = (pulse / 18 + powerUp * (LED_COUNT / 2)) % LED_COUNT;
    uint16_t hue = IDLE_NEON_PALETTE[(paletteOffset + powerUp * 2) % IDLE_NEON_COLOUR_COUNT];
    ledRing.setPixelColor(pixel, ledRing.gamma32(ledRing.ColorHSV(hue, 255, 150)));
  }

  ledRing.show();
  paletteOffset = (paletteOffset + 1) % IDLE_NEON_COLOUR_COUNT;
  pulse += 12;
}

void updateBuzzer() {
  uint32_t currentTime = millis();

  if (motorState == MotorState::STOPPED || motorState == MotorState::FINAL_SERVO_HOLD) {
    if (currentTime - lastStopBeepTime >= STOP_BEEP_INTERVAL_MS) {
      lastStopBeepTime = currentTime;
      tone(BUZZER_PIN, 1200, STOP_BEEP_DURATION_MS);
    }
    return;
  }

  if (motorState == MotorState::IDLE &&
      currentTime - idleNoteStartTime >= IDLE_NOTE_DURATIONS_MS[idleNoteIndex] + IDLE_NOTE_GAP_MS) {
    tone(BUZZER_PIN, IDLE_MELODY[idleNoteIndex], IDLE_NOTE_DURATIONS_MS[idleNoteIndex]);
    idleNoteStartTime = currentTime;
    idleNoteIndex = (idleNoteIndex + 1) % IDLE_MELODY_LENGTH;
  }
}

void updateButton() {
  bool buttonReading = digitalRead(BUTTON_PIN);

  if (buttonReading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= BUTTON_DEBOUNCE_MS && buttonReading != stableButtonState) {
    stableButtonState = buttonReading;

    if (stableButtonState == LOW) {
      if (motorState == MotorState::IDLE) {
        setMotorState(MotorState::FORWARD);
      }
    }
  }

  lastButtonReading = buttonReading;
}

void updateMotorCycle() {
  uint32_t elapsedTime = millis() - motorStateStartTime;

  if (motorState == MotorState::FORWARD && elapsedTime >= FORWARD_DURATION_MS) {
    setMotorState(MotorState::STOPPED);
  } else if (motorState == MotorState::STOPPED && elapsedTime >= STOP_DURATION_MS) {
    setMotorState(MotorState::REVERSE);
  } else if (motorState == MotorState::REVERSE && elapsedTime >= REVERSE_DURATION_MS) {
    setMotorState(MotorState::FINAL_SERVO_HOLD);
  } else if (motorState == MotorState::FINAL_SERVO_HOLD && elapsedTime >= FINAL_SERVO_HOLD_MS) {
    setMotorState(MotorState::FINAL_SERVO_DESCENT);
  } else if (motorState == MotorState::FINAL_SERVO_DESCENT &&
             elapsedTime >= FINAL_SERVO_DESCENT_DURATION_MS) {
    armServo.write(SERVO_START_ANGLE);
    setMotorState(MotorState::IDLE);
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  armServo.setPeriodHertz(50);
  armServo.attach(SERVO_PIN, 500, 2400);
  armServo.write(SERVO_START_ANGLE);
  setMotorState(MotorState::IDLE);

  ledRing.begin();
  ledRing.setBrightness(150);
  ledRing.clear();
  ledRing.show();

  randomSeed(esp_random());
}

void loop() {
  updateButton();
  updateMotorCycle();
  updateServoDescent();
  updateBuzzer();

  if (motorState == MotorState::FORWARD || motorState == MotorState::REVERSE ||
      motorState == MotorState::FINAL_SERVO_DESCENT) {
    showMotorRunning();
  } else if (motorState == MotorState::STOPPED || motorState == MotorState::FINAL_SERVO_HOLD) {
    showMotorStopped();
  } else if (millis() - lastIdleFrameTime >= IDLE_FRAME_INTERVAL_MS) {
    lastIdleFrameTime = millis();
    showIdleAnimation();
  }
}