#include <WiFi.h>
#include "esp_camera.h"
#include "esp_http_server.h"

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// AI-Thinker ESP32-CAM pin map.
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

static const char* STREAM_BOUNDARY = "tomateo-frame-boundary";
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=tomateo-frame-boundary";
static const char* STREAM_PART_HEADER = "\r\n--tomateo-frame-boundary\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t controlServer = nullptr;
httpd_handle_t streamServer = nullptr;

static esp_err_t indexHandler(httpd_req_t* request) {
  const char* message =
      "Tomateo ESP32-CAM is running.\n"
      "MJPEG stream: http://<device-ip>:81/stream\n"
      "Still image:  http://<device-ip>/capture\n";
  httpd_resp_set_type(request, "text/plain; charset=utf-8");
  return httpd_resp_send(request, message, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t captureHandler(httpd_req_t* request) {
  camera_fb_t* frame = esp_camera_fb_get();
  if (frame == nullptr) {
    return httpd_resp_send_500(request);
  }

  httpd_resp_set_type(request, "image/jpeg");
  httpd_resp_set_hdr(request, "Access-Control-Allow-Origin", "*");
  esp_err_t result = httpd_resp_send(request, reinterpret_cast<const char*>(frame->buf), frame->len);
  esp_camera_fb_return(frame);
  return result;
}

static esp_err_t streamHandler(httpd_req_t* request) {
  httpd_resp_set_type(request, STREAM_CONTENT_TYPE);
  httpd_resp_set_hdr(request, "Access-Control-Allow-Origin", "*");

  char partHeader[96];
  while (true) {
    camera_fb_t* frame = esp_camera_fb_get();
    if (frame == nullptr) {
      return ESP_FAIL;
    }

    int headerLength = snprintf(partHeader, sizeof(partHeader), STREAM_PART_HEADER, frame->len);
    esp_err_t result = httpd_resp_send_chunk(request, partHeader, headerLength);
    if (result == ESP_OK) {
      result = httpd_resp_send_chunk(request, reinterpret_cast<const char*>(frame->buf), frame->len);
    }
    esp_camera_fb_return(frame);

    if (result != ESP_OK) {
      return result;
    }
  }
}

static bool registerUri(httpd_handle_t server, const char* path, httpd_uri_func_t handler) {
  httpd_uri_t uri = {};
  uri.uri = path;
  uri.method = HTTP_GET;
  uri.handler = handler;
  uri.user_ctx = nullptr;
  return httpd_register_uri_handler(server, &uri) == ESP_OK;
}

static void startControlServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.ctrl_port = 32768;

  if (httpd_start(&controlServer, &config) != ESP_OK) {
    Serial.println("Could not start the camera control server.");
    return;
  }

  registerUri(controlServer, "/", indexHandler);
  registerUri(controlServer, "/capture", captureHandler);
}

static void startStreamServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 81;
  config.ctrl_port = 32769;
  config.max_uri_handlers = 2;

  if (httpd_start(&streamServer, &config) != ESP_OK) {
    Serial.println("Could not start the video stream server.");
    return;
  }

  registerUri(streamServer, "/stream", streamHandler);
}

static bool startCamera() {
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  const bool hasPsram = psramFound();
  config.frame_size = hasPsram ? FRAMESIZE_VGA : FRAMESIZE_QVGA;
  config.jpeg_quality = hasPsram ? 12 : 16;
  config.fb_count = hasPsram ? 2 : 1;

  esp_err_t result = esp_camera_init(&config);
  if (result != ESP_OK) {
    Serial.printf("Camera initialization failed: 0x%x\n", result);
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  if (!startCamera()) {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  const unsigned long connectStartedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - connectStartedAt < 30000) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection timed out. Check the SSID and password, then reset the board.");
    return;
  }

  Serial.print("Camera IP: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Stream:  http://%s:81/stream\n", WiFi.localIP().toString().c_str());
  Serial.printf("Capture: http://%s/capture\n", WiFi.localIP().toString().c_str());

  startControlServer();
  startStreamServer();
}

void loop() {
  delay(1000);
}
