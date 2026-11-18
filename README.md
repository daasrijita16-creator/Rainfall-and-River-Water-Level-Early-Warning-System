# Rainfall and River Water-Level Early Warning System

An ESP32-based IoT prototype that detects rainfall and measures water level, classifies the situation as **Safe**, **Warning** or **Danger**, and sends live data and alerts to a **Blynk** mobile dashboard.

Built as the final project for **CSE 342 – IoT Based Project Development** at East Delta University .

> **Disclaimer:** This is an indoor academic prototype, not a certified disaster-warning system.

## Why this project
Flash floods are a recurring hazard in Bangladesh's river-bank and hilly areas, and water levels can rise faster than manual monitoring can follow. This project explores a low-cost way to sense rain and water level locally and warn people immediately.

## Features
- Rain / wetness detection with a rain sensor module
- Water-level measurement with an HC-SR04 ultrasonic sensor, converted to a percentage of tank depth
- Three alert states with LED indicators (green / yellow / red) and a buzzer for danger
- Live monitoring in Blynk: level gauge, rain status LED and status label
- Push notification on danger events through Blynk event logging
- Serial Monitor output for debugging and calibration

## Hardware
| Component | Purpose |
|---|---|
| ESP32 DevKit | Main controller and Wi-Fi |
| Rain sensor module | Detects rain / wetness (digital output) |
| HC-SR04 ultrasonic sensor | Measures distance to the water surface |
| 3 LEDs (green, yellow, red) + 220 Ω resistors | Visual status |
| Active buzzer | Audible danger alarm |
| 1 kΩ + 2 kΩ resistors | Voltage divider on the HC-SR04 ECHO line |
| Breadboard, jumper wires, USB cable | Assembly and power |
| Container | Miniature river-and-hill demo model |

## Wiring (as used in the code)
| Device | ESP32 pin |
|---|---|
| HC-SR04 TRIG | GPIO 5 |
| HC-SR04 ECHO (via 1k/2k divider) | GPIO 18 |
| Rain sensor DO | GPIO 19 |
| Green LED (Safe) | GPIO 2 |
| Yellow LED (Warning) | GPIO 4 |
| Red LED (Danger) | GPIO 15 |
| Buzzer | GPIO 21 |

All grounds are common. The HC-SR04 ECHO pin outputs 5 V, so it must pass through the voltage divider before reaching the 3.3 V ESP32 pin.

## How it works
Every second the ESP32:
1. Reads the HC-SR04 distance and converts it to a water level percentage.
2. Reads the rain sensor.
3. Classifies the state:

| State | Condition | Output |
|---|---|---|
| Safe | Otherwise | Green LED |
| Warning | Level ≥ warning threshold **and** rain detected | Yellow LED |
| Danger | Level ≥ danger threshold **and** rain detected | Red LED, buzzer, Blynk alert |

4. Prints the reading to the Serial Monitor and pushes values to Blynk (V0 level %, V1 rain, V2 status).

## Setup
1. Install the Arduino IDE and add ESP32 board support.
2. Install the **Blynk** library from Library Manager.
3. Create a Blynk template with a gauge (V0), LED (V1) and label (V2), plus an event named `danger_alert`.
4. Copy `secrets.h.example` to `secrets.h` and enter your Blynk template ID, auth token and Wi-Fi details.
5. Set `TANK_DEPTH_CM`, `SENSOR_OFFSET_CM` and the thresholds in `flood_warning_system.ino` for your container.
6. Upload to the ESP32 and open the Serial Monitor at 115200 baud.

## Calibration
Watch the Serial Monitor at low, medium and high water levels and adjust `WARN_THRESHOLD` and `DANGER_THRESHOLD` to match your container.

## Limitations
- The rain sensor detects wetness only, not calibrated rainfall intensity.
- The HC-SR04 is not waterproof and suits an indoor model, not a real river.

## Future improvements
Waterproof distance sensor, calibrated rain gauge, GSM/SMS alerts, solar power with battery backup, and multiple sensor nodes.
