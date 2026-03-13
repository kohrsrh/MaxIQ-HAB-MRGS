# Ground Station Setup

## TinyGS Ground Station

This payload transmits LoRa telemetry on **433.775 MHz** to a TinyGS ground station.

### Radio Configuration (must match firmware)

| Parameter | Value | Notes |
|-----------|-------|-------|
| Frequency | 433.775 MHz | Adjust `LORA_FREQ` in firmware if different |
| Bandwidth | 125 kHz | Standard |
| Spreading Factor | SF10 | Higher = longer range |
| Coding Rate | 4/5 | |
| Sync Word | 0x12 | Private network |
| TX Power | 17 dBm | Ra-01 rated at 18 dBm max |

### LoRa Module

The EL4 xChip contains an **AI Thinker Ra-01** module (SX1278 chipset), rated for 410–525 MHz at up to +18 dBm. The firmware sets 17 dBm — slightly under the chip's limit for safe operation with good range.

### Packet Format

Every 30 seconds the payload transmits an abbreviated packet:

```
FS,<packet_num>,<temp_C>,<humidity_%>,<pressure_hPa>,<kx_x>,<kx_y>,<kx_z>,<tvoc_ppb>,<co2_ppm>,<mag_strength_uT>,<geiger_cpm>,<gps_lat>,<gps_lon>,<gps_alt_m>,<gps_fix>
```

Example:
```
FS,42,18.3,45.2,850.1,0.02,-0.01,0.99,123,415,48.3,22.5,38.89012,-77.03456,12450,1
```

### Duty Cycle

Transmission every 30 seconds is well within legal duty cycle limits for 433 MHz ISM band operation. The Ra-01's 17 dBm output complies with Part 15 / ISM band power limits for amateur use.

### TinyGS Resources

- [TinyGS GitHub](https://github.com/G4lile0/tinyGS)
- [TinyGS Telegram Community](https://t.me/tinyGS_community) — active HAB experience
- [TinyGS Ground Station Dashboard](https://tinygs.com)

## Pre-Flight Ground Station Test

Before launch:
1. Power on the payload on the ground
2. Confirm your TinyGS dashboard shows incoming packets
3. Verify GPS coordinates match your launch site
4. Note the packet number — after flight, confirm packets resumed after any gaps
