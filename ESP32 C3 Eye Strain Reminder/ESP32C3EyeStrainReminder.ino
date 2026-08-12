#include <WiFi.h>
#include <WebServer.h>

const int kLedPin = 12;
const char *kApSsid = "ESP32C3-EyeBreak";
const char *kApPassword = "takeabreak";
const int kApChannel = 6;
const bool kApHidden = false;
const int kApMaxConnections = 4;

const unsigned long kBlinkIntervalMs = 350;
const unsigned long kDefaultDurationMs = 10000UL;

WebServer server(80);

unsigned long selectedDurationMs = kDefaultDurationMs;
unsigned long countdownEndMs = 0;
unsigned long lastBlinkToggleMs = 0;
bool alertActive = false;
bool ledState = false;

bool accessPointOpen = false;

const char kPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Eye Break Reminder</title>
  <style>
    :root {
      color-scheme: light;
      --bg-1: #dce8f5;
      --bg-2: #f8fbff;
      --bg-3: #c5d7f2;
      --glass: rgba(255, 255, 255, 0.58);
      --glass-border: rgba(255, 255, 255, 0.72);
      --text: #162033;
      --muted: #5d6b82;
      --accent: #1677ff;
      --accent-strong: #0f5dd1;
      --danger: #ff5b57;
      --shadow: 0 30px 80px rgba(58, 85, 120, 0.18);
      --radius: 28px;
    }

    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      min-height: 100vh;
      font-family: "SF Pro Display", "Segoe UI", sans-serif;
      color: var(--text);
      background:
        radial-gradient(circle at top left, rgba(255, 255, 255, 0.9), transparent 34%),
        radial-gradient(circle at right 15% top 20%, rgba(135, 181, 255, 0.42), transparent 28%),
        linear-gradient(145deg, var(--bg-1), var(--bg-2) 46%, var(--bg-3));
      display: grid;
      place-items: center;
      padding: 24px;
    }

    .shell {
      width: min(100%, 460px);
      position: relative;
    }

    .shell::before,
    .shell::after {
      content: "";
      position: absolute;
      inset: auto;
      border-radius: 999px;
      filter: blur(10px);
      z-index: 0;
    }

    .shell::before {
      width: 150px;
      height: 150px;
      background: rgba(86, 164, 255, 0.22);
      top: -22px;
      right: -12px;
    }

    .shell::after {
      width: 120px;
      height: 120px;
      background: rgba(255, 255, 255, 0.5);
      bottom: -16px;
      left: -10px;
    }

    .card {
      position: relative;
      z-index: 1;
      border-radius: var(--radius);
      padding: 28px;
      background: var(--glass);
      border: 1px solid var(--glass-border);
      backdrop-filter: blur(22px) saturate(160%);
      box-shadow: var(--shadow);
    }

    .eyebrow {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 8px 12px;
      border-radius: 999px;
      background: rgba(255, 255, 255, 0.55);
      color: var(--muted);
      font-size: 13px;
      letter-spacing: 0.02em;
    }

    h1 {
      margin: 18px 0 10px;
      font-size: clamp(30px, 7vw, 40px);
      line-height: 1.02;
      letter-spacing: -0.04em;
    }

    .subtext {
      margin: 0;
      color: var(--muted);
      font-size: 15px;
      line-height: 1.55;
    }

    .timer {
      margin-top: 24px;
      padding: 22px;
      border-radius: 24px;
      background: rgba(255, 255, 255, 0.48);
      border: 1px solid rgba(255, 255, 255, 0.7);
    }

    .timer-label {
      color: var(--muted);
      font-size: 13px;
      text-transform: uppercase;
      letter-spacing: 0.14em;
    }

    .time {
      margin-top: 8px;
      font-size: clamp(52px, 14vw, 72px);
      line-height: 0.95;
      letter-spacing: -0.06em;
      font-variant-numeric: tabular-nums;
    }

    .status {
      margin-top: 12px;
      font-size: 15px;
      color: var(--muted);
      min-height: 23px;
    }

    .status.alert {
      color: var(--danger);
      font-weight: 600;
    }

    .controls {
      margin-top: 24px;
      display: grid;
      gap: 12px;
    }

    .presets {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
    }

    button {
      appearance: none;
      border: 0;
      border-radius: 18px;
      padding: 14px 16px;
      font: inherit;
      cursor: pointer;
      transition: transform 150ms ease, box-shadow 150ms ease, background 150ms ease;
    }

    button:active {
      transform: translateY(1px) scale(0.995);
    }

    .preset {
      background: rgba(255, 255, 255, 0.64);
      color: var(--text);
      box-shadow: inset 0 0 0 1px rgba(255, 255, 255, 0.55);
    }

    .preset.active {
      background: linear-gradient(180deg, rgba(33, 133, 255, 0.98), rgba(16, 93, 209, 0.98));
      color: white;
      box-shadow: 0 12px 24px rgba(22, 119, 255, 0.28);
    }

    .reset {
      background: linear-gradient(180deg, rgba(255, 255, 255, 0.98), rgba(236, 242, 252, 0.95));
      color: var(--text);
      box-shadow: 0 16px 32px rgba(57, 87, 128, 0.14);
      font-weight: 600;
    }

    .footer {
      margin-top: 18px;
      display: flex;
      justify-content: space-between;
      gap: 12px;
      flex-wrap: wrap;
      color: var(--muted);
      font-size: 13px;
    }

    .pill {
      padding: 10px 12px;
      border-radius: 14px;
      background: rgba(255, 255, 255, 0.5);
      border: 1px solid rgba(255, 255, 255, 0.55);
    }
  </style>
