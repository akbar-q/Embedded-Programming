/*
  EVBatteryLitServer.ino
  --------------------------------------------------
  ESP32-C3 self-hosted EV Battery Tracking Server
  - Creates its own Wi-Fi hotspot (Access Point)
  - Serves a modern battery dashboard
  - Estimates SOC, SOH, and RUL from live/derived data
  - Gives charging recommendation text
  - No cloud dependency, no external files

  Board target: ESP32 Dev Module (WROOM-32D)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

static const char* kApSsid = "EV-Battery-Tracker";
static const char* kApPass = "evtrack123";

WebServer server(80);

// ADC pins (ESP32 WROOM-32D safe defaults using ADC1)
// With Wi-Fi enabled on classic ESP32, prefer ADC1 pins (GPIO32-39).
static const int PIN_BATT_VOLTAGE = 34;
static const int PIN_BATT_CURRENT = 35;
static const int PIN_BATT_TEMP = 32;

// Runtime toggle via UI/API (1 = dummy data, 0 = real sensors)
static bool useSimulatedInputs = true;

struct BatteryState {
  float voltageV;
  float currentA;
  float temperatureC;
  float socPct;
  float sohPct;
  float rulCycles;
  float packPowerW;
  bool charging;
  String recommendation;
};

static BatteryState state;
static unsigned long startMs = 0;
static float simulatedSoc = 62.0f;
static float simulatedSoh = 96.0f;

static float clampf(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

static float mapf(float x, float inMin, float inMax, float outMin, float outMax) {
  if (inMax == inMin) return outMin;
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

static float estimateSocFromVoltage(float voltageV) {
  float soc = mapf(voltageV, 3.0f, 4.2f, 0.0f, 100.0f);
  return clampf(soc, 0.0f, 100.0f);
}

static String buildRecommendation(float soc, float tempC, float soh, bool charging) {
  if (tempC >= 48.0f) return "High temperature detected: reduce charge current and cool pack.";
  if (soh <= 70.0f) return "Battery health low: schedule diagnostic and avoid frequent fast charging.";
  if (soc < 20.0f && !charging) return "Low SOC: start charging soon to avoid deep discharge stress.";
  if (soc > 90.0f && charging) return "SOC high: taper or stop charging to reduce long-term degradation.";
  if (tempC > 40.0f) return "Temperature elevated: prefer moderate current charging.";
  return "Battery condition stable: normal charging profile recommended.";
}

static void updateBatteryState() {
  unsigned long now = millis();
  float elapsedS = (now - startMs) / 1000.0f;

  if (useSimulatedInputs) {
    float wave = sinf(elapsedS * 0.04f);
    state.currentA = 3.2f + 2.8f * wave;
    state.charging = state.currentA > 0.15f;

    float socDrift = state.charging ? 0.004f : -0.006f;
    simulatedSoc = clampf(simulatedSoc + socDrift, 8.0f, 98.0f);

    state.voltageV = 3.1f + (simulatedSoc / 100.0f) * 1.05f;
    state.temperatureC = 30.0f + 8.0f * fabsf(wave) + (state.charging ? 2.0f : 0.0f);

    float sohAging = 0.00003f + (state.temperatureC > 42.0f ? 0.00004f : 0.0f);
    simulatedSoh = clampf(simulatedSoh - sohAging, 65.0f, 100.0f);
    state.sohPct = simulatedSoh;
  } else {
    int rawV = analogRead(PIN_BATT_VOLTAGE);
    int rawI = analogRead(PIN_BATT_CURRENT);
    int rawT = analogRead(PIN_BATT_TEMP);

    // Example scaling placeholders (replace with real calibration):
    state.voltageV = mapf((float)rawV, 0.0f, 4095.0f, 3.0f, 4.25f);
    state.currentA = mapf((float)rawI, 0.0f, 4095.0f, -5.0f, 8.0f);
    state.temperatureC = mapf((float)rawT, 0.0f, 4095.0f, 20.0f, 65.0f);

    state.charging = state.currentA > 0.15f;
    state.sohPct = 94.0f - 0.02f * (state.temperatureC - 30.0f);
    state.sohPct = clampf(state.sohPct, 70.0f, 99.0f);
  }

  state.socPct = estimateSocFromVoltage(state.voltageV);
  state.packPowerW = state.voltageV * state.currentA;

  float degradation = (100.0f - state.sohPct);
  state.rulCycles = clampf(2200.0f - degradation * 35.0f, 120.0f, 2200.0f);

  state.recommendation = buildRecommendation(state.socPct, state.temperatureC, state.sohPct, state.charging);
}

static const char kPageHtml[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>EV Battery Tracking Dashboard</title>
  <style>
    :root{--bg:#0b1220;--panel:#111a2e;--panel2:#0f1730;--text:#e8eefc;--muted:#a8b4d6;--line:rgba(232,238,252,.10);--ok:#22c55e;--warn:#f59e0b;--bad:#ef4444}
    *{box-sizing:border-box} html,body{height:100%}
    body{margin:0;font-family:ui-sans-serif,system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;background:radial-gradient(900px 500px at 20% -10%,rgba(96,165,250,.22),transparent 60%),radial-gradient(700px 420px at 85% 0%,rgba(34,197,94,.14),transparent 60%),var(--bg);color:var(--text)}
    .top{position:sticky;top:0;background:rgba(11,18,32,.72);backdrop-filter:blur(10px);border-bottom:1px solid var(--line);z-index:20}
    .top-inner{max-width:1120px;margin:0 auto;padding:14px 18px;display:flex;justify-content:space-between;align-items:center;gap:12px}
    .title{font-size:16px;font-weight:800}.sub{font-size:12px;color:var(--muted);margin-top:2px}
    .pill{font-size:12px;color:var(--muted);border:1px solid var(--line);border-radius:999px;padding:8px 10px}
    .wrap{max-width:1120px;margin:0 auto;padding:18px}
    .grid{display:grid;grid-template-columns:repeat(12,1fr);gap:12px}
    .card{grid-column:span 6;background:linear-gradient(180deg,var(--panel),var(--panel2));border:1px solid var(--line);border-radius:16px;padding:14px;box-shadow:0 14px 48px rgba(0,0,0,.28)}
    .wide{grid-column:span 12}
    .head{display:flex;justify-content:space-between;align-items:center;gap:8px;margin-bottom:10px}
    h2{margin:0;font-size:14px}
    .status{font-size:12px;color:var(--muted)}
    .kpis{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}
    .kpi{border:1px solid var(--line);border-radius:12px;padding:10px;background:rgba(11,18,32,.34)}
    .k-label{font-size:12px;color:var(--muted)} .k-val{margin-top:6px;font-weight:900;font-size:20px}
    .meter{height:9px;border-radius:999px;background:rgba(255,255,255,.08);overflow:hidden;margin-top:8px}
    .fill{height:100%;width:0%;background:linear-gradient(90deg,#22c55e,#60a5fa);transition:width .4s ease}
    .list{display:grid;gap:8px}
    .row{display:flex;justify-content:space-between;gap:10px;padding:9px 10px;border-radius:10px;border:1px solid var(--line);background:rgba(11,18,32,.30);font-size:13px}
    .muted{color:var(--muted)}
    .rec{border:1px solid var(--line);border-radius:12px;padding:12px;background:rgba(255,255,255,.03);line-height:1.5}
    .ok{color:var(--ok)} .warn{color:var(--warn)} .bad{color:var(--bad)}
    @media (max-width:900px){.card{grid-column:span 12}.kpis{grid-template-columns:1fr}}
  </style>
</head>
<body>
  <header class="top">
    <div class="top-inner">
      <div>
        <div class="title">EV Battery Tracking Dashboard</div>
        <div class="sub">Self-hosted ESP32-C3 server • SOC/SOH/RUL live estimation</div>
      </div>
      <div class="pill">AP: <span id="ssid">--</span> • IP: <span id="ip">--</span></div>
    </div>
  </header>

  <main class="wrap">
    <section class="grid">
      <article class="card wide">
        <div class="head"><h2>Battery Health Core Metrics</h2><div class="status" id="mode">Mode: --</div></div>
        <div class="kpis">
          <div class="kpi"><div class="k-label">State of Charge (SOC)</div><div class="k-val" id="soc">--%</div><div class="meter"><div class="fill" id="socFill"></div></div></div>
          <div class="kpi"><div class="k-label">State of Health (SOH)</div><div class="k-val" id="soh">--%</div><div class="meter"><div class="fill" id="sohFill"></div></div></div>
          <div class="kpi"><div class="k-label">Remaining Useful Life (RUL)</div><div class="k-val" id="rul">-- cycles</div><div class="meter"><div class="fill" id="rulFill"></div></div></div>
        </div>
      </article>

      <article class="card">
        <div class="head"><h2>Electrical Metrics</h2><div class="status" id="chargeState">--</div></div>
        <div class="list">
          <div class="row"><span class="muted">Pack Voltage</span><strong id="voltage">-- V</strong></div>
          <div class="row"><span class="muted">Pack Current</span><strong id="current">-- A</strong></div>
          <div class="row"><span class="muted">Pack Power</span><strong id="power">-- W</strong></div>
          <div class="row"><span class="muted">Temperature</span><strong id="temp">-- °C</strong></div>
        </div>
      </article>

      <article class="card">
        <div class="head"><h2>Charging Recommendation</h2><div class="status" id="risk">Risk: --</div></div>
        <div class="rec" id="rec">--</div>
      </article>

      <article class="card wide">
        <div class="head"><h2>System</h2><div class="status" id="uptime">Uptime: --</div></div>
        <div class="list">
          <div class="row"><span class="muted">API Endpoint</span><strong>/api/status</strong></div>
          <div class="row"><span class="muted">Refresh Rate</span><strong>1 second</strong></div>
          <div class="row"><span class="muted">Deployment</span><strong>Self-hosted on ESP32-C3</strong></div>
          <div class="row"><span class="muted">Data Source</span><strong id="simMode">--</strong></div>
          <div class="row"><span class="muted">Dummy Data</span><button id="simBtn" onclick="toggleSimulated()">--</button></div>
        </div>
      </article>
    </section>
  </main>

  <script>
    const $ = (id)=>document.getElementById(id);
    const clamp = (v,l,h)=>Math.max(l,Math.min(h,v));

    const toggleSimulated = async () => {
      const isOn = $('simBtn').dataset.simulated === '1';
      const next = isOn ? 0 : 1;
      await fetch(`/api/simulated?value=${next}`, {method:'POST'});
      await refresh();
    };

    const setRisk = (temp,soh) => {
      const el = $('risk');
      el.classList.remove('ok','warn','bad');
      if(temp >= 48 || soh < 70){ el.textContent='Risk: High'; el.classList.add('bad'); return; }
      if(temp > 40 || soh < 80){ el.textContent='Risk: Medium'; el.classList.add('warn'); return; }
      el.textContent='Risk: Low'; el.classList.add('ok');
    };

    const fmtUptime = (ms) => {
      const s = Math.floor(ms/1000); const h=Math.floor(s/3600), m=Math.floor((s%3600)/60), ss=s%60;
      const p=(n)=>String(n).padStart(2,'0');
      return `${h}:${p(m)}:${p(ss)}`;
    };

    const refresh = async () => {
      const r = await fetch('/api/status',{cache:'no-store'});
      const d = await r.json();

      $('ssid').textContent = d.ssid;
      $('ip').textContent = d.ip;
      $('mode').textContent = `Mode: ${d.simulated ? 'Simulated Inputs' : 'Sensor Inputs'}`;
      $('chargeState').textContent = d.charging ? 'Charging' : 'Discharging/Idle';
      $('simMode').textContent = d.simulated ? 'Dummy Data' : 'Real Sensors';
      $('simBtn').textContent = d.simulated ? 'Switch to Real Sensors' : 'Switch to Dummy Data';
      $('simBtn').dataset.simulated = d.simulated ? '1' : '0';

      $('soc').textContent = `${d.soc_pct.toFixed(1)}%`;
      $('soh').textContent = `${d.soh_pct.toFixed(1)}%`;
      $('rul').textContent = `${Math.round(d.rul_cycles)} cycles`;

      $('socFill').style.width = `${clamp(d.soc_pct,0,100)}%`;
      $('sohFill').style.width = `${clamp(d.soh_pct,0,100)}%`;
      $('rulFill').style.width = `${clamp((d.rul_cycles/2200)*100,0,100)}%`;

      $('voltage').textContent = `${d.voltage_v.toFixed(3)} V`;
      $('current').textContent = `${d.current_a.toFixed(3)} A`;
      $('power').textContent = `${d.power_w.toFixed(2)} W`;
      $('temp').textContent = `${d.temperature_c.toFixed(1)} °C`;

      $('rec').textContent = d.recommendation;
      $('uptime').textContent = `Uptime: ${fmtUptime(d.uptime_ms)}`;

      setRisk(d.temperature_c, d.soh_pct);
    };

    const uiLoop = async () => {
      try{ await refresh(); }catch(_){ $('rec').textContent = 'Dashboard offline: waiting for API response...'; }
      setTimeout(uiLoop, 1000);
    };

    uiLoop();
  </script>
</body>
</html>
)rawliteral";

static void handleRoot() {
  server.send_P(200, "text/html", kPageHtml);
}

static void handleStatus() {
  updateBatteryState();

  String json = "{";
  json += "\"ssid\":\"" + String(kApSsid) + "\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"simulated\":" + String(useSimulatedInputs ? "true" : "false") + ",";
  json += "\"uptime_ms\":" + String(millis()) + ",";
  json += "\"charging\":" + String(state.charging ? "true" : "false") + ",";
  json += "\"voltage_v\":" + String(state.voltageV, 4) + ",";
  json += "\"current_a\":" + String(state.currentA, 4) + ",";
  json += "\"temperature_c\":" + String(state.temperatureC, 3) + ",";
  json += "\"power_w\":" + String(state.packPowerW, 3) + ",";
  json += "\"soc_pct\":" + String(state.socPct, 3) + ",";
  json += "\"soh_pct\":" + String(state.sohPct, 3) + ",";
  json += "\"rul_cycles\":" + String(state.rulCycles, 2) + ",";

  String rec = state.recommendation;
  rec.replace("\\", "\\\\");
  rec.replace("\"", "\\\"");
  json += "\"recommendation\":\"" + rec + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

static void handleApiSimulated() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }

  int value = server.arg("value").toInt();
  useSimulatedInputs = (value != 0);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  delay(300);

  analogReadResolution(12);
  startMs = millis();

  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(kApSsid, kApPass);

  if (!ok) {
    Serial.println("[ERROR] AP start failed.");
  } else {
    Serial.println("[OK] AP started");
    Serial.println("SSID: " + String(kApSsid));
    Serial.println("IP: " + WiFi.softAPIP().toString());
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/simulated", HTTP_POST, handleApiSimulated);
  server.onNotFound(handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(10);
}
