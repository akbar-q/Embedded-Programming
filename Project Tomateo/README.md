# Project Tomateo

Local tomato-vision workbench for an AI-Thinker ESP32-CAM and an AMD Windows laptop. The dashboard runs on the laptop; the camera sends MJPEG frames over the local Wi-Fi network.

## Start the local dashboard

```powershell
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe app.py
```

Open [http://127.0.0.1:5000](http://127.0.0.1:5000). It starts in **Test mode**, which generates a moving greenhouse scene without the ESP32-CAM. Drag a rectangle around a simulated tomato, add labels, and fit a model. The labeled feature data is stored locally in `data/training_samples.json`.

The first model is a seeded scikit-learn Random Forest for exercising the workflow, not a field-ready model. To train it for your crop, collect tight regions from the ESP32-CAM, label Green, Turning, or Red, and use examples from different lighting and viewpoints. At least two labeled examples in each of two classes are required to fit a field model. Predictions are color/ripeness cues only; this prototype does not diagnose disease.

The live pipeline uses OpenCV color segmentation to propose tomato-like regions, simple frame-to-frame association for track IDs, Farneback optical flow for camera-motion estimates, and the Random Forest for ROI classification. The proposal stage is an early prototype and may miss occluded fruit or confuse leaves with fruit.

## Run the vision tests

```powershell
.venv\Scripts\python.exe -m unittest discover -s tests
```

## AMD GPU acceleration

The scikit-learn Random Forest and current OpenCV proposal/flow stages run on the CPU. An optional ONNX image classifier can use AMD DirectML when the runtime and a compatible model are present:

```powershell
.venv\Scripts\python.exe -m pip install -r requirements-amd.txt
```

Place a classifier at `models/tomato_ripeness.onnx` (or set `TOMATEO_ONNX_MODEL`). Its input must be a 4D NCHW or NHWC RGB float tensor scaled to 0..1; its output must contain three scores ordered Green, Turning, Red. The dashboard reports whether DirectML is available and whether that model is actively using it. DirectML package support depends on the installed Python version and AMD driver; the dashboard remains usable on CPU if the optional runtime cannot be installed.

## ESP32-CAM upload

1. Install the Espressif ESP32 Arduino board package in Arduino IDE and select **AI Thinker ESP32-CAM**.
2. Open `TomateoCamera.ino` and replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD`. Use a 2.4 GHz Wi-Fi network that the laptop can also join.
3. Connect the USB-to-serial adapter: adapter 5V to board 5V, GND to GND, adapter TX to U0R, and adapter RX to U0T. Use a stable 5V supply; do not power the camera from a weak 3.3V adapter output.
4. Connect GPIO0 to GND for upload, select the adapter's COM port, then upload. If needed, tap RESET when the IDE starts connecting.
5. Disconnect GPIO0 from GND and reset the board. Open Serial Monitor at 115200 baud and note the printed IP address.

Choose **ESP32-CAM** in the dashboard, enter `http://<device-ip>:81/stream`, and connect. The still image endpoint is `http://<device-ip>/capture`. The standalone OpenCV viewer remains available with `process_feed.py`.

## Network and privacy

The camera stream has no authentication. Keep it on a trusted local network and do not expose its ports to the public internet. The dashboard binds only to `127.0.0.1`; video is processed in memory and is not recorded by default. Labeled feature vectors are saved locally so training can continue after restarting the app.

This prototype streams JPEG video from an AI-Thinker ESP32-CAM to a laptop. The laptop viewer can save frames and apply a rough HSV color cue to a manually selected tomato region. The color cue is an inspection aid, not a trained ripeness or disease classifier.

## ESP32-CAM upload

1. Install the Espressif ESP32 Arduino board package in Arduino IDE and select **AI Thinker ESP32-CAM**.
2. Open `TomateoCamera.ino` and replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD`. Use a 2.4 GHz Wi-Fi network that the laptop can also join.
3. Connect the USB-to-serial adapter: adapter 5V to board 5V, GND to GND, adapter TX to U0R, and adapter RX to U0T. Use a stable 5V supply; do not power the camera from a weak 3.3V adapter output.
4. Connect GPIO0 to GND for upload, select the adapter's COM port, then upload. If needed, tap RESET when the IDE starts connecting.
5. Disconnect GPIO0 from GND and reset the board. Open Serial Monitor at 115200 baud and note the printed IP address.

The still image endpoint is `http://<device-ip>/capture`; the MJPEG feed is `http://<device-ip>:81/stream`.

## Laptop viewer

From this folder, install the Python dependencies and launch the viewer with the IP printed by Serial Monitor:

```powershell
py -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
python process_feed.py --url http://192.168.1.50:81/stream
```

Replace `192.168.1.50` with the ESP32-CAM's address. In the video window, press `r` and draw a tight rectangle around one tomato to see its color cue, `s` to save the current frame under `captures`, `c` to clear the selected region, and `q` to quit. Select the tomato itself rather than surrounding leaves; green background pixels can skew the cue.

## Network and prototype limits

The stream has no authentication. Keep it on a trusted local network and do not expose its ports to the public internet. Test Wi-Fi coverage and image sharpness while the robot is stationary before trying moving captures. The HSV cue is sensitive to lighting, camera white balance, variety, and occlusion; reliable classification will need labeled images and a validated model.
