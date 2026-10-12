import json
import math
import os
from pathlib import Path
import threading
import time

import cv2
import numpy as np
import requests
from sklearn.ensemble import RandomForestClassifier

ROOT = Path(__file__).resolve().parent
LABELS = ("green", "turning", "red")
LABEL_TITLES = {"green": "Green", "turning": "Turning", "red": "Red"}
LABEL_COLORS = {
    "green": (118, 184, 75),
    "turning": (64, 174, 240),
    "red": (76, 82, 232),
}
MAX_JPEG_BYTES = 8 * 1024 * 1024


def iter_jpeg_frames(response):
    buffer = bytearray()
    for chunk in response.iter_content(chunk_size=8192):
        if not chunk:
            continue
        buffer.extend(chunk)
        while True:
            start = buffer.find(b"\xff\xd8")
            if start < 0:
                if len(buffer) > 1:
                    del buffer[:-1]
                break
            if start:
                del buffer[:start]
            end = buffer.find(b"\xff\xd9", 2)
            if end < 0:
                if len(buffer) > MAX_JPEG_BYTES:
                    buffer.clear()
                break
            yield bytes(buffer[: end + 2])
            del buffer[: end + 2]


def create_test_frame(seconds):
    width, height = 640, 480
    rows = np.linspace(0, 1, height, dtype=np.float32)[:, None, None]
    top = np.array([150, 179, 143], dtype=np.float32)[None, None, :]
    bottom = np.array([54, 78, 62], dtype=np.float32)[None, None, :]
    frame = np.broadcast_to(top * (1 - rows) + bottom * rows, (height, width, 3)).copy().astype(np.uint8)

    for x in (68, 246, 432, 586):
        cv2.line(frame, (x, 70), (x + 24, 405), (47, 94, 59), 7, cv2.LINE_AA)
        for offset in (-1, 1):
            center = (x + offset * 40, 155 + (x % 5) * 11)
            cv2.ellipse(frame, center, (55, 18), offset * 32, 0, 360, (64, 126, 76), -1, cv2.LINE_AA)
            cv2.ellipse(frame, (center[0] + offset * 10, center[1] + 2), (34, 4), offset * 32, 0, 360, (100, 157, 95), 2, cv2.LINE_AA)
            cv2.ellipse(frame, (x + offset * 56, 355), (48, 17), offset * 25, 0, 360, (59, 117, 73), -1, cv2.LINE_AA)

    fruit = (
        (155, 270, "green", 49),
        (322, 322, "turning", 54),
        (490, 255, "red", 47),
    )
    for index, (x, y, label, radius) in enumerate(fruit):
        sway = int(math.sin(seconds * 1.4 + index) * 4)
        center = (x + sway, y + int(math.sin(seconds + index) * 2))
        base = {"green": (69, 133, 50), "turning": (54, 157, 218), "red": (38, 48, 202)}[label]
        cv2.line(frame, (center[0], center[1] - radius), (center[0] + 2, center[1] - radius - 27), (44, 94, 53), 4, cv2.LINE_AA)
        cv2.ellipse(frame, center, (radius, int(radius * 1.08)), 0, 0, 360, base, -1, cv2.LINE_AA)
        cv2.ellipse(frame, (center[0] - radius // 4, center[1] - radius // 4), (radius // 2, int(radius * 0.65)), 0, 190, 340,
                    tuple(min(255, channel + 24) for channel in base), -1, cv2.LINE_AA)
        cv2.ellipse(frame, (center[0] - radius // 3, center[1] - radius // 3), (radius // 5, radius // 7), -30, 0, 360,
                    (194, 213, 192), -1, cv2.LINE_AA)
        if label == "turning":
            cv2.ellipse(frame, center, (radius - 5, radius - 6), -18, 205, 325, (71, 166, 232), max(7, radius // 3), cv2.LINE_AA)
        cv2.ellipse(frame, center, (radius, int(radius * 1.08)), 0, 0, 360, (45, 73, 48), 1, cv2.LINE_AA)

    shift = int(math.sin(seconds * 0.75) * 3)
    transform = np.float32([[1, 0, shift], [0, 1, 0]])
    return cv2.warpAffine(frame, transform, (width, height), borderMode=cv2.BORDER_REFLECT)


def make_training_crop(label, seed):
    rng = np.random.default_rng(seed)
    image = np.full((96, 96, 3), (46, 72, 55), dtype=np.uint8)
    center = (48 + int(rng.integers(-2, 3)), 49 + int(rng.integers(-2, 3)))
    radius = int(rng.integers(35, 43))
    base = {
        "green": np.array([67, 132, 49]),
        "turning": np.array([53, 151, 218]),
        "red": np.array([38, 48, 201]),
    }[label].astype(np.int16)
    variation = rng.integers(-16, 17, size=3)
    color = tuple(int(value) for value in np.clip(base + variation, 0, 255))
    cv2.ellipse(image, center, (radius, radius + 2), int(rng.integers(-8, 9)), 0, 360, color, -1, cv2.LINE_AA)
    if label == "turning":
        cv2.ellipse(image, center, (radius - 4, radius - 5), int(rng.integers(-35, 35)), 205, 330,
                    (62, 162, 232), -1, cv2.LINE_AA)
    image = cv2.GaussianBlur(image, (3, 3), 0.45)
    noise = rng.normal(0, 4.5, image.shape).astype(np.int16)
    return np.clip(image.astype(np.int16) + noise, 0, 255).astype(np.uint8)


def extract_features(image):
    crop = cv2.resize(image, (96, 96), interpolation=cv2.INTER_AREA)[8:88, 8:88]
    hsv = cv2.cvtColor(crop, cv2.COLOR_BGR2HSV)
    lab = cv2.cvtColor(crop, cv2.COLOR_BGR2LAB)
    hue, saturation, value = cv2.split(hsv)
    valid = saturation >= 45
    masks = (
        ((hue <= 10) | (hue >= 170)) & valid,
        (hue > 10) & (hue <= 24) & valid,
        (hue > 24) & (hue <= 35) & valid,
        (hue > 35) & (hue <= 90) & valid,
    )
    counts = np.array([np.count_nonzero(mask) for mask in masks], dtype=np.float32)
    color_shares = counts / max(float(counts.sum()), 1.0)

    histograms = []
    for channel, bins, upper in ((hue, 12, 180), (saturation, 8, 256), (value, 8, 256)):
        histogram = cv2.calcHist([channel], [0], None, [bins], [0, upper]).flatten()
        histogram /= max(float(histogram.sum()), 1.0)
        histograms.extend(histogram.tolist())

    lab_features = []
    for channel in cv2.split(lab):
        lab_features.extend((float(np.mean(channel)) / 255.0, float(np.std(channel)) / 255.0))

    gray = cv2.cvtColor(crop, cv2.COLOR_BGR2GRAY)
    gradient_x = cv2.Sobel(gray, cv2.CV_32F, 1, 0, ksize=3)
    gradient_y = cv2.Sobel(gray, cv2.CV_32F, 0, 1, ksize=3)
    gradient = cv2.magnitude(gradient_x, gradient_y)
    texture = (float(np.mean(gradient)) / 255.0, float(np.std(gradient)) / 255.0)
    return np.asarray([*color_shares, *histograms, *lab_features, *texture], dtype=np.float32)


def directml_status():
    try:
        import onnxruntime as ort
    except ImportError:
        return {"available": False, "active": False, "provider": "DirectML", "message": "Install the optional AMD runtime"}

    providers = ort.get_available_providers()
    available = "DmlExecutionProvider" in providers
    return {
        "available": available,
        "active": False,
        "provider": "DirectML",
        "message": "Provider ready; add an ONNX ripeness model to use the GPU" if available else "DirectML provider not present",
    }


class TomatoVision:
    def __init__(self):
        self.lock = threading.RLock()
        self.stop_event = threading.Event()
        self.mode = "test"
        self.device_url = ""
        self.connected = True
        self.connection_message = "Synthetic test scene"
        self.raw_frame = None
        self.latest_jpeg = None
        self.width = 640
        self.height = 480
        self.fps = 0.0
        self.motion = 0.0
        self.candidates = []
        self.selected_roi = None
        self.prediction = None
        self.last_error = ""
        self.model_source = "Synthetic training examples"
        self.model = None
        self.dml_session = None
        self.dml_input = None
        self.dml_size = (224, 224)
        self.training_path = ROOT / "data" / "training_samples.json"
        self.user_samples = self._load_user_samples()
        self._previous_gray = None
        self._tracks = {}
        self._next_track_id = 1
        self._frame_index = 0
        self._last_frame_time = time.perf_counter()
        self._accelerator = directml_status()

        self._train_synthetic_model()
        if self._can_train(self.user_samples):
            self.train_model()
        self._load_onnx_model()

        self.thread = threading.Thread(target=self._source_loop, name="tomateo-vision", daemon=True)
        self.thread.start()

    @staticmethod
    def _can_train(samples):
        counts = {}
        for sample in samples:
            counts[sample["label"]] = counts.get(sample["label"], 0) + 1
        return len(counts) >= 2 and all(count >= 2 for count in counts.values())

    def _load_user_samples(self):
        try:
            records = json.loads(self.training_path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            return []
        valid = []
        for record in records:
            if record.get("label") in LABELS and isinstance(record.get("features"), list):
                valid.append(record)
        return valid

    def _fit(self, samples):
        features = np.asarray([sample["features"] for sample in samples], dtype=np.float32)
        labels = np.asarray([sample["label"] for sample in samples])
        model = RandomForestClassifier(n_estimators=96, max_features="sqrt", class_weight="balanced",
                           min_samples_leaf=1, random_state=41, n_jobs=1)
        model.fit(features, labels)
        return model

    def _train_synthetic_model(self):
        records = []
        for label_index, label in enumerate(LABELS):
            for sample_index in range(24):
                patch = make_training_crop(label, label_index * 1000 + sample_index)
                records.append({"label": label, "features": extract_features(patch).tolist()})
        self.model = self._fit(records)

    def train_model(self):
        if not self._can_train(self.user_samples):
            return {"ok": False, "message": "Add at least two labeled samples in each of two classes before fitting."}
        model = self._fit(self.user_samples)
        with self.lock:
            self.model = model
            self.model_source = "Field-labeled random forest"
        return {"ok": True, "message": "Model fitted from your labeled tomato regions."}

    def add_label(self, label):
        if label not in LABELS:
            return {"ok": False, "message": "Unknown ripeness label."}
        with self.lock:
            if self.raw_frame is None or self.selected_roi is None:
                return {"ok": False, "message": "Draw a region around one tomato first."}
            x, y, width, height = self.selected_roi
            crop = self.raw_frame[y:y + height, x:x + width].copy()

        if crop.size == 0 or width < 12 or height < 12:
            return {"ok": False, "message": "The selected region is too small."}

        sample = {"label": label, "features": extract_features(crop).tolist()}
        self.user_samples.append(sample)
        self.training_path.parent.mkdir(parents=True, exist_ok=True)
        self.training_path.write_text(json.dumps(self.user_samples), encoding="utf-8")
        return {"ok": True, "message": f"Saved a {LABEL_TITLES[label].lower()} sample.", "counts": self._sample_counts()}

    def _sample_counts(self):
        counts = {label: 0 for label in LABELS}
        for sample in self.user_samples:
            counts[sample["label"]] += 1
        return counts

    def _load_onnx_model(self):
        model_path = Path(os.environ.get("TOMATEO_ONNX_MODEL", ROOT / "models" / "tomato_ripeness.onnx"))
        if not model_path.exists() or not self._accelerator["available"]:
            return
        try:
            import onnxruntime as ort

            session = ort.InferenceSession(str(model_path), providers=["DmlExecutionProvider", "CPUExecutionProvider"])
            model_input = session.get_inputs()[0]
            shape = model_input.shape
            if len(shape) != 4:
                raise ValueError("Expected a four-dimensional NCHW or NHWC image input")
            if shape[1] == 3:
                size = (shape[3] if isinstance(shape[3], int) else 224, shape[2] if isinstance(shape[2], int) else 224)
            elif shape[3] == 3:
                size = (shape[2] if isinstance(shape[2], int) else 224, shape[1] if isinstance(shape[1], int) else 224)
            else:
                raise ValueError("Expected three image channels")
            self.dml_session = session
            self.dml_input = model_input.name
            self.dml_size = (size[0], size[1])
            self._accelerator.update({"active": "DmlExecutionProvider" in session.get_providers(), "message": "ONNX model running with DirectML"})
        except Exception as error:
            self._accelerator.update({"active": False, "message": f"ONNX model could not load: {error}"})

    def set_source(self, mode, device_url=""):
        if mode not in ("test", "esp32"):
            return {"ok": False, "message": "Source must be test or esp32."}
        if mode == "esp32" and not device_url.startswith(("http://", "https://")):
            return {"ok": False, "message": "Enter the ESP32-CAM stream URL, including http://."}
        with self.lock:
            self.mode = mode
            self.device_url = device_url.strip()
            self.connected = mode == "test"
            self.connection_message = "Synthetic test scene" if mode == "test" else "Connecting to ESP32-CAM"
            self.last_error = ""
            self.selected_roi = None
            self.prediction = None
        return {"ok": True, "message": self.connection_message}

    def set_roi(self, rect):
        with self.lock:
            if self.raw_frame is None:
                return {"ok": False, "message": "Waiting for the first video frame."}
            x, y, width, height = (int(value) for value in rect)
            x0 = max(0, min(x, self.width - 1))
            y0 = max(0, min(y, self.height - 1))
            x1 = max(x0 + 1, min(x + width, self.width))
            y1 = max(y0 + 1, min(y + height, self.height))
            self.selected_roi = (x0, y0, x1 - x0, y1 - y0)
        return {"ok": True}

    def clear_roi(self):
        with self.lock:
            self.selected_roi = None
            self.prediction = None

    def _source_loop(self):
        while not self.stop_event.is_set():
            with self.lock:
                mode, device_url = self.mode, self.device_url

            if mode == "test":
                frame = create_test_frame(time.monotonic())
                with self.lock:
                    self.connected = True
                    self.connection_message = "Synthetic test scene"
                self._process_frame(frame)
                self.stop_event.wait(1 / 22)
                continue

            try:
                with requests.get(device_url, stream=True, timeout=(4, 8)) as response:
                    response.raise_for_status()
                    with self.lock:
                        self.connected = True
                        self.connection_message = "ESP32-CAM connected"
                        self.last_error = ""
                    for jpeg in iter_jpeg_frames(response):
                        if self.stop_event.is_set():
                            return
                        with self.lock:
                            source_changed = self.mode != mode or self.device_url != device_url
                        if source_changed:
                            break
                        frame = cv2.imdecode(np.frombuffer(jpeg, dtype=np.uint8), cv2.IMREAD_COLOR)
                        if frame is not None:
                            self._process_frame(frame)
            except requests.RequestException as error:
                with self.lock:
                    if self.mode == mode and self.device_url == device_url:
                        self.connected = False
                        self.connection_message = "ESP32-CAM offline"
                        self.last_error = str(error)
            self.stop_event.wait(0.8)

    def _find_candidates(self, frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        green = cv2.inRange(hsv, np.array([35, 55, 35]), np.array([90, 255, 255]))
        warm = cv2.inRange(hsv, np.array([0, 65, 45]), np.array([35, 255, 255]))
        red_high = cv2.inRange(hsv, np.array([170, 65, 45]), np.array([180, 255, 255]))
        warm = cv2.bitwise_or(warm, red_high)
        kernel = np.ones((5, 5), dtype=np.uint8)
        frame_area = frame.shape[0] * frame.shape[1]
        candidates = []
        for mask, minimum_circularity in ((warm, 0.46), (green, 0.68)):
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            for contour in contours:
                area = cv2.contourArea(contour)
                if area < max(350, frame_area * 0.0012) or area > frame_area * 0.06:
                    continue
                perimeter = cv2.arcLength(contour, True)
                if perimeter <= 0:
                    continue
                circularity = 4 * math.pi * area / (perimeter * perimeter)
                x, y, width, height = cv2.boundingRect(contour)
                ratio = width / max(height, 1)
                if circularity < minimum_circularity or not 0.58 <= ratio <= 1.55:
                    continue
                padding = max(5, int(max(width, height) * 0.08))
                x0, y0 = max(0, x - padding), max(0, y - padding)
                x1 = min(frame.shape[1], x + width + padding)
                y1 = min(frame.shape[0], y + height + padding)
                candidate = {"box": (x0, y0, x1 - x0, y1 - y0), "center": (x + width // 2, y + height // 2)}
                if not any(math.hypot(candidate["center"][0] - item["center"][0], candidate["center"][1] - item["center"][1]) < 24 for item in candidates):
                    candidates.append(candidate)
        return candidates[:16]

    def _predict(self, crop):
        if self.dml_session is not None:
            resized = cv2.resize(crop, self.dml_size, interpolation=cv2.INTER_AREA)
            image = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB).astype(np.float32) / 255.0
            shape = self.dml_session.get_inputs()[0].shape
            tensor = np.transpose(image, (2, 0, 1))[None, ...] if shape[1] == 3 else image[None, ...]
            output = np.asarray(self.dml_session.run(None, {self.dml_input: tensor})[0]).reshape(-1)
            if output.size != len(LABELS):
                return None
            if np.any(output < 0) or not np.isclose(float(output.sum()), 1.0, atol=0.02):
                output = np.exp(output - np.max(output))
                output /= output.sum()
            probabilities = {label: float(output[index]) for index, label in enumerate(LABELS)}
        else:
            model = self.model
            if model is None:
                return None
            vector = extract_features(crop)[None, :]
            values = model.predict_proba(vector)[0]
            probabilities = {label: 0.0 for label in LABELS}
            for label, value in zip(model.classes_, values):
                probabilities[str(label)] = float(value)

        label = max(probabilities, key=probabilities.get)
        return {"label": label, "title": LABEL_TITLES[label], "confidence": probabilities[label], "probabilities": probabilities}

    def _assign_tracks(self, candidates):
        available = set(self._tracks)
        new_tracks = {}
        for candidate in candidates:
            x, y = candidate["center"]
            nearest = None
            nearest_distance = 68.0
            for track_id in available:
                old_x, old_y = self._tracks[track_id]["center"]
                distance = math.hypot(x - old_x, y - old_y)
                if distance < nearest_distance:
                    nearest, nearest_distance = track_id, distance
            if nearest is None:
                nearest = self._next_track_id
                self._next_track_id += 1
            else:
                available.remove(nearest)
            candidate["track_id"] = nearest
            new_tracks[nearest] = {"center": candidate["center"], "missed": 0}
        for track_id in available:
            track = self._tracks[track_id]
            track["missed"] += 1
            if track["missed"] <= 3:
                new_tracks[track_id] = track
        self._tracks = new_tracks

    def _process_frame(self, frame):
        if frame.shape[1] > 960:
            scale = 960 / frame.shape[1]
            frame = cv2.resize(frame, (960, int(frame.shape[0] * scale)), interpolation=cv2.INTER_AREA)
        height, width = frame.shape[:2]
        candidates = self._find_candidates(frame)
        for candidate in candidates:
            x, y, box_width, box_height = candidate["box"]
            candidate["prediction"] = self._predict(frame[y:y + box_height, x:x + box_width])
        self._assign_tracks(candidates)

        small_gray = cv2.cvtColor(cv2.resize(frame, (160, 120)), cv2.COLOR_BGR2GRAY)
        if self._previous_gray is None:
            motion = 0.0
        else:
            flow = cv2.calcOpticalFlowFarneback(self._previous_gray, small_gray, None, 0.5, 3, 15, 3, 5, 1.2, 0)
            motion = float(np.mean(cv2.magnitude(flow[:, :, 0], flow[:, :, 1])))
        self._previous_gray = small_gray

        with self.lock:
            roi = self.selected_roi
            self.raw_frame = frame.copy()
            self.width, self.height = width, height
        selected_prediction = None
        if roi is not None:
            x, y, box_width, box_height = roi
            selected_prediction = self._predict(frame[y:y + box_height, x:x + box_width])

        display = frame.copy()
        for candidate in candidates:
            x, y, box_width, box_height = candidate["box"]
            prediction = candidate["prediction"]
            label = prediction["title"] if prediction else "Tomato?"
            confidence = f" {prediction['confidence']:.0%}" if prediction else ""
            color = LABEL_COLORS.get(prediction["label"], (80, 204, 201)) if prediction else (80, 204, 201)
            cv2.rectangle(display, (x, y), (x + box_width, y + box_height), color, 2, cv2.LINE_AA)
            cv2.putText(display, f"T{candidate['track_id']}  {label}{confidence}", (x, max(20, y - 8)),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.48, color, 2, cv2.LINE_AA)
        if roi is not None:
            x, y, box_width, box_height = roi
            cv2.rectangle(display, (x, y), (x + box_width, y + box_height), (44, 222, 237), 2, cv2.LINE_AA)

        now = time.perf_counter()
        elapsed = now - self._last_frame_time
        measured_fps = 1.0 / elapsed if elapsed > 0 else 0.0
        self.fps = measured_fps if self.fps == 0 else 0.85 * self.fps + 0.15 * measured_fps
        self._last_frame_time = now
        encoded, buffer = cv2.imencode(".jpg", display, [cv2.IMWRITE_JPEG_QUALITY, 82])
        if not encoded:
            return

        with self.lock:
            self.latest_jpeg = buffer.tobytes()
            self.candidates = candidates
            self.motion = motion
            self.prediction = selected_prediction
            self._frame_index += 1

    def mjpeg_stream(self):
        while not self.stop_event.is_set():
            with self.lock:
                jpeg = self.latest_jpeg
            if jpeg:
                yield b"--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " + str(len(jpeg)).encode() + b"\r\n\r\n" + jpeg + b"\r\n"
            time.sleep(1 / 24)

    def status(self):
        with self.lock:
            candidates = [
                {"id": item["track_id"], "label": item["prediction"]["label"] if item["prediction"] else "unknown",
                 "confidence": item["prediction"]["confidence"] if item["prediction"] else 0.0}
                for item in self.candidates
            ]
            prediction = self.prediction
            mode = self.mode
            return {
                "mode": mode,
                "connected": self.connected,
                "connectionMessage": self.connection_message,
                "lastError": self.last_error,
                "fps": round(self.fps, 1),
                "motion": round(self.motion, 2),
                "candidateCount": len(candidates),
                "candidates": candidates,
                "prediction": prediction,
                "roi": self.selected_roi,
                "modelSource": self.model_source,
                "sampleCounts": self._sample_counts(),
                "canTrain": self._can_train(self.user_samples),
                "accelerator": dict(self._accelerator),
                "frameSize": [self.width, self.height],
            }
