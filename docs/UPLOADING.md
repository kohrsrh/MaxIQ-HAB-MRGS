# Uploading Firmware

## Board Settings

| Setting | Value |
|---------|-------|
| Board | esp32 → **ESP32 Dev Module** |
| Port | COM7 (or your assigned port — check Device Manager) |
| Upload Speed | 115200 |

> ⚠️ Do NOT select "ESP32S3 Dev Module" — the CWV Extended Core contains an ESP32-WROOM-32 (original ESP32, not S3). Selecting the wrong board will cause an upload error.

## Programmer Setup

The **PPU xChip** is the programming interface for the CWV. It provides a USB-A port that connects to your PC.

- PPU must be snapped **directly onto the CWV** (not several chips away)
- USB-A cable from PPU to PC
- PPU is the only supported programming method for the CWV — no direct USB on the CWV itself

## Upload Procedure

1. **Unsnap all xChips except PPU + CWV** — other xChips on the SPI bus can interfere with flashing
2. **Remove the microSD card** from the OpenLog — the SD card can also interfere
3. Open `firmware/HAB_FlightStation/HAB_FlightStation.ino` in Arduino IDE
4. Set board: Tools → Board → esp32 → **ESP32 Dev Module**
5. Set port: Tools → Port → **COM7**
6. Set speed: Tools → Upload Speed → **115200**
7. Click **Upload** (→ arrow button)
8. Wait for: `Hash of data verified` — this confirms success
9. Snap all xChips back on and reconnect IPP sensors
10. Reinsert microSD card into OpenLog

## Troubleshooting

### "No serial data received"
- Unplug USB, wait 10 seconds, replug and **immediately** click Upload
- The board needs to be caught right at power-on

### "This chip is ESP32, not ESP32-S3"
- You have ESP32S3 Dev Module selected — change to **ESP32 Dev Module**

### "Failed to communicate with the flash chip"
- Remove ALL xChips except PPU + CWV
- Remove the microSD card from OpenLog
- Try a different USB cable
- Try a different USB port on your PC

### Upload speed keeps failing
- Drop upload speed: Tools → Upload Speed → **115200** (from default 921600)

## Confirming Upload Success

The output window should end with:
```
Compressed XXXXXX bytes...
Wrote XXXXXX bytes in X.X seconds
Hash of data verified.

Hard resetting via RTS pin...
```

"Hard resetting" is normal — it means the board rebooted into the new firmware. It is not an error.

## Verifying Firmware Version

After upload, the OLED startup screen will show **"HAB Station v3.3"**. If it shows an older version number, the old .ino file is still in the sketch folder — replace it with the latest file and re-upload.

## Sketch Folder Structure

Arduino IDE requires the .ino file to be inside a folder with the exact same name:

```
Documents/
└── Arduino/
    └── HAB_FlightStation/
        └── HAB_FlightStation.ino   ← file and folder names must match exactly
```

Do NOT place it inside the `libraries` folder.
