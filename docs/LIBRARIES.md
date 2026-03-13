# Required Libraries

## Install via Arduino IDE Library Manager
**Sketch → Include Library → Manage Libraries**

| # | Search Term | Library Name | Author | Notes |
|---|------------|-------------|--------|-------|
| 1 | RadioLib | RadioLib | Jan Gromes | LoRa radio driver |
| 2 | SparkFun SGP30 | SparkFun SGP30 Arduino Library | SparkFun | IGA xChip VOC/CO2 |
| 3 | SparkFun LIS2DH12 | SparkFun LIS2DH12 Arduino Library | SparkFun | IIA xChip accelerometer |
| 4 | Adafruit SHT31 | Adafruit SHT31 Library | Adafruit | IWA xChip temp/humidity — click **Install All** for dependencies |
| 5 | SparkFun KX13X | SparkFun KX13X Arduino Library | SparkFun | KX132 high-g accelerometer |
| 6 | SparkFun MMC5983MA | SparkFun MMC5983MA Arduino Library | SparkFun | Magnetometer |
| 7 | SparkFun AS7265X | SparkFun AS7265X Arduino Library | SparkFun | Triad Spectroscopy |
| 8 | Adafruit GPS | Adafruit GPS Library | Adafruit | INA xChip GPS (Quectel L76) |
| 9 | Freenove WS2812 | Freenove WS2812 Lib for ESP32 | Freenove | Neopixel status LED |
| 10 | SSD1306Ascii | SSD1306Ascii | greiman | ODA OLED display |

## Manual ZIP Install
**Sketch → Include Library → Add .ZIP Library**

| Library | Source | Notes |
|---------|--------|-------|
| SPL06-007 | https://github.com/rv701/SPL06-007 | IWB xChip barometer — not in Library Manager |

**How to install a ZIP library:**
1. Go to https://github.com/rv701/SPL06-007
2. Click the green **Code** button → **Download ZIP**
3. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
4. Select the downloaded ZIP file

## Library Notes

### Adafruit SHT31
When prompted "Install library dependencies?" — always click **Install All**. This installs the Adafruit BusIO dependency which several other Adafruit libraries also need.

### Adafruit GPS Library vs SparkFun u-blox
The INA xChip uses a **Quectel L76** GPS chip, which uses the Adafruit GPS Library. Do NOT install the SparkFun u-blox GNSS library for this — it is a different GPS chipset and will not work with the INA.

### GPS HAB Mode
The firmware automatically sends `$PMTK886,3` to the GPS at startup. This enables HAB mode and raises the altitude limit from the default 10 km to 80 km. Without this command, the GPS will stop reporting position above 10 km — well below typical HAB float altitude.

## Board Package
The ESP32 board package must be installed in Arduino IDE:

1. File → Preferences → Additional boards manager URLs
2. Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Tools → Board → Boards Manager → search "esp32" → install **esp32 by Espressif Systems**

Board selection: **Tools → Board → esp32 → ESP32 Dev Module**
