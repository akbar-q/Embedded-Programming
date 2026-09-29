#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>

// Smart Shower Water Conservation System
// Foam-board demonstration for an ESP32 WROOM-32D.
// Libraries required: Adafruit NeoPixel and ESP32Servo.

constexpr uint8_t COLD_POT_PIN = 15;
constexpr uint8_t HOT_POT_PIN = 34;
constexpr uint8_t HOT_SERVO_PIN = 33;
constexpr uint8_t COLD_SERVO_PIN = 25;
constexpr uint8_t TEMPERATURE_LEDS_PIN = 18;
constexpr uint8_t PROMPT_RING_PIN = 17;
constexpr uint8_t SHOWER_LED_PINS[] = {27, 14, 12};
constexpr uint8_t RESET_BUTTON_PIN = 22;
constexpr uint8_t FORCE_BUTTON_PIN = 23;

constexpr uint8_t TEMPERATURE_LED_COUNT = 8;
constexpr uint8_t PROMPT_RING_LED_COUNT = 8;
constexpr uint8_t SHOWER_LED_COUNT = sizeof(SHOWER_LED_PINS) / sizeof(SHOWER_LED_PINS[0]);

constexpr int HOT_POT_ADC_MINIMUM = 0;
constexpr int HOT_POT_ADC_MAXIMUM = 3604;
constexpr int COLD_POT_ADC_MINIMUM = 0;
constexpr int COLD_POT_ADC_MAXIMUM = 3610;
constexpr int POT_DEAD_BAND_PERCENT = 4;
constexpr int SHOWER_ON_PERCENT = 6;
constexpr bool INVERT_COLD_POTENTIOMETER = true;
constexpr bool INVERT_HOT_POTENTIOMETER = true;

constexpr int HOT_SERVO_ZERO_ANGLE = 90;
constexpr int HOT_SERVO_FULL_FLOW_ANGLE = 0;
constexpr int COLD_SERVO_ZERO_ANGLE = 100;
constexpr int COLD_SERVO_FULL_FLOW_ANGLE = 0;
constexpr int COLD_WATER_TEMPERATURE_C = 15;
constexpr int HOT_WATER_TEMPERATURE_C = 55;
constexpr int COMFORT_TEMPERATURE_C = 38;
constexpr int MINIMUM_REGULATED_TEMPERATURE_C = 30;

// Short demonstration thresholds. Change these to 240000, 300000, and 360000
// for a four-, five-, and six-minute competition demonstration.
constexpr unsigned long CAUTION_TIME_MS = 45000;
constexpr unsigned long WARNING_TIME_MS = 60000;
constexpr unsigned long LIMIT_TIME_MS = 75000;
constexpr unsigned long FLOW_STOP_TIME_MS = 95000;
constexpr unsigned long SESSION_END_IDLE_MS = 15000;
constexpr unsigned long DISPLAY_UPDATE_MS = 50;
constexpr unsigned long SERIAL_UPDATE_MS = 1000;

enum ShowerState {
  READY,
  EFFICIENT,
  CAUTION,
  WARNING,
  CONSERVATION_LIMIT,
  PAUSE_REMINDER,
  COMPLETE
};

Adafruit_NeoPixel temperatureLeds(
  TEMPERATURE_LED_COUNT,
  TEMPERATURE_LEDS_PIN,
  NEO_GRB + NEO_KHZ800
);

Adafruit_NeoPixel promptRing(
  PROMPT_RING_LED_COUNT,
  PROMPT_RING_PIN,
  NEO_GRB + NEO_KHZ800
);

Servo hotServo;
Servo coldServo;

bool sessionActive = false;
bool forcedConservationMode = false;
bool flowShutOff = false;
bool resetButtonWasPressed = false;
bool forceButtonWasPressed = false;
unsigned long sessionStartMs = 0;
unsigned long lastActiveMs = 0;
unsigned long lastLoopMs = 0;
unsigned long waterRunMs = 0;
unsigned long completeAtMs = 0;
unsigned long lastDisplayUpdateMs = 0;
unsigned long lastSerialUpdateMs = 0;
unsigned long lastAnimationMs = 0;
uint8_t showerAnimationIndex = 0;
uint8_t conservationScore = 100;

