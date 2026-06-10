/*
 * Alert Manager Implementation
 */

#include "alert_manager.h"

AlertManager::AlertManager() : alertCount(0) {
}

void AlertManager::begin() {
  alertCount = 0;
  Serial.println("Alert Manager initialized");
}

bool AlertManager::addAlert(const Alert& alert) {
  if (alertCount >= MAX_ALERTS) {
    // Remove oldest resolved alert to make space
    clearResolvedAlerts();
    
    if (alertCount >= MAX_ALERTS) {
      Serial.println("Alert list full!");
      return false;
    }
  }
  
  alerts[alertCount] = alert;
  alertCount++;
  
  Serial.print("Alert added: ");
  Serial.print(RESOURCE_NAMES[alert.resourceType]);
  Serial.print(" (Severity: ");
  Serial.print(alert.severity);
  Serial.println("%)");
  
  return true;
}

bool AlertManager::resolveAlert(int index) {
  if (index < 0 || index >= alertCount) {
    return false;
  }
  
  alerts[index].status = ALERT_RESOLVED;
  
  Serial.print("Alert resolved: ");
  Serial.println(RESOURCE_NAMES[alerts[index].resourceType]);
  
  return true;
}

bool AlertManager::resolveAlertById(uint32_t alertId) {
  for (int i = 0; i < alertCount; i++) {
    if (alerts[i].alertId == alertId) {
      return resolveAlert(i);
    }
  }
  return false;
}

void AlertManager::clearResolvedAlerts() {
  // Move active alerts to front, removing resolved ones
  int writeIndex = 0;
  
  for (int readIndex = 0; readIndex < alertCount; readIndex++) {
    if (alerts[readIndex].status == ALERT_ACTIVE) {
      if (writeIndex != readIndex) {
        alerts[writeIndex] = alerts[readIndex];
      }
      writeIndex++;
    }
  }
  
  alertCount = writeIndex;
  Serial.println("Resolved alerts cleared");
}

int AlertManager::getAlertCount() const {
  return alertCount;
}

int AlertManager::getActiveAlertCount() const {
  int count = 0;
  for (int i = 0; i < alertCount; i++) {
    if (alerts[i].status == ALERT_ACTIVE) {
      count++;
    }
  }
  return count;
}

Alert AlertManager::getAlert(int index) const {
  if (index >= 0 && index < alertCount) {
    return alerts[index];
  }
  
  // Return empty alert if index invalid
  Alert empty;
  empty.alertId = 0;
  empty.nodeId = 0;
  empty.resourceType = 0;
  empty.severity = 0;
  empty.timestamp = 0;
  empty.status = ALERT_ACTIVE;
  return empty;
}

Alert* AlertManager::getAlertPointer(int index) {
  if (index >= 0 && index < alertCount) {
    return &alerts[index];
  }
  return nullptr;
}

String AlertManager::alertToJson(const Alert& alert) {
  StaticJsonDocument<256> doc;
  
  doc["id"] = alert.alertId;
  doc["node"] = alert.nodeId;
  doc["type"] = alert.resourceType;
  doc["severity"] = alert.severity;
  doc["time"] = alert.timestamp;
  doc["status"] = alert.status;
  
  String output;
  serializeJson(doc, output);
  return output;
}

Alert AlertManager::jsonToAlert(const String& json) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, json);
  
  Alert alert;
  alert.alertId = doc["id"];
  alert.nodeId = doc["node"];
  alert.resourceType = doc["type"];
  alert.severity = doc["severity"];
  alert.timestamp = doc["time"];
  alert.status = doc["status"];
  
  return alert;
}

uint8_t AlertManager::getSeverityLevel(uint8_t severity) const {
  if (severity <= SEVERITY_LOW_MAX) {
    return SEVERITY_LOW;
  } else if (severity <= SEVERITY_MEDIUM_MAX) {
    return SEVERITY_MEDIUM;
  } else {
    return SEVERITY_HIGH;
  }
}

uint16_t AlertManager::getSeverityColor(uint8_t severity) const {
  uint8_t level = getSeverityLevel(severity);
  
  switch (level) {
    case SEVERITY_LOW:
      return COLOR_SUCCESS;  // Green
    case SEVERITY_MEDIUM:
      return COLOR_WARNING;  // Yellow
    case SEVERITY_HIGH:
      return COLOR_DANGER;   // Red
    default:
      return COLOR_TEXT_DIM; // Gray
  }
}

uint32_t AlertManager::generateAlertId() {
  // Simple ID generation using timestamp + random
  return millis() + random(1000);
}
