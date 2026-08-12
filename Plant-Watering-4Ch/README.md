# 4-Channel Plant Watering System

This project is a simple Arduino-based automatic watering system for 4 plants or 4 watering zones.

It uses:
- 4 soil moisture sensors on `A0`, `A1`, `A2`, `A3`
- 4 relay outputs on pins `8`, `9`, `10`, `11`
- 4 pumps, one for each channel

The code checks only one sensor at a time and runs only one pump at a time. This keeps the design simple and helps stop the power supply from being overloaded by several pumps starting together.

## Project Images

### Arduino
![Arduino board](Images/Arduino.jpg)

### 4-Channel Relay Module
![4 channel relay](Images/4Ch-Relay.jpg)

### Soil Moisture Sensor
![Soil moisture sensor](Images/Sensor.jpg)

### Submersible Pump
![Submersible pump](Images/Submersible-Pump.jpg)

### Example Circuit Build
![Circuit](Images/Circuit.jpg)

## Arduino IDE Screenshots

### Arduino IDE Interface
![Arduino IDE interface](Images/Interface.jpg)

### Select Board Button
![Select board button](Images/Select-Board.png)

### Port Selection Example
![Port selection example](Images/Port.png)

### Serial Monitor Button
![Serial monitor button](Images/Serial-Monitor.png)

## What This System Does

The program works in a repeating 4-step loop:

1. Check sensor 1 and decide if pump 1 should run.
2. Check sensor 2 and decide if pump 2 should run.
3. Check sensor 3 and decide if pump 3 should run.
4. Check sensor 4 and decide if pump 4 should run.

If a channel is dry, its pump turns on for up to 5 seconds, then turns off and the program moves to the next channel.

If a channel is already wet enough, its pump stays off and the program immediately moves on.

This means:
- Only one sensor is handled at a time.
- Only one relay is active at a time.
- Only one pump runs at a time.
- A pump cannot stay on forever during testing.

## Why The 5 Second Timeout Matters

Without a timeout, a dry reading could leave a pump running forever, especially during bench testing when the sensor is not in soil or no water is reaching the plant.

The 5 second timeout makes testing safer because:
- the pump stops automatically
- the code always continues to the next channel
- the system is easier to debug from the Serial Monitor

## Hardware Needed

You will usually need:
- 1 Arduino board compatible with the Arduino IDE
- 4 soil moisture sensors
- 1 four-channel relay module
- 4 small water pumps
- 1 external power supply for the pumps
- tubing for the pumps
- jumper wires
- breadboard or screw terminals if needed
- a shared ground between the Arduino and relay side if required by your wiring setup

## Important Power Note

Do not try to power several water pumps directly from the Arduino.

The Arduino should control the relays.
The pumps should normally be powered from a separate suitable power supply.

This project already reduces load by running only one pump at a time, but you still need a power supply that matches your pump voltage and current requirements.

## Pin Map

### Sensor Inputs

| Channel | Soil Sensor Pin | Arduino Pin |
|---|---|---|
| 1 | Sensor 1 output | A0 |
| 2 | Sensor 2 output | A1 |
| 3 | Sensor 3 output | A2 |
| 4 | Sensor 4 output | A3 |

### Relay Outputs

| Channel | Relay Input | Arduino Pin |
|---|---|---|
| 1 | Relay IN1 | 8 |
| 2 | Relay IN2 | 9 |
| 3 | Relay IN3 | 10 |
| 4 | Relay IN4 | 11 |

## Pin Map Diagram

```mermaid
flowchart LR
    A[Arduino]

    S1[Soil Sensor 1] -->|Signal| A0[A0]
    S2[Soil Sensor 2] -->|Signal| A1[A1]
    S3[Soil Sensor 3] -->|Signal| A2[A2]
    S4[Soil Sensor 4] -->|Signal| A3[A3]

    A -->|D8| R1[Relay 1]
    A -->|D9| R2[Relay 2]
    A -->|D10| R3[Relay 3]
    A -->|D11| R4[Relay 4]

    R1 --> P1[Pump 1]
    R2 --> P2[Pump 2]
    R3 --> P3[Pump 3]
    R4 --> P4[Pump 4]
```

## Simple Wiring Guide

This is the easy way to think about the wiring.

### 1. Soil Sensors

