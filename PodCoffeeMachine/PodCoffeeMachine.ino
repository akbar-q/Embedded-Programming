#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"

// Assign address 0x27 to the LCD address for a 16 char 2 line display
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  int thermoDO = 13;
  int thermoCS = 14;
  int thermoCLK = 12;
  const   int WATER_HEATER_TEMP = 50;
  bool BUTTON_1;
  bool MachineReady;
  
  int coffeeSize = 0;

// Define constants for pod detection, water heater temperature, and dispensing times

  #define IR_SENSOR_PIN     33
const int ESPRESSO_TIME =   10;
const int MAXI_TIME     =   20;
  #define BUTTON_1_PIN      15
  #define WATER_HEATER_PIN  16
  #define DC_MOTOR_PIN      17

// Create the MAX6675 object globally
  MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// float variable for temperature reading
  float waterTemperature;

// Define variables for pod detection status and dispensing time
  int podDetected = 0;
  int dispensingTime = 0;

//------------------------------------------------------//

void setup() {
  pinMode(BUTTON_1_PIN,  INPUT_PULLUP);
  pinMode(IR_SENSOR_PIN,        INPUT);
  pinMode(WATER_HEATER_PIN,    OUTPUT);
  pinMode(DC_MOTOR_PIN,        OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Barista One90     ");

  lcd.setCursor(0, 1);
  lcd.print("Welcome         ");

  delay(3000);

  lcd.setCursor(0, 1);
  lcd.print("At Your Service  ");

  delay(3000);

  Serial.begin(9600);
  Serial.println("MAX6675 test");

  delay(500);  // Wait for MAX chip to stabilize

}

//------------------------------------------------------//

void purgeCapsule() {
  digitalWrite(DC_MOTOR_PIN, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("Purging Capsule     ");

  lcd.setCursor(0, 1);
  lcd.print("/!\ STEAM WARNING /!\        ");  
  delay(5000);
  digitalWrite(DC_MOTOR_PIN, LOW);
}

void tempRead() {
  // Use the globally declared thermocouple object
  waterTemperature = thermocouple.readCelsius();
}

//------------------------------------------------------//

void loop() {
  lcd.clear();
  delay(1000);

    tempRead();
      Serial.print("C = ");
      Serial.println(waterTemperature);
      delay(1000);

//------------------------------------------------------//
//MASTER CONTROL OF PROGRAM FLOW
      
 if (waterTemperature > WATER_HEATER_TEMP){

    MachineReady = 1;

      lcd.setCursor(0, 0);
      lcd.print("Water Heated        "); 

      delay(1000);

      digitalWrite(WATER_HEATER_PIN, LOW);
 }

  else {
    MachineReady = 0;
    digitalWrite(WATER_HEATER_PIN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Water is heating        ");
    delay(2000);
    Serial.println("Heater in Running");
  }
  
//------------------------------------------------------//
//------------------------------------------------------//
// after the pod is detected it should follow the program code 
if(MachineReady == 1 ){

      podDetected = digitalRead(IR_SENSOR_PIN);

      if (podDetected == HIGH) {
        lcd.setCursor(0, 0);
        lcd.print("Waiting for Pod    ");

        lcd.setCursor(0, 1);
        lcd.print("Reusable Pods      ");
          delay(1500);
        lcd.setCursor(0, 1);
        lcd.print("Recommended        ");
          delay(1500);
      }

//------------------------------------------------------//

  while (podDetected == LOW) {

    lcd.setCursor(0, 0);
    lcd.print("Choose Coffee       ");
    //delay(1000);

    //lcd.setCursor(0, 0);
    //lcd.print("Choose Coffee      ");

    
        Serial.println("Water temperature reached 100°C");

  BUTTON_1 = digitalRead(BUTTON_1_PIN);

      Serial.println (BUTTON_1_PIN);

      if (BUTTON_1 == 0){
        delay(500);
          BUTTON_1 = digitalRead(BUTTON_1_PIN);

           if (BUTTON_1 == 1){
                coffeeSize++;  
              lcd.setCursor(0, 0);
              lcd.print("Coffee Changed        ");
              delay(1000);

              Serial.print("Coffee number = ");
              Serial.println(coffeeSize);
           }
        }


          switch (coffeeSize) {
            case 1:
              dispensingTime = ESPRESSO_TIME;
              lcd.setCursor(0, 0);
              lcd.print("                       ");
              lcd.setCursor(0, 1);
              lcd.print("ESPRESSO CHOSEN       ");

              break;

            case 2:
              dispensingTime = MAXI_TIME;
               lcd.setCursor(0, 0);
               lcd.print("                       ");
               lcd.setCursor(0, 1);
               lcd.print("MAXI CHOSEN       ");

              break;
          }

if (coffeeSize > 2){
  coffeeSize = 1;
}

//------------------------------------------------------//

//Start Button For Machine

BUTTON_1 = digitalRead(BUTTON_1_PIN);
delay(200);

  if (BUTTON_1 == 0){
    delay(200);

      BUTTON_1 = digitalRead(BUTTON_1_PIN);
      delay(200);

      if (BUTTON_1 == 0){

          BUTTON_1 = digitalRead(BUTTON_1_PIN);
          delay(200);

              if (BUTTON_1 == 0){

                          BUTTON_1 = digitalRead(BUTTON_1_PIN);
                          delay(200);

                     if (BUTTON_1 == 0){

                          BUTTON_1 = digitalRead(BUTTON_1_PIN);
                          delay(200);

                                if (BUTTON_1 == 0){

                                                            BUTTON_1 = digitalRead(BUTTON_1_PIN);
                                                              delay(200);

                                if (BUTTON_1 == 0){
 
                          if (coffeeSize == 1) {
                            Serial.println("Dispensing espresso...");

                            lcd.setCursor(0, 0);
                            lcd.print("                       ");
                            lcd.setCursor(0, 1);
                            lcd.print("Dispensing espresso...   ");

                          } 
                          
                          if (coffeeSize == 2) {
                            Serial.println("Dispensing maxi...");
                            
                            lcd.setCursor(0, 0);
                            lcd.print("                       ");
                            lcd.setCursor(0, 1);
                            lcd.print("Dispensing maxi...     ");

                          }


                          digitalWrite(DC_MOTOR_PIN, HIGH);
                          delay(dispensingTime * 1000);
                          digitalWrite(DC_MOTOR_PIN, LOW);

                          delay(500);

                          Serial.println("Coffee dispensing complete.");
                          Serial.println("Please remove cup.");

                            lcd.setCursor(0, 0);
                            lcd.print("Coffee is Ready    ");
                            lcd.setCursor(0, 1);
                            lcd.print("Remove Cup         ");

                          delay(10000);

                          purgeCapsule();


           }//Double Check Held Down
        }//Master Close of Start Button
      }
      }}}
  


//------------------------------------------------------//
//------------------------------------------------------//

        podDetected = digitalRead(IR_SENSOR_PIN);

    }//Pod Detect
  }//For Master Temp Check

} //For Void Loop
