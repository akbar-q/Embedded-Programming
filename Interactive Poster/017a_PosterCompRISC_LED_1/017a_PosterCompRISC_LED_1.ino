//Defining LED Array for Patterns
const int ledPins[] = {21, 32, 19, 33, 26, 17, 25, 18, 5, 16, 4, 2, 14, 27, 12, 22, 23, 13};
const int numLEDs = sizeof(ledPins) / sizeof(ledPins[0]);

//Pins for analog reading
const int analogPin1 = 32;
const int analogPin2 = 33;

// Dead zones to prevent flickering
const int deadZoneLow1 = 100;   // Define the lower dead zone threshold for analogPin1
const int deadZoneHigh1 = 900;  // Define the upper dead zone threshold for analogPin1
const int deadZoneLow2 = 50;    // Define the lower dead zone threshold for analogPin2
const int deadZoneHigh2 = 950;  // Define the upper dead zone threshold for analogPin2

//String 1 - Wind to Grid
#define S1L1 21
#define S1L2 32
#define S1L3 19

//String 2 - PV to Grid
#define S2L1 25
#define S2L2 18
#define S2L3 5

//String 3 - EV to/fro Grid
#define S3L1 16
#define S3L2 4
#define S3L3 2

//String 4 - Fossil to Grid
#define S4L1 33
#define S4L2 26
#define S4L3 17

//Grid Health String
#define   R1 14
#define   R2 27
#define   Y1 12
#define   Y2 22
#define   G1 23
#define   G2 13

//Test Variables
int TestLED;
int Counter;
int     PWM;

int Mode = 0;
int Speed;

int ModePin;
int   PwmIn = 36;

bool SysMode    = 0;
bool SysOveride = 0;

unsigned long pulseWidth = 0;


void setup() {

    pinMode(ModePin, INPUT);
    pinMode(PwmIn,   INPUT);

  //Set Up Pins as Outputs Only
    pinMode(S1L1, OUTPUT);
    pinMode(S1L2, OUTPUT);
    pinMode(S1L3, OUTPUT);

    pinMode(S2L1, OUTPUT);
    pinMode(S2L2, OUTPUT);
    pinMode(S2L3, OUTPUT);

    pinMode(S3L1, OUTPUT);
    pinMode(S3L2, OUTPUT);
    pinMode(S3L3, OUTPUT);

    pinMode(S4L1, OUTPUT);
    pinMode(S4L2, OUTPUT);
    pinMode(S4L3, OUTPUT);

    pinMode(R1,   OUTPUT);
    pinMode(R2,   OUTPUT);
    pinMode(Y1,   OUTPUT);
    pinMode(Y2,   OUTPUT);
    pinMode(G1,   OUTPUT);
    pinMode(G2,   OUTPUT);

  int TestSpeed = 200;

  Serial.begin(9600);

  //Test Code for LED - Change Next Line
    TestLED = 0;
      while(TestLED < 40){
        digitalWrite(TestLED, HIGH);
        Serial.print("Testing LED Pin No: ");
        Serial.println(TestLED);
          delay(TestSpeed);
        digitalWrite(TestLED, LOW);
          delay(100);
       TestLED++;
      }

      Serial.println("LED Test Complete");
/*
  //Toggle All LEDs On/Off
    Serial.println("Starting All LED Fade Test");
    Counter = 0;
        PWM = 0;
    //while( Counter < 10 ){
      while(PWM < 255){
        
        analogWrite(S1L1, PWM);
        analogWrite(S1L2, PWM);
        analogWrite(S1L3, PWM);

        analogWrite(S2L1, PWM);
        analogWrite(S2L2, PWM);
        analogWrite(S2L3, PWM);

        analogWrite(S3L1, PWM);
        analogWrite(S3L2, PWM);
        analogWrite(S3L3, PWM);

        analogWrite(S4L1, PWM);
        analogWrite(S4L2, PWM);
        analogWrite(S4L3, PWM);

        delay(2);
        PWM++;
      }//Fade Up Loop

      while(PWM > 0){
        
        analogWrite(S1L1, PWM);
        analogWrite(S1L2, PWM);
        analogWrite(S1L3, PWM);

        analogWrite(S2L1, PWM);
        analogWrite(S2L2, PWM);
        analogWrite(S2L3, PWM);

        analogWrite(S3L1, PWM);
        analogWrite(S3L2, PWM);
        analogWrite(S3L3, PWM);

        analogWrite(S4L1, PWM);
        analogWrite(S4L2, PWM);
        analogWrite(S4L3, PWM);  

        delay(2);
        PWM--;
      
      }//Fade Down Loop
      Counter++;
    //}//Fade PWM Cycle Counter
  */
}//Setup Close

