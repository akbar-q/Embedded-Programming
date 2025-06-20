//--------------------------------------------//
//Defining Notes for Melody Playback

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

//Library Initialization

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//--------------------------------------------//
//Global Variable Definition

//Renaming Pin 3 and Creating Variable
//To Store Status of Switch Toggle
#define enabButton 3
   bool enabStat;

//Buton to Hold-Down to Enable Debugging
#define DebugButton 4
   bool DebugStat = 0;

//IR Proximity Sensor Binary 
#define Proximity 6
   bool ProxiStat;

//Servo Motor 180-Degree
//To Choose Coloured Bin
#define Servo       5
   byte ServoPWM = 150;

//Status and Debug Active Buzzer
#define Buzzer 10

//Melody Tempo
int tempo = 105;

//Conveyor Belt Motor w/ Speed Control
#define Motor     11
    int MotorPWM = 0;

//Ultra Sonic Distance Measurement
//Large Variable for Time Storage
#define USonicTx 3
#define USonicRx 2

unsigned long PulseRx;
float distance;

//Colour Sensor Pinout
#define CS0  7
#define CS1  8
#define CS2  9
#define CS3  12
#define COUT A1
#define CLED A0

//Define 16X2 LCD on I2C Adress 0x27
LiquidCrystal_I2C lcd(0x27,16,2);

//Serial Data Return Status
bool DataRx = 0;

//Serial Bypass Variable
bool SerialBypass = 0;

//Define Servo PWM Values for Bins
#define RedBin   210
#define BlueBin  150
#define GreenBin 90

//General Flags
bool  itempresent   = 1;

//--------------------------------------------//
//Setup a PACMAN Bootup Meoldy

int melody[] = {

  // Pacman
  // Score available at https://musescore.com/user/85429/scores/107109
  NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, //1
  NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
  NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,

  NOTE_B4, 16,  NOTE_B5, 16,  NOTE_FS5, 16,   NOTE_DS5, 16,  NOTE_B5, 32,  //2
  NOTE_FS5, -16, NOTE_DS5, 8,  NOTE_DS5, 32, NOTE_E5, 32,  NOTE_F5, 32,
  NOTE_F5, 32,  NOTE_FS5, 32,  NOTE_G5, 32,  NOTE_G5, 32, NOTE_GS5, 32,  NOTE_A5, 16, NOTE_B5, 8
};

// Note Duration Definition

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

//--------------------------------------------//

void setup() {

//Initailize LCD and its Backlight
lcd.init();
lcd.backlight();

//Setup Buttons
pinMode(DebugButton, INPUT_PULLUP);
pinMode(Proximity,          INPUT);


//UltraSonic Sensor Pin Configuration
pinMode (USonicRx,  INPUT);
pinMode (USonicTx, OUTPUT); 

//Colour Sensor Pin Configuration
pinMode(CS0, OUTPUT);
pinMode(CS1, OUTPUT);
pinMode(CS2, OUTPUT);
pinMode(CS3, OUTPUT);
pinMode(COUT, INPUT);
pinMode(CLED, OUTPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1,  LOW);

//Program Counter
byte counter = 0;

//Hello Project
lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
lcd.print("Colour Sorter");     //Print Serial Debug Status
lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
lcd.print("by Akbar Q   ");      //Print Serial Debug Baud Rate

//--------------------------------------------//
//Play a PACMAN Bootup Meoldy

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }

//--------------------------------------------//

//Check and Enable Debug Mode

