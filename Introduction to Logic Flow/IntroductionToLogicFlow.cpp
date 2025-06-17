#include <ESP32Servo.h>

Servo clawServo;
int clawPin = 18;
int angle = 0; // Start at 0 degrees

void setup() {
  Serial.begin(115200);
  Serial.println("=== Logic Flow Demo: if, else if, else, ++ ===");
  clawServo.attach(clawPin);
}

void loop() {
  // Move the servo to the current angle
  clawServo.write(angle);
  Serial.print("Current angle: ");
  Serial.println(angle);

  // Demonstrate if, else if, else logic
  if (angle == 0) {
    Serial.println("Servo is at the starting position (0 degrees).");
  } else if (angle > 0 && angle < 90) {
    Serial.println("Servo is between 0 and 90 degrees.");
  } else if (angle == 90) {
    Serial.println("Servo is exactly at 90 degrees.");
  } else if (angle > 90 && angle < 180) {
    Serial.println("Servo is between 90 and 180 degrees.");
  } else if (angle >= 180) {
    Serial.println("Servo is at or beyond the maximum (180 degrees). Resetting to 0.");
    angle = -10; // Will become 0 after increment below
  } else {
    Serial.println("Unexpected angle value!");
  }

  delay(1000); // Wait 1 second

  // Increment angle by 30 degrees each time
  angle += 30; // Same as angle = angle + 30;

  // Demonstrate variable increment
  // angle++; // Uncomment to increment by 1 instead

  // Keep angle within 0-180
  if (angle > 180) {
    angle = 180;
  }
}