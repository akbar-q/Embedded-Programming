# 6-DOF ESP Arm

This project demonstrates the control of a 6-DOF robotic arm using an ESP32 and MG996R servos, with object detection via a VL53L0X time-of-flight sensor. The arm performs a pick-and-place sequence when an object is detected within a specified range.

---

## Libraries Required

To run this project, you need to install the following libraries in your Arduino IDE:

- [ESP32Servo](https://github.com/jkb-git/ESP32Servo): For controlling servo motors with the ESP32.
- [Adafruit_VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X): For interfacing with the VL53L0X time-of-flight distance sensor.

You can install these libraries via the Arduino Library Manager or by downloading them from the provided links.

---

## Project Images

Below are images of the assembled 6-DOF ESP Arm:

![Arm Front View](images/arm_front.jpg)
*Front view of the 6-DOF ESP Arm*

![Arm Side View](images/arm_side.jpg)
*Side view of the 6-DOF ESP Arm*

![Arm Gripper Close-up](images/arm_gripper.jpg)
*Close-up of the gripper mechanism*

---

## Servo Motor Pinout & Wiring

The MG996R servo motors have three wires for connection:

- **Brown:** GND (connect to ESP32 GND and external power supply GND)
- **Red:** VCC (connect to 5V external power supply)
- **Orange:** Signal (connect to ESP32 PWM-capable GPIO pins)

Refer to the pinout diagram below for correct wiring:

![Servo Motor Pinout](images/servo-pinout.png)
*MG996R Servo Motor Pinout and Wiring*

> **Note:** Always use an external 5V power supply for the servos. Connect all grounds (ESP32, servo power, and sensor) together to ensure proper operation.

---

## VL53L0X Sensor Pinout & Wiring

The VL53L0X is a time-of-flight distance sensor that communicates with the ESP32 via I2C. It has the following pins:

- **VIN:** Power supply input (connect to 3.3V or 5V from ESP32)
- **GND:** Ground (connect to ESP32 GND)
- **SCL:** I2C clock (connect to ESP32 I2C SCL pin, e.g., GPIO 22)
- **SDA:** I2C data (connect to ESP32 I2C SDA pin, e.g., GPIO 21)
- **XSHUT:** (optional) Shutdown pin, can be left unconnected or used to reset the sensor
- **GPIO1/INT:** (optional) Interrupt output, not required for basic operation

Refer to the pinout diagram below for correct wiring:

![VL53L0X Pinout](images/VL53L0X.jpg)
*VL53L0X Time-of-Flight Sensor Pinout and Wiring*

**Why these pins are needed:**
- **VIN & GND:** Power the sensor.
- **SCL & SDA:** Enable communication between the ESP32 and the sensor using the I2C protocol.
- **XSHUT & GPIO1:** Optional for advanced features like multiple sensor management or interrupts.

> **Note:** Make sure the I2C pins on the ESP32 match those defined in your code. Typical defaults are GPIO 21 (SDA) and GPIO 22 (SCL).

---

## Features

- **6 Degrees of Freedom:** Full control of base, lower arm, mid arm, upper arm, rotary claw, and gripper.
- **Object Detection:** Uses a VL53L0X time-of-flight sensor to detect objects within range.
- **Smooth Servo Motion:** Custom function for smooth, speed-controlled servo movement.
- **Pick-and-Place Automation:** Automatically picks up and moves objects detected by the sensor.
- **Serial Feedback:** Displays servo positions and sensor readings in the Serial Monitor for debugging and demonstration.

---

## How It Works

1. The VL53L0X sensor continuously measures distance.
2. When an object is detected within 150mm, the arm executes a programmed pick-and-place sequence:
   - Moves all axes to initial positions.
   - Moves to the object, closes the gripper, and lifts the object.
   - Returns to the starting position and releases the object.
3. Servo positions and sensor readings are printed to the Serial Monitor for monitoring.

---

## Hardware Requirements

- ESP32 development board
- 6x MG996R servo motors
- VL53L0X time-of-flight sensor
- External 5V power supply for servos
- Jumper wires, breadboard, and mechanical arm structure

---

## Software Setup

- Arduino IDE with ESP32 board support
- [ESP32Servo](https://github.com/jkb-git/ESP32Servo) library
- [Adafruit_VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X) library

---

## File Structure

```
6-DOF ESP Arm/
│
├── 6DOFArm.ino
├── README.md
└── images/
    ├── arm_front.jpg
    ├── arm_side.jpg
    └── arm_gripper.jpg
```

## Usage

1. Connect the servos and sensor to the ESP32 as specified in the code.
2. Upload `6DOFArm.ino` to your ESP32.
3. Open the Serial Monitor at 115200 baud to view status messages.
4. Place an object within 15cm of the sensor to trigger the pick-and-place sequence.

---

## License

This project is for educational and demonstration purposes.

---