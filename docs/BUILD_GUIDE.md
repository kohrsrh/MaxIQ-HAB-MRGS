# Build Guide

Complete step-by-step assembly guide for the MaxIQ MK10 HAB Flight Station.

---

## Step 1 — Parts to Order

### Must Order
| Part | SparkFun SKU | Notes |
|------|-------------|-------|
| SparkFun Accelerometer Breakout - KX132-1211 (Qwiic) | SEN-17871 | High-g accel for burst capture |
| SparkFun Triple Axis Magnetometer - MMC5983MA (Qwiic) | SEN-19895 | Magnetic field strength |

### Already Have — Confirmed
| Item | Status | Notes |
|------|--------|-------|
| INA xChip (GPS - Quectel L76) | Confirmed in collection | Snaps onto xBus, no wiring needed |
| ODA xChip (OLED 128x64) | Confirmed working | Snaps onto xBus, displays live data |
| SparkFun OpenLog | On hand | Wires to IPP via jumper wires |
| AS7265x Triad Spectroscopy | On hand | Wires to IPP via jumper wires |
| DFRobot SEN0463 Geiger Counter | On hand | Wires to IPP via jumper wires |
| IPP Expansion Board | On hand | Main breadboard for all wired components |
| 18650 xChip (PLA) | On hand | Powers all electronics |

---

## Step 2 — Arduino IDE Libraries

Go to **Sketch → Include Library → Manage Libraries**. Search and install each:

| # | Library Name | Author | Status |
|---|-------------|--------|--------|
| 1 | RadioLib | Jan Gromes | Install |
| 2 | SparkFun SGP30 Arduino Library | SparkFun | Install |
| 3 | SparkFun LIS2DH12 Arduino Library | SparkFun | Install |
| 4 | Adafruit SHT31 Library | Adafruit | Install — click **Install All** |
| 5 | SparkFun KX13X Arduino Library | SparkFun | Install |
| 6 | SparkFun MMC5983MA Arduino Library | SparkFun | Install |
| 7 | SparkFun AS7265X Arduino Library | SparkFun | Install |
| 8 | Adafruit GPS Library | Adafruit | Install — for INA xChip |
| 9 | Freenove WS2812 Lib for ESP32 | Freenove | Install |
| 10 | SSD1306Ascii | greiman | Install — for ODA OLED display |

Manual ZIP install (**Sketch → Include Library → Add .ZIP Library**):
- **SPL06-007**: https://github.com/rv701/SPL06-007

---

## Step 3 — Uploading Firmware

> ⚠️ **WARNING: Always strip to PPU + CWV only when uploading. Remove all other xChips before upload or it will fail.**

1. Unsnap all xChips except PPU and CWV
2. Remove SD card from OpenLog
3. Open `HAB_FlightStation.ino` (v3.3) in Arduino IDE
4. Tools → Board → esp32 → **ESP32 Dev Module**
5. Tools → Port → **COM7** (or your assigned port)
6. Tools → Upload Speed → **115200**
7. Click **Upload** — wait for `Hash of data verified`
8. Snap all xChips back on
9. Reconnect IPP jumper wire sensors
10. Plug in USB — watch OLED and Neopixel

> **NOTE:** If upload fails with `No serial data received`, unplug USB, wait 10 seconds, replug and immediately click Upload.

---

## Step 4 — IPP Wiring Guide

The IPP board snaps into the Domino4 xBus. It has dedicated bus rails along all four edges. You will be using standard breadboard jumper wires for all connections — no soldering required.

### Understanding Jumper Wires

Breadboard jumper wires come in three types:
- **Male-to-Male (M-M):** pin on both ends. Use for connecting two breadboard holes.
- **Male-to-Female (M-F):** pin on one end, socket on other. Use when connecting to a header pin on a breakout board.
- **Female-to-Female (F-F):** socket on both ends. Rarely needed for this build.

> **NOTE:** For connecting SparkFun breakout boards to the IPP rails, you will typically use Male-to-Male jumper wires since both the breakout board headers and IPP holes accept male pins.

### Understanding the IPP Rails

Look at the IPP board. Along the top and bottom edges you will see labeled rows of holes:
- **GND** — Ground (negative). Connect all GND pins here. Use black wires by convention.
- **VCC** — 3.3V power. Connect all VCC/3V3 pins here. Use red wires by convention.
- **SDA** — I2C data. Connect all SDA pins from breakout boards here.
- **SCL** — I2C clock. Connect all SCL pins from breakout boards here.
- **TXD** — Serial transmit (GPIO 17). OpenLog RXI connects here.

> **NOTE:** Every hole in a labeled rail row is connected together. GND at the far left is the same as GND at the far right. Connect to whichever hole is closest to your component.

### Wiring Table — All Breakout Boards

