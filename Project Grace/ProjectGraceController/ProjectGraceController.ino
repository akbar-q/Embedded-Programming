#include <Adafruit_NeoPixel.h>
#include <esp_system.h>

constexpr uint8_t BUTTON_PIN = 21;
constexpr uint8_t MOTOR_IN1_PIN = 3;
constexpr uint8_t MOTOR_IN2_PIN = 2;
constexpr uint8_t LED_RING_PIN = 4;
constexpr uint8_t LED_COUNT = 12;

constexpr uint16_t IDLE_FRAME_INTERVAL_MS = 90;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 25;

Adafruit_NeoPixel ledRing(LED_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

bool motorsRunning = false;
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
uint32_t lastDebounceTime = 0;
uint32_t lastIdleFrameTime = 0;

void setMotorsRunning(bool running) {
  motorsRunning = running;

  if (running) {
    digitalWrite(MOTOR_IN1_PIN, HIGH);
    digitalWrite(MOTOR_IN2_PIN, LOW);
  } else {
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);
  }
}

void showMotorRunning() {
  ledRing.fill(ledRing.Color(0, 255, 0));
  ledRing.show();
}

void showIdleAnimation() {
  ledRing.clear();

  for (uint8_t sparkle = 0; sparkle < 3; sparkle++) {
    uint8_t pixel = random(LED_COUNT);
    uint16_t hue = random(65536);
    uint8_t value = random(30, 101);
    ledRing.setPixelColor(pixel, ledRing.gamma32(ledRing.ColorHSV(hue, 255, value)));
  }

  ledRing.show();
}

void updateButton() {
  bool buttonReading = digitalRead(BUTTON_PIN);

  if (buttonReading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= BUTTON_DEBOUNCE_MS && buttonReading != stableButtonState) {
    stableButtonState = buttonReading;

    if (stableButtonState == LOW) {
      setMotorsRunning(!motorsRunning);
    }
  }

  lastButtonReading = buttonReading;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  setMotorsRunning(false);

  ledRing.begin();
  ledRing.setBrightness(80);
  ledRing.clear();
  ledRing.show();

  randomSeed(esp_random());
}

void loop() {
  updateButton();

  if (motorsRunning) {
    showMotorRunning();
  } else if (millis() - lastIdleFrameTime >= IDLE_FRAME_INTERVAL_MS) {
    lastIdleFrameTime = millis();
    showIdleAnimation();
  }
}