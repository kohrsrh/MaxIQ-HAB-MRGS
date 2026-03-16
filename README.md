# MaxIQ MK10 HAB Flight Station

A complete high-altitude balloon (HAB) flight computer and sensor payload built on the [Domino4 / MaxIQ](https://maxiq.space) ecosystem using the CWV Extended Core (ESP32-WROOM-32).

## Project Overview

This project turns the MaxIQ MK10 CubeSat Remote Sensing Kit into a fully instrumented HAB payload capable of:

- **Live LoRa telemetry** to a TinyGS ground station at 433 MHz
- **Full data logging** to microSD via SparkFun OpenLog (CSV format)
- **OLED live display** cycling through sensor pages during flight
- **Neopixel status indicator** for pre-flight go/no-go
- **Ionizing radiation measurement** via DFRobot SEN0463 Geiger counter
- **Full-spectrum light analysis** via AS7265x Triad Spectroscopy (18 channels, UV/Vis/NIR)
- **GPS tracking** with HAB mode enabled (80 km altitude limit)
- **Dual accelerometers** for redundancy and high-g burst capture

---

## Hardware

### Core Platform
| Component | Part | Notes |
|-----------|------|-------|
| Flight Computer | Domino4 CWV Extended Core v1-4 | ESP32-WROOM-32 |
| Programmer | Domino4 PPU xChip | USB-A programming interface |
| Battery | Domino4 PLA xChip (18650) | Powers all electronics ~9 hr |

### xChip Sensors (snap-on, no wiring)
| xChip | Sensor | Measures | I2C Address |
|-------|--------|----------|-------------|
| EL4 | SX1278 LoRa | 433 MHz telemetry | SPI |
| IGA | SGP30 | VOC, estimated CO2 | 0x58 |
| IIA | LIS2DH12 | Acceleration (low-g) | 0x19 |
| IWA | SHT31 | Temperature, Humidity | 0x44 |
| IWB | SPL06 | Barometric Pressure | 0x77 |
| ILB | Light sensor | Ambient light (lux) | 0x53 |
| INA | Quectel L76-L-M33 GPS | Position, Altitude, Speed | 0x10 |
| ODA | SSD1306 OLED 128x64 | Live sensor display | 0x3C |

### IPB xBus Prototyping Board Sensors (soldered connections)
| Sensor | Board | Measures | Connection |
|--------|-------|----------|-----------|
| SparkFun KX132-1211 | IPB-A | Acceleration ±16g | I2C 0x1F |
| SparkFun TMAG5273 | IPB-A | Magnetic field strength | I2C 0x22 |
| SparkFun AS7265x Triad | IPB-A | 18-channel UV/Vis/NIR spectrum | I2C 0x49 |
| DFRobot SEN0463 | IPB-A | Ionizing radiation (CPM) | GPIO 43 |
| SparkFun OpenLog | IPB-B | CSV data logger to microSD | Serial2 GPIO 17 |

### Power System
| Circuit | Powers | Battery |
|---------|--------|---------|
| Circuit 1 (18650 xChip) | All electronics above | ~9 hr runtime |
| Circuit 2 (USB power bank 10,000+ mAh) | Insta360 One X2 + 5V heating pad | Flight duration |

---

## Repository Structure

```
HAB_FlightStation/
├── firmware/
│   └── HAB_FlightStation/
│       └── HAB_FlightStation.ino   # Main flight firmware (v3.4)
├── docs/
│   ├── BUILD_GUIDE.md              # Step-by-step assembly and wiring
│   ├── WIRING.md                   # IPB wiring reference tables
│   ├── LIBRARIES.md                # All required libraries with install notes
│   ├── UPLOADING.md                # How to upload firmware to the CWV
│   ├── PREFLIGHT.md                # Launch day checklist
│   └── DATA_ANALYSIS.md            # How to retrieve and analyze flight data
├── hardware/
│   ├── SENSOR_MANIFEST.md          # Complete sensor inventory with I2C addresses
│   └── POWER_BUDGET.md             # Power consumption analysis
├── data_analysis/
│   └── README.md                   # CSV column reference for post-flight analysis
├── ground_station/
│   └── README.md                   # TinyGS ground station setup notes
└── README.md                       # This file
```

---

## Firmware Quick Start

### Required Libraries
Install all via **Sketch → Include Library → Manage Libraries** in Arduino IDE:

| # | Library Name | Author |
|---|-------------|--------|
| 1 | RadioLib | Jan Gromes |
| 2 | SparkFun SGP30 Arduino Library | SparkFun |
| 3 | SparkFun LIS2DH12 Arduino Library | SparkFun |
| 4 | Adafruit SHT31 Library | Adafruit |
| 5 | SparkFun KX13X Arduino Library | SparkFun |
| 6 | SparkFun TMAG5273 Arduino Library | SparkFun |
| 7 | SparkFun AS7265X Arduino Library | SparkFun |
| 8 | Adafruit GPS Library | Adafruit |
| 9 | Freenove WS2812 Lib for ESP32 | Freenove |
| 10 | SSD1306Ascii | greiman |

Manual ZIP install: [SPL06-007](https://github.com/rv701/SPL06-007)

### Arduino IDE Board Settings
```
Board:        esp32 → ESP32 Dev Module
Port:         COM3 (or your assigned port)
Upload Speed: 460800
PSRAM:        QSPI PSRAM
```

> ⚠️ **Always unsnap all xChips except PPU + CWV before uploading.** Remove SD card from OpenLog too.

### Upload Steps
1. Unsnap all xChips — leave only PPU + CWV
2. Open `firmware/HAB_FlightStation/HAB_FlightStation.ino` in Arduino IDE
3. Set board to **ESP32 Dev Module**
4. Select the correct COM port
5. Click Upload — wait for `Hash of data verified`
6. Snap all xChips back on

---

## Neopixel Status Codes

| Color | Meaning | Action |
|-------|---------|--------|
| 🔵 Blue (solid) | Starting up | Wait — normal |
| 🟢 Green (solid) | All systems OK | Ready to fly |
| 🟡 Yellow (solid) | One or more sensors not found | Check wiring |
| 🔴 Red (solid) | LoRa failed | Check EL4 xChip seating |
| 🟣 Purple (solid) | GPS not found | Check INA xChip seating |

---

## OLED Display Pages

The display cycles through 4 pages every 5 seconds:

| Page | Content |
|------|---------|
| 1 — Environment | Temperature, Humidity, Pressure |
| 2 — GPS | Lat/Lon, Altitude, Satellites, Fix status |
| 3 — Air/Radiation | Geiger CPM, TVOC ppb, CO2 ppm |
| 4 — System | Packet count, LoRa status, Uptime |

---

## Data Logging

The OpenLog writes a CSV file to its microSD card. After flight:
1. Power off, remove microSD from OpenLog
2. Insert into PC
3. Open `LOG#####.TXT` in Excel — it will auto-parse as CSV

See [`data_analysis/README.md`](data_analysis/README.md) for the full column reference.

---

## LoRa Telemetry

The firmware transmits an abbreviated packet every 30 seconds:
```
FS,<packet>,<temp>,<humidity>,<pressure>,<kx_x>,<kx_y>,<kx_z>,<tvoc>,<co2>,<mag_strength>,<geiger_cpm>,<lat>,<lon>,<alt>,<fix>
```

Configured for **433.775 MHz** — adjust `LORA_FREQ` in the firmware to match your TinyGS ground station.

---

## Known Issues / Status

| Issue | Status | Notes |
|-------|--------|-------|
| CWV built-in SD slot | Abandoned | Replaced by SparkFun OpenLog on IPB-B |
| Serial monitor during flight | Not used | Use OpenLog CSV for all data recovery |
| GPS HAB mode | Auto-enabled | `$PMTK886,3` sent at every startup |
| LoRa frequency | Pending test | Confirm 433.775 MHz matches TinyGS GS |
| OLED freeze on missing sensors | Fixed in v3.2 | Per-sensor OK flags prevent blocking |

---

## Flight History

| Flight | Date | Duration | Notes |
|--------|------|----------|-------|
| MK10-1 | TBD | — | First flight |

---

## License

This project is open source. Firmware and documentation are provided as-is for educational and amateur HAB use.
