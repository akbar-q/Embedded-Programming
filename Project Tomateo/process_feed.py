import argparse
from datetime import datetime
from pathlib import Path
import time

import cv2
import numpy as np
import requests

WINDOW_NAME = "Tomateo camera feed"
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
            if start > 0:
                del buffer[:start]

            end = buffer.find(b"\xff\xd9", 2)
            if end < 0:
                if len(buffer) > MAX_JPEG_BYTES:
                    buffer.clear()
                break

            yield bytes(buffer[: end + 2])
            del buffer[: end + 2]


def estimate_color_cue(roi):
    hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
    hue, saturation, value = cv2.split(hsv)
    saturated = (saturation >= 55) & (value >= 45)

    red = (((hue <= 10) | (hue >= 170)) & saturated)
    orange = ((hue > 10) & (hue <= 24) & saturated)
    yellow = ((hue > 24) & (hue <= 35) & (saturation >= 45) & (value >= 65))
    green = ((hue > 35) & (hue <= 90) & (saturation >= 45) & (value >= 40))

    counts = {
        "green": int(np.count_nonzero(green)),
        "yellow": int(np.count_nonzero(yellow)),
        "orange": int(np.count_nonzero(orange)),
        "red": int(np.count_nonzero(red)),
    }
    total = sum(counts.values())
    if total < max(30, int(roi.shape[0] * roi.shape[1] * 0.05)):
        return "No strong fruit-color pixels"

    shares = {name: count / total for name, count in counts.items()}
    warm_share = shares["yellow"] + shares["orange"] + shares["red"]

    if shares["green"] >= 0.60 and shares["green"] > warm_share:
        label = "green-dominant"
    elif shares["red"] >= 0.60:
        label = "red-dominant"
    elif warm_share >= 0.35 and shares["green"] < 0.35:
        label = "turning / warm-color"
    else:
        label = "mixed color"

    return (
        f"{label} | G {shares['green']:.0%} Y {shares['yellow']:.0%} "
        f"O {shares['orange']:.0%} R {shares['red']:.0%}"
    )


def main():
    parser = argparse.ArgumentParser(description="View and inspect an ESP32-CAM MJPEG stream.")
    parser.add_argument("--url", required=True, help="Stream URL, for example http://192.168.1.50:81/stream")
    parser.add_argument("--output-dir", type=Path, default=Path("captures"), help="Folder for saved JPEG frames")
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    roi_rect = None
    last_frame = None
    last_frame_time = time.perf_counter()
    fps = 0.0

    print("Connecting to", args.url)
    print("Keys: r select a tomato ROI, s save a frame, c clear ROI, q quit")

    try:
        with requests.get(args.url, stream=True, timeout=(5, 30)) as response:
            response.raise_for_status()
            for jpeg in iter_jpeg_frames(response):
                frame = cv2.imdecode(np.frombuffer(jpeg, dtype=np.uint8), cv2.IMREAD_COLOR)
                if frame is None:
                    continue
                last_frame = frame

                now = time.perf_counter()
                elapsed = now - last_frame_time
                if elapsed > 0:
                    measured_fps = 1.0 / elapsed
                    fps = measured_fps if fps == 0 else 0.9 * fps + 0.1 * measured_fps
                last_frame_time = now

                display = frame.copy()
                cue = "Select a tight tomato ROI with r"
                if roi_rect is not None:
                    x, y, width, height = roi_rect
                    tomato_roi = frame[y : y + height, x : x + width]
                    cue = estimate_color_cue(tomato_roi)
                    cv2.rectangle(display, (x, y), (x + width, y + height), (40, 220, 255), 2)

                cv2.putText(display, f"{fps:.1f} FPS | r ROI | s save | c clear | q quit", (12, 25),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2, cv2.LINE_AA)
                cv2.putText(display, f"Color cue: {cue}", (12, 52),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 2, cv2.LINE_AA)
                cv2.imshow(WINDOW_NAME, display)

                key = cv2.waitKey(1) & 0xFF
                if key == ord("q"):
                    break
                if key == ord("s"):
                    filename = datetime.now().strftime("tomateo_%Y%m%d_%H%M%S_%f.jpg")
                    destination = args.output_dir / filename
                    if cv2.imwrite(str(destination), frame):
                        print("Saved", destination)
                if key == ord("c"):
                    roi_rect = None
                if key == ord("r") and last_frame is not None:
                    selected = cv2.selectROI(WINDOW_NAME, last_frame, showCrosshair=True, fromCenter=False)
                    x, y, width, height = (int(value) for value in selected)
                    roi_rect = (x, y, width, height) if width > 0 and height > 0 else None

    except requests.RequestException as error:
        print(f"Stream connection failed: {error}")
        return 1
    finally:
        cv2.destroyAllWindows()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
