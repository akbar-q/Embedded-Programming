/*
ESP32 Occupancy IoT Server (Self-Contained AP)
- PIR-based occupancy detection
- Automatic control for Lights, TV, Fan, Air Con
- Web dashboard is embedded directly in this .ino file
*/

#include <WiFi.h>
#include <WebServer.h>

// -------------------- WiFi Access Point --------------------
// Self-contained server: connect your phone/laptop to this AP, then browse to http://192.168.4.1
static const char* kApSsid = "GM-EEE-Occupancy";
static const char* kApPass = "change-me-123"; // min 8 chars; change before demos

WebServer server(80);

static const char kDashboardHtml[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Greater Manchester EEE • Occupancy IoT</title>
  <style>
    :root{--bg:#0b1220;--panel:#111a2e;--panel2:#0f1730;--text:#e8eefc;--muted:#a8b4d6;--line:rgba(232,238,252,.10);--ok:#22c55e;--warn:#f59e0b;--bad:#ef4444}
    *{box-sizing:border-box}html,body{height:100%}body{margin:0;font-family:ui-sans-serif,system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;background:radial-gradient(900px 500px at 20% -10%,rgba(96,165,250,.22),transparent 60%),radial-gradient(700px 420px at 85% 0%,rgba(34,197,94,.14),transparent 60%),var(--bg);color:var(--text)}
    .mono{font-family:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,"Liberation Mono","Courier New",monospace}
    .topbar{position:sticky;top:0;z-index:10;background:rgba(11,18,32,.70);backdrop-filter:blur(12px);border-bottom:1px solid var(--line)}
    .topbar-inner{max-width:1100px;margin:0 auto;padding:16px 18px;display:flex;align-items:center;justify-content:space-between;gap:14px}
    .brand{display:flex;align-items:center;gap:12px;min-width:260px}.brand-mark{width:42px;height:42px;border-radius:12px;background:linear-gradient(135deg,rgba(96,165,250,1),rgba(34,197,94,1));box-shadow:0 10px 30px rgba(0,0,0,.22)}
    .brand-title{font-size:16px;font-weight:800;letter-spacing:.2px}.brand-subtitle{font-size:12px;color:var(--muted);margin-top:2px}
    .topbar-right{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}.pill{border:1px solid var(--line);background:rgba(17,26,46,.70);padding:8px 10px;border-radius:999px;font-size:12px;color:var(--muted)}
    .wrap{max-width:1100px;margin:0 auto;padding:20px 18px 26px}.grid{display:grid;grid-template-columns:repeat(12,1fr);gap:14px}
    .card{grid-column:span 6;background:linear-gradient(180deg,rgba(17,26,46,.85),rgba(15,23,48,.85));border:1px solid var(--line);border-radius:16px;padding:14px;box-shadow:0 18px 55px rgba(0,0,0,.28)}
    .card-wide{grid-column:span 12}.card-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:12px}.card-title{font-size:14px;font-weight:800;margin:0;letter-spacing:.2px}
    .status{display:flex;align-items:center;gap:8px;color:var(--muted);font-size:12px}.dot{width:10px;height:10px;border-radius:999px;background:var(--warn);box-shadow:0 0 0 4px rgba(245,158,11,.16)}
    .dot-ok{background:var(--ok);box-shadow:0 0 0 4px rgba(34,197,94,.18)}.dot-bad{background:var(--bad);box-shadow:0 0 0 4px rgba(239,68,68,.18)}
    .kpis{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}.kpi{border:1px solid var(--line);border-radius:14px;padding:12px;background:rgba(11,18,32,.35)}.kpi-label{font-size:12px;color:var(--muted)}.kpi-value{font-size:18px;font-weight:900;margin-top:6px;letter-spacing:.3px}
    .hint{margin-top:12px;font-size:12px;color:var(--muted);line-height:1.4}.device{padding:12px;border-radius:14px;border:1px solid var(--line);background:rgba(11,18,32,.35)}.device-state{font-size:16px;font-weight:900;letter-spacing:.4px}
    .system-row{display:flex;justify-content:space-between;gap:12px;padding:10px 0;border-top:1px solid var(--line)}.system-row:first-of-type{border-top:0}.system-key{color:var(--muted);font-size:12px}.system-val{font-size:12px}
    .footer{border-top:1px solid var(--line);color:var(--muted);padding:14px 18px}.footer-inner{max-width:1100px;margin:0 auto;display:flex;justify-content:space-between;align-items:center}.footer-right{opacity:.55;font-weight:800;letter-spacing:.35em}
    @media (max-width:880px){.card{grid-column:span 12}.kpis{grid-template-columns:1fr}.brand{min-width:0}}
  </style>
</head>
<body>
  <header class="topbar">
    <div class="topbar-inner">
      <div class="brand">
        <div class="brand-mark" aria-hidden="true"></div>
        <div>
          <div class="brand-title">Greater Manchester EEE</div>
          <div class="brand-subtitle">Occupancy IoT Server (ESP32)</div>
        </div>
      </div>
      <div class="topbar-right">
        <div class="pill">AP: <span class="mono" id="ap-ssid">—</span></div>
        <div class="pill">IP: <span class="mono" id="ap-ip">—</span></div>
      </div>
    </div>
  </header>

  <main class="wrap">
    <section class="grid">
      <article class="card card-wide">
        <div class="card-head">
          <h2 class="card-title">Occupancy</h2>
          <div class="status"><span class="dot" id="dot-occupancy" aria-hidden="true"></span><span id="txt-occupancy">—</span></div>
        </div>
        <div class="kpis">
          <div class="kpi"><div class="kpi-label">Recent PIR triggers</div><div class="kpi-value" id="kpi-triggers">—</div></div>
          <div class="kpi"><div class="kpi-label">Last trigger</div><div class="kpi-value" id="kpi-last">—</div></div>
          <div class="kpi"><div class="kpi-label">PIR level</div><div class="kpi-value" id="kpi-pir">—</div></div>
        </div>
        <div class="hint">Logic: If occupied, Lights/TV/Fan turn ON. If triggers are frequent, Air Con also turns ON.</div>
      </article>

      <article class="card"><div class="card-head"><h2 class="card-title">Lights</h2><div class="status" id="st-lights"><span class="dot" aria-hidden="true"></span><span class="label">—</span></div></div><div class="device"><div class="device-state" id="dv-lights">—</div></div></article>
      <article class="card"><div class="card-head"><h2 class="card-title">Fan</h2><div class="status" id="st-fan"><span class="dot" aria-hidden="true"></span><span class="label">—</span></div></div><div class="device"><div class="device-state" id="dv-fan">—</div></div></article>
      <article class="card"><div class="card-head"><h2 class="card-title">Air Con</h2><div class="status" id="st-ac"><span class="dot" aria-hidden="true"></span><span class="label">—</span></div></div><div class="device"><div class="device-state" id="dv-ac">—</div></div></article>
      <article class="card"><div class="card-head"><h2 class="card-title">TV</h2><div class="status" id="st-tv"><span class="dot" aria-hidden="true"></span><span class="label">—</span></div></div><div class="device"><div class="device-state" id="dv-tv">—</div></div></article>

      <article class="card card-wide">
        <div class="card-head"><h2 class="card-title">System</h2><div class="status"><span class="dot" aria-hidden="true"></span><span id="sys-uptime">Uptime: —</span></div></div>
        <div class="system-row"><div class="system-key">Endpoint</div><div class="system-val mono">/api/status</div></div>
        <div class="system-row"><div class="system-key">Refresh</div><div class="system-val"><span class="mono">1s</span> polling</div></div>
      </article>
    </section>
  </main>

  <footer class="footer"><div class="footer-inner"><div>Self-contained ESP32 IoT server</div><div class="footer-right">AQ</div></div></footer>

  <script>
    function $(id){return document.getElementById(id)}
    function setDot(el,state){el.classList.remove('dot-ok','dot-bad');if(state==='ok')el.classList.add('dot-ok');if(state==='bad')el.classList.add('dot-bad')}
    function setStatusBlock(rootId,on){const r=$(rootId);if(!r)return;const d=r.querySelector('.dot');const l=r.querySelector('.label');if(on){setDot(d,'ok');l.textContent='ON'}else{setDot(d,'bad');l.textContent='OFF'}}
    function formatMsAgo(ms){if(ms===0)return '—';if(ms<1000)return ms+' ms ago';const s=Math.round(ms/100)/10;if(s<60)return s+' s ago';const m=Math.round((ms/60000)*10)/10;return m+' min ago'}
    function formatUptime(ms){const s=Math.floor(ms/1000),hh=Math.floor(s/3600),mm=Math.floor((s%3600)/60),ss=s%60,p=n=>String(n).padStart(2,'0');return `${hh}:${p(mm)}:${p(ss)}`}
    async function refresh(){const res=await fetch('/api/status',{cache:'no-store'});const data=await res.json();$('ap-ssid').textContent=data.ap_ssid||'—';$('ap-ip').textContent=data.ip||'—';const occ=!!data.occupied;$('txt-occupancy').textContent=occ?'PRESENT':'IDLE';setDot($('dot-occupancy'),occ?'ok':'bad');$('kpi-triggers').textContent=String(data.recent_triggers??'—');$('kpi-last').textContent=formatMsAgo(data.last_trigger_ms_ago??0);$('kpi-pir').textContent=data.pir_level?'HIGH':'LOW';$('sys-uptime').textContent='Uptime: '+formatUptime(data.uptime_ms||0);const d=data.devices||{};setStatusBlock('st-lights',!!d.lights);setStatusBlock('st-fan',!!d.fan);setStatusBlock('st-ac',!!d.ac);setStatusBlock('st-tv',!!d.tv);$('dv-lights').textContent=d.lights?'ON':'OFF';$('dv-fan').textContent=d.fan?'ON':'OFF';$('dv-ac').textContent=d.ac?'ON':'OFF';$('dv-tv').textContent=d.tv?'ON':'OFF'}
    async function loop(){try{await refresh()}catch(_){$('txt-occupancy').textContent='OFFLINE';setDot($('dot-occupancy'),'bad')}setTimeout(loop,1000)}
    loop()
  </script>
</body>
</html>
)rawliteral";

