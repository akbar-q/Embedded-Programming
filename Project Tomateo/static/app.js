const feed = document.querySelector("#video-feed");
const canvas = document.querySelector("#roi-canvas");
const context = canvas.getContext("2d");
const toastElement = document.querySelector("#toast");
const probabilityRows = ["green", "turning", "red"];
let roi = null;
let dragStart = null;
let isDragging = false;
let toastTimeout;
const motionHistory = Array.from({ length: 32 }, () => 4);

async function api(path, options = {}) {
  const response = await fetch(path, {
    headers: { "Content-Type": "application/json" },
    ...options,
  });
  const body = await response.json();
  if (!response.ok) throw new Error(body.message || `Request failed (${response.status})`);
  return body;
}

function notify(message) {
  toastElement.textContent = message;
  toastElement.classList.add("visible");
  clearTimeout(toastTimeout);
  toastTimeout = setTimeout(() => toastElement.classList.remove("visible"), 2800);
}

function resizeCanvas() {
  if (!feed.naturalWidth || !feed.naturalHeight) return;
  canvas.width = feed.naturalWidth;
  canvas.height = feed.naturalHeight;
  drawRoi();
}

function drawRoi() {
  context.clearRect(0, 0, canvas.width, canvas.height);
  const rect = isDragging && dragStart ? dragStart.current : roi;
  if (!rect) return;
  const [x, y, width, height] = rect;
  context.fillStyle = "rgba(212, 235, 116, .12)";
  context.fillRect(x, y, width, height);
  context.strokeStyle = "#d4eb74";
  context.lineWidth = Math.max(2, canvas.width / 420);
  context.setLineDash([7, 5]);
  context.strokeRect(x, y, width, height);
  context.setLineDash([]);
}

function pointFromEvent(event) {
  const bounds = canvas.getBoundingClientRect();
  return {
    x: Math.max(0, Math.min(canvas.width, (event.clientX - bounds.left) * canvas.width / bounds.width)),
    y: Math.max(0, Math.min(canvas.height, (event.clientY - bounds.top) * canvas.height / bounds.height)),
  };
}

canvas.addEventListener("pointerdown", (event) => {
  if (!canvas.width) return;
  canvas.setPointerCapture(event.pointerId);
  const point = pointFromEvent(event);
  dragStart = { x: point.x, y: point.y, current: [point.x, point.y, 0, 0] };
  isDragging = true;
});

canvas.addEventListener("pointermove", (event) => {
  if (!isDragging || !dragStart) return;
  const point = pointFromEvent(event);
  const x = Math.min(dragStart.x, point.x);
  const y = Math.min(dragStart.y, point.y);
  dragStart.current = [x, y, Math.abs(point.x - dragStart.x), Math.abs(point.y - dragStart.y)];
  drawRoi();
});

canvas.addEventListener("pointerup", async (event) => {
  if (!isDragging || !dragStart) return;
  const point = pointFromEvent(event);
  const x = Math.round(Math.min(dragStart.x, point.x));
  const y = Math.round(Math.min(dragStart.y, point.y));
  const width = Math.round(Math.abs(point.x - dragStart.x));
  const height = Math.round(Math.abs(point.y - dragStart.y));
  isDragging = false;
  dragStart = null;
  if (width < 12 || height < 12) {
    drawRoi();
    return;
  }
  roi = [x, y, width, height];
  drawRoi();
  try {
    await api("/api/roi", { method: "POST", body: JSON.stringify({ rect: roi }) });
  } catch (error) {
    notify(error.message);
  }
});

async function switchSource(mode) {
  document.querySelector("#test-mode").classList.toggle("active", mode === "test");
  document.querySelector("#camera-mode").classList.toggle("active", mode === "esp32");
  document.querySelector("#camera-connect-row").hidden = mode !== "esp32";
  if (mode === "test") {
    try {
      await api("/api/source", { method: "POST", body: JSON.stringify({ mode }) });
      roi = null;
      drawRoi();
      notify("Synthetic test scene active");
    } catch (error) {
      notify(error.message);
    }
  }
}

document.querySelector("#test-mode").addEventListener("click", () => switchSource("test"));
document.querySelector("#camera-mode").addEventListener("click", () => switchSource("esp32"));
document.querySelector("#connect-camera").addEventListener("click", async () => {
  const url = document.querySelector("#camera-url").value.trim();
  try {
    await api("/api/source", { method: "POST", body: JSON.stringify({ mode: "esp32", url }) });
    roi = null;
    drawRoi();
    notify("Connecting to camera stream");
  } catch (error) {
    notify(error.message);
  }
});

document.querySelector("#clear-roi").addEventListener("click", async () => {
  roi = null;
  drawRoi();
  await api("/api/roi", { method: "DELETE" });
});

document.querySelectorAll("[data-label]").forEach((button) => {
  button.addEventListener("click", async () => {
    try {
      const result = await api("/api/label", {
        method: "POST",
        body: JSON.stringify({ label: button.dataset.label }),
      });
      notify(result.message);
      await refreshStatus();
    } catch (error) {
      notify(error.message);
    }
  });
});

