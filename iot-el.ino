/*
 * Disaster Resource Management Mesh Network
 * Main File
 * 
 * Hardware:
 * - ESP32
 * - 16-key I2C Keypad (0x3D)
 * - TFT Display (SPI)
 * - Potentiometer (Pin 36)
 * - Buzzer (Pin 25)
 * - NeoPixel LED (Pin 17)
 * - RTC DS1307 (I2C)
 */

#include "config.h"
#include "mesh_network.h"
#include "ui_manager.h"
#include "alert_manager.h"
#include "hardware_manager.h"
#include "mqtt_cloud.h"

// Function declarations
void updateUITask();
void checkKeypadTask();
void updateStatusTask();
void handleKeyInput(char key);
void handleHomeScreenInput(char key);
void handleAlertCreateInput(char key);
void handleAlertListInput(char key);
void handleAlertDetailInput(char key);
void handleStatusScreenInput(char key);

// Global objects
painlessMesh mesh;
Scheduler taskScheduler;
UIManager uiManager;
AlertManager alertManager;
HardwareManager hardwareManager;
MQTTCloud mqttCloud;

// Tasks
Task taskUpdateUI(TASK_MILLISECOND * 100, TASK_FOREVER, &updateUITask);
Task taskCheckKeypad(TASK_MILLISECOND * 50, TASK_FOREVER, &checkKeypadTask);
Task taskUpdateStatus(TASK_SECOND * 2, TASK_FOREVER, &updateStatusTask);

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== Disaster Management Mesh Network ===");
  
  #if IS_ROOT_NODE == 1
    Serial.println(">>> ROOT NODE MODE <<<");
  #else
    Serial.println(">>> CHILD NODE MODE <<<");
  #endif
  
  // Initialize hardware
  hardwareManager.begin();
  Serial.println("[✓] Hardware initialized");
  
  // Initialize UI
  uiManager.begin();
  uiManager.showBootScreen();
  delay(2000);
  Serial.println("[✓] UI initialized");
  
  // Initialize alert manager
  alertManager.begin();
  Serial.println("[✓] Alert manager initialized");
  
  // Initialize mesh network
  initMesh();
  Serial.println("[✓] Mesh network initialized");
  
  // Initialize MQTT cloud (root node only)
  mqttCloud.begin();
  Serial.println("[✓] MQTT cloud initialized");
  
  // Add tasks
  taskScheduler.addTask(taskUpdateUI);
  taskScheduler.addTask(taskCheckKeypad);
  taskScheduler.addTask(taskUpdateStatus);
  
  taskUpdateUI.enable();
  taskCheckKeypad.enable();
  taskUpdateStatus.enable();
  
  Serial.println("[✓] System ready!\n");
  
  // Show home screen
  uiManager.setScreen(SCREEN_HOME);
}

void loop() {
  mesh.update();
  mqttCloud.loop();
  taskScheduler.execute();
}

// Task: Update UI
void updateUITask() {
  uiManager.update();
}

// Task: Check keypad input
void checkKeypadTask() {
  char key = hardwareManager.readKeypad();
  
  if (key != 'N') { // 'N' means no key pressed
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    // Play feedback beep
    hardwareManager.playShortBeep();
    
    // Handle key input based on current screen
    handleKeyInput(key);
  }
}

// Task: Update status information
void updateStatusTask() {
  // Use hardcoded root flag from config.h
  bool isRoot = (IS_ROOT_NODE == 1);
  
  // Update mesh status
  uint32_t nodeId = mesh.getNodeId();
  int nodeCount = mesh.getNodeList().size() + 1; // +1 for self
  
  uiManager.updateMeshStatus(nodeId, nodeCount, isRoot);
  
  // Update NeoPixel based on priority: alerts > mesh status
  int activeAlerts = alertManager.getActiveAlertCount();
  
  if (activeAlerts > 0) {
    // Get highest severity alert
    int highestSeverity = 0;
    for (int i = 0; i < alertManager.getAlertCount(); i++) {
      Alert alert = alertManager.getAlert(i);
      if (alert.status == ALERT_ACTIVE && alert.severity > highestSeverity) {
        highestSeverity = alert.severity;
      }
    }
    
    // Show alert severity color
    if (highestSeverity > 66) {
      hardwareManager.setStatusLED(255, 0, 0); // Red for high severity
    } else if (highestSeverity > 33) {
      hardwareManager.setStatusLED(255, 165, 0); // Orange for medium severity
    } else {
      hardwareManager.setStatusLED(255, 255, 0); // Yellow for low severity
    }
  } else {
    // No alerts - show mesh status
    if (isRoot) {
      hardwareManager.setStatusLED(0, 0, 255); // Blue for root
    } else if (nodeCount > 1) {
      hardwareManager.setStatusLED(0, 255, 0); // Green for connected
    } else {
      hardwareManager.setStatusLED(255, 165, 0); // Orange for searching
    }
  }
}

