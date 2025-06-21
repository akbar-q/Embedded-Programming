# Poster LED Array Controller

This microcontroller code is part of a three-microcontroller interactive research poster project. This module is responsible for driving and animating the LED arrays that visually represent energy flows, grid health, and user interaction on the poster.

---

## Project Context

The interactive poster presents a visual walkthrough of the UAE’s energy transition, highlighting the dominance of fossil fuels, the rise of renewables, and the importance of decentralized, consumer-driven energy solutions. The LED arrays, LCD, and sensors work together to create an engaging, educational experience for viewers.

**Key Visual Elements on the Poster:**
- Wind Turbine Farm (LEDs)
- Fossil Fuel Plant (LEDs)
- Grid Health (RGB LEDs)
- Interactive 16x2 LCD Display
- "Press Me" Button for Q&A
- Ambient Light Measurement

---

## LED Array Wiring (Pseudo-Diagram)

The LED array is organized into logical "strings" representing different energy sources and grid health indicators. Each string consists of three LEDs, and the grid health is shown with a set of six LEDs (two each for Red, Yellow, Green).

```
+-----------------------------+
|  Poster LED Array Layout    |
+-----------------------------+
|  S1: Wind to Grid           | 21 -- 32 -- 19
|  S2: PV to Grid             | 25 -- 18 -- 5
|  S3: EV to/from Grid        | 16 -- 4  -- 2
|  S4: Fossil to Grid         | 33 -- 26 -- 17
|                             |
|  Grid Health:               |
|    R1 (Red)   - 14          |
|    R2 (Red)   - 27          |
|    Y1 (Yellow)- 12          |
|    Y2 (Yellow)- 22          |
|    G1 (Green) - 23          |
|    G2 (Green) - 13          |
+-----------------------------+
```

**Pin Mapping Table:**

| String         | LED 1 | LED 2 | LED 3 |
|----------------|-------|-------|-------|
| Wind to Grid   | 21    | 32    | 19    |
| PV to Grid     | 25    | 18    | 5     |
| EV to/fro Grid | 16    | 4     | 2     |
| Fossil to Grid | 33    | 26    | 17    |

| Grid Health | Pin |
|-------------|-----|
| R1 (Red)    | 14  |
| R2 (Red)    | 27  |
| Y1 (Yellow) | 12  |
| Y2 (Yellow) | 22  |
| G1 (Green)  | 23  |
| G2 (Green)  | 13  |

---

## How It Works

- **Analog Inputs:** Two analog pins (32, 33) read values (e.g., from sensors or other modules) to influence LED brightness and patterns.
- **Mode Selection:** A digital input pin allows switching between normal and "show" (demo) modes.
- **PWM Control:** Grid health LEDs are driven with varying PWM values to indicate system status.
- **Pattern Animation:** The code cycles through different patterns to visually represent energy flows and grid health.

---

## LED Patterns & Effects

The controller supports a variety of patterns, each representing different states or transitions in the energy system:

- **Sequential Up/Down Movement:** LEDs in each string light up in sequence to show energy flow direction (e.g., wind or solar to grid).
- **PWM Grid Health:** Red, yellow, and green LEDs fade in/out to indicate grid health or status.
- **Light Show Mode:** (Planned) Advanced patterns for demonstration, including:
  - Rainbow cycle
  - Random blinking
  - Fade in/out
  - Color wipe
  - Chase effect
  - Random flicker
  - Color sweep
  - Strobe effect
  - Alternate blink
  - Fade in/out sequence

*These patterns are implemented in functions such as `rainbowCycle()`, `blinkRandom()`, `fadeInOut()`, etc.*

---

## Example Pattern: Sequential Energy Flow

```
Wind to Grid (S1): 21 → 32 → 19
PV to Grid   (S2): 25 → 18 → 5
EV to Grid   (S3): 16 → 4  → 2
Fossil to Grid (S4): 33 → 26 → 17

Each string lights up its LEDs in order, then turns off, simulating energy transfer.
```

---

## Integration with Poster

- **This microcontroller**: Handles all LED animations and grid health indicators.
- **Other microcontrollers**: Manage the LCD, button, and ambient light sensor, communicating with this module as needed.

---

## Customization

- **Add new patterns**: Implement new visual effects in the code for special events or educational focus.
- **Adjust pin mapping**: Change the `ledPins[]` array and string definitions to match your hardware.
- **Integrate with sensors**: Use analog inputs for real-time interaction (e.g., ambient light, user input).

---

## Example Wiring (Schematic)

```
[MCU GPIO] ----[Resistor]----[LED]----GND
(Repeat for each pin in ledPins[])
```

---

## License

This project is for educational and demonstration purposes only.

---

*For more details on the full interactive poster, see the main project documentation.*