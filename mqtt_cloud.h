/*
 * MQTT Cloud Manager (ROOT NODE ONLY)
 * Simple HiveMQ Cloud integration
 */

#ifndef MQTT_CLOUD_H
#define MQTT_CLOUD_H

#include <Arduino.h>
#include "config.h"
#include "alert_manager.h"

#if IS_ROOT_NODE == 1
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <PubSubClient.h>
#endif

class MQTTCloud {
public:
  MQTTCloud();
  void begin();
  void loop();
  void publishAlert(const Alert& alert);
  void publishResolve(uint32_t alertId);
  
private:
  #if IS_ROOT_NODE == 1
    WiFiClientSecure wifiClient;
    PubSubClient mqttClient;
    unsigned long lastReconnect;
    
    void connectWiFi();
    void reconnectMQTT();
  #endif
};

#endif // MQTT_CLOUD_H
