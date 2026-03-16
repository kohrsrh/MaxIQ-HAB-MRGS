# Build Guide

Complete step-by-step assembly guide for the MaxIQ MK10 HAB Flight Station.

---

## Step 1 — Parts List

### Breakout Sensors (for IPB boards)
| Part | SparkFun SKU | Notes |
|------|-------------|-------|
| SparkFun Accelerometer Breakout - KX132-1211 (Qwiic) | SEN-17871 | High-g accel for burst capture |
| SparkFun Linear 3D Hall-Effect Sensor - TMAG5273 (Qwiic) | SEN-22932 | Magnetometer |

### Already Have — Confirmed
| Item | Notes |
|------|-------|
| INA xChip (GPS - Quectel L76) | Snaps onto xBus, no wiring needed |
| ODA xChip (OLED 128x64) | Snaps onto xBus, displays live data |
| SparkFun OpenLog | Solders to IPB-B |
| AS7265x Triad Spectroscopy | Solders to IPB-A |
| DFRobot SEN0463 Geiger Counter | Solders to IPB-A |
| IPB xBus Prototyping Boards (x2) | IPB-A: I2C sensors + Geiger; IPB-B: OpenLog |
| 18650 xChip (PLA) | Powers all electronics |

---

## Step 2 — Arduino IDE Libraries

Go to **Sketch → Include Library → Manage Libraries**. Search and install each:

| # | Library Name | Author | Notes |
|---|-------------|--------|-------|
| 1 | RadioLib | Jan Gromes | |
| 2 | SparkFun SGP30 Arduino Library | SparkFun | |
| 3 | SparkFun LIS2DH12 Arduino Library | SparkFun | |
| 4 | Adafruit SHT31 Library | Adafruit | Click **Install All** |
| 5 | SparkFun KX13X Arduino Library | SparkFun | |
| 6 | SparkFun TMAG5273 Arduino Library | SparkFun | |
| 7 | SparkFun AS7265X Arduino Library | SparkFun | |
| 8 | Adafruit GPS Library | Adafruit | For INA xChip |
| 9 | Freenove WS2812 Lib for ESP32 | Freenove | |
| 10 | SSD1306Ascii | greiman | For ODA OLED display |

Manual ZIP install (**Sketch → Include Library → Add .ZIP Library**):
- **SPL06-007**: https://github.com/rv701/SPL06-007

---

## Step 3 — Uploading Firmware

> ⚠️ **WARNING: Always strip to PPU + CWV only when uploading. Remove all other xChips before upload or it will fail.**

1. Unsnap all xChips except PPU and CWV
2. Remove SD card from OpenLog
3. Open `HAB_FlightStation.ino` (v3.4) in Arduino IDE
4. Tools → Board → esp32 → **ESP32 Dev Module**
5. Tools → PSRAM → **QSPI PSRAM**
6. Tools → Port → **COM3** (or your assigned port)
7. Tools → Upload Speed → **460800**
8. Click **Upload** — wait for `Hash of data verified`
9. Snap all xChips back on

> **NOTE:** If upload fails with `This chip is ESP32, not ESP32-S3`, you have the wrong board selected. Switch to **ESP32 Dev Module**.

---

## Step 4 — IPB Wiring Guide

The two IPB xBus prototyping boards replace the IPP breadboard. They snap into the Domino4 xBus chain using the same 6-pin connectors as all other xChips — no jumper wires needed for power or I2C. Sensor breakout boards connect via short soldered wires to the IPB through-hole pads.

### Understanding the IPB Through-Hole Layout

Each IPB has eight labeled signal rows running vertically down the center of the board:
**VCC, SDA, SCL, VSRC, IO, TXD, RXD, GND**

Each row has **three through-holes** in a horizontal line:
- **Left two holes** — electrically identical, both connect to that signal. Use these for sensor wires. With two holes per row, two sensors can each get their own hole on shared signals (VCC, SDA, SCL, GND).
- **Right single hole** — connects to the right-side xBus edge connector (backplane pass-through). Do not use this hole for sensor wires.

> All wiring instructions below refer to the **left-column holes only**.

### Board Assignment

| Board | Sensors |
|-------|---------|
| **IPB-A** | KX132-1211, TMAG5273, AS7265x, Geiger counter signal |
| **IPB-B** | SparkFun OpenLog |

### I2C Address Reference

| Sensor | Default I2C Address |
|--------|-------------------|
| KX132-1211 | 0x1F |
| TMAG5273 | 0x22 |
| AS7265x Triad | 0x49 (fixed) |

No address conflicts — all three share the bus without any configuration changes.

### IPB-A Wiring — KX132-1211 Accelerometer

| KX132 Pin | IPB-A Hole | Signal |
|-----------|-----------|--------|
| VCC (3.3V) | VCC row — left hole 1 | Power |
| GND | GND row — left hole 1 | Ground |
| SDA | SDA row — left hole 1 | I2C Data |
| SCL | SCL row — left hole 1 | I2C Clock |

