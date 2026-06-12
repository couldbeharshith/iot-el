/*
 * Hardware Manager Implementation
 */

#include "hardware_manager.h"

HardwareManager::HardwareManager() 
  : keypad(KEYPAD_ADDRESS),
    neopixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800),
    buzzerPin(PIN_BUZZER),
    buzzerActive(false) {
}

void HardwareManager::begin() {
  // Initialize I2C
  Wire.begin();
  
  // Initialize keypad
  if (!keypad.begin()) {
    Serial.println("ERROR: Cannot communicate with keypad!");
  } else {
    Serial.println("Keypad initialized");
  }
  
  // Initialize buzzer (active-low logic: HIGH = OFF, LOW = ON)
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);  // Ensure buzzer starts HIGH (OFF for active-low)
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcWriteTone(BUZZER_CHANNEL, 0); // Ensure buzzer is off initially
  Serial.println("Buzzer initialized");
  
  // Initialize NeoPixel
  neopixel.begin();
  neopixel.setBrightness(100);  // 0-255 range
  neopixel.clear();
  neopixel.show();
  Serial.println("NeoPixel initialized");
  
  // Initialize potentiometer
  pinMode(PIN_POTENTIOMETER, INPUT);
  Serial.println("Potentiometer initialized");
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("ERROR: Couldn't find RTC!");
  } else {
    if (!rtc.isrunning()) {
      Serial.println("RTC not running, setting time...");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    Serial.println("RTC initialized");
  }
  
  // Startup animation - quick rainbow
  setStatusLED(255, 0, 0);
  delay(100);
  setStatusLED(0, 255, 0);
  delay(100);
  setStatusLED(0, 0, 255);
  delay(100);
  setStatusLED(0, 0, 0);
  
  // Test beep
  playShortBeep();
}

char HardwareManager::readKeypad() {
  uint8_t idx = keypad.getKey();
  return keys[idx];
}

int HardwareManager::readSeverity() {
  int rawValue = analogRead(PIN_POTENTIOMETER);
  // Convert 0-4095 to 0-100
  int severity = map(rawValue, 0, 4095, 0, 100);
  return constrain(severity, 0, 100);
}

void HardwareManager::playTone(int frequency, int duration) {
  ledcAttachPin(buzzerPin, BUZZER_CHANNEL);  // Re-attach pin before playing
  ledcWriteTone(BUZZER_CHANNEL, frequency);
  buzzerActive = true;
  delay(duration);
  stopTone();
}

void HardwareManager::playShortBeep() {
  playTone(2000, BEEP_SHORT);
}

void HardwareManager::playAlertBeep() {
  // Play distinctive alert pattern
  playTone(2500, BEEP_MEDIUM);
  delay(100);
  playTone(2500, BEEP_MEDIUM);
}

void HardwareManager::stopTone() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  ledcDetachPin(buzzerPin);  // Detach pin to fully stop buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);  // Ensure pin is HIGH (OFF for active-low)
  buzzerActive = false;
}

void HardwareManager::setStatusLED(uint8_t r, uint8_t g, uint8_t b) {
  neopixel.clear();
  neopixel.setPixelColor(0, neopixel.Color(r, g, b));
  neopixel.show();
}

void HardwareManager::blinkStatusLED(uint8_t r, uint8_t g, uint8_t b, int times) {
  for (int i = 0; i < times; i++) {
    setStatusLED(r, g, b);
    delay(200);
    setStatusLED(0, 0, 0);
    delay(200);
  }
}

DateTime HardwareManager::getCurrentTime() {
  return rtc.now();
}

String HardwareManager::getTimeString() {
  DateTime now = rtc.now();
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  return String(buffer);
}

String HardwareManager::getDateString() {
  DateTime now = rtc.now();
  char buffer[11];
  sprintf(buffer, "%02d/%02d/%04d", now.day(), now.month(), now.year());
  return String(buffer);
}
