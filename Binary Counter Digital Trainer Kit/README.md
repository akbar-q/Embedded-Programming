# Binary Counter Digital Trainer Kit

This project provides an ESP32-based 3-bit binary counter for use with a digital trainer kit. The ESP32 drives three external LEDs to show the binary output bits, and the Serial Monitor is used to choose whether the counter runs upward or downward.

## Project Overview

- **Board:** ESP32
- **Function:** 3-bit binary up/down counter
- **Control Method:** Serial input from the Arduino IDE Serial Monitor
- **Outputs:** 3 GPIO pins that drive external LEDs or LED inputs on a digital trainer kit
- **Count Range:** 0 to 7
- **Default Direction:** Up
- **Update Rate:** 500 ms per count step

---

## Folder Structure

```text
Binary Counter Digital Trainer Kit/
├── BinaryCounterDigitalTrainerKit/
│   └── BinaryCounterDigitalTrainerKit.ino
└── README.md
```

---

## Pin Assignments

The sketch uses three ESP32 GPIO pins for the counter outputs.

| Counter Bit | Binary Weight | ESP32 Pin | Recommended LED Label |
|-------------|---------------|-----------|------------------------|
| Bit 0       | 1             | GPIO 25   | LED0 or Q0             |
| Bit 1       | 2             | GPIO 26   | LED1 or Q1             |
| Bit 2       | 4             | GPIO 27   | LED2 or Q2             |

Bit 0 is the least significant bit. Bit 2 is the most significant bit.

---

## Required Hardware

- 1 x ESP32 development board
- 1 x digital trainer kit with LED section or patch sockets
- 3 x LEDs if your trainer kit does not already include LEDs
- 3 x current-limiting resistors, typically 220 ohm to 330 ohm
- Jumper wires
- USB cable for programming and Serial Monitor access

---

## Wiring Instructions

There are two common ways to wire this project depending on how your digital trainer kit is arranged.

## Option 1: Trainer Kit Has Built-In LEDs With Input Terminals

If your digital trainer kit already has LEDs with sockets or input posts:

1. Connect **ESP32 GND** to the **trainer kit GND**.
2. Connect **GPIO 25** to the trainer kit input for the first LED.
3. Connect **GPIO 26** to the trainer kit input for the second LED.
4. Connect **GPIO 27** to the trainer kit input for the third LED.
5. Make sure the trainer kit LED section is referenced to the same ground as the ESP32.

If the trainer kit LED bank expects a logic signal through a built-in resistor, you may connect directly. If it expects a raw LED connection, include a resistor in series.

## Option 2: Using External LEDs On the Trainer Kit Breadboard Area

If you are using separate LEDs on the trainer kit:

1. Connect the **cathode** of each LED to **GND** through the trainer kit ground rail.
2. Connect the **anode** of each LED to an ESP32 output pin through a **220 ohm to 330 ohm resistor**.
3. Use one LED for each output:
   - GPIO 25 -> resistor -> LED for bit 0
   - GPIO 26 -> resistor -> LED for bit 1
   - GPIO 27 -> resistor -> LED for bit 2
4. Connect the ESP32 ground to the trainer kit ground rail.

### Simple Connection Example

```text
GPIO 25 ----[220R]----|>|---- GND   bit 0
GPIO 26 ----[220R]----|>|---- GND   bit 1
GPIO 27 ----[220R]----|>|---- GND   bit 2
ESP32 GND ------------------- GND rail
```

---

## What the LEDs Mean

The three LEDs display a 3-bit binary number.

| Decimal | Bit 2 | Bit 1 | Bit 0 |
|---------|-------|-------|-------|
| 0       | 0     | 0     | 0     |
| 1       | 0     | 0     | 1     |
| 2       | 0     | 1     | 0     |
| 3       | 0     | 1     | 1     |
| 4       | 1     | 0     | 0     |
| 5       | 1     | 0     | 1     |
| 6       | 1     | 1     | 0     |
| 7       | 1     | 1     | 1     |

