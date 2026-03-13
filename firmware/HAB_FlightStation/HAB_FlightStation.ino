/*
 * ============================================================
 *  MaxIQ MK10 HAB Flight Station Firmware v3.3
 *  Board:    Domino4 CWV (Extended Core, ESP32-WROOM-32)
 *  Arduino:  Tools > Board > esp32 > ESP32 Dev Module
 * ============================================================
 *
 * HARDWARE:
 *   LoRa:     EL4 xChip (SX1278, 433 MHz)
 *   GPS:      INA xChip (Quectel L76-L-M33, I2C 0x10)
 *             Uses Adafruit GPS Library over I2C
 *             HAB mode enabled at startup (allows 80km altitude)
 *             ** Default altitude limit is 10km - HAB mode MUST
 *                be set or GPS will stop reporting above 10km! **
 *   Sensors (xChips):
 *             IGA - SGP30  VOC/CO2       I2C 0x58
 *             IIA - LIS2DH12 Accel       I2C 0x19
 *             IWA - SHT31  Temp/Humidity I2C 0x44
 *             IWB - SPL06  Barometer     I2C 0x77
 *             ILB - Light sensor         I2C 0x53
 *   Sensors (IPP breadboard):
 *             KX132       Accel          I2C 0x1F
 *             MMC5983MA   Magnetometer   I2C 0x30
 *             AS7265x     Triad Spectro  I2C 0x49
 *             Geiger      DFRobot SEN0463 GPIO 43
 *   Logging:  SparkFun OpenLog           Serial2 TX GPIO 17
 *   Display:  ODA xChip (SSD1306 OLED 128x64)  I2C 0x3C
 *
 * ============================================================
 * LIBRARIES TO INSTALL (Sketch > Manage Libraries)
 * ============================================================
 *   1. RadioLib                     (Jan Gromes)
 *   2. SparkFun SGP30               (SparkFun)
 *   3. SparkFun LIS2DH12            (SparkFun)
 *   4. Adafruit SHT31               (Adafruit) - click Install All
 *   5. SparkFun KX13X               (SparkFun)
 *   6. SparkFun MMC5983MA           (SparkFun)
 *   7. SparkFun AS7265x             (SparkFun)
 *   8. Adafruit GPS Library             (Adafruit) - for INA xChip
 *   9. Freenove WS2812              (Freenove)
 *  10. SSD1306Ascii                 (greiman) - for ODA OLED display
 *
 * MANUAL ZIP INSTALL (Sketch > Add .ZIP Library):
 *   SPL06-007: https://github.com/rv701/SPL06-007
 *
 * ============================================================
 * NEOPIXEL STATUS COLORS
 * ============================================================
 *   BLUE   = Starting up
 *   GREEN  = All systems OK
 *   YELLOW = One or more sensors not found (still logging)
 *   RED    = LoRa failed (still logging to OpenLog)
 *   PURPLE = GPS not found (still logging other data)
 *
 * ============================================================
 * OLED DISPLAY PAGES (cycles every 5 seconds in loop)
 * ============================================================
 *   Page 1: Temperature, Humidity, Pressure
 *   Page 2: GPS coordinates, altitude, fix status
 *   Page 3: Radiation CPM, VOC, CO2
 *   Page 4: Packet count, LoRa status, uptime
 *
 * ============================================================
 */

// ── Libraries ──────────────────────────────────────────────
#include <RadioLib.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFun_SGP30_Arduino_Library.h>
#include <SparkFun_LIS2DH12.h>
#include "Adafruit_SHT31.h"
#include "SPL06-007.h"
#include <SparkFun_KX13X.h>
#include <SparkFun_MMC5983MA_Arduino_Library.h>
#include <SparkFun_AS7265X.h>
#include <Adafruit_GPS.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <SSD1306AsciiWire.h>     // ODA OLED display

