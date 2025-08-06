#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"
#include <HTTPClient.h>


WiFiServer server(80);

int ledPin = 5;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 second interval between loop() executions
const int moistureThreshold = 400; //this is currently using the raw analog value range and 400 could be considered to represent approxiamtely 38% moisture level

void sendMockValue() {
  HTTPClient http;
  String url = "http://" + String(ESP8266_IP) + "/send-data";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"value\":42}";
  int code = http.POST(json);
  if (code > 0) Serial.println("POST: " + http.getString());
  else Serial.println("POST failed: " + String(code));
  http.end();
}

int getMoisture() {
  HTTPClient http;
  String url = "http://" + String(ESP8266_IP) + "/soil-moisture";
  http.begin(url);
  int code = http.GET();
  int moisture = -1;

  if (code > 0){
    // Apparently http.getString() drains the response stream. Can only use it once per call. That's why this code block
   
    // String result = http.getString();
    // moisture = result.toInt();

    // returns moisture = 0. Because the second getString() returns an empty string, which when converted to int
    // returns 0.


    String result = http.getString();
    Serial.println("Moisture: " + result);
    moisture = result.toInt();
  }

  else {
    Serial.println("GET failed: " + String(code));
  }
    http.end();
    return moisture;
}

void startPump(int durationSec) {
  HTTPClient http;
  String url = "http://" + String(ESP8266_IP) + "/start-pump";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"duration\":" + String(durationSec) + "}";

  int code = http.POST(json);

  if (code > 0) {
    Serial.println("Pump start response: " + http.getString());
  } else {
    Serial.println("Pump start failed: " + String(code));
  }

  http.end();
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);  // set the LED pin mode

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    // This code is right now running the risk of re-starting the pump before the pump has even finished its first watering round.
    // This could be tidied up for the sake of practice, but ultimately the whole code block is going to run on a once-per-24h cycle
    // so it's not a real production concern at the moment. This puts more pressure on the mathematical equation for time to run pump based on
    // soil humidity benchmark and actual soil humidity, but I was expecting that.
    unsigned long now = millis();
    if (now - lastSendTime >= sendInterval) {
      lastSendTime = now;

      int moisture = getMoisture();

      if (moisture != -1 && moisture < moistureThreshold) {
          Serial.println("MainController: Moisture: " + String(moisture));
          Serial.println("MainController: Moisture Threshold: " + String(moistureThreshold));
          int deficit = moistureThreshold - moisture;
          Serial.println("MainController: Deficit: " + String(deficit));

        int pumpDuration = map(deficit, 0, 400, 1, 10); // 1 second for every 10 units below threshold, max 10 seconds
        Serial.println("Soil dry! Starting pump for " + String(pumpDuration) + " seconds");
        startPump(pumpDuration);
      } else {
        Serial.println("Soil value is fine");
      }
    }
  }
}