Each soil sensor usually has:
- `VCC`
- `GND`
- `AO` or analog output

Connect them like this:
- Sensor 1 analog output to `A0`
- Sensor 2 analog output to `A1`
- Sensor 3 analog output to `A2`
- Sensor 4 analog output to `A3`
- All sensor `VCC` pins to Arduino `5V` or `3.3V` as required by your module
- All sensor `GND` pins to Arduino `GND`

### 2. Relay Module

The relay module usually has:
- `VCC`
- `GND`
- `IN1`
- `IN2`
- `IN3`
- `IN4`

Connect them like this:
- `IN1` to Arduino pin `8`
- `IN2` to Arduino pin `9`
- `IN3` to Arduino pin `10`
- `IN4` to Arduino pin `11`
- Relay `VCC` to Arduino `5V` if your relay module needs 5V logic
- Relay `GND` to Arduino `GND`

### 3. Pumps and External Power

Each pump should be switched by its own relay channel.

Typical idea:
- external power supply positive goes into the relay switching path
- relay output goes to the pump positive lead
- pump negative lead returns to the power supply negative

The exact relay terminal names are often:
- `COM`
- `NO` for normally open
- `NC` for normally closed

For most watering systems you will usually use:
- `COM`
- `NO`

That way the pump stays off until the relay is triggered.

## How The Code Works

The sketch is in [Plant-Watering-4Ch.ino](Plant-Watering-4Ch.ino).

### User Settings In The Code

These values are near the top of the file:

```cpp
const int moistureThreshold = 700;
const unsigned long pumpRunTimeMs = 5000;
const unsigned long gapBetweenFramesMs = 1000;
```

These are the main user settings you are expected to change when tuning the project.

| Setting | Current Value | What It Does | When To Change It |
|---|---:|---|---|
| `moistureThreshold` | `700` | Decides when the code treats the soil as dry | Change it when your sensor readings do not match real wet/dry conditions |
| `pumpRunTimeMs` | `5000` | Sets the maximum pump run time in milliseconds | Change it if 5 seconds is too short or too long for your test setup |
| `gapBetweenFramesMs` | `1000` | Waits 1 second before moving to the next channel | Change it if you want faster or slower cycling between channels |

What they mean in simple terms:
- `moistureThreshold`: the dryness trigger point
- `pumpRunTimeMs`: how long the pump is allowed to run before timing out
- `gapBetweenFramesMs`: the pause between one channel and the next

### User Settings Explained For Beginners

#### `moistureThreshold`

This is the most important setting.

The code currently uses:

```cpp
if (sensorValue > moistureThreshold)
```

So at the moment:
- readings above `700` are treated as dry
- readings at `700` or below are treated as wet enough

Example:
- sensor reading `820` means dry, so the pump will run
- sensor reading `530` means wet enough, so the pump stays off

If your sensors produce the opposite type of readings, keep the threshold idea the same but reverse the comparison in the code.

#### `pumpRunTimeMs`

This is written in milliseconds.

Useful examples:
- `1000` = 1 second
- `3000` = 3 seconds
- `5000` = 5 seconds
- `10000` = 10 seconds

This setting is a safety limit. Even if a sensor keeps saying the soil is dry, the pump will stop after this time and the code will move to the next channel.

#### `gapBetweenFramesMs`

This is the waiting time after each channel is processed.

With the current setting:
- channel 1 runs or is checked
- the code waits 1 second
- then channel 2 runs or is checked

This small gap helps keep the output readable and slightly spaces out the relay activity.

### Relay Logic Setting

These two lines are important:

```cpp
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;
```

Many relay modules are active-low.

That means:
- writing `LOW` turns the relay on
- writing `HIGH` turns the relay off

If your relay works the opposite way, swap those values.

### Channel Sequence

The sketch uses a variable called `currentFrame`.

That variable decides which channel is checked next:
- frame 0 checks channel 1
- frame 1 checks channel 2
- frame 2 checks channel 3
- frame 3 checks channel 4

Then it goes back to frame 0 and repeats.

### Dry / Wet Decision

Right now the code uses this rule:

```cpp
if (sensorValue > moistureThreshold)
```

That means the current version assumes:
- a higher reading means drier soil
- a lower reading means wetter soil

