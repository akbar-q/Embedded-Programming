# Arduino-Based IoT Automatic Plant Watering System (ESP32 Dev Module / WROOM-32D)

This project implements an IoT smart irrigation dashboard and controller in a single `.ino` file.

## Project path

- `SmartIrrigationC3/SmartIrrigationC3.ino`

## What it does

- Monitors soil moisture continuously
- Monitors temperature and humidity (simulated by default)
- Controls a water pump in 3 modes:
  - **AUTO**: pump turns ON when soil moisture is below threshold
  - **MANUAL**: user turns pump ON/OFF from dashboard
  - **SCHEDULE**: pump runs for configured duration at configured interval
- Hosts a local dashboard directly from ESP32

## Why it matters for UAE sustainability

The system supports:

1. Sustainable water management
2. Reduced water wastage
3. Better water-use efficiency
4. Smart agriculture and greenhouse automation
5. Smart city landscaping integration
6. Net Zero 2050 by reducing unnecessary pumping energy

## ESP32 hotspot details

- SSID: `UAE-Smart-Irrigation`
- Password: `waterwise123`
- URL: `http://192.168.4.1/`

## Setup steps

1. Open `SmartIrrigationC3.ino` in Arduino IDE.
2. Select **ESP32 Dev Module** (WROOM-32D).
3. Upload the code.
4. Open Serial Monitor at `115200` baud.
5. Connect phone/laptop to hotspot `UAE-Smart-Irrigation`.
6. Open `http://192.168.4.1/`.

## Wiring (baseline)

- Soil Moisture Sensor output -> GPIO34 (ADC1)
- Temperature input placeholder -> GPIO35 (ADC1)
- Humidity input placeholder -> GPIO32 (ADC1)
- Pump relay control -> GPIO26

### Relay polarity

- `RELAY_ACTIVE_LOW = true` for most relay modules.
- Change to `false` if your relay is active-high.

## Simulation mode vs real sensors (runtime toggle)

From the dashboard (Control Modes card):

- **Use Dummy** -> simulated demo data
- **Use Sensors** -> reads real ADC inputs

For real hardware, calibrate in code:

- `rawDry`
- `rawWet`
- ADC scaling in `readTemperatureC()` and `readHumidityPct()`

## API endpoints

- `GET /api/status`
- `POST /api/mode?value=AUTO|MANUAL|SCHEDULE`
- `POST /api/manualPump?value=0|1`
- `POST /api/simulated?value=0|1`
- `POST /api/threshold?value=35`
- `POST /api/schedule?intervalMin=20&durationSec=20`

## Main components (as requested concept)

- Arduino/MCU controller role (implemented on ESP32-C3 here)
- Wi-Fi connectivity module (integrated in ESP32-C3)
- Soil moisture sensing
- Temperature/humidity monitoring
- Water pump actuation

## Research references included in dashboard

1. IoT-driven smart irrigation system (2026)
   - https://pubmed.ncbi.nlm.nih.gov/41545460/
2. Smart agriculture IoT irrigation (2025)
   - https://www.sciencedirect.com/science/article/pii/S2772375525003144
