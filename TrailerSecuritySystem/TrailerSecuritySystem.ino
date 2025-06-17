#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RH_ASK.h>
#include <SPI.h>

#define RF_RECEIVER_PIN 7

char mes;
char anotherVariable[13];  // Increased size to account for null terminator
int bufSize = 12;
bool isArmedTransmitting = false;
bool isDisarmedTransmitting = false;
bool forceAlarm;

LiquidCrystal_I2C lcd(0x27,20,4);  // I2C address 0x27, 16 columns and 4 rows

RH_ASK driver(2000, RF_RECEIVER_PIN, 0);

char passcode[6];  // Adjust the length as needed
char enteredPasscode[6];

int passcodeIndex = 0;
bool systemArmed = false;
bool doorOpen = false;
bool ArmButtonStat;
bool DisButtonStat;
bool Motion = 0;
bool SystemLock;

unsigned long doorOpenStartTime = 0;
unsigned long alarmTriggerTime  = 0;
unsigned long systemStartTime   = 0;
unsigned long lastUpdateTime    = 0;

// Add this line
unsigned long lcdUpdateInterval = 500;  // Change the interval as needed (in milliseconds)

// Add these lines
bool           armButtonPressed = false;
const unsigned long ARM_TIMEOUT = 10000;  // Time limit for entering the arm passcode in milliseconds
unsigned long  armButtonPressedTime = 0;

// Pin definitions
#define ARM_BUTTON_PIN 2
#define DISARM_BUTTON_PIN 3
#define DOOR_LIMIT_SWITCH_PIN 4
#define BUZZER_PIN 5
#define PIR_SENSOR_PIN 12

// Constants
#define ARMED_CODE_LENGTH 5
#define MAX_PASSCODE_ATTEMPTS 3
#define ARM_DELAY 10000   // 10 seconds
#define DOOR_OPEN_DELAY 30000  // 30 seconds
#define ALARM_DURATION 5000  // 5 seconds

// Keypad setup
const byte ROW_NUM = 4;
const byte COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte pin_rows[ROW_NUM] = {A0, A1, A2, A3};

byte pin_column[COLUMN_NUM] = {A4, A5, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

void setup() {

   Serial.begin(9600);

   Serial.println("Welcommen by Akbar Q");
    delay(5000);

  lcd.init(); //WRONG LCD INITILIZATION COMMAND USED***
  lcd.backlight();

  if (driver.init() == 0) {
    Serial.println("Driver initialization failed!");
    delay(9000);
  }

  else {
    Serial.println("RF Receiver Appears to be Working");
    delay(5000);
  }

  Serial.println("Serial Monitor Ready"); //Serial Monitor Was Not Used at All***

  pinMode(ARM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DISARM_BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(DOOR_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);

  // Load passcode from EEPROM (you can change the address if needed)
  strcpy(passcode, "12345");

  driver.init();


}

void loop() {

//----------------------------------------------------------------------------//

//Status Read

  displayStatusOnLCD(); //This Function Only Runs Every 0.5 Second

    ArmButtonStat = digitalRead(ARM_BUTTON_PIN);
      Serial.print("Arm Button Stat = ");
      Serial.println(ArmButtonStat);
      delay(100);

    DisButtonStat = digitalRead(DISARM_BUTTON_PIN);
      Serial.print("Dis Button Stat = ");
      Serial.println(DisButtonStat);
      delay(100);

    DisButtonStat = digitalRead(DISARM_BUTTON_PIN);
      Serial.print("Door Open Status = ");
      Serial.println(doorOpen);
      delay(100);

    DisButtonStat = digitalRead(DISARM_BUTTON_PIN);
      Serial.print("  System Arm Status = ");
      Serial.println(systemArmed);
      delay(100);

    Motion = digitalRead(PIR_SENSOR_PIN);
      Serial.print("  Motion Detection = ");
      Serial.println(Motion);
      delay(100);

            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");
            Serial.println(" ");

          delay(500);

 //----------------------------------------------------------------------------//
 //Control System While Armed         
//GPT CODE FLOW

  // Check for PIR sensor
  if (systemArmed == 1 && Motion == 1) {
    // PIR sensor triggered, handle the event
    triggerAlarm();
  }

  if (systemArmed == 0) {
    // PIR sensor triggered, handle the event
    digitalWrite(BUZZER_PIN, LOW);
    forceAlarm = 0;
  }

  armSystem();
  

//----------------------------------------------------------------------------//

//Custom Code Flow for Arm Disarm

if (systemArmed == 1 && doorOpen == 1){
  triggerAlarm();
  forceAlarm = 1;
}

if (forceAlarm == 1){
  triggerAlarm();
}

//----------------------------------------------------------------------------//

// WORKING RF COMMS

    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        Serial.print("Message: ");
        mes = (char*)buf;

        Serial.println((char*)buf);

        // Copy the data from buf to anotherVariable
        strncpy(anotherVariable, (char*)buf, bufSize - 1);
        anotherVariable[bufSize - 1] = '\0'; // Null-terminate the string

        // Process the message based on the type
        if (strcmp(anotherVariable, "ARMar******") == 0) {
            Serial.println("ARMED");
            // Additional processing for "ARMED" message
            // ...
            lcd.setCursor(0, 3);
            lcd.print("System Arm RF COMM");

            systemArmed = 1;


        } else if (strcmp(anotherVariable, "DISar******") == 0) {
            Serial.println("DISARMED");
            // Additional processing for "DISARMED" message
            // ...
            lcd.setCursor(0, 3);
            lcd.print("System DisArm RF COMM");

            systemArmed = 0;
        }
        
        delay(3000);
    } //RF COMMS CLOSE

//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//

} //Void Loop Close

//----------------------------------------------------------------------------//

void displayStatusOnLCD() {

      if (millis() - lastUpdateTime >= lcdUpdateInterval) //JUST USED TO REDUCE FLICKER I SUPPOSE
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Status: ");
        lcd.print(systemArmed ? "Armed" : "Disarmed"); //Chooses Between the Two States Based on High or Low

        lcd.setCursor(0, 1);

            if (systemArmed == 1 && SystemLock == 0) {
              lcd.print("Countdown: ");

              lcd.setCursor(11, 1);
              lcd.print("10");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 9");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 8");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);                

              lcd.setCursor(11, 1);
              lcd.print(" 7");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 6");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 5");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 4");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 3");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 2");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 1");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

              lcd.setCursor(11, 1);
              lcd.print(" 0");
              lcd.setCursor(13, 1);
              lcd.print("s");
                delay(999);

                SystemLock = 1;

              lcd.setCursor(0, 1);
              lcd.print("System is Locked");
            } 
                else {
                    doorOpen = digitalRead(DOOR_LIMIT_SWITCH_PIN);
                  lcd.print("Door: "); //Nothing Reads the Actual Status of the Door Switch
                  lcd.print(doorOpen ? "Open" : "Closed");
                }

        lastUpdateTime = millis();

      }
} //Void displayStatusOnLCD

