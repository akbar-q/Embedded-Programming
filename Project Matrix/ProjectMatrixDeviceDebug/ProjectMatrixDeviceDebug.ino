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

constexpr uint8_t MOTOR_TEST_SPEED = 190;
constexpr uint16_t STATUS_INTERVAL_MS = 500;
constexpr uint32_t ULTRASONIC_TIMEOUT_US = 30000;

uint32_t lastStatusTime = 0;
bool lastLimitSwitchPressed = false;
bool motorRunning = false;
const char *motorDirection = "stopped";

void setStatusLed(bool red, bool green, bool blue) {
  digitalWrite(LED_RED_PIN, red ? LOW : HIGH);
  digitalWrite(LED_GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(LED_BLUE_PIN, blue ? LOW : HIGH);
}

void stopMotor() {
  analogWrite(MOTOR_ENABLE_PIN, 0);
  digitalWrite(MOTOR_IN3_PIN, LOW);
  digitalWrite(MOTOR_IN4_PIN, LOW);
  motorRunning = false;
  motorDirection = "stopped";
}

void runMotorForward() {
  digitalWrite(MOTOR_IN3_PIN, HIGH);
  digitalWrite(MOTOR_IN4_PIN, LOW);
  analogWrite(MOTOR_ENABLE_PIN, MOTOR_TEST_SPEED);
  motorRunning = true;
  motorDirection = "forward";
}

void runMotorReverse() {
  digitalWrite(MOTOR_IN3_PIN, LOW);
  digitalWrite(MOTOR_IN4_PIN, HIGH);
  analogWrite(MOTOR_ENABLE_PIN, MOTOR_TEST_SPEED);
  motorRunning = true;
  motorDirection = "reverse";
}

bool readDistanceCm(float &distanceCm) {
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

  uint32_t echoDurationUs = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRASONIC_TIMEOUT_US);
  if (echoDurationUs == 0) {
    return false;
  }

  distanceCm = echoDurationUs * 0.0343F / 2.0F;
  return true;
}

void printHelp() {
  Serial.println();
  Serial.println("Project Matrix device debug commands:");
  Serial.println("  r, g, b, y, c, o  Set LED red, green, blue, yellow, cyan, or off");
  Serial.println("  p                 Play a buzzer test tone");
  Serial.println("  f                 Run motor forward");
  Serial.println("  v                 Run motor reverse");
  Serial.println("  s                 Stop motor immediately");
  Serial.println("  a                 Run LED and buzzer test sequence");
  Serial.println("  h                 Show this help");
  Serial.println("The limit switch and ultrasonic reading are printed every 500 ms.");
}

void runLightAndSoundTest() {
  const bool colours[][3] = {
      {true, false, false}, {false, true, false}, {false, false, true}, {true, true, true}};

  Serial.println("Running LED and buzzer test.");
  for (const auto &colour : colours) {
    setStatusLed(colour[0], colour[1], colour[2]);
    tone(BUZZER_PIN, 1000, 120);
    delay(250);
  }
  noTone(BUZZER_PIN);
  setStatusLed(false, false, false);
  Serial.println("LED and buzzer test complete.");
}

void handleCommand(char command) {
  switch (command) {
    case 'r':
      setStatusLed(true, false, false);
      Serial.println("LED: red");
      break;
    case 'g':
      setStatusLed(false, true, false);
      Serial.println("LED: green");
      break;
    case 'b':
      setStatusLed(false, false, true);
      Serial.println("LED: blue");
      break;
    case 'y':
      setStatusLed(true, true, false);
      Serial.println("LED: yellow");
      break;
    case 'c':
      setStatusLed(false, true, true);
      Serial.println("LED: cyan");
      break;
    case 'o':
      setStatusLed(false, false, false);
      Serial.println("LED: off");
      break;
    case 'p':
      tone(BUZZER_PIN, 1200, 250);
      Serial.println("Buzzer: 1200 Hz for 250 ms");
      break;
    case 'f':
      runMotorForward();
      Serial.println("Motor: forward. Send 's' to stop.");
      break;
    case 'v':
      runMotorReverse();
      Serial.println("Motor: reverse. Send 's' to stop.");
      break;
    case 's':
      stopMotor();
      Serial.println("Motor: stopped");
      break;
    case 'a':
      runLightAndSoundTest();
      break;
    case 'h':
    case '?':
      printHelp();
      break;
    case '\r':
    case '\n':
    case ' ':
      break;
    default:
      Serial.print("Unknown command: ");
      Serial.println(command);
      printHelp();
      break;
  }
}

void printDeviceStatus() {
  bool limitSwitchPressed = digitalRead(LIMIT_SWITCH_PIN) == LOW;
  float distanceCm = 0.0F;
  bool distanceValid = readDistanceCm(distanceCm);

  Serial.print("Limit switch: ");
  Serial.print(limitSwitchPressed ? "PRESSED" : "released");
  Serial.print(" | Ultrasonic: ");
  if (distanceValid) {
    Serial.print(distanceCm, 1);
    Serial.print(" cm");
  } else {
    Serial.print("no echo");
  }
  Serial.print(" | Motor: ");
  Serial.println(motorDirection);

  if (limitSwitchPressed != lastLimitSwitchPressed) {
    Serial.println(limitSwitchPressed ? "Limit switch changed to PRESSED." :
                                        "Limit switch changed to released.");
    lastLimitSwitchPressed = limitSwitchPressed;
  }
}

void setup() {
  Serial.begin(115200);
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

  stopMotor();
  setStatusLed(false, false, false);
  lastLimitSwitchPressed = digitalRead(LIMIT_SWITCH_PIN) == LOW;

  Serial.println("Project Matrix device debug started.");
  Serial.println("Serial Monitor: 115200 baud, no line ending required.");
  printHelp();
}

void loop() {
  while (Serial.available() > 0) {
    handleCommand(static_cast<char>(Serial.read()));
  }

  if (millis() - lastStatusTime >= STATUS_INTERVAL_MS) {
    lastStatusTime = millis();
    printDeviceStatus();
  }
}