// ── Pin Definitions ────────────────────────────────────────
#define SPI_MISO      12
#define SPI_MOSI      13
#define SPI_SCK       14
#define LORA_CS       15
#define LORA_DIO0     33
#define LORA_RST      32
#define LORA_DIO1     -1
#define GEIGER_PIN    43
#define OPENLOG_TX    17
#define NEOPIXEL_PIN  18
#define I2C_SDA       26
#define I2C_SCL       27

// ── I2C Addresses ──────────────────────────────────────────
#define GPS_I2C_ADDR  0x10   // INA xChip. NEO-M9N breakout = 0x42
#define OLED_ADDR     0x3C   // ODA xChip SSD1306

// ── LoRa Settings ──────────────────────────────────────────
#define LORA_FREQ     433.775
#define LORA_BW       125.0
#define LORA_SF       10
#define LORA_CR       5
#define LORA_SYNC     0x12
#define LORA_POWER    17
#define TX_INTERVAL   30000

// ── OpenLog ────────────────────────────────────────────────
#define OPENLOG_BAUD  9600

// ── Neopixel ───────────────────────────────────────────────
Freenove_ESP32_WS2812 strip(1, NEOPIXEL_PIN, 0, TYPE_GRB);

// ── Sensor Objects ─────────────────────────────────────────
SGP30 vocSensor;                              // IGA
SPARKFUN_LIS2DH12 accelIIA;                   // IIA xChip accel
Adafruit_SHT31 tempHumSensor;                 // IWA
SparkFun_KX132 accelKX132;                    // KX132 on IPP
SFE_MMC5983MA magnetometer;                   // MMC5983MA on IPP
AS7265X triadSensor;                          // AS7265x Triad on IPP
// GPS object declared below with Adafruit library

// ── GPS (INA xChip - Quectel L76 via Adafruit GPS Library) ─
Adafruit_GPS gps(&Wire);

// ── OLED Display ───────────────────────────────────────────
SSD1306AsciiWire oled;

// ── LoRa ───────────────────────────────────────────────────
SPIClass hspi(HSPI);
SX1278 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, hspi);

// ── State ──────────────────────────────────────────────────
unsigned long lastTxTime = 0;
unsigned long lastPageTime = 0;
unsigned long packetCount = 0;
volatile unsigned long geigerCount = 0;
bool loraOK = false;
bool gpsOK = false;
bool sensorWarning = false;
int oledPage = 0;

// Per-sensor OK flags - prevents hanging on missing sensors
bool vocOK = false;
bool iiaOK = false;
bool iwaOK = false;
bool kx132OK = false;
bool magOK = false;
bool triadOK = false;
bool geigerOK = false;

// Last known sensor values for OLED display
float lastTemp = 0, lastHumidity = 0, lastPressure = 0;
float lastGpsLat = 0, lastGpsLon = 0, lastGpsAlt = 0;
bool lastGpsFix = false;
int lastGpsSats = 0;
float lastGeigerCPM = 0;
uint16_t lastTVOC = 0, lastCO2 = 0;

// ── Geiger Interrupt ───────────────────────────────────────
void IRAM_ATTR geigerPulse() { geigerCount++; }

// ── Neopixel Helper ────────────────────────────────────────
void setColor(int r, int g, int b) {
  strip.setLedColorData(0, r, g, b);
  strip.show();
}

// ── OLED Helper ────────────────────────────────────────────
void oledLine(uint8_t row, const char* label, String value) {
  oled.setCursor(0, row);
  oled.clearToEOL();
  oled.print(label);
  oled.print(value);
}

