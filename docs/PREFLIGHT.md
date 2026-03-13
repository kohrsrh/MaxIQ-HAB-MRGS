# Pre-Flight Checklist

## Day Before Launch

- [ ] Full charge the 18650 cell (PLA xChip)
- [ ] Full charge the USB power bank
- [ ] Format microSD card as FAT32
- [ ] Insert microSD into SparkFun OpenLog (not the CWV)
- [ ] Upload latest firmware — confirm `Hash of data verified`
- [ ] Bench test: power on for 5 minutes, pull SD card, confirm CSV data logged
- [ ] Check TinyGS dashboard for incoming LoRa packets
- [ ] Take payload outside for GPS lock test — confirm OLED Page 2 shows real coordinates and "Fix: YES"

## Launch Day — Pre-Flight

### Electronics
- [ ] Neopixel shows GREEN (all systems OK) or YELLOW (sensor warning — note which sensor)
- [ ] OLED cycling through all 4 pages
- [ ] OLED Page 1: Temp, humidity, pressure show reasonable values
- [ ] OLED Page 2: GPS shows "Fix: YES" with correct coordinates
- [ ] OLED Page 3: Geiger CPM showing background radiation (~10–25 CPM is normal)
- [ ] OLED Page 4: Packet count incrementing, LoRa shows "OK"
- [ ] TinyGS dashboard showing live packets from payload

### Mechanical
- [ ] All xChips firmly snapped — no gaps or wobble
- [ ] All IPP jumper wires secure — tug test each wire
- [ ] OpenLog microSD inserted and clicked into place
- [ ] Battery switch ON, USB disconnected before final close-up
- [ ] USB power bank connected to Insta360 and heating pad
- [ ] Insta360 recording confirmed started
- [ ] Payload box sealed — foam tape or tape around lid edges

### Safety
- [ ] FAA notification filed if required (NOTAM / waiver)
- [ ] Balloon burst calculator checked for expected burst altitude
- [ ] Recovery team briefed
- [ ] Tracking backup confirmed (separate GPS tracker)

## Post-Flight — Data Recovery

- [ ] Power off payload immediately after recovery
- [ ] Remove microSD from OpenLog
- [ ] Copy LOG file to PC backup before opening
- [ ] Open in Excel — confirm all rows present
- [ ] Note last GPS fix coordinates before landing (may differ from recovery location)
- [ ] Check TinyGS dashboard for packet log — compare against on-board log for any gaps

## Neopixel Status Reference

| Color | Meaning | Action |
|-------|---------|--------|
| 🔵 Blue | Starting up | Wait |
| 🟢 Green | All systems OK | GO |
| 🟡 Yellow | Sensor warning | Check which sensor; proceed if non-critical |
| 🔴 Red | LoRa failed | NO-GO — reseat EL4 xChip |
| 🟣 Purple | GPS not found | NO-GO — reseat INA xChip |

> **Flight can proceed on YELLOW** if the missing sensor is non-critical (e.g., magnetometer). **Do not fly on RED or PURPLE.**