// -------------------- Pins (edit to match your wiring) --------------------
static const int PIR_PIN   = 27; // PIR digital output
static const int LIGHT_PIN = 16;
static const int AC_PIN    = 17;
static const int TV_PIN    = 18;
static const int FAN_PIN   = 19;

// Relay boards are often active-low.
static const bool RELAY_ACTIVE_LOW = true;

// -------------------- Behavior tuning --------------------
// Consider the room occupied if we saw a trigger in the last N ms.
static const unsigned long OCCUPIED_TIMEOUT_MS = 60UL * 1000UL;

// Count triggers within this rolling window.
static const unsigned long TRIGGER_WINDOW_MS = 30UL * 1000UL;

// If triggers in the rolling window >= threshold, use AC. Otherwise just Fan.
static const uint8_t AC_TRIGGER_THRESHOLD = 3;

// Debounce / edge detection (PIR signals can be "high" for a few seconds).
static const unsigned long MIN_TRIGGER_GAP_MS = 800UL;

// -------------------- State --------------------
static bool lastPirLevel = false;
static unsigned long lastTriggerMs = 0;
static unsigned long lastAcceptedTriggerMs = 0;

// Fixed-size timestamp buffer for recent triggers.
static const uint8_t MAX_TRIGGERS = 32;
static unsigned long triggerTimes[MAX_TRIGGERS] = {0};
static uint8_t triggerWriteIndex = 0;

