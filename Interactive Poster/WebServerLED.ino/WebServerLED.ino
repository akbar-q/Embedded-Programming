#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

//LATEST VERSION OF CODE WITH LCD ON GPT

const char* ssid = "WiFi-Name"; // Replace with your WiFi network name
const char* password = "WiFi-Password"; // Replace with your WiFi network password

#define WindPot 1
#define PhotoResistor 2
#define PWMOutputPin 5 // Example PWM output pin

WebServer server(80);

#define NEOPIXEL_PIN 12
#define NUM_PIXELS 90
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int pledgeCount = 0;
bool incrementRunning = false; // Flag to control whether increment effects run
unsigned long incrementStartTime;
const unsigned long incrementDuration = 3000; // Duration of the increment effect in milliseconds

// Background effect variables
int backgroundBrightness = 0; // Initial brightness
int backgroundDirection = 1; // Initial direction for brightness change
unsigned long lastBackgroundChangeTime = 0; // Last time the background color changed
const unsigned long backgroundChangeInterval = 10; // Time interval between color changes in milliseconds

void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  page += "<link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\">";
  page += "<style>body { text-align: center; font-family: 'Roboto', sans-serif; background-image: url('https://static.vecteezy.com/system/resources/previews/000/694/632/original/abstract-technology-particle-gray-background-vector.jpg'); background-size: cover; color: #000000; } .banner { background-color: #808080; color: white; padding: 10px 0; margin-bottom: 20px; animation: none; } .bottom-banner { background-color: #808080; color: white; padding: 10px 0; margin-top: 20px; } #pledgeImg { margin-left: 5%; width: 300px; height: 300px; } button { font-size: 36px; padding: 20px 60px; background-color: #40E0D0; color: white; border: none; border-radius: 8px; cursor: pointer; animation: flashButton 1s infinite alternate; } button:hover { background-color: #00CED1; } .pledge-text { font-size: 20px; font-style: italic; } .rounded-square { position: relative; background-color: #40E0D0; color: white; font-size: 64px; padding: 40px; border-radius: 50%; width: 200px; height: 200px; display: inline-flex; align-items: center; justify-content: center; animation: flashCounter 0.5s alternate; } @keyframes flashBanner { from { background-color: #808080; } to { background-color: #00CED1; } } @keyframes flashButton { from { background-color: #C0FFC0; } to { background-color: #40E0D0; } } @keyframes flashCounter { from { background-color: #40E0D0; } to { background-color: #228B22; } }</style>";
  page += "</head><body onclick=\"pledge()\"><div class=\"banner\" id=\"banner\"><h1 id=\"bannerText\">Pledge for Climate Change</h1></div>";
  page += "<img id=\"pledgeImg\" src=\"https://cdn4.iconfinder.com/data/icons/crowdfunding-volume-1/500/Make_a_Pledge-1024.png\" alt=\"Pledge Image\"><br>";
  page += "<button>Press to Pledge Now!</button><br><br>";
  page += "<div class=\"rounded-square\" id=\"pledgeCount\">" + String(pledgeCount) + "</div>";
  page += "<div class=\"bottom-banner\"><p class=\"pledge-text\">Number of Pledges Made on the Poster by</p><p class=\"pledge-text\">Replace this text with your custom text</p></div>";
  page += "<script>function pledge() {";
  page += "var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById('pledgeCount').innerHTML = this.responseText; document.getElementById('banner').style.backgroundColor = '#00CED1'; document.getElementById('bannerText').innerHTML = 'Thanks for pledging!'; setTimeout(function() { document.getElementById('banner').style.backgroundColor = '#808080'; document.getElementById('bannerText').innerHTML = 'Pledge for Climate Change'; }, 2000); } }; xhttp.open(\"GET\", \"/pledge\", true); xhttp.send(); }</script></body></html>";

  server.send(200, "text/html", page);
}

void handlePledge() {
  pledgeCount++;
  server.send(200, "text/plain", String(pledgeCount));
  incrementRunning = true;
  incrementStartTime = millis(); // Start the timer for increment effects
}

