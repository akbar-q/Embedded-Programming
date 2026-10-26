# Project Matrix

ESP32 pipe-climbing controller using L298N channel B, an HC-SR04-style ultrasonic sensor, limit switch, RGB status LED, and buzzer. It begins ascending immediately at power-on. When the limit switch is pressed, it stops for 11 seconds, then reverses. During descent it stops at 110 cm for 2 seconds, continues descending, and stops permanently at 200 cm. It also stops safely after three failed ultrasonic readings while descending.

## Connections

| ESP32 pin | Connection |
| --- | --- |
| GPIO22 | Buzzer red wire |
| GPIO4 | RGB LED R pin |
| GPIO16 | RGB LED G pin |
| GPIO17 | RGB LED B pin |
| GPIO27 | L298N ENB |
| GPIO26 | L298N IN4 |
| GPIO25 | L298N IN3 |
| GPIO12 | Ultrasonic Echo, through a 5 V to 3.3 V level shifter or resistor divider |
| GPIO14 | Ultrasonic Trig |
| GPIO15 | Limit switch NO terminal |
| GND | RGB LED common cathode, ultrasonic GND, buzzer GND, and L298N GND |

The RGB LED is active-low (normally a common-anode LED): a channel turns on when its ESP32 pin is driven `LOW`. LED colours indicate blue during descent, green while ascending, red when the cycle completes, and yellow while waiting or after an ultrasonic fault.

## Motor Power

Connect the motor to L298N output B (`OUT3` and `OUT4`). Remove the ENB jumper so GPIO27 can control speed. Power the motor from an appropriate external supply on the L298N motor supply terminal; do not power the motor from the ESP32 5 V pin. Join the external supply ground to ESP32 GND.

L298N boards vary: the `+5V` terminal can be either a regulated output or a logic supply input depending on the board and jumper configuration. Check the markings and documentation for the specific board before connecting 5 V.

## Limit Switch

Connect the limit switch COM terminal to GND and its normally open (NO) terminal to GPIO15. The controller uses ESP32 internal pull-up resistance, so the limit switch reads `LOW` when pressed. The switch stops ascent immediately.

## Notes

- HC-SR04 Echo is commonly 5 V logic. A direct connection to GPIO12 can damage the ESP32; use a level shifter or resistor divider.
- Change `MID_DESCENT_DISTANCE_CM`, `FINAL_DESCENT_DISTANCE_CM`, and `MOTOR_SPEED` in `ProjectMatrixController.ino` after bench testing the mechanical assembly.