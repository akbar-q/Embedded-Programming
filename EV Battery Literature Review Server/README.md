# EV Battery Tracking Server (ESP32 Dev Module / WROOM-32D)

Self-hosted ESP32 access-point web project for EV battery tracking.

The dashboard shows:

- State of Charge (SOC)
- State of Health (SOH)
- Remaining Useful Life (RUL, cycles)
- Pack voltage, current, power, temperature
- Charging status and smart charging recommendation

## Project structure

- `EVBatteryLitServer/EVBatteryLitServer.ino` -> main single-file project

## ESP32 setup (WROOM-32D)

1. Open `EVBatteryLitServer.ino` in Arduino IDE.
2. Select **ESP32 Dev Module** (WROOM-32D).
3. Upload the sketch.
4. Open Serial Monitor at `115200` baud.
5. Look for:
   - `[OK] AP started`
   - `SSID: EV-Battery-Tracker`
   - `IP: 192.168.4.1`

## Connect and open dashboard

1. Connect phone/laptop to Wi-Fi hotspot:
   - SSID: `EV-Battery-Tracker`
   - Password: `evtrack123`
2. Open browser and visit:
   - `http://192.168.4.1/`

## Sensor mode vs simulation mode (runtime toggle)

From the dashboard System card:

- Click **Switch to Real Sensors** to disable dummy data.
- Click **Switch to Dummy Data** to re-enable simulation.

Default ADC pin mapping (ADC1-safe with Wi-Fi):

- `PIN_BATT_VOLTAGE = GPIO34`
- `PIN_BATT_CURRENT = GPIO35`
- `PIN_BATT_TEMP = GPIO32`

When using real sensors, calibrate scaling in `updateBatteryState()`.

## Notes

- This project is fully self-hosted and does not need internet after flashing.
- The webpage is embedded directly inside the `.ino` file (no LittleFS upload required).
- API endpoints:
   - `GET /api/status`
   - `POST /api/simulated?value=0|1`
