# Wiring Reference

Quick-reference wiring tables for all sensors connected to the IPB xBus prototyping boards.

---

## IPB Board Layout

Each IPB has eight labeled signal rows: **VCC, SDA, SCL, VSRC, IO, TXD, RXD, GND**

Each row has three through-holes:
- **Left two holes** — your working solder points (electrically identical)
- **Right single hole** — xBus backplane pass-through (do not use for sensor wires)

When a row says "left hole 1" and "left hole 2", both are the same signal — one hole per sensor wire.
When a row is full (both left holes occupied), solder additional wires directly onto an existing joint.

---

## IPB-A — I2C Sensors + Geiger

### KX132-1211 Accelerometer (I2C 0x1F)

| KX132 Pin | IPB-A Hole |
|-----------|-----------|
| VCC / 3V3 | VCC row — left hole 1 |
| GND | GND row — left hole 1 |
| SDA | SDA row — left hole 1 |
| SCL | SCL row — left hole 1 |

### TMAG5273 Magnetometer (I2C 0x22)

| TMAG5273 Pin | IPB-A Hole | Notes |
|-------------|-----------|-------|
| VCC / 3V3 | VCC row — left hole 2 | Use VCC (3.3V) only — never VSRC |
| GND | GND row — left hole 2 | |
| SDA | SDA row — left hole 2 | |
| SCL | SCL row — left hole 2 | |

### AS7265x Triad Spectroscopy (I2C 0x49)

Both left holes in VCC, GND, SDA, SCL are occupied. Tin the wire end and solder onto either existing left-hole joint in each row.

| AS7265x Pin | IPB-A Connection |
|-------------|-----------------|
| VCC / 3V3 | Splice onto VCC row — either left hole |
| GND | Splice onto GND row — either left hole |
| SDA | Splice onto SDA row — either left hole |
| SCL | Splice onto SCL row — either left hole |

### DFRobot SEN0463 Geiger Counter (GPIO 43)

| Geiger Pin | IPB-A Hole | Notes |
|-----------|-----------|-------|
| Signal / OUT | IO row — left hole 1 | Falling-edge interrupt to GPIO 43 |
| VCC | VCC row — splice | 3.3V; if module requires 5V use VSRC row instead |
| GND | GND row — splice | |

> ⚠️ The IO pad is 3.3V logic. If the Geiger signal output is 5V, add a voltage divider on the signal wire before the IO hole.

---

## IPB-B — OpenLog UART

### SparkFun OpenLog

| OpenLog Pin | IPB-B Hole | Notes |
|------------|-----------|-------|
| VCC / 3V3 | VCC row — left hole 1 | |
| GND | GND row — left hole 1 | |
| RXI | TXD row — left hole 1 | CWV transmits on GPIO 17; OpenLog receives and logs |
| TXO | Not connected | Firmware does not read from OpenLog |

> The TXD label on the IPB is from the CWV's perspective. OpenLog's RXI (receive input) connects here — this is correct.

---

## Signal Summary

| Signal | CWV v1-4 GPIO | Notes |
|--------|--------------|-------|
| I2C SDA | 26 | Shared by all I2C sensors |
| I2C SCL | 27 | Shared by all I2C sensors |
| Geiger interrupt | 43 | IO pad on IPB-A |
| OpenLog TX | 17 | TXD pad on IPB-B |
| LoRa MISO | 12 | SPI via extension slot |
| LoRa MOSI | 13 | SPI via extension slot |
| LoRa SCK | 14 | SPI via extension slot |
| LoRa CS | 15 | SPI via extension slot |
| LoRa DIO0 | 33 | SPI via extension slot |
| Neopixel | 18 | On CWV board |