void loop() {

  Speed = 200;

  //Determine Whether to Go Into Show Mode or Regular Mode
  Mode = digitalRead(ModePin);
  delay(200);

  //If Mode Needs to be Forced to Normal Mode = 0;

  //If Input Signal is High Set Sys to LightShow
  if(Mode == 1 && SysOveride == 0){
      SysMode =  1;
  }


  //If SysOveride is Set High after 1 Cycle Set
  //Sys to Normal Pattern 
  if(Mode == 1 && SysOveride == 1){
      SysMode = 0;
  }

  //If Input Becomes Low and Overide is Low Mode Norma
  if(Mode == 0 && SysOveride == 0){
      SysMode = 0;
  }

  //Clear Overide Flag when Input Low
  if(Mode == 0 && SysOveride == 1 ){
      SysMode = 0;
      SysOveride = 0;
  }

 SysMode = 0;

// Read analog values from pins
  int analogValue1 = analogRead(analogPin1);

delay(200);

  int analogValue2 = analogRead(analogPin2);
delay(200);

  // Calculate the sum of analog values from pins 32 and 35
  int pwmValue = analogValue1 + analogValue2;

  // Map the sum of analog values to the PWM range (0-255)
  pwmValue = map(pwmValue, 2500, 5000, 10, 140);

// Serial print the values
Serial.print("Analog Value 1: ");
Serial.println(analogValue1);
Serial.print("Analog Value 2: ");
Serial.println(analogValue2);
Serial.print("PWM Value: ");
Serial.println(pwmValue);


  // Set different PWM values for each LED
  analogWrite(R1, pwmValue > 14 ? pwmValue - 14 : 0); // Adjust the threshold according to your requirement
  analogWrite(R2, pwmValue > 28 ? pwmValue - 28 : 0);

  analogWrite(Y1, pwmValue > 42 ? pwmValue - 42 : 0);
  analogWrite(Y2, pwmValue > 56 ? pwmValue - 56 : 0);
  
  analogWrite(G1, pwmValue > 70 ? pwmValue - 70 : 0);
  analogWrite(G2, pwmValue > 84 ? pwmValue - 84 : 0);

    // Set different PWM values for each LED
      analogWrite(R1, pwmValue > 14 ? pwmValue - 14 : 0); // Adjust the threshold according to your requirement
      analogWrite(R2, pwmValue > 28 ? pwmValue - 28 : 0);
      analogWrite(Y1, pwmValue > 42 ? pwmValue - 42 : 0);
      analogWrite(Y2, pwmValue > 56 ? pwmValue - 56 : 0);
      analogWrite(G1, pwmValue > 70 ? pwmValue - 70 : 0);
      analogWrite(G2, pwmValue > 84 ? pwmValue - 84 : 0);

  //Defining Up Movement:

if (SysMode == 0){
  Speed = random(300, 600);

    if(Mode == 0){
      //Start Bit
        digitalWrite(S1L1, HIGH);
        digitalWrite(S2L1, HIGH);
        digitalWrite(S3L1, HIGH);
        digitalWrite(S4L1, HIGH);
          delay(Speed);
        digitalWrite(S1L1,  LOW);
        digitalWrite(S2L1,  LOW);
        digitalWrite(S3L1,  LOW);
        digitalWrite(S4L1,  LOW);
      //Second Bit
        digitalWrite(S1L2, HIGH);
        digitalWrite(S2L2, HIGH);
        digitalWrite(S3L2, HIGH);
        digitalWrite(S4L2, HIGH);
          delay(Speed);
        digitalWrite(S1L2,  LOW);
        digitalWrite(S2L2,  LOW);
        digitalWrite(S3L2,  LOW);
        digitalWrite(S4L2,  LOW);
      //Third Bit
        digitalWrite(S1L3, HIGH);
        digitalWrite(S2L3, HIGH);
        digitalWrite(S3L3, HIGH);
        digitalWrite(S4L3, HIGH);
          delay(Speed);
        digitalWrite(S1L3,  LOW);
        digitalWrite(S2L3,  LOW);
        digitalWrite(S3L3,  LOW);
        digitalWrite(S4L3,  LOW);

        Mode = 1;
    }//Up Movement

  //Defining Down Movement:

    if(Mode == 1){
          //Start Bit
            digitalWrite(S1L1, HIGH);
            digitalWrite(S2L1, HIGH);
            digitalWrite(S3L3, HIGH);
            digitalWrite(S4L1, HIGH);
              delay(Speed);
            digitalWrite(S1L1,  LOW);
            digitalWrite(S2L1,  LOW);
            digitalWrite(S3L3,  LOW);
            digitalWrite(S4L1,  LOW);
          //Second Bit
            digitalWrite(S1L2, HIGH);
            digitalWrite(S2L2, HIGH);
            digitalWrite(S3L2, HIGH);
            digitalWrite(S4L2, HIGH);
              delay(Speed);
            digitalWrite(S1L2,  LOW);
            digitalWrite(S2L2,  LOW);
            digitalWrite(S3L2,  LOW);
            digitalWrite(S4L2,  LOW);
          //Third Bit
            digitalWrite(S1L3, HIGH);
            digitalWrite(S2L3, HIGH);
            digitalWrite(S3L1, HIGH);
            digitalWrite(S4L3, HIGH);
              delay(Speed);
            digitalWrite(S1L3,  LOW);
            digitalWrite(S2L3,  LOW);
            digitalWrite(S3L1,  LOW);
            digitalWrite(S4L3,  LOW);

            Mode = 0;
        }//Down Movement
  }//SysMode = 0

  if(SysMode == 1){

  }

}//Void Loop Close