</head>
<body>
  <main class="shell">
    <section class="card">
      <div class="eyebrow">ESP32-C3 eye break timer</div>
      <h1>Look away before your eyes complain.</h1>
      <p class="subtext">The LED on GPIO 4 starts blinking when the countdown finishes. Reset it after you take a short screen break.</p>

      <div class="timer">
        <div class="timer-label">Time Left</div>
        <div class="time" id="timeLeft">10.0s</div>
        <div class="status" id="statusText">Reminder armed.</div>
      </div>

      <div class="controls">
        <div class="presets">
          <button class="preset active" data-seconds="10">10s</button>
          <button class="preset" data-seconds="30">30s</button>
          <button class="preset" data-seconds="60">60s</button>
        </div>
        <button class="reset" id="resetButton">Reset Countdown</button>
      </div>

      <div class="footer">
        <div class="pill" id="networkName">AP: loading...</div>
        <div class="pill" id="ipAddress">IP: loading...</div>
      </div>
    </section>
  </main>

  <script>
    const timeLeftEl = document.getElementById('timeLeft');
    const statusTextEl = document.getElementById('statusText');
    const networkNameEl = document.getElementById('networkName');
    const ipAddressEl = document.getElementById('ipAddress');
    const resetButton = document.getElementById('resetButton');
    const presetButtons = Array.from(document.querySelectorAll('.preset'));

    let lastRemainingMs = 10000;
    let lastSyncAt = Date.now();
    let selectedSeconds = 10;
    let isAlertActive = false;

    function formatRemaining(ms) {
      return (Math.max(ms, 0) / 1000).toFixed(1) + 's';
    }

    function updatePresetButtons() {
      presetButtons.forEach((button) => {
        const seconds = Number(button.dataset.seconds);
        button.classList.toggle('active', seconds === selectedSeconds);
      });
    }

    function renderCountdown() {
      const elapsed = Date.now() - lastSyncAt;
      const displayMs = isAlertActive ? 0 : Math.max(0, lastRemainingMs - elapsed);
      timeLeftEl.textContent = formatRemaining(displayMs);
      statusTextEl.textContent = isAlertActive
        ? 'Time is up. Look away from the screen and press reset when you are done.'
        : 'Reminder armed.';
      statusTextEl.classList.toggle('alert', isAlertActive);
    }

    async function refreshState() {
      const response = await fetch('/api/state');
      const state = await response.json();

      lastRemainingMs = state.remainingMs;
      lastSyncAt = Date.now();
      selectedSeconds = Math.round(state.durationMs / 1000);
      isAlertActive = state.alertActive;

      networkNameEl.textContent = 'AP: ' + state.ssid;
      ipAddressEl.textContent = 'IP: ' + state.ip;
      updatePresetButtons();
      renderCountdown();
    }

    async function post(path) {
      await fetch(path, { method: 'POST' });
      await refreshState();
    }

    resetButton.addEventListener('click', () => post('/api/reset'));

    presetButtons.forEach((button) => {
      button.addEventListener('click', () => {
        const seconds = Number(button.dataset.seconds);
        post('/api/set?seconds=' + seconds);
      });
    });

    setInterval(renderCountdown, 100);
    setInterval(() => {
      refreshState().catch(() => {
        statusTextEl.textContent = 'Connection lost. Reconnect to the ESP32-C3 hotspot.';
      });
    }, 1000);

    refreshState().catch(() => {
      statusTextEl.textContent = 'Waiting for the ESP32-C3 web server.';
    });
  </script>
