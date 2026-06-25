/*
 * Configuration File
 * Contains all constants, pin definitions, and settings
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== MESH NETWORK SETTINGS =====
#define MESH_PREFIX     "DisasterMesh"
#define MESH_PASSWORD   "DisasterNet2024"
#define MESH_PORT       5555

// ===== ROOT NODE CONFIGURATION =====
// Set to 1 to designate this node as ROOT (gateway), 0 for child node
#define IS_ROOT_NODE    0  // Change to 1 for root node

// ===== WIFI SETTINGS (ROOT NODE ONLY) =====
#define WIFI_SSID       "Batman_"
#define WIFI_PASSWORD   "123456789"

// ===== MQTT HIVEMQ CLOUD SETTINGS (ROOT NODE ONLY) =====
#define MQTT_BROKER     "a2d174684b10434db4fed867424bf8e6.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883
#define MQTT_USER       "rvceIOT"
#define MQTT_PASSWORD   "rvceIOT4"
#define MQTT_TOPIC      "disaster/alerts"

// ===== PIN DEFINITIONS =====
#define PIN_BUZZER      25
#define PIN_NEOPIXEL    17
#define PIN_POTENTIOMETER 36

// I2C Devices
#define KEYPAD_ADDRESS  0x3D

// ===== RESOURCE TYPES =====
#define RESOURCE_FOOD           1
#define RESOURCE_WATER          2
#define RESOURCE_MEDICAL        3
#define RESOURCE_SHELTER        4
#define RESOURCE_CLOTHING       5
#define RESOURCE_POWER          6
#define RESOURCE_TRANSPORT      7
#define RESOURCE_COMMUNICATION  8
#define RESOURCE_RESCUE         9

// Resource names
const char* const RESOURCE_NAMES[] = {
  "Unknown",
  "Food",
  "Water",
  "Medical",
  "Shelter",
  "Clothing",
  "Power",
  "Transport",
  "Communication",
  "Rescue"
};

// ===== ALERT SETTINGS =====
#define MAX_ALERTS      20
#define ALERT_TIMEOUT   3600000  // 1 hour in milliseconds

// Alert status
enum AlertStatus {
  ALERT_ACTIVE = 0,
  ALERT_RESOLVED = 1
};

// ===== SEVERITY LEVELS =====
#define SEVERITY_LOW    0
#define SEVERITY_MEDIUM 1
#define SEVERITY_HIGH   2

// Severity thresholds (0-100)
#define SEVERITY_LOW_MAX     33
#define SEVERITY_MEDIUM_MAX  66

// ===== UI SETTINGS =====
#define SCREEN_TIMEOUT  30000  // 30 seconds
#define HOME_REFRESH_INTERVAL 3000  // 3 seconds for home screen refresh
#define ALERT_CREATE_REFRESH_INTERVAL 500  // 0.5 seconds for alert creation (faster for pot)

// Screen types
enum UIScreen {
  SCREEN_HOME = 0,
  SCREEN_ALERT_CREATE = 1,
  SCREEN_ALERT_LIST = 2,
  SCREEN_ALERT_DETAIL = 3,
  SCREEN_STATUS = 4
};

// Colors (RGB565 format for TFT)
#define COLOR_BG        0x0000  // Black
#define COLOR_PRIMARY   0x05FF  // Cyan
#define COLOR_SECONDARY 0xFD20  // Orange
#define COLOR_TEXT      0xFFFF  // White
#define COLOR_TEXT_DIM  0x7BEF  // Gray
#define COLOR_SUCCESS   0x07E0  // Green
#define COLOR_WARNING   0xFFE0  // Yellow
#define COLOR_DANGER    0xF800  // Red
#define COLOR_INFO      0x001F  // Blue

// ===== BUZZER SETTINGS =====
#define BUZZER_CHANNEL  0
#define BUZZER_FREQ     2000
#define BUZZER_RESOLUTION 8

// Beep durations (milliseconds)
#define BEEP_SHORT      50
#define BEEP_MEDIUM     150
#define BEEP_LONG       300

// ===== DEBUG SETTINGS =====
#define DEBUG_MODE      true

#endif // CONFIG_H
