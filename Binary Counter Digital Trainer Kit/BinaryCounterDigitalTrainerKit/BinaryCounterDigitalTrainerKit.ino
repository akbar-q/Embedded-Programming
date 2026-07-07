const int bitPins[3] = {25, 26, 27};
const unsigned long countIntervalMs = 500;

int counterValue = 0;
bool countUp = true;
unsigned long lastStepTime = 0;

void printBanner() {
  Serial.println("============================================");
  Serial.println("ESP32 3-Bit Serial Counter");
  Serial.println("============================================");
  Serial.println("Commands:");
  Serial.println("  U  -> count up");
  Serial.println("  D  -> count down");
  Serial.println("  S  -> show current status");
  Serial.println("  0-7 -> load a value directly into the counter");
  Serial.println("  H  -> show this help again");
  Serial.println();
}

void writeCounterToPins() {
  for (int bitIndex = 0; bitIndex < 3; bitIndex++) {
    int bitState = (counterValue >> bitIndex) & 0x01;
    digitalWrite(bitPins[bitIndex], bitState);
  }
}

void printCounterState(const char *reason) {
  Serial.print(reason);
  Serial.print(" | direction: ");
  Serial.print(countUp ? "UP" : "DOWN");
  Serial.print(" | decimal: ");
  Serial.print(counterValue);
  Serial.print(" | binary: ");

  for (int bitIndex = 2; bitIndex >= 0; bitIndex--) {
    Serial.print((counterValue >> bitIndex) & 0x01);
  }

  Serial.println();
}

void stepCounter() {
  if (countUp) {
    counterValue = (counterValue + 1) & 0x07;
  } else {
    counterValue = (counterValue + 7) & 0x07;
  }

  writeCounterToPins();
  printCounterState("Counter updated");
}

void handleSerialCommand(char command) {
  if (command >= 'a' && command <= 'z') {
    command = command - 'a' + 'A';
  }

  if (command == 'U') {
    countUp = true;
    printCounterState("Direction changed");
    return;
  }

  if (command == 'D') {
    countUp = false;
    printCounterState("Direction changed");
    return;
  }

  if (command == 'S') {
    printCounterState("Status requested");
    return;
  }

  if (command == 'H') {
    printBanner();
    printCounterState("Current state");
    return;
  }

  if (command >= '0' && command <= '7') {
    counterValue = command - '0';
    writeCounterToPins();
    printCounterState("Counter loaded from serial input");
    return;
  }

  if (command == '\r' || command == '\n' || command == ' ') {
    return;
  }

  Serial.print("Unknown command: ");
  Serial.println(command);
  Serial.println("Type H for the help menu.");
}

void setup() {
  Serial.begin(115200);

  for (int bitIndex = 0; bitIndex < 3; bitIndex++) {
    pinMode(bitPins[bitIndex], OUTPUT);
    digitalWrite(bitPins[bitIndex], LOW);
  }

  delay(250);
  printBanner();
  writeCounterToPins();
  printCounterState("System ready");
}

void loop() {
  while (Serial.available() > 0) {
    char incoming = Serial.read();
    handleSerialCommand(incoming);
  }

  unsigned long now = millis();
  if (now - lastStepTime >= countIntervalMs) {
    lastStepTime = now;
    stepCounter();
  }
}
