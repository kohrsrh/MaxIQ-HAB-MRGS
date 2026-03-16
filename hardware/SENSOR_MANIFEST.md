# Sensor Manifest

Complete inventory of all sensors in the HAB Flight Station payload.

## xChip Sensors (snap-on, no wiring)

| Sensor | xChip | Location | Measures | I2C Address |
|--------|-------|----------|----------|-------------|
| IGA — SGP30 | xChip | xBus | VOC, estimated CO2 | 0x58 |
| IIA — LIS2DH12 | xChip | xBus | Acceleration (low-g precision) | 0x19 |
| IWA — SHT31 | xChip | xBus | Temperature, Humidity | 0x44 |
| IWB — SPL06 | xChip | xBus | Barometric Pressure | 0x77 |
| ILB — Light | xChip | xBus | Ambient light (lux) | 0x53 |
| INA — Quectel L76 | xChip | xBus | GPS Position, Altitude, Speed (80 km limit) | 0x10 |
| ODA — SSD1306 | xChip | xBus | OLED display — live sensor readout | 0x3C |

## IPB-A Sensors (soldered to IPB xBus prototyping board)

| Sensor | Measures | Connection | SparkFun SKU |
|--------|----------|-----------|--------------|
| KX132-1211 | Acceleration up to ±16g | I2C 0x1F | SEN-17871 |
| TMAG5273 | Magnetic field strength + direction | I2C 0x22 | SEN-22932 |
| AS7265x Triad | Full spectrum UV/Vis/NIR — 18 channels | I2C 0x49 | SEN-15050 |
| DFRobot SEN0463 | Ionizing radiation (CPM) | GPIO 43 | — |

## IPB-B Sensors (soldered to IPB xBus prototyping board)

| Sensor | Measures | Connection | SparkFun SKU |
|--------|----------|-----------|--------------|
| SparkFun OpenLog | Data logger — writes CSV to microSD | Serial2 GPIO 17 | DEV-13712 |

## LoRa

| Sensor | xChip | Measures | Connection |
|--------|-------|----------|-----------|
| EL4 SX1278 | xChip | LoRa 433MHz telemetry to TinyGS | SPI |

## Notes on Redundancy

The payload intentionally carries **two accelerometers**:
- **IIA (LIS2DH12)** — lower range, higher precision during gentle ascent
- **KX132** — up to ±16g, captures violent burst events without saturating

This provides cross-validation: if both agree, readings are trustworthy. If they diverge, a sensor issue can be identified in post-flight analysis.

## Notes on Magnetometer

The TMAG5273 logs:
- **Raw field strength magnitude** (`sqrt(X² + Y² + Z²)`) — scientifically useful, shows how field varies with altitude
- **Raw X/Y/Z values in µT** — useful for analyzing payload spin rate (the oscillating pattern reveals rotation)
- **Compass heading** is NOT calculated — a spinning/tumbling payload makes heading data meaningless

The TMAG5273 reports data natively in milliTesla (mT). The firmware multiplies by 1000 to convert to µT for CSV logging, keeping units consistent with typical geomagnetic field references (Earth's field is approximately 25–65 µT).

## GPS HAB Mode

The INA xChip (Quectel L76) has a default altitude limit of 10 km. The firmware sends `$PMTK886,3` at every startup to enable HAB mode, raising the limit to 80 km. This command must execute at each power-on — it is not stored permanently in the GPS chip.
