/*
  SmartIrrigationC3.ino
  --------------------------------------------------
  ESP32 self-hosted IoT Automatic Plant Watering System
  - Real-time soil moisture monitoring
  - Automatic / Manual / Scheduled watering modes
  - Local web dashboard hosted from ESP32-C3
  - UAE-focused sustainability context

  Notes:
  - This is a single-file project (no LittleFS upload).
  - For first testing, USE_SIMULATED_INPUTS is true.
  - Switch to false when real sensors are wired and calibrated.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

static const char* kApSsid = "UAE-Smart-Irrigation";
static const char* kApPass = "waterwise123";

WebServer server(80);

// ---------- Hardware Pins (ESP32 WROOM-32D safe defaults) ----------
// Use ADC1 pins when Wi-Fi is active (GPIO32-39).
static const int PIN_SOIL_MOISTURE = 34; // ADC1
static const int PIN_TEMP_INPUT = 35;    // ADC1 (placeholder for DHT temp in this simplified build)
static const int PIN_HUM_INPUT = 32;     // ADC1 (placeholder for DHT humidity in this simplified build)
static const int PIN_PUMP_RELAY = 26;    // Digital output to relay or MOSFET

static const bool RELAY_ACTIVE_LOW = true;
static bool useSimulatedInputs = true;

// ---------- Moisture Calibration ----------
// Update these values with real calibration once hardware is connected:
// rawDry = ADC value when sensor is completely dry
// rawWet = ADC value when sensor is fully wet
static int rawDry = 3300;
static int rawWet = 1400;

// ---------- Watering Configuration ----------
enum ControlMode { MODE_AUTO, MODE_MANUAL, MODE_SCHEDULE };
static ControlMode controlMode = MODE_AUTO;

static float moistureThresholdPct = 35.0f; // if below this in AUTO mode -> water ON
static bool manualPumpRequest = false;

// Schedule mode settings
static unsigned long scheduleIntervalMs = 20UL * 60UL * 1000UL; // every 20 minutes
static unsigned long scheduleDurationMs = 20UL * 1000UL;        // water for 20 seconds
static unsigned long lastScheduleStartMs = 0;
static bool scheduleWindowActive = false;

// Runtime state
struct IrrigationState {
  float soilMoisturePct;
  float temperatureC;
  float humidityPct;
  bool pumpOn;
  String recommendation;
};

static IrrigationState state;
static unsigned long bootMs = 0;

static float clampf(float v, float low, float high) {
  if (v < low) return low;
  if (v > high) return high;
  return v;
}

static float mapf(float x, float inMin, float inMax, float outMin, float outMax) {
  if (inMax == inMin) return outMin;
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

static const char* modeToText(ControlMode mode) {
  if (mode == MODE_AUTO) return "AUTO";
  if (mode == MODE_MANUAL) return "MANUAL";
  return "SCHEDULE";
}

static bool textToMode(const String& value, ControlMode& outMode) {
  if (value.equalsIgnoreCase("AUTO")) {
    outMode = MODE_AUTO;
    return true;
  }
  if (value.equalsIgnoreCase("MANUAL")) {
    outMode = MODE_MANUAL;
    return true;
  }
  if (value.equalsIgnoreCase("SCHEDULE")) {
    outMode = MODE_SCHEDULE;
    return true;
  }
  return false;
}

static void writePump(bool on) {
  int level = on ? (RELAY_ACTIVE_LOW ? LOW : HIGH) : (RELAY_ACTIVE_LOW ? HIGH : LOW);
  digitalWrite(PIN_PUMP_RELAY, level);
  state.pumpOn = on;
}

static float readSoilMoisturePct() {
  if (useSimulatedInputs) {
    float t = (millis() - bootMs) / 1000.0f;
    float wave = 0.5f + 0.5f * sinf(t * 0.05f);
    return clampf(22.0f + wave * 56.0f, 5.0f, 95.0f);
  }

  int raw = analogRead(PIN_SOIL_MOISTURE);
  // rawDry -> 0%, rawWet -> 100%
  float pct = mapf((float)raw, (float)rawDry, (float)rawWet, 0.0f, 100.0f);
  return clampf(pct, 0.0f, 100.0f);
}

static float readTemperatureC() {
  if (useSimulatedInputs) {
    float t = (millis() - bootMs) / 1000.0f;
    return 27.0f + 5.0f * fabsf(sinf(t * 0.04f));
  }

  int raw = analogRead(PIN_TEMP_INPUT);
  return mapf((float)raw, 0.0f, 4095.0f, 20.0f, 45.0f);
}

static float readHumidityPct() {
  if (useSimulatedInputs) {
    float t = (millis() - bootMs) / 1000.0f;
    return 35.0f + 35.0f * (0.5f + 0.5f * sinf(t * 0.03f));
  }

  int raw = analogRead(PIN_HUM_INPUT);
  return mapf((float)raw, 0.0f, 4095.0f, 20.0f, 90.0f);
}

static String buildRecommendation(float moisture, float tempC, bool pumpOnNow) {
  if (tempC > 40.0f && moisture < 30.0f) {
    return "Hot and dry conditions detected: increase irrigation frequency carefully.";
  }
  if (moisture < moistureThresholdPct && !pumpOnNow) {
    return "Soil below threshold: irrigation should start now.";
  }
  if (moisture > 80.0f && pumpOnNow) {
    return "Moisture high: stop irrigation to prevent over-watering.";
  }
  if (moisture > 65.0f) {
    return "Soil moisture healthy: no irrigation needed right now.";
  }
  return "Monitoring active: maintain current watering strategy.";
}

static void applyControlLogic() {
  unsigned long now = millis();
  bool desiredPump = false;

  if (controlMode == MODE_AUTO) {
    desiredPump = state.soilMoisturePct < moistureThresholdPct;
  } else if (controlMode == MODE_MANUAL) {
    desiredPump = manualPumpRequest;
  } else {
    if (!scheduleWindowActive) {
      if (lastScheduleStartMs == 0 || (now - lastScheduleStartMs) >= scheduleIntervalMs) {
        scheduleWindowActive = true;
        lastScheduleStartMs = now;
      }
    }

    if (scheduleWindowActive) {
      desiredPump = true;
      if ((now - lastScheduleStartMs) >= scheduleDurationMs) {
        scheduleWindowActive = false;
        desiredPump = false;
      }
    }
  }

  writePump(desiredPump);
}

static void refreshSensorsAndState() {
  state.soilMoisturePct = readSoilMoisturePct();
  state.temperatureC = readTemperatureC();
  state.humidityPct = readHumidityPct();

  applyControlLogic();

  state.recommendation = buildRecommendation(state.soilMoisturePct, state.temperatureC, state.pumpOn);
}

static const char kPageHtml[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>IoT Automatic Plant Watering • ESP32-C3</title>
  <style>
    :root{--bg:#0a1422;--panel:#0f1d34;--panel2:#122441;--text:#e8efff;--muted:#9fb2d8;--line:rgba(232,239,255,.10);--ok:#22c55e;--warn:#f59e0b;--bad:#ef4444;--accent:#60a5fa}
    *{box-sizing:border-box} html,body{height:100%}
    body{margin:0;font-family:ui-sans-serif,system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;background:radial-gradient(900px 500px at 20% -10%,rgba(96,165,250,.22),transparent 60%),radial-gradient(600px 380px at 85% 0%,rgba(34,197,94,.15),transparent 60%),var(--bg);color:var(--text)}
    .top{position:sticky;top:0;z-index:20;background:rgba(10,20,34,.78);backdrop-filter:blur(10px);border-bottom:1px solid var(--line)}
    .top-inner{max-width:1180px;margin:0 auto;padding:14px 18px;display:flex;justify-content:space-between;align-items:center;gap:12px}
    .title{font-size:16px;font-weight:800}.sub{font-size:12px;color:var(--muted);margin-top:2px}
    .pill{font-size:12px;color:var(--muted);padding:8px 10px;border-radius:999px;border:1px solid var(--line)}
    .wrap{max-width:1180px;margin:0 auto;padding:18px}
    .grid{display:grid;grid-template-columns:repeat(12,1fr);gap:12px}
    .card{grid-column:span 6;background:linear-gradient(180deg,var(--panel),var(--panel2));border:1px solid var(--line);border-radius:16px;padding:14px;box-shadow:0 14px 48px rgba(0,0,0,.28)}
    .wide{grid-column:span 12}
    .head{display:flex;justify-content:space-between;align-items:center;gap:10px;margin-bottom:10px}
    h2{margin:0;font-size:14px}.status{font-size:12px;color:var(--muted)}
    .kpis{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}
    .kpi{border:1px solid var(--line);border-radius:12px;padding:10px;background:rgba(10,20,34,.35)}
    .k-label{font-size:12px;color:var(--muted)} .k-val{margin-top:6px;font-size:20px;font-weight:900}
    .meter{height:9px;border-radius:999px;background:rgba(255,255,255,.08);margin-top:8px;overflow:hidden}
    .fill{height:100%;width:0%;background:linear-gradient(90deg,var(--ok),var(--accent));transition:width .3s ease}
    .btns{display:flex;flex-wrap:wrap;gap:8px}
    button{background:#1b335a;color:var(--text);border:1px solid var(--line);padding:8px 12px;border-radius:10px;cursor:pointer;font-weight:600}
    button:hover{filter:brightness(1.1)}
    .btn-on{background:#225c36}.btn-off{background:#5a2424}
    .row{display:grid;grid-template-columns:160px 1fr auto;gap:8px;align-items:center;margin:10px 0}
    input[type=range]{width:100%}
    input[type=number]{width:100px;background:#0d1b30;color:var(--text);border:1px solid var(--line);padding:6px 8px;border-radius:8px}
    .rec{border:1px solid var(--line);padding:12px;border-radius:12px;background:rgba(255,255,255,.03);line-height:1.5}
    .list{display:grid;gap:8px}
    .list-item{display:flex;justify-content:space-between;gap:8px;padding:9px 10px;border:1px solid var(--line);border-radius:10px;background:rgba(10,20,34,.3);font-size:13px}
    .muted{color:var(--muted)}
    .uae{font-size:13px;line-height:1.6;color:#d3e1ff}
    a{color:#9ec5ff}
    @media (max-width:940px){.card{grid-column:span 12}.kpis{grid-template-columns:1fr}.row{grid-template-columns:1fr}}
  </style>
</head>
<body>
  <header class="top">
    <div class="top-inner">
      <div>
        <div class="title">IoT Automatic Plant Watering System</div>
        <div class="sub">Self-hosted ESP32-C3 dashboard • Auto / Manual / Schedule irrigation</div>
      </div>
      <div class="pill">AP: <span id="ssid">--</span> • IP: <span id="ip">--</span></div>
    </div>
  </header>

  <main class="wrap">
    <section class="grid">
      <article class="card wide">
        <div class="head"><h2>Live Irrigation Metrics</h2><div class="status" id="modeStatus">Mode: --</div></div>
        <div class="kpis">
          <div class="kpi"><div class="k-label">Soil Moisture</div><div class="k-val" id="moisture">--%</div><div class="meter"><div class="fill" id="moistFill"></div></div></div>
          <div class="kpi"><div class="k-label">Temperature</div><div class="k-val" id="temp">-- °C</div></div>
          <div class="kpi"><div class="k-label">Humidity</div><div class="k-val" id="hum">-- %</div></div>
        </div>
      </article>

      <article class="card">
        <div class="head"><h2>Control Modes</h2><div class="status" id="pumpStatus">Pump: --</div></div>
        <div class="btns">
          <button onclick="setMode('AUTO')">AUTO</button>
          <button onclick="setMode('MANUAL')">MANUAL</button>
          <button onclick="setMode('SCHEDULE')">SCHEDULE</button>
        </div>

        <div class="row">
          <div>Manual Pump</div>
          <div class="muted">Only used in MANUAL mode</div>
          <div class="btns">
            <button class="btn-on" onclick="setManual(1)">ON</button>
            <button class="btn-off" onclick="setManual(0)">OFF</button>
          </div>
        </div>

        <div class="row">
          <div>Dummy Data</div>
          <div class="muted">Toggle between simulated and real sensor input</div>
          <div class="btns">
            <button class="btn-on" onclick="setSimulated(1)">Use Dummy</button>
            <button class="btn-off" onclick="setSimulated(0)">Use Sensors</button>
          </div>
        </div>

        <div class="row">
          <div>Auto Threshold</div>
          <input id="threshold" type="range" min="10" max="80" value="35" oninput="thresholdVal.textContent=this.value + '%'" />
          <div id="thresholdVal">35%</div>
        </div>
        <div class="btns"><button onclick="applyThreshold()">Apply Threshold</button></div>
      </article>

      <article class="card">
        <div class="head"><h2>Schedule</h2><div class="status">For SCHEDULE mode</div></div>
        <div class="row">
          <div>Interval (min)</div>
          <input id="intervalMin" type="number" min="1" max="240" value="20" />
          <div class="muted">how often</div>
        </div>
        <div class="row">
          <div>Duration (sec)</div>
          <input id="durationSec" type="number" min="2" max="600" value="20" />
          <div class="muted">pump ON time</div>
        </div>
        <div class="btns"><button onclick="applySchedule()">Apply Schedule</button></div>
      </article>

      <article class="card wide">
        <div class="head"><h2>Recommendation</h2><div class="status" id="risk">Risk: --</div></div>
        <div class="rec" id="rec">--</div>
      </article>

      <article class="card">
        <div class="head"><h2>System</h2><div class="status" id="uptime">Uptime: --</div></div>
        <div class="list">
          <div class="list-item"><span class="muted">API Endpoint</span><strong>/api/status</strong></div>
          <div class="list-item"><span class="muted">Input Mode</span><strong id="inputMode">--</strong></div>
          <div class="list-item"><span class="muted">Refresh</span><strong>1 second</strong></div>
        </div>
      </article>

      <article class="card">
        <div class="head"><h2>UAE Sustainability Context</h2><div class="status">Project relevance</div></div>
        <div class="uae">
          This system supports UAE goals by reducing water wastage, improving water-use efficiency,
          enabling smart agriculture, and supporting smart-city landscaping automation.
          It aligns with Water Security Strategy 2036, National Food Security Strategy 2051,
          and Net Zero 2050 direction.
          <br/><br/>
          Recent references:
          <br/>1) <a target="_blank" href="https://pubmed.ncbi.nlm.nih.gov/41545460/">IoT-driven smart irrigation (2026)</a>
          <br/>2) <a target="_blank" href="https://www.sciencedirect.com/science/article/pii/S2772375525003144">Smart agriculture IoT irrigation (2025)</a>
        </div>
      </article>
    </section>
  </main>

  <script>
    const $ = (id)=>document.getElementById(id);
    const clamp=(v,l,h)=>Math.max(l,Math.min(h,v));

    const fmtUptime = (ms) => {
      const s=Math.floor(ms/1000),h=Math.floor(s/3600),m=Math.floor((s%3600)/60),ss=s%60;
      const p=(n)=>String(n).padStart(2,'0');
      return `${h}:${p(m)}:${p(ss)}`;
    };

    const updateRisk = (m) => {
      const r=$('risk');
      if(m<20) r.textContent='Risk: High (very dry)';
      else if(m<35) r.textContent='Risk: Medium (dry)';
      else r.textContent='Risk: Low';
    };

    const api = async (url) => {
      const res = await fetch(url, {method:'POST'});
      if(!res.ok) throw new Error('API failed');
    };

    const setMode = async (mode) => {
      await api(`/api/mode?value=${encodeURIComponent(mode)}`);
      await refresh();
    };

    const setManual = async (v) => {
      await api(`/api/manualPump?value=${v}`);
      await refresh();
    };

    const setSimulated = async (v) => {
      await api(`/api/simulated?value=${v}`);
      await refresh();
    };

    const applyThreshold = async () => {
      const v = $('threshold').value;
      await api(`/api/threshold?value=${v}`);
      await refresh();
    };

    const applySchedule = async () => {
      const i = $('intervalMin').value;
      const d = $('durationSec').value;
      await api(`/api/schedule?intervalMin=${i}&durationSec=${d}`);
      await refresh();
    };

    const refresh = async () => {
      const res = await fetch('/api/status', {cache:'no-store'});
      const d = await res.json();

      $('ssid').textContent = d.ssid;
      $('ip').textContent = d.ip;
      $('modeStatus').textContent = `Mode: ${d.mode}`;
      $('pumpStatus').textContent = `Pump: ${d.pump_on ? 'ON' : 'OFF'}`;
      $('inputMode').textContent = d.simulated ? 'Simulated' : 'Sensor';

      $('moisture').textContent = `${d.soil_moisture_pct.toFixed(1)}%`;
      $('temp').textContent = `${d.temperature_c.toFixed(1)} °C`;
      $('hum').textContent = `${d.humidity_pct.toFixed(1)} %`;
      $('moistFill').style.width = `${clamp(d.soil_moisture_pct,0,100)}%`;

      $('threshold').value = d.threshold_pct.toFixed(0);
      $('thresholdVal').textContent = `${d.threshold_pct.toFixed(0)}%`;
      $('intervalMin').value = d.schedule_interval_min;
      $('durationSec').value = d.schedule_duration_sec;

      $('rec').textContent = d.recommendation;
      $('uptime').textContent = `Uptime: ${fmtUptime(d.uptime_ms)}`;
      updateRisk(d.soil_moisture_pct);
    };

    const uiLoop = async () => {
      try{ await refresh(); }
      catch(_){ $('rec').textContent = 'Dashboard offline: waiting for API response...'; }
      setTimeout(uiLoop, 1000);
    };

    uiLoop();
  </script>
</body>
</html>
)rawliteral";

static void sendJsonStatus() {
  refreshSensorsAndState();

  String json = "{";
  json += "\"ssid\":\"" + String(kApSsid) + "\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"simulated\":" + String(useSimulatedInputs ? "true" : "false") + ",";
  json += "\"uptime_ms\":" + String(millis()) + ",";
  json += "\"mode\":\"" + String(modeToText(controlMode)) + "\",";
  json += "\"pump_on\":" + String(state.pumpOn ? "true" : "false") + ",";
  json += "\"soil_moisture_pct\":" + String(state.soilMoisturePct, 3) + ",";
  json += "\"temperature_c\":" + String(state.temperatureC, 3) + ",";
  json += "\"humidity_pct\":" + String(state.humidityPct, 3) + ",";
  json += "\"threshold_pct\":" + String(moistureThresholdPct, 2) + ",";
  json += "\"schedule_interval_min\":" + String((int)(scheduleIntervalMs / 60000UL)) + ",";
  json += "\"schedule_duration_sec\":" + String((int)(scheduleDurationMs / 1000UL)) + ",";

  String rec = state.recommendation;
  rec.replace("\\", "\\\\");
  rec.replace("\"", "\\\"");
  json += "\"recommendation\":\"" + rec + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

static void handleRoot() {
  server.send_P(200, "text/html", kPageHtml);
}

static void handleApiStatus() {
  sendJsonStatus();
}

static void handleApiMode() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }

  ControlMode parsed;
  if (!textToMode(server.arg("value"), parsed)) {
    server.send(400, "text/plain", "Invalid mode");
    return;
  }

  controlMode = parsed;
  server.send(200, "text/plain", "OK");
}

static void handleApiManualPump() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }

  int v = server.arg("value").toInt();
  manualPumpRequest = (v != 0);
  server.send(200, "text/plain", "OK");
}

static void handleApiSimulated() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }

  int v = server.arg("value").toInt();
  useSimulatedInputs = (v != 0);
  server.send(200, "text/plain", "OK");
}

static void handleApiThreshold() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }

  float v = server.arg("value").toFloat();
  moistureThresholdPct = clampf(v, 10.0f, 80.0f);
  server.send(200, "text/plain", "OK");
}

static void handleApiSchedule() {
  if (!server.hasArg("intervalMin") || !server.hasArg("durationSec")) {
    server.send(400, "text/plain", "Missing schedule params");
    return;
  }

  int intervalMin = server.arg("intervalMin").toInt();
  int durationSec = server.arg("durationSec").toInt();

  intervalMin = max(1, min(240, intervalMin));
  durationSec = max(2, min(600, durationSec));

  scheduleIntervalMs = (unsigned long)intervalMin * 60000UL;
  scheduleDurationMs = (unsigned long)durationSec * 1000UL;

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  delay(300);

  analogReadResolution(12);

  pinMode(PIN_PUMP_RELAY, OUTPUT);
  writePump(false);

  bootMs = millis();

  WiFi.mode(WIFI_AP);
  bool apOk = WiFi.softAP(kApSsid, kApPass);

  if (!apOk) {
    Serial.println("[ERROR] WiFi AP start failed.");
  } else {
    Serial.println("[OK] Smart Irrigation AP started.");
    Serial.println("SSID: " + String(kApSsid));
    Serial.println("IP: " + WiFi.softAPIP().toString());
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/mode", HTTP_POST, handleApiMode);
  server.on("/api/manualPump", HTTP_POST, handleApiManualPump);
  server.on("/api/simulated", HTTP_POST, handleApiSimulated);
  server.on("/api/threshold", HTTP_POST, handleApiThreshold);
  server.on("/api/schedule", HTTP_POST, handleApiSchedule);
  server.onNotFound(handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(10);
}
