/*
 * Hardware Manager
 * Manages all hardware peripherals (keypad, buzzer, NeoPixel, potentiometer, RTC)
 */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "I2CKeyPad.h"
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include "config.h"

class HardwareManager {
public:
  HardwareManager();
  
  void begin();
  
  // Keypad
  char readKeypad();
  
  // Potentiometer
  int readSeverity();  // Returns 0-100
  
  // Buzzer
  void playTone(int frequency, int duration);
  void playShortBeep();
  void playAlertBeep();
  void stopTone();
  
  // NeoPixel
  void setStatusLED(uint8_t r, uint8_t g, uint8_t b);
  void blinkStatusLED(uint8_t r, uint8_t g, uint8_t b, int times = 3);
  
  // RTC
  DateTime getCurrentTime();
  String getTimeString();
  String getDateString();
  
private:
  I2CKeyPad keypad;
  Adafruit_NeoPixel neopixel;
  RTC_DS1307 rtc;
  
  int buzzerPin;
  bool buzzerActive;
  
  // Keypad key mapping
  const char keys[19] = "147*2580369#ABCDNF";
};

#endif // HARDWARE_MANAGER_H