void updateOLED() {
  oled.clear();
  switch (oledPage) {
    case 0:  // Temp / Humidity / Pressure
      oled.setCursor(0, 0); oled.print("-- Environment --");
      oledLine(1, "Temp: ", String(lastTemp, 1) + " C");
      oledLine(2, "Hum:  ", String(lastHumidity, 1) + " %");
      oledLine(3, "Pres: ", String(lastPressure, 1) + " hPa");
      break;
    case 1:  // GPS
      oled.setCursor(0, 0); oled.print("------ GPS ------");
      oledLine(1, "Lat: ", String(lastGpsLat, 5));
      oledLine(2, "Lon: ", String(lastGpsLon, 5));
      oledLine(3, "Alt: ", String(lastGpsAlt, 0) + "m Sats:" + String(lastGpsSats));
      oledLine(4, "Fix: ", lastGpsFix ? "YES" : "NO - searching");
      break;
    case 2:  // Radiation / Air Quality
      oled.setCursor(0, 0); oled.print("-- Air/Radiation -");
      oledLine(1, "CPM:  ", String(lastGeigerCPM, 1));
      oledLine(2, "TVOC: ", String(lastTVOC) + " ppb");
      oledLine(3, "CO2:  ", String(lastCO2) + " ppm");
      break;
    case 3:  // System status
      oled.setCursor(0, 0); oled.print("---- System -----");
      oledLine(1, "Pkts: ", String(packetCount));
      oledLine(2, "LoRa: ", loraOK ? "OK" : "FAIL");
      oledLine(3, "Up:   ", String(millis() / 60000) + " min");
      oledLine(4, "v3.3  HAB Station", "");
      break;
  }
}