When counting up, the display cycles like this:

```text
000 -> 001 -> 010 -> 011 -> 100 -> 101 -> 110 -> 111 -> 000
```

When counting down, it cycles like this:

```text
111 -> 110 -> 101 -> 100 -> 011 -> 010 -> 001 -> 000 -> 111
```

---

## Software Behavior

The sketch:

- starts the Serial port at **115200 baud**
- configures GPIO 25, 26, and 27 as outputs
- updates the LED outputs every **500 ms**
- listens for serial commands to set the direction or directly load a value

The counter wraps automatically:

- up counting: `7 -> 0`
- down counting: `0 -> 7`

---

## Serial Commands

Open the Arduino IDE Serial Monitor and set the baud rate to **115200**.

Then send one of the following commands:

| Command | Action |
|---------|--------|
| `U`     | Count up |
| `D`     | Count down |
| `S`     | Show the current counter state |
| `H`     | Show help information |
| `0` to `7` | Force the counter to a specific value |

Lowercase commands also work.

---

## How To Upload and Run

1. Open the Arduino IDE.
2. Open `BinaryCounterDigitalTrainerKit.ino` from the `BinaryCounterDigitalTrainerKit` folder.
3. Select your **ESP32 board** from the **Tools > Board** menu.
4. Select the correct COM port from **Tools > Port**.
5. Upload the sketch.
6. Open the **Serial Monitor**.
7. Set the baud rate to **115200**.
8. Type `U` and press Send to make the counter run upward.
9. Type `D` and press Send to make the counter run downward.
10. Watch the three LEDs display the binary count.

---

## Example Serial Monitor Session

```text
============================================
ESP32 3-Bit Serial Counter
============================================
Commands:
  U  -> count up
  D  -> count down
  S  -> show current status
  0-7 -> load a value directly into the counter
  H  -> show this help again

System ready | direction: UP | decimal: 0 | binary: 000
Counter updated | direction: UP | decimal: 1 | binary: 001
Counter updated | direction: UP | decimal: 2 | binary: 010
Direction changed | direction: DOWN | decimal: 2 | binary: 010
Counter updated | direction: DOWN | decimal: 1 | binary: 001
Counter updated | direction: DOWN | decimal: 0 | binary: 000
Counter updated | direction: DOWN | decimal: 7 | binary: 111
```

---

## Trainer Kit Lab Procedure

This procedure works well for a classroom or lab demonstration.

1. Power the ESP32 from USB.
2. Build the three LED connections on the digital trainer kit.
3. Verify that all grounds are shared.
4. Upload the sketch.
5. Open the Serial Monitor.
6. Send `S` and confirm the current state is reported.
7. Send `U` and observe the LEDs count from `000` to `111`.
8. Send `D` and observe the LEDs reverse direction.
9. Send `5` and confirm the LEDs immediately show `101`.
10. Send `H` if you want to reprint the command list.

---

## Troubleshooting

## The LEDs do not turn on

- Check that the LED polarity is correct.
- Confirm each LED has a resistor if needed.
- Confirm the ESP32 ground is connected to the trainer kit ground.
- Make sure you are using GPIO 25, 26, and 27.

## The binary pattern looks wrong

- Check that bit 0 is wired to GPIO 25.
- Check that bit 1 is wired to GPIO 26.
- Check that bit 2 is wired to GPIO 27.
- Make sure you are reading the LEDs from bit 2 to bit 0 when interpreting the binary number.

## Serial commands do nothing

- Confirm the Serial Monitor baud rate is set to 115200.
- Make sure the line ending setting does not add unexpected characters. The sketch ignores spaces and line endings, so any normal setting should work.
- Verify that the correct COM port is selected.

---

## Educational Notes

This project is useful for demonstrating:

- binary number representation
- digital output control on the ESP32
- serial communication between a computer and a microcontroller
- the difference between least significant bit and most significant bit
- up counters and down counters in digital systems

---

## License

This project is provided for educational use.
