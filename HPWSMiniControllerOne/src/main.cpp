#include <Arduino.h>
#include <secrets.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);


unsigned long previousMillisLED = 0;
unsigned long previousMillisMOSFET = 0;
unsigned long previousMillisSoilSensor = 0;

const long intervalLED_ON = 2000;
const long intervalLED_OFF = 5000;
const long intervalMOSFET = 4000;

bool ledState = false;
bool mosfetState = false;
bool soilSensorState = false;

int LedPin = 2; // GPIO pin for the LED
int mosfetPin = D1; // GPIO pin for the MOSFET
const int soilPin = A0; // GPIO pin for the soil moisture sensor
int latestMoisture = 0;

bool pumpActive = false;
unsigned long pumpStartMillis = 0;
unsigned long pumpDurationMillis = 0;


void handleMockData() {
  if (server.hasArg("value")) {
    String val = server.arg("value");
    Serial.println("Received value from ESP32: " + val);
    server.send(200, "text/plain", "Value received: " + val);
  } else {
    server.send(400, "text/plain", "Missing 'value' param");
  }
}

void handleSendData() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (!error) {
      int value = doc["value"];
      Serial.println("Received value from ESP32 (JSON): " + String(value));
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No JSON body\"}");
  }
}


void handleSoilMoisture() {
  latestMoisture = analogRead(soilPin);
  String response = String(latestMoisture);
  Serial.println("Sending moisture value: " + response);
  server.send(200, "text/plain", response);
}

void handleStartPump(){

    if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (!error) {
      int durationSec = doc["duration"];
      Serial.println("Received value from ESP32 (JSON): " + String(durationSec));
      server.send(200, "application/json", "{\"status\":\"ok\"}");

      pumpDurationMillis = durationSec * 1000;
      pumpStartMillis = millis();
      pumpActive = true;

      digitalWrite(mosfetPin, HIGH); // ON
      Serial.println("Pump ON for " + String(durationSec) + "s");


    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No JSON body\"}");
  }



}



void setup() {
  pinMode(LedPin, OUTPUT);
  pinMode(mosfetPin, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println(String("Connecting to WiFi: ") + WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.println("Connecting..");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println('.');
  }

  Serial.print("NodeMCU IP Address: ");
  Serial.print(WiFi.localIP());

  server.on("/soil-moisture", HTTP_GET, handleSoilMoisture);
  server.on("/send-data", HTTP_POST, handleSendData);
  server.on("/start-pump", HTTP_POST, handleStartPump); 

  server.begin();


  digitalWrite(LedPin, HIGH); //LOW actually lets current flow, meaning LED is turned off. LOW = light on, HIGH = light off
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  // --- LED Logic ---
  if (ledState && currentMillis - previousMillisLED >= intervalLED_ON) {
    digitalWrite(LedPin, LOW); // LED ON
    previousMillisLED = currentMillis;
    ledState = false;
    Serial.println("LED ON");
  } else if (!ledState && currentMillis - previousMillisLED >= intervalLED_OFF) {
    digitalWrite(LedPin, HIGH); // LED OFF
    previousMillisLED = currentMillis;
    ledState = true;
    Serial.println("LED OFF");
  }


  if (pumpActive && millis() - pumpStartMillis >= pumpDurationMillis) {
  digitalWrite(mosfetPin, LOW); // Stop pump
  pumpActive = false;
  Serial.println("Pump OFF (duration elapsed)");
  }

}



// This can be used to normalize analog reading values between 0-100%. Use raw values for now, for intial testing.

// int moisture = analogRead(A0);
// int percent = map(moisture, 1023, 300, 0, 100); // adjust 300 based on wet soil
// percent = constrain(percent, 0, 100);
// Serial.print("Soil moisture: ");
// Serial.print(percent);
// Serial.println("%");