// Handle key input
void handleKeyInput(char key) {
  UIScreen currentScreen = uiManager.getCurrentScreen();
  
  switch (currentScreen) {
    case SCREEN_HOME:
      handleHomeScreenInput(key);
      break;
      
    case SCREEN_ALERT_CREATE:
      handleAlertCreateInput(key);
      break;
      
    case SCREEN_ALERT_LIST:
      handleAlertListInput(key);
      break;
      
    case SCREEN_ALERT_DETAIL:
      handleAlertDetailInput(key);
      break;
      
    case SCREEN_STATUS:
      handleStatusScreenInput(key);
      break;
      
    default:
      break;
  }
}

// Home screen input handler
void handleHomeScreenInput(char key) {
  if (key >= '1' && key <= '9') {
    // Resource key pressed - start alert creation
    int resourceType = key - '0'; // Convert char to int
    uiManager.startAlertCreation(resourceType);
    uiManager.setScreen(SCREEN_ALERT_CREATE);
    
  } else if (key == 'A') {
    // View alerts
    uiManager.setScreen(SCREEN_ALERT_LIST);
    
  } else if (key == 'B') {
    // Clear alert (will show in alert list)
    uiManager.setScreen(SCREEN_ALERT_LIST);
    
  } else if (key == 'C') {
    // Refresh/sync - request all alerts from network
    requestSync();
    Serial.println("Manual sync requested");
    
  } else if (key == 'D') {
    // Show status
    uiManager.setScreen(SCREEN_STATUS);
  }
}

// Alert creation screen input handler
void handleAlertCreateInput(char key) {
  if (key == '#') {
    // Confirm alert creation
    int severity = hardwareManager.readSeverity();
    Alert newAlert = uiManager.confirmAlertCreation(severity, mesh.getNodeId());
    
    // Add to local alert list
    alertManager.addAlert(newAlert);
    
    // Send to mesh network
    sendAlertToMesh(newAlert);
    
    // Publish to MQTT cloud (root node only)
    mqttCloud.publishAlert(newAlert);
    
    // Play confirmation
    hardwareManager.playShortBeep();
    delay(100);
    hardwareManager.playShortBeep();
    
    // Back to home
    uiManager.setScreen(SCREEN_HOME);
    
  } else if (key == '*') {
    // Cancel
    uiManager.setScreen(SCREEN_HOME);
  }
}

// Alert list screen input handler
void handleAlertListInput(char key) {
  if (key == '0') {
    // Select/view detail
    uiManager.setScreen(SCREEN_ALERT_DETAIL);
    
  } else if (key == '*') {
    // Previous alert
    uiManager.navigateAlerts(-1);
    
  } else if (key == '#') {
    // Next alert
    uiManager.navigateAlerts(1);
    
  } else if (key == 'B') {
    // Resolve current alert
    int currentIndex = uiManager.getCurrentAlertIndex();
    Alert* alertPtr = alertManager.getAlertPointer(currentIndex);
    
    if (alertPtr != nullptr && alertPtr->status == ALERT_ACTIVE) {
      uint32_t alertId = alertPtr->alertId;
      
      // Resolve locally
      alertManager.resolveAlert(currentIndex);
      
      // Broadcast resolve to mesh network
      broadcastResolve(alertId);
      
      // Publish resolve to MQTT cloud (root node only)
      mqttCloud.publishResolve(alertId);
      
      Serial.print("Alert resolved and broadcasted: ID ");
      Serial.println(alertId);
    }
    
    uiManager.setScreen(SCREEN_HOME);
    
  } else if (key == 'D') {
    // Back to home
    uiManager.setScreen(SCREEN_HOME);
  }
}

// Alert detail screen input handler
void handleAlertDetailInput(char key) {
  if (key == 'D' || key == '*') {
    // Back to list
    uiManager.setScreen(SCREEN_ALERT_LIST);
  }
}

// Status screen input handler
void handleStatusScreenInput(char key) {
  Serial.print("Status screen key: ");
  Serial.println(key);
  
  if (key == 'D' || key == '*') {
    // Back to home
    Serial.println("Going back to home from status");
    uiManager.setScreen(SCREEN_HOME);
  }
}
