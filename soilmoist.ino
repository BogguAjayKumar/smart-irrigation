/*
 * =====================================================================
 * Project: Arduino Automated Soil Moisture & Water Spray System
 * File: soilmoist.ino
 * Description: Reads soil moisture from the Flying-Fish (MH-Sensor Series)
 *              probe. When soil is dry, activates a low-level trigger relay
 *              to spray water into the soil, waits for water to soak, and
 *              prevents over-watering.
 * =====================================================================
 * 
 * HARDWARE SETUP & WIRING:
 * 
 * 1. Flying-Fish Soil Moisture Sensor (MH-Sensor Module):
 *    - VCC  ---> Arduino 5V
 *    - GND  ---> Arduino GND
 *    - A0   ---> Arduino Analog Pin A0
 *    - (The 2-pin header connects to the two-prong soil probe)
 * 
 * 2. 5V Relay Module (Low-Level Trigger):
 *    - VCC  ---> Arduino 5V
 *    - GND  ---> Arduino GND
 *    - IN   ---> Arduino Digital Pin 8 (PUMP_PIN)
 * 
 * 3. Water Pump / Motor Connections (Switched through Relay):
 *    - Motor Power Supply (+) ---> Relay COM (Common)
 *    - Relay NO (Normally Open) ---> Water Pump (+)
 *    - Water Pump (-)           ---> Motor Power Supply (-)
 *    (Note: Using an external 5V/battery pack for the motor is recommended
 *     so motor electrical noise doesn't reset the Arduino).
 * 
 * 4. Built-in LED:
 *    - Pin 13 ---> Lights up whenever the water pump is actively spraying.
 * =====================================================================
 */

// ================= Pin Definitions =================
const int SOIL_ANALOG_PIN = A0;  // Analog pin from Flying-Fish A0
const int PUMP_RELAY_PIN  = 8;   // Digital pin connected to Relay IN
const int STATUS_LED_PIN  = 13;  // Arduino onboard LED (Pin 13)

// ================= Relay Logic Configuration =================
// Low-Level Trigger Relay:
// LOW  (0V) = Relay ON (Pump running)
// HIGH (5V) = Relay OFF (Pump stopped)
const int RELAY_ON  = LOW;
const int RELAY_OFF = HIGH;

// ================= Calibration & Thresholds =================
// Calibrate using 'calibration/calibration.ino' if needed.
// Flying-Fish / FC-28 typical values:
// Air (completely dry)  ~ 1023
// Water (submerged)     ~ 300 - 400
const int AIR_VALUE   = 950;  // Raw ADC in dry air
const int WATER_VALUE = 350;  // Raw ADC in water

// Moisture thresholds (0% to 100%)
const int DRY_THRESHOLD     = 30; // Below 30%: Trigger water spray
const int OPTIMAL_THRESHOLD = 60; // Above 60%: Soil is sufficiently moist

// Spraying & Soak Timers (in milliseconds)
const unsigned long SPRAY_DURATION_MS = 3000;  // Spray for 3 seconds per burst
const unsigned long SOAK_DELAY_MS     = 7000;  // Wait 7 seconds for water to absorb
const unsigned long CHECK_INTERVAL_MS = 1500;  // Regular check interval

// ================= Setup =================
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port on USB boards
  }

  // Safety first: Initialize relay state to OFF BEFORE setting pin to OUTPUT
  // This prevents the relay from momentarily clicking ON when Arduino powers up
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  pinMode(PUMP_RELAY_PIN, OUTPUT);

  // Initialize LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Startup Banner
  Serial.println(F("================================================"));
  Serial.println(F("   Automated Soil Moisture Irrigation System    "));
  Serial.println(F("   Sensor: Flying-Fish MH-Sensor Series         "));
  Serial.println(F("   Relay: Low-Level Trigger                     "));
  Serial.println(F("================================================"));
  Serial.println(F("System initialized. Monitoring soil moisture...\n"));
}

// ================= Main Loop =================
void loop() {
  // 1. Take a smoothed analog reading from soil sensor
  int rawADC = getAveragedReading(SOIL_ANALOG_PIN, 5);

  // 2. Map raw reading to moisture percentage (0% = dry, 100% = wet)
  int moisturePercent = map(rawADC, AIR_VALUE, WATER_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // 3. Print current status
  Serial.print(F("Raw ADC: "));
  Serial.print(rawADC);
  Serial.print(F(" | Moisture: "));
  if (moisturePercent < 10) Serial.print(F(" "));
  Serial.print(moisturePercent);
  Serial.print(F("% | Status: "));

  // 4. Check if soil is dry and needs watering
  if (moisturePercent < DRY_THRESHOLD) {
    Serial.println(F("DRY -> SPRAYING WATER!"));
    triggerWaterSpray();
  } else if (moisturePercent <= OPTIMAL_THRESHOLD) {
    Serial.println(F("OPTIMAL (Moist)"));
    delay(CHECK_INTERVAL_MS);
  } else {
    Serial.println(F("WET (Saturated)"));
    delay(CHECK_INTERVAL_MS);
  }
}

// ================= Action Functions =================

// Activates the low-level trigger relay to spray water, then waits for soak
void triggerWaterSpray() {
  Serial.print(F(">>> Pump ON (Spraying for "));
  Serial.print(SPRAY_DURATION_MS / 1000);
  Serial.println(F(" seconds)..."));

  // Turn ON Relay & Indicator LED
  digitalWrite(PUMP_RELAY_PIN, RELAY_ON);
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(SPRAY_DURATION_MS);

  // Turn OFF Relay & Indicator LED
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.println(F(">>> Pump OFF."));

  // Soak time: Allow water to penetrate the soil before taking the next reading
  Serial.print(F(">>> Waiting "));
  Serial.print(SOAK_DELAY_MS / 1000);
  Serial.println(F(" seconds for water to soak into soil..."));
  delay(SOAK_DELAY_MS);
  Serial.println(F("------------------------------------------------"));
}

// Reads the analog pin 'samples' times and returns the average
int getAveragedReading(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(15);
  }
  return (int)(sum / samples);
}
