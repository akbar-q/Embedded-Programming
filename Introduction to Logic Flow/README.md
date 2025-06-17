# Introduction to Logic Flow with ESP32 and Servo

This project demonstrates how to use **logic flow statements** (`if`, `else if`, `else`) and variable incrementing (`++`, `+=`) in C++ on the ESP32, using a servo motor as a practical example. It is designed for Mechatronics students to learn about conditional statements, comparison operators, and variable manipulation while seeing real-world effects on hardware.

---

## Features

- **Servo Control**: Moves a servo to different angles based on a variable.
- **Logic Flow Demonstration**: Uses `if`, `else if`, and `else` to print different messages and take actions depending on the servo's angle.
- **Variable Incrementing**: Shows how to increase a variable using both `+=` and `++`.
- **Serial Output**: Prints detailed messages to the Serial Monitor for step-by-step understanding.
- **Educational Comments**: The code is heavily commented to explain each step and logic decision.

---

## What this file covers

- Setting up the ESP32Servo library
- Attaching a servo to a GPIO pin
- Using a variable to control the servo's angle
- Demonstrating logic flow with `if`, `else if`, and `else`
- Using comparison operators: `==`, `>`, `<`, `>=`, `<=`
- Incrementing variables with `+=` and `++`
- Serial output for feedback and debugging

---

## Example Serial Monitor Output

```
=== Logic Flow Demo: if, else if, else, ++ ===
Current angle: 0
Servo is at the starting position (0 degrees).
Current angle: 30
Servo is between 0 and 90 degrees.
Current angle: 60
Servo is between 0 and 90 degrees.
Current angle: 90
Servo is exactly at 90 degrees.
Current angle: 120
Servo is between 90 and 180 degrees.
Current angle: 150
Servo is between 90 and 180 degrees.
Current angle: 180
Servo is at or beyond the maximum (180 degrees). Resetting to 0.
Current angle: 0
Servo is at the starting position (0 degrees).
...
```

---

## Code Explanation (Step-by-Step for Beginners)

```cpp
#include <ESP32Servo.h> // Include the library to control servo motors on ESP32

Servo clawServo;        // Create a Servo object called 'clawServo'
int clawPin = 18;       // Set the GPIO pin number for the servo's signal wire
int angle = 0;          // Variable to store the current angle (starts at 0)

void setup() {
  Serial.begin(115200); // Start serial communication for debugging
  Serial.println("=== Logic Flow Demo: if, else if, else, ++ ===");
  clawServo.attach(clawPin); // Attach the servo to the specified pin
}

void loop() {
  clawServo.write(angle); // Move the servo to the current angle

  Serial.print("Current angle: ");
  Serial.println(angle);  // Print the current angle to the Serial Monitor

  // Logic flow demonstration using if, else if, else
  if (angle == 0) {
    // Runs if angle is exactly 0
    Serial.println("Servo is at the starting position (0 degrees).");
  } else if (angle > 0 && angle < 90) {
    // Runs if angle is greater than 0 but less than 90
    Serial.println("Servo is between 0 and 90 degrees.");
  } else if (angle == 90) {
    // Runs if angle is exactly 90
    Serial.println("Servo is exactly at 90 degrees.");
  } else if (angle > 90 && angle < 180) {
    // Runs if angle is greater than 90 but less than 180
    Serial.println("Servo is between 90 and 180 degrees.");
  } else if (angle >= 180) {
    // Runs if angle is 180 or more
    Serial.println("Servo is at or beyond the maximum (180 degrees). Resetting to 0.");
    angle = -10; // Will become 0 after increment below
  } else {
    // Runs if none of the above conditions are true
    Serial.println("Unexpected angle value!");
  }

  delay(1000); // Wait 1 second before the next movement

  // Increment angle by 30 degrees each time through the loop
  angle += 30; // Same as angle = angle + 30;

  // Demonstrate variable increment with ++ (uncomment to use)
  // angle++; // Increases angle by 1

  // Keep angle within 0-180 range
  if (angle > 180) {
    angle = 180; // Limit the angle to a maximum of 180
  }
}
```

---

## What should you see?

- The servo will move to different angles (0, 30, 60, 90, 120, 150, 180), pausing for 1 second at each position.
- The Serial Monitor will show the current angle and a message explaining which logic branch was taken.
- When the angle reaches or exceeds 180, it resets to 0 and the cycle repeats.
- You can experiment by changing the increment value or by uncommenting `angle++;` to see how the logic changes.

---

## Learning Outcomes

- Understand how to use `if`, `else if`, and `else` statements in C++.
- Learn about comparison operators (`==`, `>`, `<`, `>=`, `<=`) and logical AND (`&&`).
- See how variable incrementing works with `+=` and `++`.
- Observe how code logic affects hardware (servo movement) and software (serial output).

---

**Tip:**  
Try changing the increment value or adding your own conditions to see how the logic flow changes!

---