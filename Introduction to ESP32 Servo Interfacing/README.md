# Introduction to ESP32 Servo Interfacing

This project demonstrates how to interface a single servo motor with an ESP32 using the [ESP32Servo](https://github.com/jkb-git/ESP32Servo) library. It is designed as an introduction for Mechatronics students to learn about basic servo control and serial communication on the ESP32 platform.

## Features

- **Serial Communication**: The code prints detailed messages to the Serial Monitor to help students understand what is happening at each step.
- **Single Servo Control**: Demonstrates how to attach a servo to a GPIO pin and move it to different angles using both variables and hardcoded values.
- **Educational Comments**: The code is heavily commented to explain each step and the reasoning behind it.

## What this file covers

- Setting up the ESP32Servo library
- Attaching a single servo to a GPIO pin
- Moving the servo to specific angles (0°, 90°, 180°)
- Using both variable and hardcoded values for servo angles
- Serial output for step-by-step feedback

---

## Example Serial Monitor Output

===========================================
Welcome to the Mechatronics Serial Demo!
This program demonstrates Serial communication. You will see how Serial.print and Serial println work.

Serial.print stays on the same line, while Serial.println moves to a new line.

Mechatronics combines:

Mechanics
Electronics
Computer Science
Control Engineering
Let's get started!

===========================================



**Note:**  
This file covers only single servo interfacing. For controlling five servos simultaneously, please refer to the next file in this project.

## Code Explanation (Step-by-Step for Beginners)

// The following comments explain each part of the code as if you are programming for the first time.

```cpp
#include <ESP32Servo.h> // This line tells the ESP32 to use a special library that knows how to control servo motors.

Servo clawServo;        // Here, we create a "Servo" object called 'clawServo'. Think of this as a remote control for your servo motor.

int clawPin = 18;       // We set the pin number where the servo's signal wire is connected. Pin 18 is used here, but you can change it if needed.
int angle = 50;         // This variable will store the angle we want the servo to move to. It can be any number from 0 to 180.

void setup() {
  Serial.begin(115200); // This starts communication between your ESP32 and your computer, so you can see messages in the Serial Monitor.
  
  // The lines below print messages to the Serial Monitor.
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

  // Now we tell the ESP32 which pin the servo is connected to.
  clawServo.attach(clawPin); // This "attaches" the servo to the pin, so we can control it.

  // We can move the servo using a variable:
  clawServo.write(angle); // This moves the servo to the angle stored in 'angle' (which is 50 here).

  // Or we can move the servo using a hardcoded number:
  //clawServo.write(0);   // This would move the servo to 0 degrees.

  Serial.println("Servo to 0 degrees."); // This prints a message to the Serial Monitor.
}

void loop() {
    // This part of the code runs over and over again.

    Serial.println("Looping... Mechatronics in action!"); // Prints a message every time the loop runs.

    // Now we move the servo to different positions, one after the other, with a short pause in between.
    clawServo.write(0);      // Move the servo to 0 degrees.
    Serial.println("Servo angle: 0"); // Print the current angle.
    delay(500);              // Wait for half a second (500 milliseconds).

    clawServo.write(90);     // Move the servo to 90 degrees.
    Serial.println("Servo angle: 90");
    delay(500);

    clawServo.write(180);    // Move the servo to 180 degrees.
    Serial.println("Servo angle: 180");
    delay(500);
}

// What should you see?
// - The servo will move to 0°, pause, then to 90°, pause, then to 180°, pause, and repeat.
// - The Serial Monitor will show messages telling you what the code is doing and what angle the servo is at.
// - This is a basic example to help you understand how to control a servo and use serial messages for feedback.
```