document.querySelector("#train-model").addEventListener("click", async () => {
  try {
    const result = await api("/api/train", { method: "POST" });
    notify(result.message);
    await refreshStatus();
  } catch (error) {
    notify(error.message);
  }
});

function renderMotion(value) {
  motionHistory.push(Math.max(1, Math.min(28, 3 + value * 3.4)));
  motionHistory.shift();
  document.querySelector("#motion-bars").innerHTML = motionHistory
    .map((height) => `<span style="height:${height}px"></span>`)
    .join("");
}

function renderProbabilities(prediction) {
  const root = document.querySelector("#probability-list");
  const probabilities = prediction?.probabilities || {};
  const names = { green: "GREEN", turning: "TURNING", red: "RED" };
  root.innerHTML = probabilityRows.map((label) => {
    const share = Math.round((probabilities[label] || 0) * 100);
    return `<div class="probability-row"><span>${names[label]}</span><span class="probability-track"><i style="width:${share}%"></i></span><span class="probability-value">${share}%</span></div>`;
  }).join("");
}

async function refreshStatus() {
  try {
    const status = await api("/api/status");
    const state = document.querySelector("#connection-state");
    state.classList.toggle("is-offline", !status.connected);
    document.querySelector("#connection-text").textContent = status.connectionMessage;
    document.querySelector("#video-offline").hidden = status.connected || status.mode === "test";
    document.querySelector("#source-metric").textContent = status.mode === "test" ? "TEST SCENE" : "ESP32-CAM";
    document.querySelector("#feed-label").textContent = status.mode === "test" ? "SIMULATION" : "ESP32-CAM";
    document.querySelector("#fps-metric").textContent = status.fps.toFixed(1);
    document.querySelector("#fruit-metric").textContent = status.candidateCount;
    document.querySelector("#motion-metric").textContent = status.motion.toFixed(1);
    document.querySelector("#motion-reading").textContent = `${status.motion.toFixed(2)} PX / FRAME`;
    document.querySelector("#resolution").textContent = `${status.frameSize[0]} × ${status.frameSize[1]}`;
    document.querySelector("#count-green").textContent = status.sampleCounts.green;
    document.querySelector("#count-turning").textContent = status.sampleCounts.turning;
    document.querySelector("#count-red").textContent = status.sampleCounts.red;
    document.querySelector("#sample-hint").textContent = `${Object.values(status.sampleCounts).reduce((sum, count) => sum + count, 0)} saved`;
    document.querySelector("#train-model").disabled = !status.canTrain;

    const fieldModel = status.modelSource === "Field-labeled random forest";
    document.querySelector("#model-chip").textContent = fieldModel ? "FIELD MODEL" : "DEMO MODEL";
    document.querySelector("#model-chip").classList.toggle("field-model", fieldModel);
    document.querySelector("#model-description").textContent = fieldModel
      ? "Random Forest fitted from your labeled tomato regions. Add balanced examples from varied lighting and views."
      : "A seeded Random Forest is active for the test scene. Label real fruit and fit a field model before relying on field predictions.";
    document.querySelector("#training-note").textContent = status.canTrain
      ? "Ready to fit: at least 2 samples in each of 2 classes."
      : "Needs 2 samples in each of at least 2 classes. Samples stay on this laptop.";

    const accelerator = status.accelerator;
    document.querySelector("#accelerator-metric").textContent = accelerator.active ? "DIRECTML ACTIVE" : accelerator.available ? "DIRECTML READY" : "CPU / DML OPTIONAL";
    document.querySelector("#compute-title").textContent = accelerator.active ? "AMD DirectML active" : "AMD DirectML";
    document.querySelector("#compute-detail").textContent = accelerator.message;
    const indicator = document.querySelector("#compute-indicator");
    indicator.classList.toggle("ready", accelerator.available);
    indicator.classList.toggle("active", accelerator.active);

    if (status.prediction) {
      document.querySelector("#prediction-label").textContent = status.prediction.title;
      const confidence = Math.round(status.prediction.confidence * 100);
      document.querySelector("#prediction-confidence").textContent = `${confidence}%`;
      document.querySelector("#confidence-bar").style.width = `${confidence}%`;
    } else {
      document.querySelector("#prediction-label").textContent = "Select a tomato";
      document.querySelector("#prediction-confidence").textContent = "--";
      document.querySelector("#confidence-bar").style.width = "0";
    }
    renderProbabilities(status.prediction);
    renderMotion(status.motion);
  } catch (error) {
    document.querySelector("#connection-text").textContent = "Local vision server unavailable";
    document.querySelector("#connection-state").classList.add("is-offline");
  }
}

feed.addEventListener("load", resizeCanvas);
window.addEventListener("resize", resizeCanvas);
setInterval(refreshStatus, 700);
refreshStatus();
