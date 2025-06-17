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

...

---

## Why Are Logic Operators and Variable Increments Required?

Logic operators and variable increments are essential tools in programming because they allow your code to make decisions and repeat actions based on changing conditions. In real-world applications—like controlling a servo motor—you often need to:

- **Check conditions:** For example, "Is the angle less than 90?" or "Has the servo reached its maximum position?"
- **Respond differently:** Take different actions depending on the situation, such as moving the servo to a new position or resetting it.
- **Change values:** Increase or decrease a variable to track progress, count events, or step through a sequence.

### When to Use `val = val + 30` vs. `val++`

- **`val = val + 30;` or `val += 30;`**  
  Use this when you want to increase the value by more than 1 (e.g., move the servo in steps of 30 degrees).

- **`val++;`**  
  Use this when you want to increase the value by exactly 1 (e.g., count how many times something happens, or move in single steps).

**Example:**
```cpp
angle += 30; // Increases angle by 30 each time
angle++;     // Increases angle by 1 each time
```

---

## Common Operators Reference

| Operator | Name                  | Example         | Description                                 |
|----------|-----------------------|----------------|---------------------------------------------|
| `==`     | Equal to              | `a == b`       | True if a is equal to b                     |
| `!=`     | Not equal to          | `a != b`       | True if a is not equal to b                 |
| `>`      | Greater than          | `a > b`        | True if a is greater than b                 |
| `<`      | Less than             | `a < b`        | True if a is less than b                    |
| `>=`     | Greater than or equal | `a >= b`       | True if a is greater than or equal to b     |
| `<=`     | Less than or equal    | `a <= b`       | True if a is less than or equal to b        |
| `&&`     | Logical AND           | `a && b`       | True if both a and b are true               |
| `||`     | Logical OR            | `a || b`       | True if either a or b is true               |
| `!`      | Logical NOT           | `!a`           | True if a is false                          |
| `++`     | Increment by 1        | `a++`          | Increases a by 1                            |
| `--`     | Decrement by 1        | `a--`          | Decreases a by 1                            |
| `+=`     | Add and assign        | `a += 5`       | Increases a by 5                            |
| `-=`     | Subtract and assign   | `a -= 5`       | Decreases a by 5                            |

---

**Tip:**  
Try changing the increment value or adding your own conditions to see how the logic flow changes!

---
...
---

## Understanding `if`, `else if`, and `else` Statements

Conditional statements are fundamental in programming. They allow your code to make decisions and execute different blocks of code depending on whether certain conditions are true or false.

### When and Why to Use `if`, `else if`, and `else`

- **`if`**: Use this to check a condition. If the condition is true, the code inside the block runs.
- **`else if`**: Use this to check another condition if the previous `if` (or `else if`) was false. You can have multiple `else if` statements.
- **`else`**: Use this as a "catch-all" for when none of the previous conditions were true.

**Example:**
```cpp
if (angle == 0) {
  // Runs if angle is exactly 0
} else if (angle > 0 && angle < 90) {
  // Runs if angle is between 1 and 89
} else {
  // Runs if none of the above conditions are true
}
```

**Why use them?**
- To handle different cases in your logic, such as moving a servo to different positions or responding to sensor input.
- To make your code readable and organized, with clear actions for each possible situation.

---

## Using `switch` and `case` Statements

Sometimes, you have many possible values for a variable and want to run different code for each value. Instead of writing many `if...else if...else` statements, you can use a `switch` statement, which is often clearer and more efficient for this purpose.

### When to Use `switch`

- When you have a single variable that can take on several discrete values (like 0, 1, 2, 3, ...).
- When each value should trigger a different block of code.
- When you want your code to be more readable than a long chain of `if...else if...else`.

**Example:**
```cpp
switch (angle) {
  case 0:
    Serial.println("Servo is at 0 degrees.");
    break;
  case 90:
    Serial.println("Servo is at 90 degrees.");
    break;
  case 180:
    Serial.println("Servo is at 180 degrees.");
    break;
  default:
    Serial.println("Servo is at an intermediate angle.");
    break;
}
```

**Why use `switch`?**
- It is easier to read and maintain when you have many specific values to check.
- It can be more efficient for the compiler to optimize.

**Note:**  
`switch` works best with integer or character values. For ranges or complex conditions, use `if...else if...else`.

---

**Summary Table: When to Use Which**

| Structure         | Best For                                      | Example Use Case                        |
|-------------------|-----------------------------------------------|-----------------------------------------|
| `if`              | Single condition                              | Check if a button is pressed            |
| `if...else`       | Two possible outcomes                         | Light ON or OFF                         |
| `if...else if...` | Multiple, possibly complex, conditions        | Servo in different angle ranges         |
| `switch...case`   | Many specific, discrete values of one variable| Different actions for each menu option  |

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

## About the ESP32Servo Library

This project uses the [ESP32Servo](https://github.com/jkb-git/ESP32Servo) library to control servo motors with the ESP32. The standard Arduino Servo library does not work with the ESP32, so you must use this special library.

### How to Install the ESP32Servo Library

1. **Using the Arduino Library Manager:**
   - Open the Arduino IDE.
   - Go to **Tools > Manage Libraries...**
   - In the Library Manager, search for `ESP32Servo`.
   - Click **Install** on the ESP32Servo library by John Bennett.

2. **Manual Installation:**
   - Visit the [ESP32Servo GitHub page](https://github.com/jkb-git/ESP32Servo).
   - Click the green **Code** button and select **Download ZIP**.
   - In the Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...** and select the downloaded ZIP file.

**Why do you need this library?**  
The ESP32Servo library provides functions to easily attach a servo to a GPIO pin and control its angle, making it simple to use servos with the ESP32.

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

## Using `delay` and `value++` for Servo Speed Control

By combining the `delay()` function and incremental changes to the servo angle (such as `angle++`), you can control how fast the servo arm moves from one position to another. This is a simple way to create smooth, gradual motion instead of jumping instantly to a new angle.

### How It Works

- **`angle++` or `angle += small_value;`**  
  Increases the servo angle by a small amount each loop, moving the arm in small steps.
- **`delay(time_in_ms);`**  
  Pauses the program for a set amount of milliseconds after each step, controlling how quickly the next step happens.

By adjusting the size of the increment and the delay, you can make the servo move faster or slower.

### Example: Smooth Servo Sweep

```cpp
for (int angle = 0; angle <= 180; angle++) {
  clawServo.write(angle);   // Move servo to the current angle
  delay(15);                // Wait 15 milliseconds before next step
}
```
- **Smaller delay (e.g., `delay(5);`)** = Faster movement
- **Larger delay (e.g., `delay(50);`)** = Slower movement
- **Larger increment (e.g., `angle += 5;`)** = Bigger jumps, less smooth

### Why Use This Method?

- **Smooth Movement:** Instead of jumping directly from 0° to 180°, the servo moves smoothly through all intermediate positions.
- **Speed Control:** You can easily adjust the speed by changing the delay or the increment value.
- **Animation Effects:** Useful for robotic arms, animatronics, or any project where you want lifelike motion.

### Try It Yourself

- Change the value of `delay()` to see how it affects speed.
- Try `angle += 2;` or `angle += 5;` for faster, less smooth movement.
- Combine with logic (e.g., only sweep if a button is pressed) for interactive control.

---

**Summary:**  
The combination of a small increment (`angle++`) and a controlled delay (`delay(…)`) is a simple and effective way to control the speed and smoothness of servo movements in your projects.

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