Some moisture sensors behave the other way around.

If your readings are reversed, change the line above to:

```cpp
if (sensorValue < moistureThreshold)
```

## Arduino IDE Upload Guide

### 1. Open The Sketch

Open [Plant-Watering-4Ch.ino](Plant-Watering-4Ch.ino) in the Arduino IDE.

### 2. Connect Your Arduino

Plug the Arduino into your computer using USB.

### 3. Select The Correct Board

Use the Arduino IDE window shown below.

![Arduino IDE main window](Images/Interface.jpg)

Click the board selector area in the top bar.

![Select board step](Images/Select-Board.png)

Then choose your board and COM port.

In your screenshot example, the selected hardware is:
- Arduino Uno
- COM3

![Board and port example](Images/Port.png)

If your computer shows a different COM port, select that instead. The exact COM number is not always the same on every PC.

### 4. Upload The Code

Click Upload.

When upload is complete, open the Serial Monitor.

### 5. Set The Serial Monitor

Click the Serial Monitor button in the Arduino IDE.

![Open Serial Monitor](Images/Serial-Monitor.png)

Set the Serial Monitor baud rate to:

```text
9600
```

If the baud rate is wrong, the text will look corrupted or you may see no useful output.

## Example Serial Monitor Outputs

At startup, you should see messages showing:
- the project name
- sensor pin list
- relay pin list
- moisture threshold
- pump timeout

### Example 1: Startup Output

```text
========================================
4-Channel Plant Watering System Starting
Sensors: A0, A1, A2, A3
Relays : 8, 9, 10, 11
Moisture threshold: 700
Pump timeout (ms): 5000
Only one channel is checked at a time.
Only one pump is allowed to run at a time.
========================================
```

### Example 2: Channel Is Dry And Pump Runs

```text
----- FRAME 1: Checking Channel 1 -----
Channel 1 sensor reading: 823
Channel 1 threshold check: reading 823 > 700
Channel 1 is DRY. Pump ON.
Channel 1 watering... 0 second(s) elapsed
Channel 1 watering... 1 second(s) elapsed
Channel 1 watering... 2 second(s) elapsed
Channel 1 watering... 3 second(s) elapsed
Channel 1 watering... 4 second(s) elapsed
Channel 1 pump timeout reached. Pump OFF.
Moving to next channel.
```

### Example 3: Channel Is Wet Enough And Pump Stays Off

```text
----- FRAME 2: Checking Channel 2 -----
Channel 2 sensor reading: 450
Channel 2 threshold check: reading 450 > 700
Channel 2 moisture is OK. Pump stays OFF.
Moving to next channel.
```

### Example 4: Full Sequence Across Multiple Channels

```text
----- FRAME 1: Checking Channel 1 -----
Channel 1 sensor reading: 760
Channel 1 threshold check: reading 760 > 700
Channel 1 is DRY. Pump ON.
Channel 1 watering... 0 second(s) elapsed
Channel 1 watering... 1 second(s) elapsed
Channel 1 watering... 2 second(s) elapsed
Channel 1 watering... 3 second(s) elapsed
Channel 1 watering... 4 second(s) elapsed
Channel 1 pump timeout reached. Pump OFF.
Moving to next channel.

----- FRAME 2: Checking Channel 2 -----
Channel 2 sensor reading: 610
Channel 2 threshold check: reading 610 > 700
Channel 2 moisture is OK. Pump stays OFF.
Moving to next channel.

----- FRAME 3: Checking Channel 3 -----
Channel 3 sensor reading: 744
Channel 3 threshold check: reading 744 > 700
Channel 3 is DRY. Pump ON.
Channel 3 watering... 0 second(s) elapsed
Channel 3 watering... 1 second(s) elapsed
Channel 3 watering... 2 second(s) elapsed
Channel 3 watering... 3 second(s) elapsed
Channel 3 watering... 4 second(s) elapsed
Channel 3 pump timeout reached. Pump OFF.
Moving to next channel.

----- FRAME 4: Checking Channel 4 -----
Channel 4 sensor reading: 390
Channel 4 threshold check: reading 390 > 700
Channel 4 moisture is OK. Pump stays OFF.
Moving to next channel.
```

### How To Read These Messages

