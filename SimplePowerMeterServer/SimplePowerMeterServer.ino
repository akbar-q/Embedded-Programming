#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials for the Access Point
const char* ssid = "PowerMonitorAP";
const char* password = "12345678";

// Fixed resistance value (in ohms)
const float R = 10.0; // Change this to your actual resistance value

// ADC parameters
const float VREF = 3.3; // ESP32 ADC reference voltage
const int ADC_MAX = 4095; // ESP32 has 12-bit ADC

// Energy calculation variables
float dailyEnergy = 0.0; // in watt-hours
unsigned long lastMeasurementTime = 0;
unsigned long lastDailyReset = millis();

WebServer server(80);

void handleRoot() {
  float voltage = readVoltage();
  float current = voltage / R;
  float power = voltage * current;
  
  // Update energy calculation
  unsigned long currentTime = millis();
  if (lastMeasurementTime > 0) {
    float timeHours = (currentTime - lastMeasurementTime) / 3600000.0; // convert ms to hours
    dailyEnergy += power * timeHours;
  }
  lastMeasurementTime = currentTime;
  
  // Check if a day has passed (24 hours)
  if (currentTime - lastDailyReset >= 86400000) {
    dailyEnergy = 0.0;
    lastDailyReset = currentTime;
  }
  
  // HTML response
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<meta http-equiv=\"refresh\" content=\"1\">"; // Auto-refresh every 1 second
  html += "<style>";
  html += "body {font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; background-color: #f5f5f5;}";
  html += ".header {background-color: #d40000; color: white; padding: 15px; margin-bottom: 20px; border-radius: 5px;}";
  html += ".power-display {font-size: 72px; font-weight: bold; color: #d40000; margin: 20px 0;}";
  html += ".data-container {display: flex; justify-content: space-around; margin: 20px 0;}";
  html += ".data-box {background-color: white; border-radius: 5px; padding: 15px; width: 45%; box-shadow: 0 2px 5px rgba(0,0,0,0.1);}";
  html += ".energy-box {background-color: white; border-radius: 5px; padding: 15px; margin: 20px auto; width: 80%; box-shadow: 0 2px 5px rgba(0,0,0,0.1);}";
  html += "h2 {color: #333;}";
  html += "p.value {font-size: 24px; color: #d40000;}";
  html += "</style></head><body>";
  
  html += "<div class=\"header\">";
  html += "<h1>EEE Manchester 2025</h1>";
  html += "</div>";
  
  html += "<div class=\"power-display\">" + String(power, 2) + " W</div>";
  
  html += "<div class=\"data-container\">";
  html += "<div class=\"data-box\">";
  html += "<h2>Voltage</h2>";
  html += "<p class=\"value\">" + String(voltage, 3) + " V</p>";
  html += "</div>";
  
  html += "<div class=\"data-box\">";
  html += "<h2>Current</h2>";
  html += "<p class=\"value\">" + String(current, 3) + " A</p>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class=\"energy-box\">";
  html += "<h2>Today's Energy Consumption</h2>";
  html += "<p class=\"value\">" + String(dailyEnergy, 2) + " Wh</p>";
  html += "</div>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

float readVoltage() {
  int adcValue = analogRead(34); // Using GPIO34 (ADC1_CH6)
  float voltage = (adcValue * VREF) / ADC_MAX;
  return voltage;
}

void setup() {
  Serial.begin(115200);
  
  // Configure ADC
  analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)
  analogSetAttenuation(ADC_11db); // Set ADC attenuation to 11dB for full 0-3.3V range
  
  // Create Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Start web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  // Check if a day has passed (24 hours)
  if (millis() - lastDailyReset >= 86400000) {
    dailyEnergy = 0.0;
    lastDailyReset = millis();
  }
}