int hotPercent = 0;
int coldPercent = 0;
int demandPercent = 0;
int requestedFlowPercent = 0;
int actualFlowPercent = 0;
int regulatedHotFlowPercent = 0;
int regulatedColdFlowPercent = 0;
int flowLimitPercent = 100;
int estimatedTemperatureC = COLD_WATER_TEMPERATURE_C;
int hotPotAdc = 0;
int coldPotAdc = 0;
int filteredColdPercent = 0;
int lastHotServoAngle = -1;
int lastColdServoAngle = -1;
bool coldControlInitialised = false;

void updateFlowLimit();

int readSmoothedAdc(uint8_t pin) {
  constexpr uint8_t SAMPLE_COUNT = 8;
  uint32_t total = 0;

  for (uint8_t sample = 0; sample < SAMPLE_COUNT; sample++) {
    total += analogRead(pin);
  }

  return total / SAMPLE_COUNT;
}

int adcToPercent(int rawValue, int minimum, int maximum) {
  int percent = map(rawValue, minimum, maximum, 0, 100);
  percent = constrain(percent, 0, 100);

  if (percent < POT_DEAD_BAND_PERCENT) {
    return 0;
  }

  return percent;
}

void readControls() {
  hotPotAdc = readSmoothedAdc(HOT_POT_PIN);
  hotPercent = adcToPercent(hotPotAdc, HOT_POT_ADC_MINIMUM, HOT_POT_ADC_MAXIMUM);
  if (INVERT_HOT_POTENTIOMETER) {
    hotPercent = 100 - hotPercent;
  }

  coldPotAdc = readSmoothedAdc(COLD_POT_PIN);
  int rawColdPercent = adcToPercent(
    coldPotAdc,
    COLD_POT_ADC_MINIMUM,
    COLD_POT_ADC_MAXIMUM
  );
  if (INVERT_COLD_POTENTIOMETER) {
    rawColdPercent = 100 - rawColdPercent;
  }

  if (!coldControlInitialised) {
    filteredColdPercent = rawColdPercent;
    coldControlInitialised = true;
  } else if (abs(rawColdPercent - filteredColdPercent) >= 2) {
    filteredColdPercent = rawColdPercent;
  }

  coldPercent = filteredColdPercent;
  requestedFlowPercent = constrain(hotPercent + coldPercent, 0, 100);
  regulatedHotFlowPercent = hotPercent * flowLimitPercent / 100;
  regulatedColdFlowPercent = coldPercent * flowLimitPercent / 100;
  actualFlowPercent = constrain(
    regulatedHotFlowPercent + regulatedColdFlowPercent,
    0,
    100
  );
  demandPercent = actualFlowPercent;

  int totalTapOpening = hotPercent + coldPercent;
  if (totalTapOpening == 0) {
    estimatedTemperatureC = COLD_WATER_TEMPERATURE_C;
    return;
  }

  estimatedTemperatureC = (
    HOT_WATER_TEMPERATURE_C * hotPercent +
    COLD_WATER_TEMPERATURE_C * coldPercent
  ) / totalTapOpening;
}

bool isShowerActive() {
  return requestedFlowPercent >= SHOWER_ON_PERCENT;
}

bool isWaterFlowing() {
  return actualFlowPercent > 0;
}

void updateServoGauges() {
  // The physical servos are mounted opposite to their signal-wire names.
  int hotAngle = regulatedColdFlowPercent == 0 ? HOT_SERVO_ZERO_ANGLE :
    map(regulatedColdFlowPercent, 0, 100, HOT_SERVO_ZERO_ANGLE, HOT_SERVO_FULL_FLOW_ANGLE);
  int coldAngle = regulatedHotFlowPercent == 0 ? COLD_SERVO_ZERO_ANGLE :
    map(regulatedHotFlowPercent, 0, 100, COLD_SERVO_ZERO_ANGLE, COLD_SERVO_FULL_FLOW_ANGLE);

  if (hotAngle != lastHotServoAngle) {
    hotServo.write(hotAngle);
    lastHotServoAngle = hotAngle;
  }

  if (coldAngle != lastColdServoAngle) {
    coldServo.write(coldAngle);
    lastColdServoAngle = coldAngle;
  }
}

