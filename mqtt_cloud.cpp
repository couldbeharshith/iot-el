/*
 * MQTT Cloud Implementation
 */

#include "mqtt_cloud.h"
#include <ArduinoJson.h>
#include <painlessMesh.h>

#if IS_ROOT_NODE == 1

extern painlessMesh mesh;

MQTTCloud::MQTTCloud() 
  : mqttClient(wifiClient), lastReconnect(0) {
}

void MQTTCloud::begin() {
  Serial.println("\n=== MQTT CLOUD INIT (ROOT NODE) ===");
  
  // Connect to WiFi
  connectWiFi();
  
  // Skip certificate validation (testing only)
  wifiClient.setInsecure();
  
  // Setup MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setBufferSize(512);
  
  Serial.println("MQTT configured");
}

void MQTTCloud::connectWiFi() {
  Serial.print("WiFi connecting in background to: ");
  Serial.println(WIFI_SSID);
  
  // Use painlessMesh's built-in station manual connection to avoid Wi-Fi channel conflicts
  mesh.stationManual(WIFI_SSID, WIFI_PASSWORD);
}

void MQTTCloud::reconnectMQTT() {
  static bool wifiWasConnected = false;
  
  if (WiFi.status() != WL_CONNECTED) {
    if (wifiWasConnected) {
      Serial.println("WiFi disconnected!");
      wifiWasConnected = false;
    }
    Serial.println("WiFi down, skipping MQTT");
    return;
  }
  
  if (!wifiWasConnected) {
    Serial.println("\n[✓] WiFi connected in background!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    wifiWasConnected = true;
  }
  
  Serial.print("MQTT connecting to HiveMQ...");
  String clientId = "DisasterMesh-" + String(random(0xFFFF), HEX);
  
  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("Connected!");
  } else {
    Serial.print("Failed, rc=");
    Serial.println(mqttClient.state());
  }
}

void MQTTCloud::loop() {
  unsigned long now = millis();
  
  // Reconnect if needed (every 5s)
  if (!mqttClient.connected()) {
    if (now - lastReconnect > 5000) {
      lastReconnect = now;
      reconnectMQTT();
    }
  } else {
    mqttClient.loop();
  }
}

void MQTTCloud::publishAlert(const Alert& alert) {
  if (!mqttClient.connected()) {
    Serial.println("MQTT not connected, alert not published");
    return;
  }
  
  StaticJsonDocument<256> doc;
  doc["alertId"] = alert.alertId;
  doc["nodeId"] = alert.nodeId;
  doc["resource"] = RESOURCE_NAMES[alert.resourceType];
  doc["severity"] = alert.severity;
  doc["timestamp"] = alert.timestamp;
  doc["status"] = (alert.status == ALERT_ACTIVE) ? "active" : "resolved";
  
  String json;
  serializeJson(doc, json);
  
  if (mqttClient.publish(MQTT_TOPIC, json.c_str())) {
    Serial.println("MQTT published: " + json);
  } else {
    Serial.println("MQTT publish FAILED");
  }
}

void MQTTCloud::publishResolve(uint32_t alertId) {
  if (!mqttClient.connected()) return;
  
  StaticJsonDocument<128> doc;
  doc["alertId"] = alertId;
  doc["status"] = "resolved";
  
  String json;
  serializeJson(doc, json);
  
  mqttClient.publish(MQTT_TOPIC, json.c_str());
  Serial.println("MQTT resolve: " + json);
}

#else

// Empty implementation for child nodes
MQTTCloud::MQTTCloud() {}
void MQTTCloud::begin() {
  Serial.println("MQTT disabled (child node)");
}
void MQTTCloud::loop() {}
void MQTTCloud::publishAlert(const Alert& alert) {}
void MQTTCloud::publishResolve(uint32_t alertId) {}

#endif