struct DeviceState {
  bool lights = false;
  bool ac = false;
  bool tv = false;
  bool fan = false;
};

static DeviceState devices;

static inline void writeRelay(int pin, bool on) {
  // active-low relay: ON -> LOW
  int level = on ? (RELAY_ACTIVE_LOW ? LOW : HIGH) : (RELAY_ACTIVE_LOW ? HIGH : LOW);
  digitalWrite(pin, level);
}

static uint8_t countTriggersInWindow(unsigned long nowMs) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_TRIGGERS; i++) {
    unsigned long t = triggerTimes[i];
    if (t == 0) continue;
    if (nowMs - t <= TRIGGER_WINDOW_MS) count++;
  }
  return count;
}

static bool isOccupied(unsigned long nowMs) {
  if (lastTriggerMs == 0) return false;
  return (nowMs - lastTriggerMs) <= OCCUPIED_TIMEOUT_MS;
}

static void applyAutomation(unsigned long nowMs) {
  const bool occupied = isOccupied(nowMs);
  const uint8_t recentTriggers = countTriggersInWindow(nowMs);
  const bool useAc = occupied && (recentTriggers >= AC_TRIGGER_THRESHOLD);

  // If occupied: lights, TV, and ventilation always on.
  // AC only on when there are "more" triggers.
  devices.lights = occupied;
  devices.tv = occupied;
  devices.fan = occupied;
  devices.ac = useAc;

  // If using AC, keep fan ON as well (common HVAC setups). If you want mutually exclusive,
  // set devices.fan = occupied && !useAc;

  writeRelay(LIGHT_PIN, devices.lights);
  writeRelay(TV_PIN, devices.tv);
  writeRelay(FAN_PIN, devices.fan);
  writeRelay(AC_PIN, devices.ac);
}

