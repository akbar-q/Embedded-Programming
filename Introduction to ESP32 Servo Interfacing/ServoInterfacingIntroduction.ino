#include <ESP32Servo.h> // Include the ESP32 Servo library for controlling servos

Servo clawServo;        // Create a Servo object to control the servo motor

int clawPin = 18;       // GPIO pin number where the servo signal wire is connected (change if needed)

int angle = 50;         // Variable to store the desired servo angle (0 to 180 degrees)

// Servo setup for
  /*
    You can use a variable for the angle:
    The 'clawPin' variable holds the pin number, and 'angle' holds the desired angle.
    This makes your code flexible and easy to change.
  */
  clawServo.attach(clawPin); // Attach the servo to the specified pin (using variable)

  /*
    You can also use a hardcoded number directly instead of a variable:
    For example, to move the servo to 0 degrees, you can write:
    clawServo.write(0);
    This is less flexible, but sometimes useful for quick tests.
  */
  clawServo.write(angle); // Move servo to the angle stored in 'angle' variable
  //clawServo.write(0);   // Move servo to 0 degrees (hardcoded value)

  Serial.println("Servo to 0 degrees."); // Print message to Serial Monitor

void setup() {
  // Introduction for a Mechatronics student
  Serial.begin(115200);
  Serial.println("===========================================");
  Serial.println(" Welcome to the Mechatronics Serial Demo! ");
  Serial.println("===========================================");
  Serial.println();
  Serial.println("This program demonstrates Serial communication.");
  Serial.println("You will see how Serial.print and Serial.println work.");
  Serial.println();
  Serial.print("Serial.print stays on the ");
  Serial.print("same line, ");
  Serial.println("while Serial.println moves to a new line.");
  Serial.println();
  Serial.println("Mechatronics combines:");
  Serial.println("- Mechanics");
  Serial.println("- Electronics");
  Serial.println("- Computer Science");
  Serial.println("- Control Engineering");
  Serial.println();
  Serial.println("Let's get started!");
  Serial.println("-------------------------------------------");

  
}

void loop() {
    // Main code runs repeatedly
    Serial.println("Looping... Mechatronics in action!");

    // Move servo to specific static angles with a delay between each move
    clawServo.write(0);      // Move to 0 degrees (hardcoded value)
    Serial.println("Servo angle: 0");
    delay(500);

    clawServo.write(90);     // Move to 90 degrees (hardcoded value)
    Serial.println("Servo angle: 90");
    delay(500);

    clawServo.write(180);    // Move to 180 degrees (hardcoded value)
    Serial.println("Servo angle: 180");
    delay(500);
}