void RunShow() {
  // Call various light show patterns
  rainbowCycle(10);    // Rainbow cycle
  blinkRandom(100, 20); // Blink in random LEDs
  fadeInOut(20); // Fade in/out
  colorWipe(255, 0, 0, 50); // Color wipe (red)
  colorWipe(0, 255, 0, 50); // Color wipe (green)
  colorWipe(0, 0, 255, 50); // Color wipe (blue)
  chase(100); // Chase effect
  randomFlicker(100, 20); // Random flicker
  colorSweep(20); // Color sweep
  strobeEffect(50); // Strobe effect
  alternateBlink(100, 20); // Alternate blink
  fadeInOutSequence(20); // Fade in/out sequence
  delay(1000);
  SysOveride = 1; 
}//Void Run Show

// Rainbow cycle
void rainbowCycle(int wait) {
  for (int j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (int i = 0; i < numLEDs; i++) {
      int hue = (i * 256 / numLEDs + j) % 256;
      analogWrite(ledPins[i], Wheel(hue));
    }
    delay(wait);
  }
}//Rainbow Cycle

// Blink random LEDs
void blinkRandom(int wait, int iterations) {
  for (int n = 0; n < iterations; n++) {
    int ledIndex = random(numLEDs);
    digitalWrite(ledPins[ledIndex], HIGH);
    delay(wait);
    digitalWrite(ledPins[ledIndex], LOW);
    delay(wait);
  }
}//Randomized Blink

// Fade in/out effect
void fadeInOut(int wait) {
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < numLEDs; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(wait);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < numLEDs; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(wait);
  }
}//In/Out Fade

// Color wipe
void colorWipe(int red, int green, int blue, int wait) {
  for (int i = 0; i < numLEDs; i++) {
    analogWrite(ledPins[i], red);
    delay(wait);
    analogWrite(ledPins[i], green);
    delay(wait);
    analogWrite(ledPins[i], blue);
    delay(wait);
  }
}//Somehow Colour Wipe

// Chase effect
void chase(int wait) {
  for (int i = 0; i < numLEDs; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(wait);
    digitalWrite(ledPins[i], LOW);
  }
}//LED Chaser

// Random flicker
void randomFlicker(int wait, int iterations) {
  for (int n = 0; n < iterations; n++) {
    for (int i = 0; i < numLEDs; i++) {
      digitalWrite(ledPins[i], random(2) == 0 ? HIGH : LOW);
    }
    delay(wait);
  }
}//Randomized Flicker

// Color sweep
void colorSweep(int wait) {
  for (int hue = 0; hue < 256; hue++) {
    for (int i = 0; i < numLEDs; i++) {
      analogWrite(ledPins[i], Wheel(hue));
    }
    delay(wait);
  }
}//Sweeping Colour

// Strobe effect
void strobeEffect(int wait) {
  for (int i = 0; i < numLEDs; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  delay(wait);
  for (int i = 0; i < numLEDs; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  delay(wait);
}//LED Strobe

// Alternate blink
void alternateBlink(int wait, int iterations) {
  for (int n = 0; n < iterations; n++) {
    for (int i = 0; i < numLEDs; i += 2) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(wait);
    for (int i = 0; i < numLEDs; i += 2) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(wait);
  }
}//Blink Revisa

// Fade in/out sequence
void fadeInOutSequence(int wait) {
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < numLEDs; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(wait);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < numLEDs; i++) {
      analogWrite(ledPins[i], brightness);
    }
    delay(wait);
  }
}//In/Out Fader
  
// Function to convert hue to RGB value
int Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return WheelPos * 3;
  } else if (WheelPos < 170) {
    return 255 - WheelPos * 3;
  } else {
    return 0;
  }
}//Hue to RGB ???