| Component | Component Pin | Connect To | Wire Color | Notes |
|-----------|--------------|------------|------------|-------|
| SparkFun OpenLog | VCC | VCC rail | Red | 3.3V only — critical! |
| SparkFun OpenLog | GND | GND rail | Black | |
| SparkFun OpenLog | RXI | TXD pin (GPIO 17) | Any | Data from ESP32 to OpenLog |
| SparkFun OpenLog | TXO | Leave unconnected | — | Not needed |
| SparkFun OpenLog | GRN | Leave unconnected | — | Not needed |
| SparkFun OpenLog | BLK | Leave unconnected | — | Not needed |
| | | | | |
| KX132 Accel | VCC / 3V3 | VCC rail | Red | |
| KX132 Accel | GND | GND rail | Black | |
| KX132 Accel | SDA | SDA rail | Blue | |
| KX132 Accel | SCL | SCL rail | Yellow | |
| | | | | |
| MMC5983MA Mag | VCC / 3V3 | VCC rail | Red | |
| MMC5983MA Mag | GND | GND rail | Black | |
| MMC5983MA Mag | SDA | SDA rail | Blue | |
| MMC5983MA Mag | SCL | SCL rail | Yellow | |
| | | | | |
| AS7265x Triad | VCC / 3V3 | VCC rail | Red | |
| AS7265x Triad | GND | GND rail | Black | |
| AS7265x Triad | SDA | SDA rail | Blue | |
| AS7265x Triad | SCL | SCL rail | Yellow | |
| | | | | |
| DFRobot Geiger | VCC | VCC rail | Red | 3.3V — do not use 5V |
| DFRobot Geiger | GND | GND rail | Black | |
| DFRobot Geiger | SIG | GPIO 43 hole in grid | Any | Pulse signal to ESP32 |

> **NOTE:** Color coding is a convention to help you stay organized, not a requirement. However using red for VCC and black for GND consistently will prevent accidentally swapping power and ground, which can damage components.

### Step-by-Step Wiring Instructions

#### Before You Start
1. Unplug USB — never wire with power connected
2. Snap the IPP onto the CWV xBus connector
3. Lay out all breakout boards and jumper wires on your desk
4. Identify the VCC, GND, SDA, SCL pins on each breakout board — check the silkscreen labels on the board

#### Wire OpenLog First (Most Important)
1. Place OpenLog on the IPP center grid or nearby
2. Run red wire from OpenLog VCC to any VCC rail hole
3. Run black wire from OpenLog GND to any GND rail hole
4. Run a wire from OpenLog RXI to the TXD labeled hole on the IPP left side
5. Leave TXO, GRN, BLK pins unconnected
6. Insert FAT32 microSD card into OpenLog slot
7. Power on — OpenLog green LED should blink then go solid
8. Wait 2 minutes, power off, check SD card for LOG file with CSV data

#### Wire Each I2C Sensor (Same Pattern for All)
1. Place breakout board on IPP center grid
2. Red wire: board VCC/3V3 pin → VCC rail
3. Black wire: board GND pin → GND rail
4. Blue wire: board SDA pin → SDA rail
5. Yellow wire: board SCL pin → SCL rail
6. Power on and check Neopixel — yellow means still missing sensors, green means all found
7. Repeat for next sensor

#### Wire Geiger Counter
1. Red wire: Geiger VCC → VCC rail
2. Black wire: Geiger GND → GND rail
3. Wire: Geiger SIG → GPIO 43 hole in center grid

#### Verify Before Each Power-On
- VCC (red) goes to VCC rail — not GND rail
- GND (black) goes to GND rail — not VCC rail
- SDA and SCL not swapped
- OpenLog RXI goes to TXD, not SDA or SCL
- No bare wire ends touching each other

---

## Step 5 — xChip Snap-On Assembly

These chips require no wiring — they snap directly onto the CWV xBus connector. Order does not matter electrically but keep the stack manageable.

| xChip | Function | Notes |
|-------|----------|-------|
| EL4 | LoRa 433MHz transmitter | Must be snapped on for telemetry |
| IGA | SGP30 VOC/CO2 sensor | Needs ~15 sec warmup after power-on |
| IIA | LIS2DH12 accelerometer | Redundant with KX132 on IPP |
| IWA | SHT31 temp/humidity | Most reliable temp sensor in stack |
| IWB | SPL06 barometer | Cross-check altitude with GPS |
| ILB | Light sensor | Ambient light level |
| INA | Quectel L76 GPS | HAB mode auto-enabled at startup |
| ODA | SSD1306 OLED display | Shows live sensor data, cycles every 5 sec |
| PLA | 18650 battery | Powers all electronics |

---

## Step 6 — Testing Before Flight

### Startup Sequence
1. Insert FAT32 microSD into OpenLog
2. Snap all xChips on
3. Connect all IPP jumper wire sensors
4. Plug in USB (or flip battery switch)
5. Watch OLED startup screen — should show LoRa/GPS/Sensor status
6. Watch Neopixel — should go blue → then green (or yellow if sensors still being added)

### OpenLog Verification Test
1. Power on the full stack
2. Wait 2 minutes
3. Power off
4. Remove microSD from OpenLog
5. Insert into PC
6. Open LOG file — should contain CSV header row + data rows

### GPS Lock Test
1. Take the payload outside to an open area
2. Power on
3. Wait 2–5 minutes
4. OLED Page 2 should show real lat/lon and "Fix: YES"
5. Neopixel should be green (not purple)
