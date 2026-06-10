/*
 * Mesh Network Implementation
 */

#include "mesh_network.h"
#include "hardware_manager.h"
#include "ui_manager.h"

extern UIManager uiManager;

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

void receivedCallback(uint32_t from, String &msg) {
  Serial.print("Received message from: ");
  Serial.print(from);
  Serial.print(" - ");
  Serial.println(msg);
  
  // Parse the alert from JSON
  Alert receivedAlert = alertManager.jsonToAlert(msg);
  
  // Check if alert already exists (to prevent duplicates)
  bool exists = false;
  for (int i = 0; i < alertManager.getAlertCount(); i++) {
    Alert existing = alertManager.getAlert(i);
    if (existing.alertId == receivedAlert.alertId) {
      exists = true;
      break;
    }
  }
  
  if (!exists) {
    // Add to local alert list
    alertManager.addAlert(receivedAlert);
    
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
