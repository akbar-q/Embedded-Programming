//--------------------------------------------//
// Defining Notes for Melody Playback

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Library Initialization
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//--------------------------------------------//
// Global Variable Definition

// Button to enable system
#define enabButton 3
bool enabStat;

// Button to hold down for Debugging
#define DebugButton 4
bool DebugStat = 0;

// IR Proximity Sensor
#define Proximity 6
bool ProxiStat;

// Servo Motor (180-Degree) for bin selection
#define Servo 5
byte ServoPWM = 150;

// Buzzer for status and debug
#define Buzzer 10

// Melody tempo
int tempo = 105;

// Conveyor Belt Motor with speed control
#define Motor 11
int MotorPWM = 0;

// Ultrasonic Distance Measurement
#define USonicTx 3
#define USonicRx 2
unsigned long PulseRx;
float distance;

// Colour Sensor Pinout
#define CS0 7
#define CS1 8
#define CS2 9
#define CS3 12
#define COUT A1
#define CLED A0

// Define 16x2 LCD on I2C Address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Serial Data Return Status
bool DataRx = 0;

// Serial Bypass Variable
bool SerialBypass = 0;

// Define Servo PWM Values for Bins
#define RedBin 210
#define BlueBin 150
#define GreenBin 90

// General Flags
bool itempresent = 1;

//--------------------------------------------//
// Setup a PACMAN Bootup Melody

