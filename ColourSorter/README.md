# Colour Sorter

This project is an Arduino-based Colour Sorter that uses a TCS3200 colour sensor, ultrasonic and IR sensors, servo and DC motors, an LCD display, and a buzzer to detect, identify, and sort objects by colour. The system features a conveyor belt, interactive debugging, and musical feedback for user engagement and diagnostics.

---

## Features

- **Automatic Colour Sorting:** Detects objects, measures their colour, and sorts them into bins using a servo.
- **Conveyor Belt Control:** Moves objects into position for colour measurement and sorting.
- **Ultrasonic & IR Sensing:** Detects object presence and position for accurate sorting.
- **LCD Display:** Provides real-time feedback on system status, sensor readings, and sorting results.
- **Buzzer Feedback:** Plays melodies and tones for bootup, status, and debugging.
- **Debug/Test Mode:** Hold the debug button at startup to run comprehensive I/O and sensor tests with serial output.
- **Interactive Easter Egg:** Hidden musical feature accessible via the debug button.

---

## Hardware Requirements

- **Arduino Board** (Uno, Nano, etc.)
- **TCS3200 Colour Sensor**
- **Ultrasonic Sensor** (HC-SR04 or similar)
- **IR Proximity Sensor**
- **Servo Motor** (for bin selection)
- **DC Motor** (for conveyor belt)
- **16x2 I2C LCD Display**
- **Buzzer**
- **Push Buttons** (for enable and debug)
- **Assorted Resistors, Wires, Breadboard, Power Supply**

---

## Pin Assignments

| Component           | Pin(s)      |
|---------------------|-------------|
| Enable Button       | 3           |
| Debug Button        | 4           |
| IR Proximity        | 6           |
| Servo Motor         | 5 (PWM)     |
| Buzzer              | 10          |
| Conveyor Motor      | 11 (PWM)    |
| Ultrasonic Trigger  | 3           |
| Ultrasonic Echo     | 2           |
| Colour Sensor S0    | 7           |
| Colour Sensor S1    | 8           |
| Colour Sensor S2    | 9           |
| Colour Sensor S3    | 12          |
| Colour Sensor OUT   | A1          |
| Colour Sensor LED   | A0          |
| LCD I2C Address     | 0x27        |

---

## Libraries Required

- **Wire.h** (built-in)
- **LiquidCrystal_I2C.h** (for I2C LCD display)

Install `LiquidCrystal_I2C` via the Arduino Library Manager.

---

## How It Works

1. **Startup:**  
    - LCD displays project info.
    - Plays a Pac-Man melody on the buzzer.
    - Optionally enters Debug Mode if the debug button is held.

2. **Debug Mode:**  
    - Tests all I/O: servo, proximity, ultrasonic, motor, and colour sensor.
    - Outputs detailed results to the LCD and Serial Monitor.
    - Easter egg: musical notes when debug button is pressed during operation.

3. **Normal Operation:**  
    - Waits for an object to be detected by the ultrasonic and IR sensors.
    - Moves the conveyor to bring the object into position.
    - Measures the object's colour using the TCS3200 sensor.
    - Determines the dominant colour (Red, Green, or Blue).
    - Rotates the servo to direct the object into the correct bin.
    - Provides feedback on the LCD and via buzzer tones.

---

## File Structure

```
ColourSorter/
├── ColourSorter.ino
├── Diagram.bmp
└── README.md
```

---

## Usage

1. **Wire up the hardware** according to the pin assignments above.
2. **Upload** `ColourSorter.ino` to your Arduino board.
3. **Power on** the system.
4. **(Optional) Hold the debug button** at startup to run diagnostics.
5. **Place an object** on the conveyor. The system will detect, measure, and sort it automatically.
6. **Watch the LCD** for real-time feedback and listen for buzzer tones.

---

## Example LCD Output

```
Colour Sorter by Akbar Q

Arduino is Ready

Distance Calc XX.XX cm

Item Present?..

Max Hue = Red
```

---

## Customization

- **Servo PWM Values:** Adjust `RedBin`, `GreenBin`, and `BlueBin` for your servo and bin positions.
- **Motor Speed:** Tune `MotorPWM` ramp-up/ramp-down for your conveyor.
- **Colour Thresholds:** Modify the logic in the loop for more precise colour detection if needed.

---

## Notes

- The project includes musical feedback for both normal and debug operation.
- Debug mode is highly recommended for troubleshooting hardware connections.
- The code is heavily commented for educational purposes.

---

## System Diagram

A rough system diagram is provided to help you visualize the hardware connections and sorting layout:

![Colour Sorter System Diagram](Diagram.bmp)

- **Red, Blue, Green Bins:** Servo directs sorted items to the correct bin.
- **LCD Display:** Shows status and feedback.
- **Ultrasonic Sensor:** Detects approaching objects.
- **Proximity Sensor:** Confirms object presence for sorting.
- **Colour Sensor:** Identifies object colour.
- **Buzzer:** Plays tones and melodies for feedback.
- **Motor:** Drives the conveyor belt.

Refer to this diagram when assembling your hardware for the Colour Sorter project.

---

## Credits & Code Citations

Some musical note definitions, tone tables, and melody sequences (such as the Pac-Man melody and note frequency tables) used in this project are adapted from or inspired by the work of other Arduino developers. These resources were used to provide accurate musical playback and to enhance the user experience with familiar melodies and sound effects.

Special thanks to the following open-source Arduino projects and contributors for their work on tone generation and melody code, which served as references and inspiration:

- [wobot/pitches.h](https://github.com/reenberg/wobot/tree/ce7f112096ba1abd43b8d29da7e8944c2a004125/arduino-dist/examples/2.Digital/toneMelody/pitches.h)
- [jknight1725/arduino/pitches.h](https://github.com/jknight1725/arduino/tree/cfe1112779f3b34c8559a73bacee721098583fb9/pitches.h)
- [lcortesg/sumo-rc/Songs.ino](https://github.com/lcortesg/sumo-rc/tree/f84231c364d421a4da7ad0df003c2e4cb5ef87a3/Songs.ino)
- [cleggacus/not-pacman-on-arduino/game.ino](https://github.com/cleggacus/not-pacman-on-arduino/tree/0ca71d22488527058e84302fccc560d118de603d/game.ino)
- [claudia-maria-dudau/Pac-Man/buzzer.h](https://github.com/claudia-maria-dudau/Pac-Man/tree/2b50ec19903866e7af606b9fe8e11b396d931704/main/buzzer.h)

These references were used solely for educational and non-commercial purposes to provide musical feedback and enhance the interactivity of the Colour Sorter project.

---

## License

This project is for educational and demonstration purposes only.