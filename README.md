# 🌱 Smart Irrigation System (Arduino Uno)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino: Uno](https://img.shields.io/badge/Board-Arduino%20Uno-00979D.svg?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Status: Active](https://img.shields.io/badge/Status-Complete%20%26%20Tested-brightgreen.svg)]()

An automated plant watering and irrigation system built with an **Arduino Uno**, a **Flying-Fish (MH-Sensor Series) Soil Moisture Sensor**, and a **Low-Level Trigger 5V Relay Module** driving a water pump from a reservoir chamber.

---

## 📋 Table of Contents
- [Overview](#-overview)
- [Key Features](#-key-features)
- [Hardware Architecture & Components](#-hardware-architecture--components)
- [Circuit & Wiring Diagram](#-circuit--wiring-diagram)
- [Project Structure](#-project-structure)
- [How to Upload & Run](#-how-to-upload--run)
- [Calibration Guide](#-calibration-guide)
- [Troubleshooting & FAQs](#-troubleshooting--faqs)
- [Future Enhancements](#-future-enhancements)
- [License](#-license)

---

## 🌟 Overview

Overwatering and underwatering are two of the biggest causes of plant failure in home gardens and precision agriculture. This project monitors real-time soil moisture levels using an analog probe and automatically triggers a water pump when the soil becomes dry. 

To protect the soil and pump, it uses an **automated soak cycle** (burst spray $\rightarrow$ absorption delay $\rightarrow$ re-check) rather than pumping continuously.

---

## ✨ Key Features

- **Automated Irrigation Control:** Switches on a water pump when soil moisture falls below threshold ($< 30\%$).
- **Anti-Flooding & Soak Protection:** Runs the pump in timed bursts ($3\text{ seconds}$), followed by a soaking wait time ($7\text{ seconds}$) so water penetrates down to root depth before re-evaluating.
- **Low-Level Relay Protection:** Configured specifically for active-low relays to ensure safe boot states without accidental triggering during Arduino resets.
- **Analog Noise Filtering:** Implements rolling multisample averaging on ADC pin `A0` for stable, noise-free readings.
- **Dedicated Calibration Tool:** Includes a companion `calibration.ino` sketch to calibrate the dry (air) and saturated (water) values for any soil type or probe.
- **Serial Telemetry:** Real-time data logging formatted for the Arduino Serial Monitor at 9600 baud.

---

## 🛠️ Hardware Architecture & Components

| Component | Quantity | Purpose |
| :--- | :---: | :--- |
| **Arduino Uno R3** | 1 | Central microcontroller |
| **Flying-Fish Soil Moisture Probe (MH-Sensor Series)** | 1 | Measures electrical conductivity/resistance in soil |
| **5V Relay Module (Low-Level Trigger)** | 1 | Opto-isolated electronic switch for high-current pump |
| **5V Submersible Water Pump / Motor** | 1 | Delivers water from the chamber to the plant |
| **External 5V Battery Pack or Adapter** | 1 | Dedicated power for pump (prevents Arduino reset) |
| **Silicone Tubing / Spray Nozzle** | 1 | Directs water flow |
| **Jumper Wires & USB Cable** | Set | Connections and programming |

---

## 🔌 Circuit & Wiring Diagram

### Pin Connection Summary

| Module Pin | Arduino Uno Pin | Description |
| :--- | :--- | :--- |
| **Soil Sensor VCC** | **5V** | Sensor power |
| **Soil Sensor GND** | **GND** | System ground |
| **Soil Sensor A0** | **A0** | Analog output signal |
| **Relay VCC** | **5V** | Relay coil power |
| **Relay GND** | **GND** | Relay ground |
| **Relay IN** | **Digital Pin 8** | Active-low trigger control pin |
| **Built-in LED** | **Pin 13** | Lights up when pump is actively spraying |

### Power & Switching Circuit (Relay to Pump)

```text
               +--------------------------------------+
               |       External DC Power Supply       |
               |       (+)                      (-)   |
               +--------|------------------------|----+
                        |                        |
                        v                        |
                 [ Relay COM ]                   |
                 [ Relay NO  ] -------> [ Pump (+) ]
                                        [ Pump (-) ] <---+
```

- Connect **Power Supply (+)** to Relay **COM** (Common).
- Connect Relay **NO** (Normally Open) to **Pump (+)**.
- Connect **Pump (-)** directly to **Power Supply (-)**.

> [!IMPORTANT]
> **Power Isolation Best Practice:** DC motors create electromagnetic interference (EMI) and current spikes that cause the Arduino Uno to brown out or freeze. Always power the water pump from an external battery pack or separate 5V supply rather than directly through the Arduino's 5V pin.

---

## 📁 Project Structure

```text
smart-irrigation/
├── soilmoist.ino          # Main production sketch (Moisture sensing + Relay control)
├── README.md              # Complete technical documentation & wiring instructions
├── LICENSE                # Open-source MIT License
├── .gitignore             # Standard Arduino & build output exclusion rules
└── calibration/
    └── calibration.ino    # Calibration utility to determine ADC Air and Water baselines
```

---

## 🚀 How to Upload & Run

### 1. Requirements
- [Arduino IDE](https://www.arduino.cc/en/software) (version 1.8.x or 2.x).
- Standard USB Type-A to B cable.

### 2. Steps to Upload
1. Open Arduino IDE.
2. Open [`soilmoist.ino`](soilmoist.ino) (**File** > **Open...**).
3. Connect your Arduino Uno to your computer.
4. Select board: **Tools** > **Board** > **Arduino AVR Boards** > **Arduino Uno**.
5. Select port: **Tools** > **Port** > Choose your Uno's COM port (e.g. `COM8`).
6. Click **Upload** (`Ctrl + U`).

### 3. Monitoring Live Data
1. Open the Serial Monitor via **Tools** > **Serial Monitor** or `Ctrl + Shift + M`.
2. Ensure baud rate is set to **9600 baud**.
3. Live output sample:
   ```text
   ================================================
      Automated Soil Moisture Irrigation System    
      Sensor: Flying-Fish MH-Sensor Series         
      Relay: Low-Level Trigger                     
   ================================================
   Raw ADC: 932 | Moisture: 12% | Status: DRY -> SPRAYING WATER!
   >>> Pump ON (Spraying for 3 seconds)...
   >>> Pump OFF.
   >>> Waiting 7 seconds for water to soak into soil...
   ------------------------------------------------
   Raw ADC: 520 | Moisture: 71% | Status: OPTIMAL (Moist)
   ```

---

## ⚖️ Calibration Guide

Sensors and soils vary based on salinity, mineral content, and probe geometry.

1. Open and upload [`calibration/calibration.ino`](calibration/calibration.ino).
2. Open Serial Monitor at 9600 baud.
3. **Dry Reading (Air):** Hold the probe dry in the air. Note the ADC reading (e.g., `950`).
4. **Wet Reading (Water):** Dip the probe prongs into a cup of water up to the probe line. Note the reading (e.g., `350`).
5. Open [`soilmoist.ino`](soilmoist.ino) and update lines 50–51:
   ```cpp
   const int AIR_VALUE   = 950;  // Your measured air value
   const int WATER_VALUE = 350;  // Your measured water value
   ```
6. Re-upload `soilmoist.ino`.

---

## 🔧 Troubleshooting & FAQs

### Q: `avrdude: stk500_getsync(): not in sync: resp=0x00`
- **Cause 1:** Selected the wrong COM port in **Tools > Port**. (Ensure you select the Arduino Uno COM port, e.g. `COM8`).
- **Cause 2:** Something is plugged into Arduino **Pin 0 (RX)** or **Pin 1 (TX)**. Ensure these pins are completely disconnected during upload.
- **Cause 3:** Serial Monitor is open in another window, locking the COM port. Close it and retry.

### Q: Relay triggers on boot or stays turned on
- Check if your relay is **Low-Level Trigger** or **High-Level Trigger**. In `soilmoist.ino`, line 42 sets `RELAY_ON = LOW;`. If your relay is high-level triggered, simply swap to `RELAY_ON = HIGH;`.

### Q: Soil moisture probe corrodes quickly
- Resistive probes corrode over time due to electrolysis. To extend probe life, connect the probe's `VCC` to a digital pin on the Arduino and only power it high for a fraction of a second when reading values. Alternatively, upgrade to a **Capacitive Soil Moisture Sensor (v1.2)**.

---

## 🔮 Future Enhancements

- [ ] Add an ambient temperature & humidity sensor (DHT11/DHT22) for VPD (Vapor Pressure Deficit) calculation.
- [ ] Incorporate an I2C 16x2 LCD or 0.96" SSD1306 OLED display for on-site monitoring.
- [ ] Connect a float-switch sensor in the water reservoir to alert when the water chamber is empty.
- [ ] Port to ESP32 / ESP8266 for IoT cloud dashboard integration (Blynk / Home Assistant / MQTT).

---

## 📜 License

This project is licensed under the [MIT License](LICENSE) - feel free to use, modify, and distribute for personal and commercial projects.