int melody[] = {
  // Pacman melody notes and durations
  NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16,
  NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
  NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,
  NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5, 32,
  NOTE_FS5, -16, NOTE_DS5, 8, NOTE_DS5, 32, NOTE_E5, 32, NOTE_F5, 32,
  NOTE_F5, 32, NOTE_FS5, 32, NOTE_G5, 32, NOTE_G5, 32, NOTE_GS5, 32, NOTE_A5, 16, NOTE_B5, 8
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

//--------------------------------------------//
void setup() {
  // Initialize LCD and its backlight
  lcd.init();
  lcd.backlight();

  // Setup Buttons
  pinMode(DebugButton, INPUT_PULLUP);
  pinMode(Proximity, INPUT);

  // Ultrasonic Sensor Pin Configuration
  pinMode(USonicRx, INPUT);
  pinMode(USonicTx, OUTPUT);

  // Colour Sensor Pin Configuration
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(COUT, INPUT);
  pinMode(CLED, OUTPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1, LOW);

  // Display project info
  lcd.setCursor(0, 0);
  lcd.print("Colour Sorter");
  lcd.setCursor(0, 1);
  lcd.print("by Akbar Q   ");

  //--------------------------------------------//
  // Play PACMAN Bootup Melody
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    // Play note for 90% of duration
    tone(Buzzer, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(Buzzer);
  }

  //--------------------------------------------//
  // Check and Enable Debug Mode
  DebugStat = digitalRead(DebugButton);
  delay(100);

  if (DebugStat == 0) {
    Serial.begin(19200);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SerialDebugEnabl");
    lcd.setCursor(0, 1);
    lcd.print("Baud = 19 200");

    tone(Buzzer, NOTE_GS3);
    delay(400);
    tone(Buzzer, NOTE_F1);
    delay(200);
    noTone(Buzzer);

    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Serial Connection");
    lcd.setCursor(0, 1);
    lcd.print("is Pending");

    // Print blank lines for serial monitor
    for (int i = 0; i < 10; i++) Serial.println("   ");

    Serial.println("//------------------//");
    Serial.println("Press Any Key on your Keyboard");
    Serial.println("To Iniatialize the Serial Connection");
    Serial.println("...");
    Serial.println("...");
    Serial.println("...");
    Serial.println("//------------------//");

    char SerialRx = '0';
    while (DataRx == 0) {
      // Play waiting melody
      tone(Buzzer, NOTE_D6);
      delay(400);
      tone(Buzzer, NOTE_E7);
      delay(200);
      tone(Buzzer, NOTE_F5);
      delay(400);
      tone(Buzzer, NOTE_G5);
      delay(200);
      tone(Buzzer, NOTE_D6);
      delay(400);
      tone(Buzzer, NOTE_E7);
      delay(200);

      if (Serial.available() > 0) {
        SerialRx = Serial.read();
        if (SerialRx != 0) {
          DataRx = 1;
        }
      }

      DebugStat = digitalRead(DebugButton);
      if (DebugStat == 1) {
        SerialBypass = 1;
        DataRx = 1;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SerialConnection");
        lcd.setCursor(0, 1);
        lcd.print("is Bypassed     ");

        tone(Buzzer, NOTE_G1);
        delay(200);
        noTone(Buzzer);

        tone(Buzzer, NOTE_A2);
        delay(400);
        noTone(Buzzer);

        tone(Buzzer, NOTE_G1);
        delay(200);
        noTone(Buzzer);
      }
    }
    delay(200);
  }

  // If Serial Connection is Present
  if (DataRx == 1) {
    noTone(Buzzer);
    delay(1000);

    if (SerialBypass == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SerialConnection");
      lcd.setCursor(0, 1);
      lcd.print("is Established");
    }

    tone(Buzzer, NOTE_C7);
    delay(300);
    tone(Buzzer, NOTE_A7);
    delay(200);
    tone(Buzzer, NOTE_E7);
    delay(300);
    tone(Buzzer, NOTE_D8);
    delay(200);
    noTone(Buzzer);

    Serial.println("//------------------//");
    Serial.print("SerialConnection");
    Serial.println("is Established");
    delay(4000);
  }

  //--------------------------------------------//
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hold Debug for");
  lcd.setCursor(0, 1);
  lcd.print("I/O Test");
  delay(4000);

  DebugStat = digitalRead(DebugButton);

  // Test All I/O
  if (DebugStat == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I/O Debug Begin");
    lcd.setCursor(0, 1);
    lcd.print("Waiting.....");

    Serial.println("I/O Debug Begin");

    tone(Buzzer, NOTE_C7);
    delay(300);
    tone(Buzzer, NOTE_A7);
    delay(200);

    delay(2000);

    // Servo Test
    Serial.println("Running Servo Test");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Servo Test");
    lcd.setCursor(0, 1);
    lcd.print("Running.....");
    noTone(Buzzer);

    Serial.println("Writing PWM = 90");
    lcd.setCursor(0, 1);
    lcd.print("Move to Left....");
    analogWrite(Servo, 90);
    delay(4000);

    Serial.println("Writing PWM = 220");
    lcd.setCursor(0, 1);
    lcd.print("Move to Right....");
    analogWrite(Servo, 220);
    delay(4000);

    Serial.println("Writing PWM = 140");
    lcd.setCursor(0, 1);
    lcd.print("Move to Middle....");
    analogWrite(Servo, 140);
    delay(4000);

    Serial.println("//------------------//");

    // Proximity Sensor Test
    Serial.println("Proximity Sensor Test");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Proximity Read");

    byte counter = 0;
    while (counter < 100) {
      ProxiStat = digitalRead(Proximity);
      lcd.setCursor(0, 1);
      lcd.print("Status = ");

      if (ProxiStat == 0) {
        tone(Buzzer, NOTE_C2);

        Serial.print("Test Number ");
        Serial.println(counter);
        Serial.println("Proximity Detected");

        lcd.setCursor(10, 1);
        lcd.print("High  ");

        delay(50);
        noTone(Buzzer);
        delay(150);
      } else {
        lcd.setCursor(10, 1);
        lcd.print("Low   ");

        Serial.print("Test Number");
        Serial.println(counter);
        Serial.println("Absent");
      }
      counter++;
    }

    counter = 0;

    //--------------------------------------------//
    // Ultrasonic Sensor Test
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UltraSonic Test");
    lcd.setCursor(0, 1);
    lcd.print("Test Pending   ");

    Serial.println("//------------------//");
    Serial.println("UltraSonic Sensor Test");

    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("US Test Start");

    while (counter < 30) {
      // Pulse the Trigger Pin
      digitalWrite(USonicTx, LOW);
      delayMicroseconds(2);
      digitalWrite(USonicTx, HIGH);
      delayMicroseconds(10);
      digitalWrite(USonicTx, LOW);

      PulseRx = pulseIn(USonicRx, HIGH);

      lcd.setCursor(0, 0);
      lcd.print("Pulse Recieved ");
      Serial.println("Pulse Captured");

      tone(Buzzer, NOTE_G1);
      delay(300);
      tone(Buzzer, NOTE_B1);
      delay(200);
      noTone(Buzzer);

      delay(200);

      distance = PulseRx * 0.017;
      lcd.setCursor(13, 1);
      lcd.print("        cm");
      lcd.setCursor(0, 1);
      lcd.print("           ");
      lcd.setCursor(0, 1);
      lcd.print(distance);

      counter++;

      Serial.print(counter);
      Serial.print(" Distance = ");
      Serial.print(distance);
      Serial.println(" cm");
    }

    counter = 0;

    //--------------------------------------------//
    // Conveyor Motor Test
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conveyor Test");

    Serial.println("//------------------//");
    Serial.println("Motor Test");

    tone(Buzzer, NOTE_GS3);
    delay(300);
    tone(Buzzer, NOTE_AS4);
    delay(200);
    noTone(Buzzer);

    delay(2000);

    Serial.println("Motor Test Start");
    lcd.setCursor(0, 0);
    lcd.print("Motor Spinning ");
    lcd.setCursor(0, 1);
    lcd.print("PWM = ");
    lcd.setCursor(0, 0);
    Serial.println("Motor Ramp Up");

    while (MotorPWM < 255) {
      analogWrite(Motor, MotorPWM);
      lcd.setCursor(6, 1);
      lcd.print("    ");
      lcd.setCursor(6, 1);
      lcd.print((MotorPWM * 100) / 255);
      lcd.setCursor(10, 1);
      lcd.print("%");

      tone(Buzzer, NOTE_B5);
      delay(20);
      noTone(Buzzer);

      Serial.print("Motor Speed = ");
      Serial.print((MotorPWM * 100) / 255);
      Serial.println(" %");

      MotorPWM++;
    }

    Serial.println("Motor Ramp Down");

    while (MotorPWM > 1) {
      analogWrite(Motor, MotorPWM);
      lcd.setCursor(6, 1);
      lcd.print("    ");
      lcd.setCursor(6, 1);
      lcd.print((MotorPWM * 100) / 255);
      lcd.setCursor(10, 1);
      lcd.print("%");

      tone(Buzzer, NOTE_B5);
      delay(20);
      noTone(Buzzer);

      Serial.print("Motor Speed = ");
      Serial.print((MotorPWM * 100) / 255);
      Serial.println(" %");

      MotorPWM--;
    }

    Serial.println("Motor Disabled");
    MotorPWM = 0;

    //--------------------------------------------//
    // Colour Sensor Test
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Colour Test");

    Serial.println("//------------------//");
    Serial.println("Colour Sensor Test");

    while (counter < 50) {
      int r, g, b;
      r = process_red_value();
      delay(100);
      g = process_green_value();
      delay(100);
      b = process_blue_value();
      delay(100);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("R G B Values");

      lcd.setCursor(0, 1);
      lcd.print(r);
      lcd.setCursor(5, 1);
      lcd.print(g);
      lcd.setCursor(10, 1);
      lcd.print(b);

      Serial.print(counter);
      Serial.println(" RBG Values");
      Serial.print("R = ");
      Serial.print(r);
      Serial.print(" G = ");
      Serial.print(g);
      Serial.print(" B = ");
      Serial.println(b);

      tone(Buzzer, NOTE_F3);
      delay(100);
      noTone(Buzzer);

      counter++;
    }

    tone(Buzzer, NOTE_A7);
    delay(400);
    tone(Buzzer, NOTE_D8);
    delay(200);
    noTone(Buzzer);
  }

  counter = 0;

  //--------------------------------------------//
  // Wait for Debug button to be released
  if (DebugStat == 0) {
    while (DebugStat == 0) {
      DebugStat = digitalRead(DebugButton);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Depress Debug");
      lcd.setCursor(0, 1);
      lcd.print("Button on Pin 4");
      delay(3000);

      tone(Buzzer, NOTE_CS3);
      delay(200);
      noTone(Buzzer);
    }
  }
} // End of setup()

//--------------------------------------------//
//--------------------------------------------//
void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Arduino is");
  lcd.setCursor(0, 1);
  lcd.print("Ready");
  delay(200);

  ProxiStat = digitalRead(Proximity);

  //--------------------------------------------//
  // Detecting Item Placed in front of US Sensor
  // Measure Distance on US until reading makes sense
  // Enable Conveyor and Read Movement
  // If No Movement towards Conveyor Detected, ramp down RPM
  // At RPM Zero, throw object error

  // Pulse the Trigger Pin
  digitalWrite(USonicTx, LOW);
  delayMicroseconds(2);
  digitalWrite(USonicTx, HIGH);
  delayMicroseconds(10);
  digitalWrite(USonicTx, LOW);

  PulseRx = pulseIn(USonicRx, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance Calc  ");

  distance = PulseRx * 0.017;
  lcd.setCursor(12, 1);
  lcd.print("        cm");
  lcd.setCursor(0, 1);
  lcd.print(distance);

  if (distance > 100) {
    lcd.setCursor(0, 0);
    lcd.print("No Item Present  ");
  }

  if (distance < 100 && ProxiStat == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Item Present?..  ");

    itempresent = 1;
    float interdistance = distance;

    delay(1000);

    // Item Check System
    while (MotorPWM < 100) {
      analogWrite(Motor, MotorPWM);
      delay(20);
      MotorPWM++;
    }

    // Update distance after moving conveyor
    digitalWrite(USonicTx, LOW);
    delayMicroseconds(2);
    digitalWrite(USonicTx, HIGH);
    delayMicroseconds(10);
    digitalWrite(USonicTx, LOW);

    PulseRx = pulseIn(USonicRx, HIGH);
    distance = PulseRx * 0.017;

    if (distance / 100 < interdistance / 100 && ProxiStat == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Item Detected   ");
      itempresent = 1;
    } else {
      itempresent = 0;
      while (MotorPWM != 0) {
        analogWrite(Motor, MotorPWM);
        delay(10);
        MotorPWM--;
      }

      lcd.setCursor(0, 0);
      lcd.print("Not Detected");
      lcd.setCursor(0, 1);
      lcd.print("System Idle");

      byte timedown = 10;
      while (timedown > 0) {
        lcd.setCursor(14, 1);
        lcd.print("    ");
        lcd.setCursor(14, 1);
        lcd.print(timedown);
        timedown--;
        delay(999);
      }
    }
  } // If Distance < 100

  if (itempresent == 1 && ProxiStat == 1) {
    while (MotorPWM < 255 && distance < 20) {
      analogWrite(Motor, MotorPWM);
      delay(20);

      digitalWrite(USonicTx, LOW);
      delayMicroseconds(2);
      digitalWrite(USonicTx, HIGH);
      delayMicroseconds(10);
      digitalWrite(USonicTx, LOW);

      PulseRx = pulseIn(USonicRx, HIGH);
      distance = PulseRx * 0.017;
      MotorPWM++;
    }

    if (itempresent == 1 && distance < 20 && ProxiStat == 1) {
      float errordistance = distance - 5;
      if (errordistance > 0) {
        MotorPWM = 255 - errordistance * 12.7;
      }
    }
  }

  //--------------------------------------------//
  // Item Should have Arrived at Proximity
  // Make Sure Motor has Stopped

  ProxiStat = digitalRead(Proximity);

  if (ProxiStat == 0 && MotorPWM > 0) {
    MotorPWM--;
    delay(10);
  }

  if (ProxiStat == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Proxi Detected");

    // Check Colour of Item
    int r, g, b;
    r = process_red_value();
    delay(100);
    g = process_green_value();
    delay(100);
    b = process_blue_value();
    delay(100);

    lcd.setCursor(0, 1);
    lcd.print("Colour Measured");
    delay(1500);

    // Determine dominant colour and set servo
    if (r < g && r < b) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Max Hue = Red");

      int noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(300);

      noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(200);
      noTone(Buzzer);

      ServoPWM = RedBin;
    }

    if (g < r && g < b) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Max Hue = Green");

      int noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(300);

      noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(200);
      noTone(Buzzer);

      ServoPWM = GreenBin;
    }

    if (b < r && b < g) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Max Hue = Blue");

      int noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(300);

      noterand = random(31, 4978);
      tone(Buzzer, noterand);
      delay(200);
      noTone(Buzzer);

      ServoPWM = BlueBin;
    }
  }

  //--------------------------------------------//
  // Easter Egg: Play random notes if Debug button is held
  DebugStat = digitalRead(DebugButton);

  if (DebugStat == 0) {
    while (DebugStat == 0) {
      DebugStat = digitalRead(DebugButton);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Easter Egg");

      int noterand = random(31, 4978);
      int notetime = random(50, 400);

      lcd.setCursor(0, 1);
      lcd.print("Note # ");
      lcd.print(noterand);

      lcd.setCursor(11, 1);
      lcd.print(notetime);

      lcd.setCursor(15, 1);
      lcd.print("ms");

      tone(Buzzer, noterand);
      delay(notetime);
      noTone(Buzzer);
    }
  }
} // End of loop()

//--------------------------------------------//
// Colour Sensor Processing Functions
// Credits: https://circuitdigest.com/microcontroller-projects/interfacing-color-sensor-with-arduino
//--------------------------------------------//

int process_red_value() {
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, LOW);
  digitalWrite(CS3, LOW);
  int pulse_length = pulseIn(COUT, LOW) * 0.47;
  return pulse_length;
  digitalWrite(CLED, LOW);
}

int process_green_value() {
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, HIGH);
  digitalWrite(CS3, HIGH);
  int pulse_length = pulseIn(COUT, LOW) * 0.35;
  return pulse_length;
  digitalWrite(CLED, LOW);
}

int process_blue_value() {
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, LOW);
  digitalWrite(CS3, HIGH);
  int pulse_length = pulseIn(COUT, LOW) * 0.46;
  return pulse_length;
  digitalWrite(CLED, LOW);
}

//--------------------------------------------//