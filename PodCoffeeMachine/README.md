# Pod Coffee Machine Control System

This project is a prototype control system for a pod-based coffee machine, developed for the Barista One90 client brief. The system automates the coffee dispensing process using a microcontroller, temperature sensor, LCD display, and user input buttons.

---

## Project Overview

The coffee machine supports two coffee sizes:
- **Espresso** (short measure)
- **Maxi** (standard measure)

The system detects pod insertion, allows the user to select the coffee size, heats the water, dispenses coffee for the correct time, and includes safety and pause features.

![System Diagram](diagram.png)

---

## Features & Requirements

- **Pod Detection:** Detects when a new single-use coffee pod is inserted.
- **User Selection:** User can select espresso or maxi size.
- **Water Heating:** Heats water to the required temperature.
- **Temperature Indication:** Indicates when water is ready.
- **Timed Dispensing:** Pumps water for 10s (espresso) or 20s (maxi).
- **Process Complete Indication:** Notifies when dispensing is complete.
- **Safety Measures:** Prevents harm and allows pausing on faults.
- **Low Power Mode:** (To be implemented) Enters low power after 10 minutes inactivity.
- **User Feedback:** LCD provides clear instructions and status.

---

## Logic Flow

1. **Startup:** Display welcome message.
2. **Pod Detection:** Wait for pod insertion via IR sensor.
3. **Water Heating:** Activate heater and monitor temperature.
4. **Ready State:** When temperature is reached, allow coffee selection.
5. **Coffee Selection:** User cycles through options with button.
6. **Dispensing:** User holds button to start dispensing; DC motor runs for selected time.
7. **Completion:** Notify user, prompt to remove cup, and purge capsule.
8. **Repeat or Enter Low Power:** Wait for next pod or enter low power mode after inactivity.

---

## Hardware Used

- **Microcontroller:** (e.g., ESP32 or Arduino)
- **LCD Display:** 16x2 I2C LCD
- **Temperature Sensor:** MAX6675 with K-type thermocouple
- **IR Sensor:** For pod detection
- **Button:** For coffee size selection and starting the process
- **DC Motor:** Simulates water pump
- **Heater Control:** Simulated via output pin

---

## How It Works

1. **Startup:** The LCD displays a welcome message.
2. **Pod Detection:** The system waits for a pod to be inserted (detected by IR sensor).
3. **Water Heating:** The heater turns on and heats water. The LCD shows the heating status.
4. **Ready State:** When the water reaches the set temperature, the machine is ready.
5. **Coffee Selection:** The user selects espresso or maxi using the button.
6. **Dispensing:** The user holds the button to start dispensing. The DC motor runs for the set time (10s or 20s).
7. **Completion:** The LCD indicates when coffee is ready and prompts the user to remove the cup.
8. **Capsule Purge:** The system purges the used capsule.
9. **Safety:** The system can be paused if a fault occurs, and includes multiple checks to ensure safe operation.

---

## File Structure

```
PodCoffeeMachine/
│
├── PodCoffeeMachine.ino
├── README.md
└── Diagram.png
```

---

## Usage

1. **Connect all hardware** as per the pin assignments in the code.
2. **Upload** `PodCoffeeMachine.ino` to your microcontroller.
3. **Power on** the system. The LCD will guide you through the process.
4. **Insert a pod** to begin.
5. **Select coffee size** using the button.
6. **Hold the button** to start dispensing.
7. **Remove the cup** when prompted.
8. **Observe safety messages** and system feedback on the LCD.

---

## Development Process

This project follows a structured development process as outlined in the client brief:

- **Task Planning:** Time plan and progress monitoring.
- **Analysis:** Technical specification and test plan based on operational requirements.
- **System Design:** Selection and justification of input/output devices, system schematics, and program structure.
- **Programming:** Well-annotated code, hardware assembly, and refinement for unexpected events.
- **Testing:** System tested against the plan, including unexpected events.
- **Demonstration:** Audio-visual recording of the system in operation.

---

## Notes

- The system uses simulated heating and dispensing for safety.
- The temperature threshold and dispensing times can be adjusted in the code.
- For demonstration, a small temperature increase (e.g., from body heat) is sufficient.

---

## License

This project is for educational and demonstration purposes only.

---