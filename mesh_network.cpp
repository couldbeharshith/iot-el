/*
 * Mesh Network Implementation
 */

#include "mesh_network.h"
#include "hardware_manager.h"
#include "ui_manager.h"
#include "mqtt_cloud.h"
#include <ArduinoJson.h>

extern UIManager uiManager;
extern MQTTCloud mqttCloud;

// Cache root node ID
uint32_t cachedRootNodeId = 0;
bool cachedIsRoot = false;

void initMesh() {
  // Set debug output
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  
  // Initialize mesh
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &taskScheduler, MESH_PORT);
  
  // Set callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  
  Serial.println("Mesh network initialized");
  Serial.print("Node ID: ");
  Serial.println(mesh.getNodeId());
}

// Root node detection functions
bool findRoot(const painlessmesh::protocol::NodeTree& tree, uint32_t& rootId) {
  if (tree.root) {
    rootId = tree.nodeId;
    return true;
  }
  
  for (const auto& child : tree.subs) {
    if (findRoot(child, rootId)) {
      return true;
    }
  }
  
  return false;
}

void refreshRootInfo() {
  auto tree = mesh.asNodeTree();
  
  uint32_t rootId = 0;
  bool found = findRoot(tree, rootId);
  
  cachedRootNodeId = found ? rootId : 0;
  cachedIsRoot = found && (rootId == mesh.getNodeId());
  
  Serial.print("Root check: found=");
  Serial.print(found);
  Serial.print(", rootId=");
  Serial.print(cachedRootNodeId, HEX);
  Serial.print(", isRoot=");
  Serial.println(cachedIsRoot);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.print("Received message from: ");
  Serial.print(from);
  Serial.print(" - ");
  Serial.println(msg);
  
  // Parse JSON to check message type
  StaticJsonDocument<256> doc;
  deserializeJson(doc, msg);
  
  // Check message type
  if (doc.containsKey("type")) {
    String msgType = doc["type"].as<String>();
    
    // Handle sync request
    if (msgType == "sync_req") {
      Serial.println("Sync request received, sending all alerts");
      sendSyncResponse(from);
      return;
    }
    
    // Handle sync response
    if (msgType == "sync_resp") {
      Serial.println("Sync response received");
      hardwareManager.playShortBeep();
      return;
    }
  }
  
  // Check if it's a resolve message
  if (doc.containsKey("resolve")) {
    uint32_t alertId = doc["resolve"];
    Serial.print("Resolve message received for alert ID: ");
    Serial.println(alertId);
    
    // Find and resolve the alert locally
    bool resolved = alertManager.resolveAlertById(alertId);
    
    if (resolved) {
      Serial.println("Alert resolved successfully");
      hardwareManager.playShortBeep();
      
      // Publish the resolve to the MQTT cloud (root node only)
      mqttCloud.publishResolve(alertId);
    }
    return;
  }
  
  // Otherwise, it's an alert message
  Alert receivedAlert = alertManager.jsonToAlert(msg);
  
  // Check if alert already exists (to prevent duplicates)
  bool exists = false;
  for (int i = 0; i < alertManager.getAlertCount(); i++) {
    Alert existing = alertManager.getAlert(i);
    if (existing.alertId == receivedAlert.alertId) {
      exists = true;
      // Update the status if it changed
      if (existing.status != receivedAlert.status) {
        alertManager.resolveAlertById(receivedAlert.alertId);
        Serial.println("Alert status updated");
      }
      break;
    }
  }
  
  if (!exists) {
    // Add to local alert list
    alertManager.addAlert(receivedAlert);
    
    // Publish to MQTT cloud (root node forwards all alerts)
    mqttCloud.publishAlert(receivedAlert);
    
    // Play alert sound
    hardwareManager.playAlertBeep();
    
    // Blink LED
    hardwareManager.blinkStatusLED(255, 0, 0, 2);
    
    Serial.println("New alert received and added!");
  } else {
    Serial.println("Alert already exists, ignoring duplicate");
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.print("New connection: ");
  Serial.println(nodeId);
  
  // Play connection sound
  hardwareManager.playShortBeep();
}

void changedConnectionCallback() {
  Serial.println("Connections changed");
  Serial.print("Connected nodes: ");
  Serial.println(mesh.getNodeList().size());
  
  // Refresh root node info
  refreshRootInfo();
  
  if (cachedIsRoot) {
    Serial.println(">>> I am ROOT <<<");
  } else {
    Serial.print("Root node is: ");
    Serial.println(cachedRootNodeId, HEX);
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.print("Time adjusted by: ");
  Serial.print(offset);
  Serial.println(" us");
}

void sendAlertToMesh(const Alert& alert) {
  String json = alertManager.alertToJson(alert);
  
  // Broadcast to all nodes in the mesh
  mesh.sendBroadcast(json);
  
  Serial.println("Alert broadcast to mesh:");
  Serial.println(json);
}

void broadcastAlert(const Alert& alert) {
  sendAlertToMesh(alert);
}

void broadcastResolve(uint32_t alertId) {
  // Create a resolve message
  StaticJsonDocument<128> doc;
  doc["resolve"] = alertId;
  
  String json;
  serializeJson(doc, json);
  
  // Broadcast to all nodes
  mesh.sendBroadcast(json);
  
  Serial.print("Resolve broadcast to mesh for alert ID: ");
  Serial.println(alertId);
}

// Sync functions
void requestSync() {
  broadcastSyncRequest();
  
  Serial.println("Sync request sent to all nodes");
  hardwareManager.playShortBeep();
  hardwareManager.playShortBeep();
}

void broadcastSyncRequest() {
  StaticJsonDocument<64> doc;
  doc["type"] = "sync_req";
  doc["from"] = mesh.getNodeId();
  
  String json;
  serializeJson(doc, json);
  
  mesh.sendBroadcast(json);
  Serial.println("Sync request broadcast");
}

void sendSyncResponse(uint32_t targetNode) {
  // Send all active alerts to requesting node
  int alertCount = alertManager.getAlertCount();
  
  Serial.print("Sending ");
  Serial.print(alertCount);
  Serial.println(" alerts as sync response");
  
  for (int i = 0; i < alertCount; i++) {
    Alert alert = alertManager.getAlert(i);
    String json = alertManager.alertToJson(alert);
    
    // Send to specific node
    mesh.sendSingle(targetNode, json);
    delay(50); // Small delay between messages
  }
  
  // Send sync complete message
  StaticJsonDocument<64> doc;
  doc["type"] = "sync_resp";
  doc["count"] = alertCount;
  
  String json;
  serializeJson(doc, json);
  mesh.sendSingle(targetNode, json);
}
