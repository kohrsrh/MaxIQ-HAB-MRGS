# Power Budget

## Circuit 1 — Electronics (18650 xChip PLA)

All science electronics run from the Domino4 PLA xChip loaded with a single 18650 lithium-ion cell.

| Component | Current Draw | Notes |
|-----------|-------------|-------|
| ESP32 (CWV) | ~80 mA | |
| LoRa TX peak | ~120 mA | Only during 30-sec transmission burst |
| All xChip sensors | ~30 mA | Combined (IGA, IIA, IWA, IWB, ILB) |
| KX132 | ~1 mA | |
| TMAG5273 | ~2.3 mA | Active mode |
| AS7265x Triad | ~5 mA | Passive mode, LEDs disabled |
| OpenLog | ~20 mA | |
| Geiger SEN0463 | ~20 mA | |
| GPS INA | ~25 mA | |
| OLED ODA | ~10 mA | |
| Neopixel | ~15 mA | At 40/255 brightness |
| **TOTAL PEAK** | **~328 mA** | Well within 18650 capacity |
| **Runtime (3000 mAh cell)** | **~9 hours** | More than enough for 4 hr flight |

## Circuit 2 — Camera + Heaters (USB Power Bank)

A separate USB power bank (10,000–20,000 mAh recommended) powers the camera and heating system independently. This ensures that if the power bank shuts off, all science electronics continue running.

| Device | Current Draw | Over 4 Hours |
|--------|-------------|-------------|
| Insta360 One X2 (recording + charging) | ~900 mA | ~3,600 mAh |
| 5V/5W silicone heating pad | ~1,000 mA | ~4,000 mAh |
| **Total** | **~1,900 mA** | **~7,600 mAh** |

A 10,000 mAh bank provides adequate margin. A 20,000 mAh bank provides a comfortable safety buffer.

## Power Bank Auto-Shutoff Warning

Most consumer power banks auto-shutoff if load drops below ~50–100 mA. With the Insta360 and heater drawing ~1.9 A continuously, auto-shutoff is not a concern. Verify with your specific bank by running it on the bench for several hours before flight.

## Thermal Notes

The 18650 cell will lose capacity in cold stratospheric temperatures. Key mitigations:
- Keep the 18650 xChip inside the insulated styrofoam enclosure
- Position it near the heating pad if possible
- A backup chemical hand warmer inside the box is cheap insurance

A styrofoam enclosure with a 5V/5W USB heating pad can maintain internal temperatures above -20°C even at stratospheric exterior temperatures of -50°C to -60°C, for a 2–4 hour flight.