### IPB-A Wiring — TMAG5273 Magnetometer

| TMAG5273 Pin | IPB-A Hole | Signal | Notes |
|-------------|-----------|--------|-------|
| VCC (3.3V) | VCC row — left hole 2 | Power | Never use VSRC — unregulated voltage will damage this sensor |
| GND | GND row — left hole 2 | Ground | |
| SDA | SDA row — left hole 2 | I2C Data | |
| SCL | SCL row — left hole 2 | I2C Clock | |

### IPB-A Wiring — AS7265x Triad Spectroscopy

Both left holes in VCC, GND, SDA, and SCL are occupied by the KX132 and TMAG5273. For the AS7265x, solder its wires directly onto the existing solder joints at either left hole in each row.

| AS7265x Pin | IPB-A Connection | Signal |
|-------------|-----------------|--------|
| VCC (3.3V) | Splice onto either left hole in VCC row | Power |
| GND | Splice onto either left hole in GND row | Ground |
| SDA | Splice onto either left hole in SDA row | I2C Data |
| SCL | Splice onto either left hole in SCL row | I2C Clock |

### IPB-A Wiring — Geiger Counter (DFRobot SEN0463)

| Geiger Pin | IPB-A Hole | Signal | Notes |
|-----------|-----------|--------|-------|
| Signal/OUT | IO row — left hole 1 | GPIO 43 interrupt | Falling-edge, firmware counts pulses |
| VCC | VCC row — splice | Power | Confirm module accepts 3.3V; if 5V required use VSRC row instead |
| GND | GND row — splice | Ground | |

> ⚠️ The IO pad carries 3.3V logic. If your Geiger module outputs a 5V signal, add a voltage divider on the signal wire before the IO hole.

### IPB-B Wiring — SparkFun OpenLog

Only one device on IPB-B, so only one hole per row is needed.

| OpenLog Pin | IPB-B Hole | Signal | Notes |
|------------|-----------|--------|-------|
| VCC (3.3V) | VCC row — left hole 1 | Power | |
| GND | GND row — left hole 1 | Ground | |
| RXI | TXD row — left hole 1 | UART TX from CWV | CWV transmits, OpenLog receives and logs |
| TXO | Not connected | — | Firmware does not read from OpenLog |

> **Note on naming:** The TXD label on the IPB refers to the signal direction from the CWV's perspective. The OpenLog's RXI (receive input) correctly connects here — the CWV transmits data, the OpenLog receives and logs it.

---

## Step 5 — Connecting IPB Boards to the xChip Assembly

> ⚠️ **Always power down (remove 18650 battery) before connecting or disconnecting IPB boards.**

1. Remove the 18650 battery from the PLA xChip
2. Disconnect USB if connected
3. Select a position in the xChip chain for IPB-A — near the end of the chain is recommended for cable routing
4. Align the 6-pin connector on IPB-A with the adjacent xChip connector and press firmly until fully seated
5. Connect IPB-B to the chain in the same way, adjacent to IPB-A or at the end
6. Mount the KX132, TMAG5273, AS7265x, and Geiger module near IPB-A and secure with foam adhesive, Velcro, or standoffs
7. Mount the OpenLog near IPB-B
8. Complete all soldered wiring per the tables above
9. Insert a freshly FAT32-formatted microSD card into the OpenLog

---

## Step 6 — xChip Snap-On Assembly

These chips require no wiring — they snap directly onto the CWV xBus connector.

| xChip | Function | Notes |
|-------|----------|-------|
| EL4 | LoRa 433MHz transmitter | Must be snapped on for telemetry |
| IGA | SGP30 VOC/CO2 sensor | Needs ~15 sec warmup after power-on |
| IIA | LIS2DH12 accelerometer | Redundant with KX132 on IPB-A |
| IWA | SHT31 temp/humidity | Most reliable temp sensor in stack |
| IWB | SPL06 barometer | Cross-check altitude with GPS |
| ILB | Light sensor | Ambient light level |
| INA | Quectel L76 GPS | HAB mode auto-enabled at startup |
| ODA | SSD1306 OLED display | Shows live sensor data, cycles every 5 sec |
| PLA | 18650 battery | Powers all electronics |

---

## Step 7 — Testing Before Flight

### Startup Sequence
1. Insert FAT32 microSD into OpenLog
2. Snap all xChips on and connect IPB boards
3. Reinstall 18650 battery
4. Watch OLED startup screen — should show LoRa/GPS/Sensor status
5. Watch Neopixel — should go blue → green (all OK) or yellow (sensor warning)

### OpenLog Verification Test
1. Power on the full stack
2. Wait 2 minutes (at least one 30-second TX cycle)
3. Power off
4. Remove microSD from OpenLog
5. Insert into PC — open LOG file — should contain CSV header row + data rows

### GPS Lock Test
1. Take the payload outside to an open area
2. Power on
3. Wait 2–5 minutes
4. OLED Page 2 should show real lat/lon and "Fix: YES"
5. Neopixel should be green (not purple)