ShowerState determineState(unsigned long now, bool showerActive) {
  if (!sessionActive) {
    if (completeAtMs != 0 && now - completeAtMs < 4000) {
      return COMPLETE;
    }
    return READY;
  }

  if (!showerActive) {
    return PAUSE_REMINDER;
  }

  if (forcedConservationMode || waterRunMs >= LIMIT_TIME_MS) {
    return CONSERVATION_LIMIT;
  }

  if (waterRunMs >= WARNING_TIME_MS) {
    return WARNING;
  }

  if (waterRunMs >= CAUTION_TIME_MS || demandPercent >= 80) {
    return CAUTION;
  }

  return EFFICIENT;
}

uint32_t stateColour(ShowerState state) {
  switch (state) {
    case EFFICIENT:
      return promptRing.Color(0, 180, 20);
    case CAUTION:
      return promptRing.Color(190, 120, 0);
    case WARNING:
      return promptRing.Color(255, 65, 0);
    case CONSERVATION_LIMIT:
      return promptRing.Color(220, 0, 0);
    case PAUSE_REMINDER:
      return promptRing.Color(0, 60, 220);
    case COMPLETE:
      return promptRing.Color(150, 150, 150);
    case READY:
    default:
      return promptRing.Color(80, 80, 80);
  }
}

uint32_t temperatureColour() {
  if (estimatedTemperatureC < COMFORT_TEMPERATURE_C) {
    return temperatureLeds.Color(0, 80, 255);
  }

  return temperatureLeds.Color(255, 110, 0);
}

uint8_t triangleWave(unsigned long now, unsigned long periodMs, uint8_t minimum, uint8_t maximum) {
  unsigned long phase = now % periodMs;
  unsigned long halfPeriod = periodMs / 2;
  unsigned long ramp = phase < halfPeriod ? phase : periodMs - phase;
  return map(ramp, 0, halfPeriod, minimum, maximum);
}

uint32_t scaleColour(uint32_t colour, uint8_t brightness) {
  uint8_t red = ((colour >> 16) & 0xFF) * brightness / 255;
  uint8_t green = ((colour >> 8) & 0xFF) * brightness / 255;
  uint8_t blue = (colour & 0xFF) * brightness / 255;
  return temperatureLeds.Color(red, green, blue);
}

void updateTemperatureLeds(unsigned long now) {
  uint32_t colour = temperatureColour();
  uint8_t activeLeds = map(
    constrain(estimatedTemperatureC, COLD_WATER_TEMPERATURE_C, HOT_WATER_TEMPERATURE_C),
    COLD_WATER_TEMPERATURE_C,
    HOT_WATER_TEMPERATURE_C,
    0,
    TEMPERATURE_LED_COUNT
  );
  uint8_t pulseBrightness = triangleWave(now, 1200, 70, 180);
  unsigned long sparkleInterval = map(estimatedTemperatureC, COLD_WATER_TEMPERATURE_C, HOT_WATER_TEMPERATURE_C, 500, 90);
  uint8_t sparkleIndex = (now / sparkleInterval) % TEMPERATURE_LED_COUNT;

  for (uint8_t index = 0; index < TEMPERATURE_LED_COUNT; index++) {
    uint32_t pixelColour = 0;

    if (index < activeLeds) {
      uint8_t brightness = index == sparkleIndex ? 255 : pulseBrightness;
      pixelColour = scaleColour(colour, brightness);
    }

    temperatureLeds.setPixelColor(index, pixelColour);
  }

  temperatureLeds.show();
}