- `FRAME 1`, `FRAME 2`, `FRAME 3`, `FRAME 4` show which channel is currently being processed.
- `sensor reading` shows the raw analog value from that moisture sensor.
- `threshold check` shows the exact test the code is using.
- `is DRY. Pump ON.` means that channel crossed the watering threshold.
- `moisture is OK. Pump stays OFF.` means no watering was needed.
- `pump timeout reached. Pump OFF.` confirms the 5 second safety timeout worked.

## How To Test It Safely

If you are testing for the first time, do it in small steps.

### Test 1: Sensors Only

Disconnect the pumps.

Upload the code and check the Serial Monitor.

Watch the sensor values for all 4 channels and note whether the reading goes up or down when the sensor is dry.

### Test 2: Relays Only

Leave the pumps disconnected if possible.

Listen for relay clicks while forcing dry conditions on each sensor.

This confirms the Arduino-to-relay side is working.

### Test 3: Full Pump Test

Connect one pump first.

Make sure it runs only when the correct channel is dry and stops after 5 seconds.

Then connect the remaining pumps.

## How To Adjust The Moisture Threshold

The `moistureThreshold` value is not universal.
You need to tune it for your own sensors.

### Easy Method

1. Put a sensor into wet soil and note the reading.
2. Put the same sensor into dry soil and note the reading.
3. Pick a threshold roughly between those two readings.

Example:
- wet soil gives `280`
- dry soil gives `620`
- a starting threshold could be `400`

If watering starts too often, increase or decrease the threshold depending on how your sensor behaves.

With the current sketch set to `700`, that suggests your own testing likely showed drier readings are higher. If your real wet and dry readings are much lower than that, reduce the threshold.

## Common Problems And Fixes

### Problem: Pump never turns on

Check:
- the sensor reading in the Serial Monitor
- the threshold value
- whether your sensor logic is reversed
- whether the relay module is active-low or active-high
- whether the pump power supply is connected properly

### Problem: Pump is always on

Check:
- whether `RELAY_ON` and `RELAY_OFF` are the right way around
- whether the relay is wired to `NO` instead of `NC`
- whether the moisture threshold is set incorrectly

### Problem: The wrong pump runs

Check:
- sensor channel wiring
- relay input wiring
- pump wiring on the relay outputs

### Problem: Readings look strange or unstable

Check:
- loose wires
- poor ground connection
- cheap sensors with noisy output
- whether the sensor is inserted properly into soil

### Problem: Upload works but nothing prints in Serial Monitor

Check:
- Serial Monitor baud rate is `9600`
- the correct board and COM port are selected
- the Arduino has restarted after upload

## Safety Notes

- Keep water away from the Arduino and USB connection.
- Use the correct pump voltage.
- Do not overload the relay contacts.
- Do not power pumps directly from Arduino pins.
- If you are switching higher voltages, use proper isolation and safe wiring practice.

## File In This Folder

- [Plant-Watering-4Ch.ino](Plant-Watering-4Ch.ino) contains the full Arduino sketch.
- [Images/Arduino.jpg](Images/Arduino.jpg) is the Arduino photo.
- [Images/4Ch-Relay.jpg](Images/4Ch-Relay.jpg) is the relay module photo.
- [Images/Sensor.jpg](Images/Sensor.jpg) is the moisture sensor photo.
- [Images/Submersible-Pump.jpg](Images/Submersible-Pump.jpg) is the pump photo.
- [Images/Circuit.jpg](Images/Circuit.jpg) is the circuit photo.
- [Images/Interface.jpg](Images/Interface.jpg) shows the Arduino IDE workspace.
- [Images/Select-Board.png](Images/Select-Board.png) shows where to click to choose the board.
- [Images/Port.png](Images/Port.png) shows the Arduino Uno and COM3 selection example.
- [Images/Serial-Monitor.png](Images/Serial-Monitor.png) shows how to open the Serial Monitor.

## Summary

This project is designed to be simple, safe to test, and easy to understand.

The main ideas are:
- 4 sensors
- 4 relays
- 4 pumps
- one channel checked at a time
- one pump allowed at a time
- 5 second pump timeout for safety and testing

If you want to improve it later, the next common upgrades would be:
- adding an LCD or OLED display
- adding manual start buttons
- adding Wi-Fi logging
- replacing `delay()` with non-blocking timing