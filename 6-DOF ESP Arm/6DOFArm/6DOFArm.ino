#include <ESP32Servo.h>
//#include "Adafruit_VL53L0X.h"    //  lines 1-2 include the necessary libraries to code the MG996r servos and VL53L0x time of flight sensor.

Servo base;                                                                
Servo lowerarm; 
Servo midarm;                                                          
Servo upperarm;                                                           
Servo rotataryclaw;                                                       
Servo claw;         //lines 4-9 Define the servos in the program and name them, since this is a code for 6 servo arm so 6 servos were defined


//Adafruit_VL53L0X tofSensor = Adafruit_VL53L0X();  //This line adds the time of flight sensor to the program


void displayServoPositions(int pos, const char* servoName) {
  Serial.print(servoName);
  Serial.print(": ");
  Serial.println(pos);
}     //This is a function created to display the position of each servos in the serial monitor while the code is running


void moveServoWithSpeed(Servo &servo, int startPos, int targetPos, int speed, const char* servoName) {
  if (startPos < targetPos) {
    for (int pos = startPos; pos <= targetPos; pos++) {
      servo.write(pos);
      displayServoPositions(pos, servoName); // Optional: Update position in the serial monitor
      delay(speed);
    }
  } else {
    for (int pos = startPos; pos >= targetPos; pos--) {
      servo.write(pos);
      displayServoPositions(pos, servoName); // Optional: Update position in the serial monitor
      delay(speed);
    }
  }
} //This funcions controls how quickly the servo moves from its intial position to the defined position, the servos gets quicker as the value of the speed decreases,
// and the servo gets slower as the value of int speed is increased

void setup(){
  // Initialize serial communication
  Serial.begin(115200);
  //tof sensor activation
//  Serial.println("Adafruit VL53L0X test");
//  if (!tofSensor.begin()) {
//    Serial.println(F("Failed to boot VL53L0X"));
//   while (1);
//  }
//  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));


  // Attach servos to pins
  base.attach(33);
  lowerarm.attach(26);
  upperarm.attach(18);
  midarm.attach(32);
  rotataryclaw.attach(13);
  claw.attach(12);
} // This function runs once on initiating the code and it connects the digital pins of the ESP 32 to the servos, 
// also an IF condition is used to Display the sensor failure in the serial monitor and to display the distance being measured by the tof sensor


void loop() {
  //Object detection by tof
//  VL53L0X_RangingMeasurementData_t measure;
    
//  Serial.print("Reading a measurement... ");
//  tofSensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

//  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  
    
  // Check if object is within 5 cm (50 mm)
 //   if (measure.RangeMilliMeter > 0 && measure.RangeMilliMeter <= 150) {
//Step 2 move all the axes to initial position
moveServoWithSpeed(base, 90, 90, 15, "base");
moveServoWithSpeed(lowerarm, 120, 120, 15, "lowerarm");
moveServoWithSpeed(midarm, 150, 150, 15, "lowerarm");
moveServoWithSpeed(upperarm, 120, 120, 15, "lowerarm");

//Step 2: Moving to pick the object

moveServoWithSpeed(base, 90, 180, 10, "lowerarm");
moveServoWithSpeed(rotataryclaw, 60, 60, 10, "lowerarm");
moveServoWithSpeed(claw, 0, 0, 10, "lowerarm");

//step 3 : Picking the object

moveServoWithSpeed(midarm, 150, 70, 15, "lowerarm");
moveServoWithSpeed(lowerarm, 120, 120, 15, "lowerarm");
moveServoWithSpeed(upperarm, 120, 85, 15, "lowerarm");
moveServoWithSpeed(claw, 0, 180, 10, "lowerarm");

//step 4: Returning to the initial position

moveServoWithSpeed(lowerarm, 120, 120, 10, "lowerarm");
moveServoWithSpeed(midarm, 70, 150, 10, "lowerarm");
moveServoWithSpeed(upperarm, 85, 120, 10, "lowerarm");
moveServoWithSpeed(base, 180, 90, 10, "lowerarm");

//dropping the object 
moveServoWithSpeed(midarm, 150, 70, 15, "lowerarm");
moveServoWithSpeed(lowerarm, 120, 120, 15, "lowerarm");
moveServoWithSpeed(upperarm, 120, 85, 15, "lowerarm");
moveServoWithSpeed(claw, 180, 0, 10, "lowerarm");

// returning to starting point
moveServoWithSpeed(lowerarm, 120, 120, 10, "lowerarm");
moveServoWithSpeed(midarm, 70, 150, 10, "lowerarm");
moveServoWithSpeed(upperarm, 85, 120, 10, "lowerarm");


/* }
} else {
    Serial.println("Out of range"); //This condition here keeps the servos from running incase the object is not detected by the tof sensor
  } 
*/
  delay(300); // Delay for sensor updates
}
