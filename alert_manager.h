/*
 * Alert Manager
 * Handles alert storage, retrieval, and management
 */

#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"

// Alert structure
struct Alert {
  uint32_t nodeId;        // Node that created the alert
  uint8_t resourceType;   // 1-9
  uint8_t severity;       // 0-100
  uint32_t timestamp;     // Unix timestamp
  AlertStatus status;     // Active or resolved
  uint32_t alertId;       // Unique alert ID
};

class AlertManager {
public:
  AlertManager();
  
  void begin();
  
  // Alert operations
  bool addAlert(const Alert& alert);
  bool resolveAlert(int index);
  bool resolveAlertById(uint32_t alertId);
  void clearResolvedAlerts();
  
  // Getters
  int getAlertCount() const;
  int getActiveAlertCount() const;
  Alert getAlert(int index) const;
  Alert* getAlertPointer(int index);
  
  // JSON operations
  String alertToJson(const Alert& alert);
  Alert jsonToAlert(const String& json);
  
  // Utility
  uint8_t getSeverityLevel(uint8_t severity) const;
  uint16_t getSeverityColor(uint8_t severity) const;
  
private:
  Alert alerts[MAX_ALERTS];
  int alertCount;
  
  uint32_t generateAlertId();
};

#endif // ALERT_MANAGER_H
