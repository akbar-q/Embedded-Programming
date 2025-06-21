# Poster Web Server & Green Feedback Controller

This microcontroller code is part of a three-microcontroller interactive research poster project. It acts as a WiFi web server, allowing visitors to make a "climate pledge" via a tablet interface. When a pledge is made, the poster glows green as positive feedback, reinforcing the impact of individual action.

---

## Project Overview

The interactive poster educates viewers about the UAE’s energy transition and the importance of consumer-driven change. This module provides a digital pledge interface and real-time visual feedback.

**Key Features:**
- Tablet-based web interface for pledges
- NeoPixel LED strip for glowing feedback
- Live pledge counter and background animation
- PWM output for integration with other poster elements

---

## How It Works

- **WiFi Web Server:** Hosts a web page accessible from any device on the same network.
- **Pledge Button:** Visitors press a button to make a pledge.
- **Green Glow Feedback:** LED strip animates green when a pledge is made.
- **Pledge Counter:** Displayed and updated in real time.
- **Idle Animation:** Blue-turquoise effect when idle.
- **PWM Output:** Adjusted based on wind and light sensor readings.

---

## Hardware Connections

- **Microcontroller:** ESP32-S3 (WiFi & PWM support)
- **NeoPixel LED Strip:**  
  - Data Pin: GPIO 12  
  - Number of LEDs: 90
- **Sensors:**  
  - Wind Potentiometer: GPIO 1 (analog)
  - Photoresistor: GPIO 2 (analog)
- **PWM Output:** GPIO 5

**Power & Wiring Notes:**
- Use a regulated 5V supply rated for at least 6A.
- Place a 1000µF (6.3V+) capacitor across 5V and GND near the LED strip.
- Connect power to both ends of the LED strip to prevent voltage drop.
- Ensure all grounds (ESP32, LED strip, power supply) are connected.

**Wiring Diagram:**
```
[ESP32 GPIO 12] ----[NeoPixel DIN]
[ESP32 GPIO 1]  ----[Wind Potentiometer]
[ESP32 GPIO 2]  ----[Photoresistor]
[ESP32 GPIO 5]  ----[PWM Output]
[5V Power Supply 6A+] ----[NeoPixel VCC]----[1000µF+ Capacitor]----[NeoPixel GND]
[ESP32 GND] ----[NeoPixel GND]
```

---

## Web Interface

- **Access:** Tablet or device on the same WiFi network
- **Features:**  
  - Animated "Pledge" button  
  - Live pledge counter  
  - Climate messaging  
  - Responsive design

---

## LED Feedback Patterns

- **Idle:** Blue-turquoise breathing/fading animation
- **Pledge:** Chasing green effect, then all LEDs flash green

---

## Setup & Usage

1. **Update WiFi Credentials:** Edit `ssid` and `password` in the code.
2. **Upload Code:** Flash to your ESP32.
3. **Connect Hardware:** Wire as described above.
4. **Access Web Page:** Check the serial monitor for the IP address and open it on a tablet.
5. **Make a Pledge:** Tap the pledge button to trigger the green glow and increment the counter.

---

## Customization

- **Number of LEDs:** Change `NUM_PIXELS` in the code.
- **Web Page Content:** Edit HTML in `handleRoot()`.
- **Animations:** Modify `backgroundEffect()` and `incrementEffect()`.

---

## Power & Safety

- Use a power supply rated for the full current draw (see below).
- Use a large buffer capacitor to prevent brownouts.
- Keep all grounds connected.

---

## Power Calculations & Electrical Considerations

| Component         | Voltage | Max Current | Notes                                      |
|-------------------|---------|-------------|--------------------------------------------|
| NeoPixel Strip    | 5V      | 5.4A        | 90 LEDs full white; inject power at ends   |
| ESP32 S3 Board    | 5V→3.3V | ~0.2A       | Sensitive to voltage dips                  |
| Buffer Capacitors | 5V/3.3V | —           | ≥220µF each, 1000µF+ on 5V recommended     |

- **NeoPixel Current:** 90 × 60mA = 5.4A (all white, max brightness)
- **Power Supply:** Use at least 6A at 5V for headroom.
- **Voltage Drop:** Use thick, short wires (18 AWG or thicker). Inject power at both ends for long strips.
- **Capacitors:** Place ≥220µF on 5V and 3.3V rails near ESP32 and ≥1000µF near LED strip.

**Code Safeguards:**
- Moderate brightness settings
- Avoids full white across all LEDs
- Favors single-color/low-power animations

---

## License

This project is for educational and demonstration purposes only.

---

*For more details on the full interactive poster, see the main project documentation.*