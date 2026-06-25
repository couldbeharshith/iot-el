/*
 * MQTT Cloud Implementation
 */

#include "mqtt_cloud.h"
#include "mesh_network.h"
#include <ArduinoJson.h>

extern painlessMesh mesh;

#if IS_ROOT_NODE == 1

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
  Serial.print("WiFi connecting to: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED!");
  }
}

void MQTTCloud::reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi down, skipping MQTT");
    return;
  }
  
  Serial.print("MQTT connecting to HiveMQ...");
  String clientId = "ESP32-" + String(mesh.getNodeId(), HEX);
  
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
