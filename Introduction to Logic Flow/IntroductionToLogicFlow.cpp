#include <ESP32Servo.h>

// Create a Servo object to control the servo motor
Servo clawServo;
int clawPin = 18;   // GPIO pin connected to the servo signal wire
int angle = 0;      // Variable to store the current angle of the servo

void setup() {
  Serial.begin(115200); // Start serial communication for debugging
  Serial.println("=== Logic Flow Demo: if, else if, else, ++ ===");
  clawServo.attach(clawPin); // Attach the servo object to the specified pin
}

void loop() {
  // Move the servo to the current angle
  clawServo.write(angle);

  // Print the current angle to the Serial Monitor
  Serial.print("Current Angle: ");
  Serial.println(angle);

  // Demonstrate if, else if, else logic with different conditions
  if (angle == 0) {
    // This block runs if angle is exactly 0
    Serial.println("Servo is at the starting position (0 degrees).");
  } else if (angle > 0 && angle < 90) {
    // This block runs if angle is greater than 0 but less than 90
    Serial.println("Servo is between 0 and 90 degrees.");




  } else if (angle == 90) {
    // This block runs if angle is exactly 90
    Serial.println("Servo is exactly at 90 degrees.");
  } else if (angle > 90 && angle < 180) {
    // This block runs if angle is greater than 90 but less than 180
    Serial.println("Servo is between 90 and 180 degrees.");
  } else if (angle >= 180) {
    // This block runs if angle is 180 or more
    Serial.println("Servo is at or beyond the maximum (180 degrees). Resetting to 0.");
    angle = -10; // Will become 0 after increment below, Wrong on Purpose
  } else {
    // This block runs if none of the above conditions are true
    Serial.println("Unexpected angle value!");
  }

  delay(1000); // Wait 1 second before the next movement

  // Increment angle by 30 degrees each time through the loop
     angle += 30; // Same as angle = angle + 30;
  // angle  = angle + 30; // Alternative way to increment angle


  // Demonstrate variable increment with ++ (uncomment to use)
  // angle++; // Increases angle by 1

  // Keep angle within 0-180 range
  if (angle > 180) {
    angle = 180; // Limit the angle to a maximum of 180
  }
}