DebugStat = digitalRead(DebugButton);
delay(100);

  if (DebugStat == 0){

    Serial.begin(19200);

    lcd.clear();
    lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
    lcd.print("SerialDebugEnabl");     //Print Serial Debug Status
    lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
    lcd.print("Baud = 19 200");      //Print Serial Debug Baud Rate

      tone(Buzzer, NOTE_GS3);
      delay(400);
      tone(Buzzer, NOTE_F1);
      delay(200);     
      noTone(Buzzer);

    delay(5000);

// Wait for Serial Port to Connect

    lcd.clear();                     //Clear Display
    lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
    lcd.print("Serial Connection");  //Print Serial Connection Status
    lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
    lcd.print("is Pending");         //Print Serial Connection Status

    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");
    Serial.println("   ");

    Serial.println("//------------------//");
    Serial.println("Press Any Key on your Keyboard");
    Serial.println("To Iniatialize the Serial Connection");
    Serial.println("...");
    Serial.println("...");
    Serial.println("...");
    Serial.println("//------------------//"); 

    char SerialRx = '0';
    while (DataRx == 0) {

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

//--------------------------------------------//

    if (Serial.available() > 0) {
    // read the incoming byte:
    SerialRx = Serial.read();

      if(SerialRx != 0){
        DataRx = 1;
                   }
              }

  DebugStat = digitalRead(DebugButton);

    if (DebugStat == 1 ){
      
      SerialBypass  = 1;
      DataRx        = 1;

      lcd.clear();                     //Clear Display
      lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
      lcd.print("SerialConnection");   //Print Serial Connection Status
      lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
      lcd.print("is Bypassed     ");   //Print Serial Connection Status

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

    } //While DataRx = 0 
    delay(200);     
  } //If DebugStat = 0 

//If Serial Connection is Present
    if (DataRx == 1){
      
   noTone(Buzzer);
   delay(1000);

      if (SerialBypass == 0){
    lcd.clear();                     //Clear Display
    lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
    lcd.print("SerialConnection");   //Print Serial Connection Status
    lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
    lcd.print("is Established");     //Print Serial Connection Status

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

    Serial.println("//------------------//" );
    Serial.print  ("SerialConnection"   	  );
    Serial.println("is Established"         );
    delay(4000);     

    }

//--------------------------------------------//

    lcd.clear();                     //Clear Display
    lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
    lcd.print("Hold Debug for");   //Print Serial Connection Status
    lcd.setCursor(0,1);              //Setting the Cursor to Line 1 Char 0
    lcd.print("I/O Test");     //Print Serial Connection Status

    delay(4000);

    DebugStat = digitalRead(DebugButton);

//Test All I/O

    if (DebugStat == 0){

      lcd.clear();                   //Clear Display
      lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
      lcd.print("I/O Debug Begin");    //Print
      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Waiting.....");    //Print

      Serial.println("I/O Debug Begin"); 

      tone(Buzzer, NOTE_C7);
      delay(300);
      tone(Buzzer, NOTE_A7);
      delay(200);

//--------------------------------------------//

      delay(2000);

      Serial.println("Running Servo Test");

      lcd.clear();                   //Clear Display
      lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
      lcd.print("Servo Test");       //Print
      
      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Running.....");    //Print 
      noTone(Buzzer);

      Serial.println("Writing PWM = 90");
     
      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Move to Left....");    //Print
      analogWrite(Servo, 90);
      delay(4000);

      Serial.println("Writing PWM = 220");

      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Move to Right....");    //Print
      analogWrite(Servo, 220);
      delay(4000);

      Serial.println("Writing PWM = 140");

      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Move to Middle....");    //Print
      analogWrite(Servo, 140);
      delay(4000);

//--------------------------------------------//

      Serial.println("//------------------//");

      Serial.println("Proximity Sensor Test");

      lcd.clear();
      lcd.setCursor(0,0);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Proximity Read");    //Print

      counter = 0;

      while(counter < 100){
  
  	  ProxiStat = digitalRead(Proximity);
      lcd.setCursor(0,1);
      lcd.print("Status = ");

        if(ProxiStat == 0){
          tone(Buzzer, NOTE_C2);


          Serial.print  ("Test Number ");
          Serial.println(counter);
          
          Serial.println("Proximity Detected");

          lcd.setCursor(10,1);
          lcd.print("High  ");



          delay(50);     
          noTone(Buzzer);
          delay(150);
        }

        else {
          lcd.setCursor(10,1);
          lcd.print("Low   ");

          Serial.print  ("Test Number");
          Serial.println(counter);
          
          Serial.println("Absent");
        } 

      counter++;
                      } //While Counter Proximity Sensor

    counter = 0;
    

//--------------------------------------------//
//UltraSonic Sensor Test

  lcd.clear();                   //Clear Display
  lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
  lcd.print("UltraSonic Test");    //Print 
  lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
  lcd.print("Test Pending   ");  //Print 

  Serial.println("//------------------//");
  Serial.println("UltraSonic Sensor Test");

  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
  lcd.print("US Test Start");    //Print 

  while(counter < 30){ 

  //Pulse the Trigger Pin  
   digitalWrite(USonicTx, LOW);
   delayMicroseconds(2);
   digitalWrite(USonicTx, HIGH);
   delayMicroseconds(10);
   digitalWrite(USonicTx, LOW);

  PulseRx = pulseIn(USonicRx, HIGH);

  lcd.setCursor(0,0);
  lcd.print("Pulse Recieved ");
  Serial.println("Pulse Captured");
  
  tone(Buzzer, NOTE_G1);
  delay(300);
  tone(Buzzer, NOTE_B1);
  delay(200);
  noTone(Buzzer);

  delay(200);

  distance = PulseRx * 0.017;
  lcd.setCursor(13,1);
  lcd.print("        cm");
  lcd.setCursor(0,1);
  lcd.print("           ");
  lcd.setCursor(0,1);
  lcd.print(distance);

  counter++;

  Serial.print(counter);
  Serial.print(" Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

  } //While Counter US Sensor

    counter = 0;

//--------------------------------------------//
//Conveyor Motor Test

  lcd.clear();                   //Clear Display
  lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
  lcd.print("Conveyor Test");    //Print

  Serial.println("//------------------//");
  Serial.println("Motor Test");
  
  tone(Buzzer, NOTE_GS3);
  delay(300);
  tone(Buzzer, NOTE_AS4);
  delay(200);
  noTone(Buzzer);

  delay(2000);

    Serial.println("Motor Test Start");

  lcd.setCursor(0,0);            //Setting the Cursor to Line 1 Char 0 
  lcd.print("Motor Spinning ");  //Print

    lcd.setCursor(0,1);
    lcd.print("PWM = ");

    lcd.setCursor(0,0);
    Serial.println("Motor Ramp Up");

  while(MotorPWM < 255){

    analogWrite(Motor, MotorPWM);
    lcd.setCursor(6,1);
    lcd.print("    ");
    lcd.setCursor(6,1);    
    lcd.print((MotorPWM * 100) / 255);
    lcd.setCursor(10, 1);
    lcd.print("%");

    tone(Buzzer, NOTE_B5);
    delay(20);
    noTone(Buzzer);

    Serial.print  ("Motor Speed = ");
    Serial.print  ((MotorPWM * 100) / 255);
    Serial.println(" %");


    MotorPWM++;

    }

    Serial.println("Motor Ramp Down");

    while(MotorPWM > 1){

    analogWrite(Motor, MotorPWM);
    lcd.setCursor(6,1);
    lcd.print("    ");
    lcd.setCursor(6,1);
    lcd.print((MotorPWM * 100) / 255);
    lcd.setCursor(10, 1);
    lcd.print("%");

    tone(Buzzer, NOTE_B5);
    delay(20);
    noTone(Buzzer);

    Serial.print  ("Motor Speed = ");
    Serial.print((MotorPWM * 100) / 255);
    Serial.println(" %");

    MotorPWM--;

    }

    Serial.println("Motor Disabled");

    MotorPWM = 0;
    
    

//--------------------------------------------//
//Colour Sensor Test

  lcd.clear();                   //Clear Display
  lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
  lcd.print("Colour Test");    //Print

  Serial.println("//------------------//");
  Serial.println("Colour Sensor Test");

  while(counter < 50){

int r, g, b;
  r = process_red_value();
  delay(100);
  g = process_green_value();
  delay(100);
  b = process_blue_value();
  delay(100);

  lcd.clear();                   //Clear Display
  lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
  lcd.print("R G B Values");    //Print

  lcd.setCursor(0,1);            //Setting the Cursor to Line 0 Char 0
  lcd.print(r);    //Print
  lcd.setCursor(5,1);            //Setting the Cursor to Line 0 Char 0
  lcd.print(g);    //Print  
  lcd.setCursor(10,1);            //Setting the Cursor to Line 0 Char 0
  lcd.print(b);    //Print

  Serial.print  (counter);
  Serial.println(" RBG Values");
  Serial.print  ("R = ");
  Serial.print  (r);
  Serial.print  (" G = ");
  Serial.print  (g);
  Serial.print  (" B = ");
  Serial.println(b);


  tone(Buzzer, NOTE_F3);
  delay(100);
  noTone(Buzzer);

  counter++;

    } //While Counter Colour Sensor Loop

    tone(Buzzer, NOTE_A7);
    delay(400);
    tone(Buzzer, NOTE_D8);
    delay(200);
    noTone(Buzzer);    

    } //if DebugStat = 0       

counter = 0;             

//--------------------------------------------//

    if (DebugStat == 0){

      while (DebugStat == 0){

      DebugStat = digitalRead(DebugButton);

      lcd.clear();                   //Clear Display
      lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
      lcd.print("Depress Debug");    //Print Serial Connection Status
      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Button on Pin 4");  //Print Serial Connection Status 
      delay(3000);

      tone(Buzzer, NOTE_CS3);
      delay(200);     
      noTone(Buzzer);
      }

  }

} //Void Setup

//--------------------------------------------//
//--------------------------------------------//

void loop() {

lcd.clear();                   //Clear Display
lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
lcd.print("Arduino is");    //Print Serial Connection Status
lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
lcd.print("Ready");  //Print Serial Connection Status 

delay(200);

ProxiStat = digitalRead(Proximity);

//--------------------------------------------//

//Detecting Item Placed infront of US Sensor
//Measure Distance on US untill reading makes Sense
//Enable Conveyor and Read Movement
//If No Movement towards Conveyor Detected
//Continue to Ramp Down RPM
//At RPM Zero Throw Object Error

  //Pulse the Trigger Pin  
   digitalWrite(USonicTx, LOW);
   delayMicroseconds(2);
   digitalWrite(USonicTx, HIGH);
   delayMicroseconds(10);
   digitalWrite(USonicTx, LOW);

  PulseRx = pulseIn(USonicRx, HIGH);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance Calc  ");

  distance = PulseRx * 0.017;
  lcd.setCursor(12,1);
  lcd.print("        cm");
  lcd.setCursor(0,1);
  lcd.print(distance);

  if(distance > 100){

  lcd.setCursor(0,0);
  lcd.print("No Item Present  ");

  }

  if(distance < 100 && ProxiStat == 1){

  lcd.setCursor(0,0);
  lcd.print("Item Present?..  ");

  itempresent   = 1;
  float interdistance = distance;

  delay(1000);

    //Item Check System

    while(MotorPWM < 100){
      analogWrite(Motor, MotorPWM);
      delay(20);
      MotorPWM++;
    }

   digitalWrite(USonicTx, LOW);
   delayMicroseconds(2);
   digitalWrite(USonicTx, HIGH);
   delayMicroseconds(10);
   digitalWrite(USonicTx, LOW);

  PulseRx = pulseIn(USonicRx, HIGH);
  distance = PulseRx * 0.017;

  if(distance/100 < interdistance/100 && ProxiStat == 1){

  lcd.setCursor(0,0);
  lcd.print("Item Detected   ");
  itempresent   = 1 ;

  }
      else{
        itempresent   = 0;
            while(MotorPWM != 0){
            analogWrite(Motor, MotorPWM);
            delay(10);
            MotorPWM--;
          }

          lcd.setCursor(0,0);
          lcd.print("Not Detected");
          lcd.setCursor(0,1);
          lcd.print("System Idle");

          byte timedown = 10;

            while(timedown > 0){
              lcd.setCursor(14,1);
              lcd.print("    ");
              lcd.setCursor(14,1);
              lcd.print(timedown);
              timedown--;
              delay(999);
              }


      }

  } //If Distance < 100

if (itempresent == 1 && ProxiStat == 1){

      while(MotorPWM < 255 && distance < 20){
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

float errordistance;
      errordistance = distance - 5;

      if (errordistance > 0){
        
        MotorPWM = 255 - errordistance * 12.7;

      }

  }


} //If ItemPresent = 1  

//--------------------------------------------//

//Item Should have Arrived at Proximity
//Make Sure Motor has Stopped

 ProxiStat = digitalRead(Proximity);

 if (ProxiStat == 0 && MotorPWM > 0){
   MotorPWM--;
   delay(10);
 }

  if(ProxiStat == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Proxi Detected");

    //Check Colour of Item
    int r, g, b;
    r = process_red_value();
    delay(100);
    g = process_green_value();
    delay(100);
    b = process_blue_value();
    delay(100);

    lcd.setCursor(0,1);
    lcd.print("Colour Measured");

    delay(1500);

      if ( r < g && r < b){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Max Hue = Red");

        int noterand;

          noterand = random(31, 4978);
          tone(Buzzer, noterand);
          delay(300);

          noterand = random(31, 4978);
          tone(Buzzer, noterand);
          delay(200);
          noTone(Buzzer);

        ServoPWM = RedBin;
      }

      if ( g < r && g < b){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Max Hue = Green");

        int noterand;

          noterand = random(31, 4978);
          tone(Buzzer, noterand);
          delay(300);

          noterand = random(31, 4978);
          tone(Buzzer, noterand);
          delay(200);
          noTone(Buzzer);

        ServoPWM = GreenBin;
      }

      if ( b < r && b < g){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Max Hue = Blue");

        int noterand;

          noterand = random(31, 4978);
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

DebugStat = digitalRead(DebugButton);

if (DebugStat == 0){

      while (DebugStat == 0){

      DebugStat = digitalRead(DebugButton);

      lcd.clear();                   //Clear Display
      lcd.setCursor(0,0);            //Setting the Cursor to Line 0 Char 0
      lcd.print("Easter Egg");    //Print Serial Connection Status

          int noterand;
          int notetime;

          noterand = random(31, 4978);
          notetime = random(50, 400 );


      lcd.setCursor(0,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("Note # ");  //Print Serial Connection Status 

      lcd.print(noterand);  //Print Serial Connection Status

      lcd.setCursor(11,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print(notetime);  //Print Serial Connection Status 

      lcd.setCursor(15,1);            //Setting the Cursor to Line 1 Char 0
      lcd.print("ms");  //Print Serial Connection Status 

      tone(Buzzer, noterand);
      delay(notetime);     
      noTone(Buzzer);
      }

  } //Debug Stat Easter Egg

//--------------------------------------------//

} //Void Loop

//--------------------------------------------// 
//Setup for Colour Sensor
//Credits:
//https://circuitdigest.com/microcontroller-projects/interfacing-color-sensor-with-arduino
//--------------------------------------------//

int process_red_value()
{
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, LOW);
  digitalWrite(CS3, LOW);
  int pulse_length = pulseIn(COUT, LOW) * 0.47; 
  return pulse_length;
  digitalWrite(CLED, LOW);
}

int process_green_value()
{
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, HIGH);
  digitalWrite(CS3, HIGH);
  int pulse_length = pulseIn(COUT, LOW) * 0.35;
  return pulse_length;
  digitalWrite(CLED, LOW);
}

int process_blue_value()
{
  digitalWrite(CLED, HIGH);
  digitalWrite(CS2, LOW);
  digitalWrite(CS3, HIGH);
  int pulse_length = pulseIn(COUT, LOW) * 0.46;
  return pulse_length;
  digitalWrite(CLED, LOW);
}

//--------------------------------------------//
//--------------------------------------------//