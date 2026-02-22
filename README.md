# HPWS – Home Plant Watering System

This project documents the development of an automated indoor plant watering system.

The goal is twofold:
1. Build a fully functional automated watering system for personal use.
2. Eventually create a beginner-friendly "How-To" electronics guide based on the process (planned, not yet started).

---

## System Architecture

The system follows a **client-server architecture**:

- **Main Control Unit (ESP32)** → Client
- **Mini Controller (ESP8266)** → Server
- Communication via HTTP + JSON

---

## Core Components

### Mini Controller – ESP8266 (Server)

**Responsibilities:**
- Read analog data from capacitive soil humidity sensor
- Convert analog signal to digital value
- Control peristaltic pump via MOSFET
- Respond to HTTP requests with soil humidity values
- Receive pump runtime instructions from main controller

---

### Capacitive Soil Humidity Sensor

**Responsibilities:**
- Send analog signal representing soil humidity level

**Important Discovery:**
- Lower analog value = more moisture
- Higher analog value = dryer soil
- ~0 → Very wet
- ~1037 → Very dry

---

### Peristaltic Pump

**Responsibilities:**
- Pump water from container
- Feed branching hose system
- Distribute water across 50x20cm flower pot

**Correction (6/8/2025):**
Product title listed **12–24V**, but pump label states **5V**.  
Single MT3608 boost converter can power both ESP8266 and pump.

---

### Main Control Unit – ESP32 (Client)

**Responsibilities:**
- Store soil humidity benchmark values
- Request real-time soil data via HTTP
- Compare actual humidity vs benchmark
- Calculate pump runtime dynamically
- Send pump activation instructions
- Designed for scalability (multiple mini controllers)

---

# Development Log

---

## 19/7/2025
### Setup
- Installed CPP, Arduino & PlatformIO
- Set up ESP32 Devboard
- Verified first successful build and upload

---

## 20/7/2025
- Connected ESP32 to WiFi
- Need LED to further test I/O flow

---

## 21/7/2025
- Connected:
  - Breadboard → ESP32
  - MT3608 regulator
  - 3.7V lithium battery
- Created physical “Workflow & Diagnosis” schema in notebook

---

## 22/7/2025
- Tested ESP8266 → MT3608 electrical components

---

## 23/7/2025
- Spliced battery wires → MT3608
- Soldered MT3608 IN/OUT +/- pins
- Soldered potentiometer pins
- Connected MT3608 → ESP32
- Confirmed external battery power works
- Proceeding with ESP8266 WiFi connectivity

---

## 25/7/2025
### HPWS Mini Controller (ESP8266)
- Initialized project
- Confirmed build + setup working
- Designed for:
  - Sensor reading
  - ESP32 communication
  - MOSFET pump control

---

## 26/7/2025
- Connected MOSFET (SIG/VCC/GND) → ESP8266
- Connected MOSFET VIN/GND → MT3608
- Initially thought pump required 12V (later corrected)

### Testing
- Verified MOSFET switching between 12V and 0V
- Created HTTP endpoint on ESP8266
- ESP32 successfully sends mock data and receives response

---

### Later Note (6/8/2025)
Pump is **5V**, not 12V.  
Single MT3608 can power entire mini-controller system.

---

## 27/7/2025

### Electronics
- Connected capacitive soil sensor → ESP8266

### Communication
- Created soil data endpoint
- ESP32 successfully retrieves real soil value
- Example reading: `645 / 1037`

### Later Discovery (6/8/2025)
Sensor values reversed from expectation:
- ~637 in open air
- Adding moisture lowers value
- Therefore:
  - 0 = very wet
  - 1037 = very dry

---

## 28/7/2025
- Refactored request handling
- Switched GET → POST

---

## 29/7/2025
- All HTTP communication switched to JSON
- Added pump-start endpoint

---

## 31/7/2025

### Hardware Progress
- Pumps arrived
- Soldered diode + pump wires
- Connected to MOSFET screw terminals
- Connected pump → MT3608
- Prototype nearing full functionality

---

## To-Do (31/7)

- Test full endpoint integration
- Implement ESP32 deep sleep intervals
- Design pump runtime algorithm
- Analyze battery consumption
- Finalize physical mounting prototype
- Waterproof hose → container connection

---

## 1/8/2025

### Voltage Correction
- Adjusted MT3608 output to 5V
- Confirmed pump operates at 5V

### System Testing
- Replaced mock data with real humidity readings

---

### Major Observations

- Humidity values must be reversed
- Benchmark currently set to 400
- Open air returns ~637

---

### Bug Investigation

**Issue:**
If ESP8266 disconnects unexpectedly (short circuit scenario):
- ESP32 stops sending HTTP requests
- Loop cycle appears interrupted

**However:**
If battery is manually disconnected and reconnected:
- ESP32 continues loop normally

Possible causes:
- Short circuit behavior
- Unexpected blocking in request logic
- Need further investigation

---

### Updated To-Do

- Reverse humidity benchmark values
- Implement deep sleep for ESP32
- Test reconnection behavior
- Test voltage spike when MOSFET turns OFF
- Verify diode effectiveness

---

## 3/8/2025

- Created schematics for Main + Mini controllers
- Prepared PCB for Mini controller
- Realized perfboard prototyping should be done before PCB

---

# Current Status

- Communication working
- Sensor reading working
- Pump control working
- Voltage configuration corrected
- System stability + algorithm refinement pending

---

# Long-Term Goal

- Fully stable automated watering system
- Scalable architecture
- Publish beginner-friendly electronics guide