void backgroundEffect() {
  if (millis() - lastBackgroundChangeTime >= backgroundChangeInterval) {
    lastBackgroundChangeTime = millis();

    // Update brightness
    backgroundBrightness += backgroundDirection;

    // Change direction when reaching the limits
    if (backgroundBrightness <= 0 || backgroundBrightness >= 255) {
      backgroundDirection *= -1;
    }

    // Set color based on brightness
    for (int i = 0; i < NUM_PIXELS; i++) {
      if (backgroundBrightness < 128) {
        // Blue to Turquoise
        strip.setPixelColor(i, 0, 0, backgroundBrightness * 2);
      } else {
        // Turquoise to Blue
        strip.setPixelColor(i, 0, backgroundBrightness * 2 - 255, 255);
      }
    }

    strip.show();
  }
}

void incrementEffect() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - incrementStartTime;
  int ledIndex;

  if (elapsedTime < incrementDuration / 3) {
    // Chasing up effect
    ledIndex = map(elapsedTime, 0, incrementDuration / 3, 0, NUM_PIXELS);
    for (int i = 0; i < ledIndex; i++) {
      strip.setPixelColor(i, 0, 255, 0); // Set current pixel to green
    }
  } else if (elapsedTime < 2 * incrementDuration / 3) {
    // Chasing down effect
    ledIndex = map(elapsedTime, incrementDuration / 3, 2 * incrementDuration / 3, NUM_PIXELS, 0);
    for (int i = NUM_PIXELS - 1; i >= ledIndex; i--) {
      strip.setPixelColor(i, 0, 255, 0); // Set current pixel to green
    }
  } else {
    // Flashing effect
    if (elapsedTime % 500 < 250) {
      strip.fill(0); // Turn off all pixels
    } else {
      strip.fill(0, 255, 0); // Set all pixels to green
    }
  }

  strip.show();
}

void setup() {
  Serial.begin(115200);

    // Set PWM pin as output
  pinMode(PWMOutputPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Once connected, obtain and print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  strip.begin(); // Initialize NeoPixel strip
  strip.show(); // Initialize all pixels to 'off'

  server.on("/", HTTP_GET, handleRoot);
  server.on("/pledge", HTTP_GET, handlePledge);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (incrementRunning) {
    incrementEffect();
    if (millis() - incrementStartTime >= incrementDuration) {
      incrementRunning = false; // Reset the flag after running the increment effect
    }
  } else {
    backgroundEffect();
  }
  
int windValue = analogRead(WindPot);
  int lightValue = analogRead(PhotoResistor);
  
  // Define upper and lower dead zones (adjust these values as needed)
  int windDeadZoneLower = 100;
  int windDeadZoneUpper = 900;
  int lightDeadZoneLower = 100;
  int lightDeadZoneUpper = 900;
  
  // Apply dead zones
  if (windValue < windDeadZoneLower || windValue > windDeadZoneUpper) {
    windValue = constrain(windValue, windDeadZoneLower, windDeadZoneUpper);
  } else {
    windValue = map(windValue, windDeadZoneLower, windDeadZoneUpper, 0, 1023);
  }
  
  if (lightValue < lightDeadZoneLower || lightValue > lightDeadZoneUpper) {
    lightValue = constrain(lightValue, lightDeadZoneLower, lightDeadZoneUpper);
  } else {
    lightValue = map(lightValue, lightDeadZoneLower, lightDeadZoneUpper, 0, 1023);
  }
  
  // Calculate duty cycle
  float dutyCycle = (windValue / 1023.0) * (lightValue / 1023.0) * 100.0;
  
  // Map duty cycle to PWM range (0-255)
  int pwmValue = map(dutyCycle, 0, 100, 0, 255);
  
  // Output PWM signal
  analogWrite(PWMOutputPin, pwmValue);
  
  // Print for debugging
  Serial.print("Wind Potentiometer: ");
  Serial.print(windValue);
  Serial.print(", Photoresistor: ");
  Serial.print(lightValue);
  Serial.print(", Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.print("%, PWM Value: ");
  Serial.println(pwmValue);
  


}