void armSystem() {
  // Check if the arm button is pressed
  if (digitalRead(ARM_BUTTON_PIN) == LOW) {
    // Set system armed status
    systemArmed = true;

    // Sound the buzzer for confirmation
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }

    // Check passcode
    checkPasscode();

    // Set SystemLock after checking passcode
    SystemLock = 1;
  }
}


void disarmSystem() {
  // Set system disarmed status
  systemArmed = false;
  Serial.println("System Disarmed");
  digitalWrite(BUZZER_PIN, LOW);
}

void triggerAlarm() {
  // Trigger the alarm
  analogWrite(BUZZER_PIN, 10);
  delay(500);
  analogWrite(BUZZER_PIN,  0);
  delay(300);

  alarmTriggerTime = millis();
  Serial.println("Alarm Triggered");
}

void updateSystem() {
  // Check for door open time limit
  if (systemArmed && doorOpen && (millis() - doorOpenStartTime >= DOOR_OPEN_DELAY)) {
    triggerAlarm();
  }

  // Check for alarm reset time
  if (digitalRead(BUZZER_PIN) == HIGH && (millis() - alarmTriggerTime >= ALARM_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Check for system reset time
  if (systemArmed && (millis() - systemStartTime >= ARM_DELAY)) {
    disarmSystem();
  }
}

void checkPasscode() {
  Serial.println("Enter Passcode:");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Passcode:");

  passcodeIndex = 0;
  memset(enteredPasscode, 0, sizeof(enteredPasscode));  // Clear the entered passcode

  while (armButtonPressed && (millis() - armButtonPressedTime < ARM_TIMEOUT)) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      enteredPasscode[passcodeIndex] = key;
      passcodeIndex++;

      // Print the entered key on both Serial monitor and LCD
      Serial.print("Key: ");
      Serial.println(key);
      lcd.setCursor(passcodeIndex - 1, 1);
      lcd.print(key);

      if (passcodeIndex == ARMED_CODE_LENGTH) {
        armButtonPressed = false;  // Reset the flag
        lcd.clear();
        Serial.print("Entered Passcode: ");
        Serial.println(enteredPasscode);

        if (strcmp(enteredPasscode, passcode) == 0) {
          // Only arm the system if the correct passcode is entered
          systemArmed = true;
          doorOpen = false;
          systemStartTime = millis();
          Serial.println("System Armed");
        } else {
          // Incorrect passcode, handle as needed
          // For example, display an error on the LCD
          lcd.setCursor(0, 0);
          lcd.print("Incorrect Passcode");
          Serial.println("Incorrect Passcode");
          delay(2000);  // Display the error message for 2 seconds
          lcd.clear();
        }
      }
    }
  }

  // Check if timeout for entering passcode has occurred
  if (armButtonPressed && (millis() - armButtonPressedTime >= ARM_TIMEOUT)) {
    armButtonPressed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Passcode Timeout");
    Serial.println("Passcode Timeout");
    delay(2000);  // Display the timeout message for 2 seconds
    lcd.clear();
  }
}