// ===========================================================
//  SETUP
// ===========================================================
void setup() {
  strip.begin();
  strip.setBrightness(40);
  setColor(0, 0, 255);  // BLUE = starting

  Serial.begin(115200);
  delay(2000);

  Serial2.begin(OPENLOG_BAUD, SERIAL_8N1, -1, OPENLOG_TX);
  delay(500);

  Serial.println("=== HAB Flight Station v3.0 Starting ===");

  Wire.begin(I2C_SDA, I2C_SCL);

  // ── OLED Display ──
  oled.begin(&Adafruit128x64, OLED_ADDR);
  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("HAB Station v3.3");
  oled.setCursor(0, 2);
  oled.print("Initializing...");
  oled.setCursor(0, 4);
  oled.print("Please wait");

  // ── LoRa ──
  hspi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, -1);
  pinMode(LORA_CS, OUTPUT);
  digitalWrite(LORA_CS, HIGH);
  Serial.print("LoRa... ");
  int loraState = radio.begin(LORA_FREQ, LORA_BW, LORA_SF, LORA_CR, LORA_SYNC, LORA_POWER);
  if (loraState == RADIOLIB_ERR_NONE) {
    Serial.println("OK"); loraOK = true;
  } else {
    Serial.print("FAILED err="); Serial.println(loraState); loraOK = false;
  }

  // ── GPS (INA xChip - Quectel L76) ──
  Serial.print("GPS INA... ");
  if (gps.begin(GPS_I2C_ADDR)) {
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // RMC + GGA sentences
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    // 1Hz update rate
    gps.sendCommand("$PMTK886,3");                // HAB mode - allows 80km altitude!
    Serial.println("OK - HAB mode enabled");
    gpsOK = true;
  } else {
    Serial.println("NOT FOUND"); gpsOK = false; sensorWarning = true;
  }

  // ── SGP30 VOC (IGA) ──
  Serial.print("IGA VOC... ");
  if (vocSensor.begin()) {
    vocSensor.initAirQuality(); Serial.println("OK"); vocOK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── LIS2DH12 Accel (IIA xChip) ──
  Serial.print("IIA Accel... ");
  if (accelIIA.begin()) {
    Serial.println("OK"); iiaOK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── SHT31 Temp/Humidity (IWA) ──
  Serial.print("IWA Temp/Hum... ");
  if (tempHumSensor.begin(0x44)) {
    Serial.println("OK"); iwaOK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── SPL06 Barometer (IWB) ──
  Serial.print("IWB Baro... ");
  SPL_init(0x77);
  Serial.println("OK");

  // ── KX132 Accel (IPP) ──
  Serial.print("KX132 Accel... ");
  if (accelKX132.begin()) {
    accelKX132.setRange(SFE_KX132_RANGE16G);
    Serial.println("OK"); kx132OK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── MMC5983MA Magnetometer (IPP) ──
  Serial.print("Magnetometer... ");
  if (magnetometer.begin()) {
    magnetometer.softReset(); Serial.println("OK"); magOK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── AS7265x Triad Spectroscopy (IPP) ──
  Serial.print("Triad Spectro... ");
  if (triadSensor.begin()) {
    triadSensor.disableIndicator();
    Serial.println("OK"); triadOK = true;
  } else {
    Serial.println("NOT FOUND"); sensorWarning = true;
  }

  // ── Geiger ──
  pinMode(GEIGER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GEIGER_PIN), geigerPulse, FALLING);
  Serial.println("Geiger OK pin " + String(GEIGER_PIN));
  geigerOK = true;

  // ── OpenLog CSV Header ──
  Serial2.println("Packet,Millis,"
    "Temp_C,Humidity_%,Pressure_hPa,"
    "IIA_AccelX_g,IIA_AccelY_g,IIA_AccelZ_g,"
    "KX132_AccelX_g,KX132_AccelY_g,KX132_AccelZ_g,"
    "TVOC_ppb,CO2eq_ppm,"
    "Mag_X_uT,Mag_Y_uT,Mag_Z_uT,Mag_Strength_uT,"
    "Geiger_CPM,"
    "Triad_410nm,Triad_435nm,Triad_460nm,Triad_485nm,"
    "Triad_510nm,Triad_535nm,Triad_560nm,Triad_585nm,"
    "Triad_610nm,Triad_645nm,Triad_680nm,Triad_705nm,"
    "Triad_730nm,Triad_760nm,Triad_810nm,Triad_860nm,"
    "Triad_900nm,Triad_940nm,"
    "GPS_Lat,GPS_Lon,GPS_Alt_m,GPS_Speed_kmh,GPS_Sats,GPS_Fix,"
    "LoRa_OK");
  Serial.println("CSV header sent to OpenLog");

  // ── Status LED ──
  if (!loraOK) {
    setColor(255, 0, 0);       // RED = LoRa failed
  } else if (!gpsOK) {
    setColor(128, 0, 128);     // PURPLE = GPS not found
  } else if (sensorWarning) {
    setColor(255, 150, 0);     // YELLOW = sensor warning
  } else {
    setColor(0, 255, 0);       // GREEN = all good
  }

  Serial.println("=== Setup Complete ===\n");

  // Show ready screen on OLED
  oled.clear();
  oled.setCursor(0, 0); oled.print("HAB Station v3.3");
  oled.setCursor(0, 2); oled.print(loraOK  ? "LoRa:  OK" : "LoRa:  FAIL");
  oled.setCursor(0, 3); oled.print(gpsOK   ? "GPS:   OK" : "GPS:   NOT FOUND");
  oled.setCursor(0, 4); oled.print(sensorWarning ? "Sensors: CHECK" : "Sensors: OK");
  oled.setCursor(0, 6); oled.print("Starting loop...");
}

// ===========================================================
//  MAIN LOOP
// ===========================================================
void loop() {
  // ── OLED Page Cycling (every 5 seconds, independent of TX) ──
  if (millis() - lastPageTime >= 5000) {
    lastPageTime = millis();
    oledPage = (oledPage + 1) % 4;
    updateOLED();
  }

  if (millis() - lastTxTime < TX_INTERVAL) return;
  lastTxTime = millis();
  packetCount++;

  // ── Temp / Humidity / Pressure ──
  float temp_C = 0, humidity = 0;
  if (iwaOK) {
    temp_C   = tempHumSensor.readTemperature();
    humidity = tempHumSensor.readHumidity();
  }
  float pressure = get_pressure() / 100.0;  // SPL06 always available

  // ── IIA Accelerometer ──
  float iiaX = 0, iiaY = 0, iiaZ = 0;
  if (iiaOK && accelIIA.available()) {
    iiaX = accelIIA.getX();
    iiaY = accelIIA.getY();
    iiaZ = accelIIA.getZ();
  }

  // ── KX132 Accelerometer ──
  float kxX = 0, kxY = 0, kxZ = 0;
  if (kx132OK) {
    outputData kxData;
    if (accelKX132.dataReady()) {
      accelKX132.getAccelData(&kxData);
      kxX = kxData.xData;
      kxY = kxData.yData;
      kxZ = kxData.zData;
    }
  }

  // ── VOC ──
  uint16_t tvoc = 0, co2eq = 0;
  if (vocOK) {
    vocSensor.measureAirQuality();
    tvoc  = vocSensor.TVOC;
    co2eq = vocSensor.CO2;
  }

  // ── Magnetometer ──
  float magX = 0, magY = 0, magZ = 0, magStrength = 0;
  if (magOK) {
    uint32_t magRawX = 0, magRawY = 0, magRawZ = 0;
    magnetometer.getMeasurementXYZ(&magRawX, &magRawY, &magRawZ);
    magX = ((float)magRawX - 131072.0) / 16384.0 * 100.0;
    magY = ((float)magRawY - 131072.0) / 16384.0 * 100.0;
    magZ = ((float)magRawZ - 131072.0) / 16384.0 * 100.0;
    magStrength = sqrt(magX*magX + magY*magY + magZ*magZ);
  }

  // ── Geiger ──
  float geigerCPM = 0;
  if (geigerOK) {
    unsigned long geigerSnapshot = geigerCount;
    geigerCount = 0;
    geigerCPM = (float)geigerSnapshot * (60000.0 / TX_INTERVAL);
  }

  // ── Triad Spectroscopy (blocking ~3 sec - only if connected) ──
  float t410=0,t435=0,t460=0,t485=0,t510=0,t535=0,t560=0,t585=0,t610=0;
  float t645=0,t680=0,t705=0,t730=0,t760=0,t810=0,t860=0,t900=0,t940=0;
  if (triadOK) {
    triadSensor.takeMeasurements();
    t410 = triadSensor.getCalibratedA();
    t435 = triadSensor.getCalibratedB();
    t460 = triadSensor.getCalibratedC();
    t485 = triadSensor.getCalibratedD();
    t510 = triadSensor.getCalibratedE();
    t535 = triadSensor.getCalibratedF();
    t560 = triadSensor.getCalibratedG();
    t585 = triadSensor.getCalibratedH();
    t610 = triadSensor.getCalibratedR();
    t645 = triadSensor.getCalibratedI();
    t680 = triadSensor.getCalibratedS();
    t705 = triadSensor.getCalibratedJ();
    t730 = triadSensor.getCalibratedT();
    t760 = triadSensor.getCalibratedU();
    t810 = triadSensor.getCalibratedV();
    t860 = triadSensor.getCalibratedW();
    t900 = triadSensor.getCalibratedK();
    t940 = triadSensor.getCalibratedL();
  }

  // ── GPS ──
  float gpsLat = 0, gpsLon = 0, gpsAlt = 0, gpsSpeed = 0;
  int gpsSats = 0;
  bool gpsFix = false;
  if (gpsOK) {
    // Read NMEA with 1 second timeout - never block indefinitely
    unsigned long gpsDeadline = millis() + 1000;
    while (millis() < gpsDeadline) {
      if (gps.available()) {
        char c = gps.read();
        if (gps.newNMEAreceived()) {
          gps.parse(gps.lastNMEA());
          break;
        }
      }
    }
    if (gps.fix) {
      gpsLat   = gps.latitudeDegrees  * (gps.lat  == 'N' ? 1 : -1);
      gpsLon   = gps.longitudeDegrees * (gps.lon  == 'E' ? 1 : -1);
      gpsAlt   = gps.altitude;
      gpsSpeed = gps.speed * 1.852;
      gpsSats  = gps.satellites;
      gpsFix   = true;
    }
  }

  // ── Cache values for OLED ──
  lastTemp = temp_C; lastHumidity = humidity; lastPressure = pressure;
  lastTVOC = tvoc; lastCO2 = co2eq;
  lastGeigerCPM = geigerCPM;
  lastGpsLat = gpsLat; lastGpsLon = gpsLon;
  lastGpsAlt = gpsAlt; lastGpsFix = gpsFix; lastGpsSats = gpsSats;

  // ── Serial Monitor ──
  Serial.printf("--- Packet #%lu ---\n", packetCount);
  Serial.printf("  Temp: %.1fC  Hum: %.1f%%  Pressure: %.1fhPa\n", temp_C, humidity, pressure);
  Serial.printf("  IIA Accel: X%.2f Y%.2f Z%.2f g\n", iiaX, iiaY, iiaZ);
  Serial.printf("  KX132:     X%.2f Y%.2f Z%.2f g\n", kxX, kxY, kxZ);
  Serial.printf("  TVOC: %u ppb  CO2eq: %u ppm\n", tvoc, co2eq);
  Serial.printf("  Mag: X%.1f Y%.1f Z%.1f uT  Strength: %.1f uT\n", magX, magY, magZ, magStrength);
  Serial.printf("  Geiger: %.1f CPM\n", geigerCPM);
  Serial.printf("  GPS: %.6f, %.6f  Alt: %.1fm  Speed: %.1fkm/h  Sats: %d  Fix: %s\n",
    gpsLat, gpsLon, gpsAlt, gpsSpeed, gpsSats, gpsFix ? "YES" : "NO");

  // ── Log to OpenLog ──
  String csv =
    String(packetCount) + "," + String(millis()) + "," +
    String(temp_C, 1) + "," + String(humidity, 1) + "," + String(pressure, 1) + "," +
    String(iiaX, 3) + "," + String(iiaY, 3) + "," + String(iiaZ, 3) + "," +
    String(kxX, 3) + "," + String(kxY, 3) + "," + String(kxZ, 3) + "," +
    String(tvoc) + "," + String(co2eq) + "," +
    String(magX, 2) + "," + String(magY, 2) + "," + String(magZ, 2) + "," +
    String(magStrength, 2) + "," +
    String(geigerCPM, 1) + "," +
    String(t410,1)+","+String(t435,1)+","+String(t460,1)+","+String(t485,1)+"," +
    String(t510,1)+","+String(t535,1)+","+String(t560,1)+","+String(t585,1)+"," +
    String(t610,1)+","+String(t645,1)+","+String(t680,1)+","+String(t705,1)+"," +
    String(t730,1)+","+String(t760,1)+","+String(t810,1)+","+String(t860,1)+"," +
    String(t900,1)+","+String(t940,1)+"," +
    String(gpsLat, 6) + "," + String(gpsLon, 6) + "," +
    String(gpsAlt, 1) + "," + String(gpsSpeed, 1) + "," +
    String(gpsSats) + "," + String(gpsFix ? 1 : 0) + "," +
    String(loraOK ? 1 : 0);

  Serial2.println(csv);
  Serial.println("  >> Logged to OpenLog");

  // ── LoRa Transmit (abbreviated packet - key fields only) ──
  if (loraOK) {
    String pkt = "FS," +
      String(packetCount) + "," +
      String(temp_C, 1) + "," + String(humidity, 1) + "," + String(pressure, 1) + "," +
      String(kxX, 2) + "," + String(kxY, 2) + "," + String(kxZ, 2) + "," +
      String(tvoc) + "," + String(co2eq) + "," +
      String(magStrength, 1) + "," +
      String(geigerCPM, 1) + "," +
      String(gpsLat, 5) + "," + String(gpsLon, 5) + "," +
      String(gpsAlt, 0) + "," + String(gpsFix ? 1 : 0);

    Serial.print("  >> LoRa TX... ");
    int txState = radio.transmit(pkt);
    if (txState == RADIOLIB_ERR_NONE) {
      Serial.println("OK");
    } else {
      Serial.print("FAILED err="); Serial.println(txState);
    }
  }

  Serial.println();
}
