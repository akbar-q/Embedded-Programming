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

**Note:**  
This file covers only single servo interfacing. For controlling five servos simultaneously, please refer to the next file in this project.
