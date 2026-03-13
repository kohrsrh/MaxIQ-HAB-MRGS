# Data Analysis

## Retrieving Flight Data

After landing and recovery:

1. Power off the payload completely
2. Remove the microSD card from the **SparkFun OpenLog** (not the CWV — the CWV's built-in SD slot is not used)
3. Insert the microSD into your PC using a card reader
4. You will find a file named `LOG#####.TXT` (e.g., `LOG00001.TXT`)
5. Open it in Excel: **File → Open → select the file** — Excel will auto-detect the CSV format
6. Each row = one 30-second snapshot from the flight

---

## CSV Column Reference

The OpenLog writes one row every 30 seconds. Columns are:

| # | Column Name | Unit | Notes |
|---|-------------|------|-------|
| 1 | Packet | count | Sequential packet number starting at 1 |
| 2 | Millis | ms | Milliseconds since power-on |
| 3 | Temp_C | °C | From IWA (SHT31) xChip |
| 4 | Humidity_% | % RH | From IWA (SHT31) xChip |
| 5 | Pressure_hPa | hPa | From IWB (SPL06) xChip |
| 6 | IIA_AccelX_g | g | IIA (LIS2DH12) — low-g precision accel |
| 7 | IIA_AccelY_g | g | |
| 8 | IIA_AccelZ_g | g | |
| 9 | KX132_AccelX_g | g | KX132 — high-g accel (±16g range) |
| 10 | KX132_AccelY_g | g | |
| 11 | KX132_AccelZ_g | g | |
| 12 | TVOC_ppb | ppb | From IGA (SGP30) |
| 13 | CO2eq_ppm | ppm | Estimated from SGP30 |
| 14 | Mag_X_uT | µT | MMC5983MA raw X component |
| 15 | Mag_Y_uT | µT | MMC5983MA raw Y component |
| 16 | Mag_Z_uT | µT | MMC5983MA raw Z component |
| 17 | Mag_Strength_uT | µT | sqrt(X²+Y²+Z²) — field magnitude |
| 18 | Geiger_CPM | CPM | Counts per minute (ionizing radiation) |
| 19–36 | Triad_###nm | calibrated units | AS7265x — 18 channels from 410 nm to 940 nm |
| 37 | GPS_Lat | decimal degrees | Positive = North |
| 38 | GPS_Lon | decimal degrees | Positive = East |
| 39 | GPS_Alt_m | meters | GPS altitude above sea level |
| 40 | GPS_Speed_kmh | km/h | Ground speed |
| 41 | GPS_Sats | count | Satellites in use |
| 42 | GPS_Fix | 0/1 | 1 = valid fix, 0 = no fix |
| 43 | LoRa_OK | 0/1 | 1 = LoRa initialized OK |

### AS7265x Triad Spectroscopy Channels (columns 19–36)

| Column | Wavelength | Spectrum Region |
|--------|-----------|----------------|
| Triad_410nm | 410 nm | Near-UV |
| Triad_435nm | 435 nm | Violet |
| Triad_460nm | 460 nm | Blue |
| Triad_485nm | 485 nm | Blue-green |
| Triad_510nm | 510 nm | Green |
| Triad_535nm | 535 nm | Green |
| Triad_560nm | 560 nm | Yellow-green |
| Triad_585nm | 585 nm | Yellow |
| Triad_610nm | 610 nm | Orange |
| Triad_645nm | 645 nm | Orange-red |
| Triad_680nm | 680 nm | Red |
| Triad_705nm | 705 nm | Deep red |
| Triad_730nm | 730 nm | Near-IR |
| Triad_760nm | 760 nm | Near-IR |
| Triad_810nm | 810 nm | Near-IR |
| Triad_860nm | 860 nm | Near-IR |
| Triad_900nm | 900 nm | Near-IR |
| Triad_940nm | 940 nm | Near-IR |

---

## Useful Excel Formulas

### Convert milliseconds to flight time (minutes)
```
=B2/60000
```

### Estimate altitude from pressure (barometric formula)
```
=44330*(1-(E2/1013.25)^0.1903)
```
Where E2 is the `Pressure_hPa` column. Compare against `GPS_Alt_m` for cross-validation.

### Total acceleration magnitude (KX132)
```
=SQRT(I2^2+J2^2+K2^2)
```

### Magnetic field magnitude (already logged, but to recalculate)
```
=SQRT(N2^2+O2^2+P2^2)
```

---

## Science Highlights to Look For

| Measurement | What to Expect |
|-------------|---------------|
| Temperature | Sharp drop through troposphere (~-6.5°C/km), warming in stratosphere |
| Pressure | Exponential decrease with altitude |
| GPS Altitude | Confirms barometric altitude — differences indicate sensor drift |
| Geiger CPM | Rises ~10× from ground to stratosphere (cosmic ray increase) |
| UV/Vis channels | UV intensity increases dramatically as atmosphere thins |
| Mag Strength | Slight decrease with altitude as distance from Earth's core increases |
| Mag X/Y/Z oscillation | Reveals payload spin rate — oscillation frequency = rotation rate |
| TVOC/CO2 | Drops through clean stratospheric air, possible anomalies on descent |