</body>
</html>
)rawliteral";

bool hasExpired(unsigned long nowMs, unsigned long endMs) {
  return static_cast<long>(nowMs - endMs) >= 0;
}

unsigned long remainingMs(unsigned long nowMs, unsigned long endMs) {
  if (hasExpired(nowMs, endMs)) {
    return 0;
  }

  return endMs - nowMs;
}

void startCountdown() {
  countdownEndMs = millis() + selectedDurationMs;
  alertActive = false;
  ledState = false;
  digitalWrite(kLedPin, LOW);
}

void handleRoot() {
  server.send_P(200, "text/html", kPage);
}

void handleApiState() {
  unsigned long nowMs = millis();
  String json = "{";
  json += "\"remainingMs\":" + String(remainingMs(nowMs, countdownEndMs));
  json += ",\"durationMs\":" + String(selectedDurationMs);
  json += ",\"alertActive\":" + String(alertActive ? "true" : "false");
  json += ",\"ssid\":\"" + String(kApSsid) + "\"";
  json += ",\"ip\":\"" + WiFi.softAPIP().toString() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleApiReset() {
  startCountdown();
  server.send(200, "text/plain", "OK");
}

void handleApiSet() {
  if (!server.hasArg("seconds")) {
    server.send(400, "text/plain", "Missing seconds parameter");
    return;
  }

  int seconds = server.arg("seconds").toInt();
  if (seconds != 10 && seconds != 30 && seconds != 60) {
    server.send(400, "text/plain", "Allowed values: 10, 30, 60");
    return;
  }

  selectedDurationMs = static_cast<unsigned long>(seconds) * 1000UL;
  startCountdown();
  server.send(200, "text/plain", "OK");
}

bool startAccessPoint() {
  const IPAddress localIp(192, 168, 4, 1);
  const IPAddress gateway(192, 168, 4, 1);
  const IPAddress subnet(255, 255, 255, 0);

  WiFi.persistent(false);
  WiFi.disconnect(true, true);
  delay(100);
  WiFi.mode(WIFI_MODE_NULL);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  if (!WiFi.softAPConfig(localIp, gateway, subnet)) {
    Serial.println("[WARN] softAPConfig() failed. Continuing with default AP network settings.");
  }

  accessPointOpen = false;
  bool apOk = WiFi.softAP(kApSsid, kApPassword, kApChannel, kApHidden, kApMaxConnections);

  if (!apOk) {
    Serial.println("[WARN] Secured hotspot start failed. Retrying as an open network.");
    apOk = WiFi.softAP(kApSsid, nullptr, kApChannel, kApHidden, kApMaxConnections);
    accessPointOpen = apOk;
  }

  return apOk;
}

void updateReminder() {
  unsigned long nowMs = millis();

  if (!alertActive && hasExpired(nowMs, countdownEndMs)) {
    alertActive = true;
    lastBlinkToggleMs = nowMs;
    ledState = true;
    digitalWrite(kLedPin, HIGH);
  }

  if (alertActive && nowMs - lastBlinkToggleMs >= kBlinkIntervalMs) {
    lastBlinkToggleMs = nowMs;
    ledState = !ledState;
    digitalWrite(kLedPin, ledState ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW);

  bool apOk = startAccessPoint();

  if (!apOk) {
    Serial.println("[ERROR] Failed to start access point.");
    Serial.println("[TIP] Confirm the board is set to an ESP32-C3 target and press the reset button after upload.");
  } else {
    Serial.println("[OK] Access point started.");
    Serial.println("SSID: " + String(kApSsid));
    if (accessPointOpen) {
      Serial.println("Security: OPEN (fallback mode)");
    } else {
      Serial.println("Password: " + String(kApPassword));
    }
    Serial.println("Channel: " + String(kApChannel));
    Serial.println("Open: http://" + WiFi.softAPIP().toString());
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, handleApiState);
  server.on("/api/reset", HTTP_POST, handleApiReset);
  server.on("/api/set", HTTP_POST, handleApiSet);
  server.onNotFound(handleRoot);
  server.begin();

  startCountdown();
}

void loop() {
  server.handleClient();
  updateReminder();
  delay(5);
}