void updatePromptRing(ShowerState state, unsigned long now) {
  uint32_t colour = stateColour(state);
  uint8_t litLeds = 0;

  if (state == READY || state == COMPLETE) {
    litLeds = PROMPT_RING_LED_COUNT;
  } else if (state == PAUSE_REMINDER) {
    litLeds = 1;
  } else {
    unsigned long remainingMs = waterRunMs >= LIMIT_TIME_MS ? 0 : LIMIT_TIME_MS - waterRunMs;
    litLeds = map(remainingMs, 0, LIMIT_TIME_MS, 0, PROMPT_RING_LED_COUNT);
    litLeds = constrain(litLeds, 1, PROMPT_RING_LED_COUNT);
  }

  uint8_t markerIndex = (now / 140) % PROMPT_RING_LED_COUNT;
  uint8_t pulseBrightness = triangleWave(now, 900, 45, 255);
  bool warningFlashOn = (now / 180) % 2 == 0;

  for (uint8_t index = 0; index < PROMPT_RING_LED_COUNT; index++) {
    uint32_t pixelColour = 0;

    switch (state) {
      case READY:
        pixelColour = scaleColour(colour, triangleWave(now, 1800, 15, 100));
        break;
      case EFFICIENT:
        if (index < litLeds) {
          pixelColour = scaleColour(colour, index == markerIndex ? 255 : 75);
        }
        break;
      case CAUTION:
        if (index < litLeds) {
          pixelColour = scaleColour(colour, index == markerIndex ? 255 : pulseBrightness);
        }
        break;
      case WARNING:
        if (index < litLeds && (warningFlashOn || index == markerIndex)) {
          pixelColour = scaleColour(colour, index == markerIndex ? 255 : 135);
        }
        break;
      case CONSERVATION_LIMIT:
        pixelColour = warningFlashOn ? colour : 0;
        break;
      case PAUSE_REMINDER:
        if (index == markerIndex) {
          pixelColour = colour;
        } else if (index == (markerIndex + PROMPT_RING_LED_COUNT - 1) % PROMPT_RING_LED_COUNT) {
          pixelColour = scaleColour(colour, 80);
        }
        break;
      case COMPLETE:
        pixelColour = index == markerIndex ? promptRing.Color(0, 180, 20) : scaleColour(colour, 35);
        break;
    }

    promptRing.setPixelColor(index, pixelColour);
  }

  promptRing.show();
}

void updateShowerAnimation(unsigned long now, bool waterFlowing) {
  if (!waterFlowing) {
    for (uint8_t index = 0; index < SHOWER_LED_COUNT; index++) {
      digitalWrite(SHOWER_LED_PINS[index], LOW);
    }
    return;
  }

  // Higher tap demand represents higher water flow, so droplets travel faster.
  unsigned long animationInterval = map(demandPercent, SHOWER_ON_PERCENT, 100, 650, 70);
  if (now - lastAnimationMs >= animationInterval) {
    lastAnimationMs = now;
    showerAnimationIndex = (showerAnimationIndex + 1) % SHOWER_LED_COUNT;
  }

  for (uint8_t index = 0; index < SHOWER_LED_COUNT; index++) {
    bool isDropletHead = index == showerAnimationIndex;
    bool isDropletTrail = demandPercent >= 55 &&
      index == (showerAnimationIndex + SHOWER_LED_COUNT - 1) % SHOWER_LED_COUNT;
    digitalWrite(SHOWER_LED_PINS[index], isDropletHead || isDropletTrail ? HIGH : LOW);
  }
}

void startSession(unsigned long now) {
  sessionActive = true;
  forcedConservationMode = false;
  flowShutOff = false;
  sessionStartMs = now;
  waterRunMs = 0;
  conservationScore = 100;
  completeAtMs = 0;
  Serial.println("Shower session started");
}

void resetSystem(unsigned long now) {
  sessionActive = false;
  forcedConservationMode = false;
  flowShutOff = false;
  waterRunMs = 0;
  flowLimitPercent = 100;
  conservationScore = 100;
  completeAtMs = 0;
  lastActiveMs = now;
  Serial.println("System reset");
}

void readButtons(unsigned long now) {
  bool resetPressed = digitalRead(RESET_BUTTON_PIN) == LOW;
  bool forcePressed = digitalRead(FORCE_BUTTON_PIN) == LOW;

  if (resetPressed && !resetButtonWasPressed) {
    resetSystem(now);
  }

  if (forcePressed && !forceButtonWasPressed) {
    if (!sessionActive) {
      startSession(now);
    }

    forcedConservationMode = true;
    flowShutOff = false;
    waterRunMs = LIMIT_TIME_MS;
    Serial.println("Forced conservation sequence started");
  }

  resetButtonWasPressed = resetPressed;
  forceButtonWasPressed = forcePressed;
}

void finishSession(unsigned long now) {
  sessionActive = false;
  completeAtMs = now;
  Serial.print("Session complete. Conservation score: ");
  Serial.println(conservationScore);
}

