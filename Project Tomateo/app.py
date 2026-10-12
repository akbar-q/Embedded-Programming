from flask import Flask, Response, jsonify, render_template, request, stream_with_context

from vision import TomatoVision

app = Flask(__name__)
vision = TomatoVision()


@app.get("/")
def home():
    return render_template("index.html")


@app.get("/video_feed")
def video_feed():
    return Response(
        stream_with_context(vision.mjpeg_stream()),
        mimetype="multipart/x-mixed-replace; boundary=frame",
        headers={"Cache-Control": "no-store, no-cache, must-revalidate", "X-Accel-Buffering": "no"},
    )


@app.get("/api/status")
def status():
    return jsonify(vision.status())


@app.post("/api/source")
def set_source():
    body = request.get_json(silent=True) or {}
    result = vision.set_source(body.get("mode", ""), body.get("url", ""))
    return jsonify(result), (200 if result["ok"] else 400)


@app.post("/api/roi")
def set_roi():
    body = request.get_json(silent=True) or {}
    try:
        result = vision.set_roi(body["rect"])
    except (KeyError, TypeError, ValueError):
        return jsonify({"ok": False, "message": "ROI must contain x, y, width, and height."}), 400
    return jsonify(result), (200 if result["ok"] else 400)


@app.delete("/api/roi")
def clear_roi():
    vision.clear_roi()
    return jsonify({"ok": True})


@app.post("/api/label")
def add_label():
    body = request.get_json(silent=True) or {}
    result = vision.add_label(body.get("label", ""))
    return jsonify(result), (200 if result["ok"] else 400)


@app.post("/api/train")
def train_model():
    result = vision.train_model()
    return jsonify(result), (200 if result["ok"] else 400)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, threaded=True, debug=False, use_reloader=False)
