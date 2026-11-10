/*
 * =====================================================================
 * Project: Soil Moisture Sensor Calibration Tool
 * File: calibration.ino
 * Description: Use this utility sketch to determine the exact RAW ADC 
 *              values for your specific sensor in DRY and WET conditions.
 * =====================================================================
 * 
 * INSTRUCTIONS:
 * 1. Wire the sensor:
 *    - VCC -> 5V (or 3.3V)
 *    - GND -> GND
 *    - AOUT/A0 -> A0
 * 2. Upload this sketch to your Arduino Uno.
 * 3. Open Serial Monitor at 9600 baud.
 * 4. Step 1 (DRY AIR):
 *    - Hold the sensor dry in open air.
 *    - Note down the average Raw ADC value shown (e.g., 750 or 1020).
 *    - This will be your AIR_VALUE.
 * 5. Step 2 (WET WATER):
 *    - Dip the probe into a cup of water (do NOT submerge past the probe lines / electronics!).
 *    - Note down the average Raw ADC value shown (e.g., 280 or 350).
 *    - This will be your WATER_VALUE.
 * 6. Copy these two numbers into 'soilmoist.ino':
 *       const int AIR_VALUE   = <your dry value>;
 *       const int WATER_VALUE = <your wet value>;
 * =====================================================================
 */

const int SENSOR_PIN = A0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port
  }

  Serial.println(F("========================================"));
  Serial.println(F("   Soil Moisture Sensor Calibration     "));
  Serial.println(F("========================================"));
  Serial.println(F("Keep sensor dry in air first, then dip into water."));
  Serial.println(F("Watch the values below:\n"));
}

void loop() {
  // Take 10 readings to provide a stable average
  long total = 0;
  int minVal = 1023;
  int maxVal = 0;

  for (int i = 0; i < 10; i++) {
    int val = analogRead(SENSOR_PIN);
    total += val;
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    delay(20);
  }

  int avgVal = total / 10;

  Serial.print(F("Current Raw ADC: "));
  Serial.print(avgVal);
  Serial.print(F("  [Min: "));
  Serial.print(minVal);
  Serial.print(F(", Max: "));
  Serial.print(maxVal);
  Serial.println(F("]"));

  delay(1000);
}