void updateSession(unsigned long now, bool showerActive) {
  unsigned long loopDurationMs = now - lastLoopMs;

  if (showerActive && !sessionActive) {
    startSession(now);
  }

  if (sessionActive && !flowShutOff && (showerActive || forcedConservationMode)) {
    waterRunMs += loopDurationMs;
    lastActiveMs = now;
  }

  if (sessionActive && !showerActive && now - lastActiveMs >= SESSION_END_IDLE_MS) {
    finishSession(now);
  }

  if (sessionActive) {
    uint8_t timePenalty = map(constrain(waterRunMs, 0UL, LIMIT_TIME_MS), 0UL, LIMIT_TIME_MS, 0, 55);
    uint8_t demandPenalty = map(demandPercent, 0, 100, 0, 25);
    uint8_t heatPenalty = estimatedTemperatureC > COMFORT_TEMPERATURE_C ?
      map(constrain(estimatedTemperatureC, COMFORT_TEMPERATURE_C, HOT_WATER_TEMPERATURE_C), COMFORT_TEMPERATURE_C, HOT_WATER_TEMPERATURE_C, 0, 20) : 0;
    conservationScore = constrain(100 - timePenalty - demandPenalty - heatPenalty, 0, 100);
  }

}

void printStatus(ShowerState state) {
  if (millis() - lastSerialUpdateMs < SERIAL_UPDATE_MS) {
    return;
  }

  lastSerialUpdateMs = millis();
  Serial.print("Hot ADC: ");
  Serial.print(hotPotAdc);
  Serial.print(" | Cold ADC: ");
  Serial.print(coldPotAdc);
  Serial.print(" | Hot: ");
  Serial.print(hotPercent);
  Serial.print("% | Cold: ");
  Serial.print(coldPercent);
  Serial.print("% | Temp: ");
  Serial.print(estimatedTemperatureC);
  Serial.print("C | Flow: ");
  Serial.print(actualFlowPercent);
  Serial.print("% | Limit: ");
  Serial.print(flowLimitPercent);
  Serial.print("% | Water time: ");
  Serial.print(waterRunMs / 1000);
  Serial.print("s | Score: ");
  Serial.print(conservationScore);
  Serial.print(" | State: ");
  Serial.println(state);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  hotServo.setPeriodHertz(50);
  coldServo.setPeriodHertz(50);
  hotServo.attach(HOT_SERVO_PIN, 500, 2400);
  coldServo.attach(COLD_SERVO_PIN, 500, 2400);

  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FORCE_BUTTON_PIN, INPUT_PULLUP);

  for (uint8_t index = 0; index < SHOWER_LED_COUNT; index++) {
    pinMode(SHOWER_LED_PINS[index], OUTPUT);
    digitalWrite(SHOWER_LED_PINS[index], LOW);
  }

  temperatureLeds.begin();
  promptRing.begin();
  temperatureLeds.setBrightness(80);
  promptRing.setBrightness(80);
  temperatureLeds.clear();
  promptRing.clear();
  temperatureLeds.show();
  promptRing.show();

  lastLoopMs = millis();
  lastActiveMs = lastLoopMs;
  Serial.println("Smart Shower Water Conservation System ready");
}

void loop() {
  unsigned long now = millis();
  readButtons(now);
  updateFlowLimit();
  readControls();

  bool showerRequested = isShowerActive();
  updateSession(now, showerRequested);
  updateFlowLimit();
  readControls();
  bool waterFlowing = isWaterFlowing();
  updateServoGauges();

  ShowerState state = determineState(now, showerRequested);
  if (now - lastDisplayUpdateMs >= DISPLAY_UPDATE_MS) {
    lastDisplayUpdateMs = now;
    updateTemperatureLeds(now);
    updatePromptRing(state, now);
    updateShowerAnimation(now, waterFlowing);
  }

  printStatus(state);
  lastLoopMs = now;
}

void updateFlowLimit() {
  if (!sessionActive) {
    flowLimitPercent = 100;
    return;
  }

  if (flowShutOff || waterRunMs >= FLOW_STOP_TIME_MS) {
    flowLimitPercent = 0;
    flowShutOff = true;
    return;
  }

  if (forcedConservationMode || waterRunMs >= LIMIT_TIME_MS) {
    flowLimitPercent = map(
      constrain(waterRunMs, LIMIT_TIME_MS, FLOW_STOP_TIME_MS),
      LIMIT_TIME_MS,
      FLOW_STOP_TIME_MS,
      40,
      0
    );
    return;
  }

  if (waterRunMs >= WARNING_TIME_MS) {
    flowLimitPercent = map(waterRunMs, WARNING_TIME_MS, LIMIT_TIME_MS, 80, 40);
    return;
  }

  if (waterRunMs >= CAUTION_TIME_MS) {
    flowLimitPercent = 80;
    return;
  }

  flowLimitPercent = 100;
}