static String jsonEscape(const String& s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"') out += "\\\"";
    else if (c == '\\') out += "\\\\";
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else if (c == '\t') out += "\\t";
    else out += c;
  }
  return out;
}

static void handleApiStatus() {
  const unsigned long nowMs = millis();
  const bool occupied = isOccupied(nowMs);
  const uint8_t recentTriggers = countTriggersInWindow(nowMs);

  String ip = WiFi.softAPIP().toString();

  String json;
  json.reserve(300);
  json += "{";
  json += "\"uptime_ms\":" + String(nowMs) + ",";
  json += "\"ap_ssid\":\"" + jsonEscape(String(kApSsid)) + "\",";
  json += "\"ip\":\"" + jsonEscape(ip) + "\",";
  json += "\"pir_level\":" + String(digitalRead(PIR_PIN) == HIGH ? 1 : 0) + ",";
  json += "\"occupied\":" + String(occupied ? 1 : 0) + ",";
  json += "\"recent_triggers\":" + String(recentTriggers) + ",";
  json += "\"last_trigger_ms_ago\":" + String(lastTriggerMs == 0 ? 0 : (nowMs - lastTriggerMs)) + ",";

  json += "\"devices\":{";
  json += "\"lights\":" + String(devices.lights ? 1 : 0) + ",";
  json += "\"tv\":" + String(devices.tv ? 1 : 0) + ",";
  json += "\"fan\":" + String(devices.fan ? 1 : 0) + ",";
  json += "\"ac\":" + String(devices.ac ? 1 : 0);
  json += "}";

  json += "}";

  server.send(200, "application/json", json);
}

static void setupRoutes() {
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", kDashboardHtml);
  });

  server.on("/api/status", HTTP_GET, handleApiStatus);

  server.onNotFound([]() {
    server.send_P(200, "text/html", kDashboardHtml);
  });
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(AC_PIN, OUTPUT);
  pinMode(TV_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Default all outputs OFF.
  writeRelay(LIGHT_PIN, false);
  writeRelay(AC_PIN, false);
  writeRelay(TV_PIN, false);
  writeRelay(FAN_PIN, false);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(kApSsid, kApPass);
  Serial.println("AP SSID: " + String(kApSsid));
  Serial.println("AP IP: " + WiFi.softAPIP().toString());

  setupRoutes();
  server.begin();
}

void loop() {
  const unsigned long nowMs = millis();

  // PIR rising-edge detect with a minimum gap.
  const bool pirLevel = (digitalRead(PIR_PIN) == HIGH);
  if (pirLevel && !lastPirLevel) {
    if (nowMs - lastAcceptedTriggerMs >= MIN_TRIGGER_GAP_MS) {
      lastAcceptedTriggerMs = nowMs;
      lastTriggerMs = nowMs;
      triggerTimes[triggerWriteIndex] = nowMs;
      triggerWriteIndex = (triggerWriteIndex + 1) % MAX_TRIGGERS;
    }
  }
  lastPirLevel = pirLevel;

  applyAutomation(nowMs);
  server.handleClient();

  // Small delay to reduce busy looping.
  delay(10);
}
