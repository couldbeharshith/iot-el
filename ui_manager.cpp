/*
 * UI Manager Implementation
 */

#include "ui_manager.h"

UIManager::UIManager() 
  : currentScreen(SCREEN_HOME),
    previousScreen(SCREEN_HOME),
    meshNodeId(0),
    meshNodeCount(0),
    meshIsRoot(false),
    creatingResourceType(0),
    currentAlertIndex(0),
    screenDirty(true),
    lastUpdate(0) {
}

void UIManager::begin() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(COLOR_BG);
  Serial.println("TFT Display initialized");
}

void UIManager::update() {
  unsigned long now = millis();
  
  // Different throttle times for different screens
  unsigned long throttleTime = 100;  // Default
  
  if (currentScreen == SCREEN_HOME) {
    throttleTime = 3000;  // 3 seconds for home screen
  } else if (currentScreen == SCREEN_ALERT_CREATE) {
    throttleTime = 500;  // 500ms for alert creation (live potentiometer)
  }
  
  // Throttle updates to avoid flickering
  if (now - lastUpdate < throttleTime) {
    return;
  }
  
  // Force refresh for screens that need live updates
  if (currentScreen == SCREEN_HOME || currentScreen == SCREEN_ALERT_CREATE) {
    screenDirty = true;
  }
  
  // Only redraw if screen changed or needs update
  if (screenDirty) {
    switch (currentScreen) {
      case SCREEN_HOME:
        showHomeScreen();
        break;
      case SCREEN_ALERT_CREATE:
        showAlertCreateScreen();
        break;
      case SCREEN_ALERT_LIST:
        showAlertListScreen();
        break;
      case SCREEN_ALERT_DETAIL:
        showAlertDetailScreen();
        break;
      case SCREEN_STATUS:
        showStatusScreen();
        break;
    }
    screenDirty = false;
  }
  
  lastUpdate = now;
}

void UIManager::setScreen(UIScreen screen) {
  if (currentScreen != screen) {
    previousScreen = currentScreen;
    currentScreen = screen;
    screenDirty = true;
    currentAlertIndex = 0; // Reset navigation
    Serial.print("Screen changed to: ");
    Serial.println(screen);
  }
}

UIScreen UIManager::getCurrentScreen() const {
  return currentScreen;
}

void UIManager::showBootScreen() {
  clearScreen();
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_PRIMARY);
  
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  
  // Title
  String title = "DISASTER";
  int titleWidth = title.length() * 12;
  tft.setCursor(centerX - titleWidth / 2, centerY - 30);
  tft.println(title);
  
  title = "MANAGEMENT";
  titleWidth = title.length() * 12;
  tft.setCursor(centerX - titleWidth / 2, centerY - 10);
  tft.println(title);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT_DIM);
  title = "Mesh Network v1.0";
  titleWidth = title.length() * 6;
  tft.setCursor(centerX - titleWidth / 2, centerY + 20);
  tft.println(title);
  
  // Draw animation dots
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(centerX - 12, centerY + 35);
  tft.print("...");
}

void UIManager::showHomeScreen() {
  clearScreen();
  
  // Draw header
  drawHeader("HOME");
  
  // Draw mesh status box
  drawRoundedBox(5, 25, tft.width() - 10, 35, 5, COLOR_PRIMARY, true);
  
  tft.setTextColor(COLOR_BG);
  tft.setTextSize(1);
  
  // Node ID
  tft.setCursor(10, 30);
  tft.print("Node: ");
  tft.print(meshNodeId, HEX);
  
  // Role
  tft.setCursor(10, 42);
  tft.print("Role: ");
  tft.print(meshIsRoot ? "ROOT" : "CHILD");
  
  // Connected nodes
  tft.setCursor(10, 54);
  tft.print("Nodes: ");
  tft.print(meshNodeCount);
  
  // Active alerts count
  int activeAlerts = alertManager.getActiveAlertCount();
  drawRoundedBox(tft.width() - 60, 30, 50, 25, 3, COLOR_DANGER, true);
  
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(tft.width() - 48, 35);
  tft.print(activeAlerts);
  
  tft.setTextSize(1);
  tft.setCursor(tft.width() - 58, 50);
  tft.setTextColor(COLOR_TEXT_DIM);
  tft.print("Alerts");
  
  // Resource buttons
  tft.setTextSize(1);
  int startY = 70;
  int spacing = 15;
  
  tft.setTextColor(COLOR_TEXT);
  for (int i = 1; i <= 9; i++) {
    tft.setCursor(10, startY + (i - 1) * spacing);
    tft.print(i);
    tft.print(": ");
    tft.setTextColor(COLOR_PRIMARY);
    tft.print(RESOURCE_NAMES[i]);
    tft.setTextColor(COLOR_TEXT);
  }
  
  // Footer with controls
  drawFooter("A:List B:Clear C:Sync D:Info");
  
  screenDirty = false;
}

void UIManager::showAlertCreateScreen() {
  clearScreen();
  
  // Header
  drawHeader("CREATE ALERT");
  
  // Resource type box
  drawRoundedBox(10, 30, tft.width() - 20, 30, 5, COLOR_SUCCESS, true);
  
  tft.setTextColor(COLOR_BG);
  tft.setTextSize(1);
  tft.setCursor(15, 35);
  tft.print("Resource:");
  
  tft.setTextSize(2);
  tft.setCursor(15, 47);
  tft.print(RESOURCE_NAMES[creatingResourceType]);
  
  // Severity label
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, 70);
  tft.print("Adjust Potentiometer for Severity:");
  
  // Live severity display
  int severity = hardwareManager.readSeverity();
  
  // Severity bar
  drawProgressBar(10, 85, tft.width() - 20, 25, severity, alertManager.getSeverityColor(severity));
  
  // Severity percentage
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(tft.width() / 2 - 20, 90);
  tft.print(severity);
  tft.print("%");
  
  // Severity level text
  tft.setTextSize(1);
  tft.setCursor(10, 115);
  uint8_t level = alertManager.getSeverityLevel(severity);
  tft.setTextColor(alertManager.getSeverityColor(severity));
  
  if (level == SEVERITY_LOW) {
    tft.print("Low Priority");
  } else if (level == SEVERITY_MEDIUM) {
    tft.print("Medium Priority");
  } else {
    tft.print("HIGH PRIORITY");
  }
  
  // Footer
  drawFooter("#:Confirm  *:Cancel");
}

void UIManager::showAlertListScreen() {
  clearScreen();
  
  drawHeader("ALERTS");
  
  int alertCount = alertManager.getAlertCount();
  
  if (alertCount == 0) {
    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setCursor(30, tft.height() / 2 - 10);
    tft.print("No alerts");
  } else {
    // Ensure index is valid
    if (currentAlertIndex >= alertCount) {
      currentAlertIndex = alertCount - 1;
    }
    
    Alert alert = alertManager.getAlert(currentAlertIndex);
    
    // Alert counter
    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setCursor(10, 25);
    tft.print("Alert ");
    tft.print(currentAlertIndex + 1);
    tft.print(" of ");
    tft.print(alertCount);
    
    // Alert card
    int cardY = 40;
    drawRoundedBox(10, cardY, tft.width() - 20, 80, 8, COLOR_PRIMARY, false);
    
    // Resource type
    tft.setTextSize(2);
    tft.setTextColor(COLOR_PRIMARY);
    tft.setCursor(20, cardY + 8);
    tft.print(RESOURCE_NAMES[alert.resourceType]);
    
    // Severity badge
    drawSeverityBadge(tft.width() - 60, cardY + 5, alert.severity);
    
    // Node ID
    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(20, cardY + 32);
    tft.print("From Node: ");
    tft.print(alert.nodeId, HEX);
    
    // Time
    tft.setCursor(20, cardY + 45);
    tft.print("Time: ");
    tft.print(formatTimestamp(alert.timestamp));
    
    // Status
    tft.setCursor(20, cardY + 58);
    tft.print("Status: ");
    if (alert.status == ALERT_ACTIVE) {
      tft.setTextColor(COLOR_DANGER);
      tft.print("ACTIVE");
    } else {
      tft.setTextColor(COLOR_SUCCESS);
      tft.print("RESOLVED");
    }
  }
  
  // Footer
  drawFooter("*:Prev #:Next 0:View B:Resolve");
  
  screenDirty = false;
}

void UIManager::showAlertDetailScreen() {
  clearScreen();
  
  drawHeader("ALERT DETAIL");
  
  Alert alert = alertManager.getAlert(currentAlertIndex);
  
  int y = 30;
  int lineHeight = 18;
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  
  // Resource
  tft.setCursor(10, y);
  tft.print("Resource:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(RESOURCE_NAMES[alert.resourceType]);
  y += lineHeight;
  
  // Severity
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Severity:");
  drawSeverityBadge(100, y - 2, alert.severity);
  y += lineHeight + 5;
  
  // Node ID
  tft.setCursor(10, y);
  tft.print("Node ID:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(alert.nodeId, HEX);
  y += lineHeight;
  
  // Alert ID
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Alert ID:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(alert.alertId, HEX);
  y += lineHeight;
  
  // Time
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Time:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(formatTimestamp(alert.timestamp));
  y += lineHeight;
  
  // Status
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Status:");
  tft.setCursor(100, y);
  if (alert.status == ALERT_ACTIVE) {
    tft.setTextColor(COLOR_DANGER);
    tft.print("ACTIVE");
  } else {
    tft.setTextColor(COLOR_SUCCESS);
    tft.print("RESOLVED");
  }
  
  drawFooter("*:Back D:Home");
  
  screenDirty = false;
}

void UIManager::showStatusScreen() {
  clearScreen();
  
  drawHeader("SYSTEM STATUS");
  
  int y = 30;
  int lineHeight = 18;
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  
  // Current time
  tft.setCursor(10, y);
  tft.print("Time:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(hardwareManager.getTimeString());
  y += lineHeight;
  
  // Date
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Date:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(hardwareManager.getDateString());
  y += lineHeight + 5;
  
  // Node ID
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Node ID:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(meshNodeId, HEX);
  y += lineHeight;
  
  // Role
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Role:");
  tft.setTextColor(meshIsRoot ? COLOR_INFO : COLOR_SUCCESS);
  tft.setCursor(100, y);
  tft.print(meshIsRoot ? "ROOT NODE" : "CHILD NODE");
  y += lineHeight;
  
  // Connected nodes
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Network:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(100, y);
  tft.print(meshNodeCount);
  tft.print(" nodes");
  y += lineHeight;
  
  // Active alerts
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Active:");
  tft.setTextColor(COLOR_DANGER);
  tft.setCursor(100, y);
  tft.print(alertManager.getActiveAlertCount());
  tft.print(" alerts");
  y += lineHeight;
  
  // Total alerts
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, y);
  tft.print("Total:");
  tft.setTextColor(COLOR_TEXT_DIM);
  tft.setCursor(100, y);
  tft.print(alertManager.getAlertCount());
  tft.print(" alerts");
  
  drawFooter("*:Back D:Home");
  
  screenDirty = false;
}

void UIManager::startAlertCreation(int resourceType) {
  creatingResourceType = resourceType;
}

Alert UIManager::confirmAlertCreation(int severity, uint32_t nodeId) {
  Alert alert;
  alert.nodeId = nodeId;
  alert.resourceType = creatingResourceType;
  alert.severity = severity;
  alert.timestamp = millis() / 1000; // Simple timestamp
  alert.status = ALERT_ACTIVE;
  alert.alertId = random(100000);
  
  return alert;
}

void UIManager::navigateAlerts(int direction) {
  int alertCount = alertManager.getAlertCount();
  
  if (alertCount == 0) return;
  
  currentAlertIndex += direction;
  
  if (currentAlertIndex < 0) {
    currentAlertIndex = alertCount - 1;
  } else if (currentAlertIndex >= alertCount) {
    currentAlertIndex = 0;
  }
  
  screenDirty = true;
}

int UIManager::getCurrentAlertIndex() const {
  return currentAlertIndex;
}

void UIManager::updateMeshStatus(uint32_t nodeId, int nodeCount, bool isRoot) {
  if (meshNodeId != nodeId || meshNodeCount != nodeCount || meshIsRoot != isRoot) {
    meshNodeId = nodeId;
    meshNodeCount = nodeCount;
    meshIsRoot = isRoot;
    screenDirty = true;
  }
}

void UIManager::clearScreen() {
  tft.fillScreen(COLOR_BG);
}

void UIManager::drawHeader(const char* title) {
  tft.fillRect(0, 0, tft.width(), 20, COLOR_PRIMARY);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BG);
  tft.setCursor(5, 6);
  tft.print(title);
  
  // Time in header
  tft.setCursor(tft.width() - 50, 6);
  tft.print(hardwareManager.getTimeString());
}

void UIManager::drawFooter(const char* text) {
  int footerY = tft.height() - 12;
  tft.fillRect(0, footerY, tft.width(), 12, COLOR_BG);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT_DIM);
  tft.setCursor(3, footerY + 2);
  tft.print(text);
}

void UIManager::drawRoundedBox(int x, int y, int w, int h, int r, uint16_t color, bool filled) {
  if (filled) {
    tft.fillRoundRect(x, y, w, h, r, color);
  } else {
    tft.drawRoundRect(x, y, w, h, r, color);
  }
}

void UIManager::drawProgressBar(int x, int y, int w, int h, int value, uint16_t color) {
  // Background
  tft.fillRoundRect(x, y, w, h, 3, COLOR_TEXT_DIM);
  
  // Foreground
  int fillWidth = map(value, 0, 100, 0, w);
  tft.fillRoundRect(x, y, fillWidth, h, 3, color);
}

void UIManager::drawSeverityBadge(int x, int y, uint8_t severity) {
  uint16_t color = alertManager.getSeverityColor(severity);
  
  drawRoundedBox(x, y, 45, 18, 3, color, true);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BG);
  tft.setCursor(x + 5, y + 5);
  tft.print(severity);
  tft.print("%");
}

void UIManager::drawStatusIcon(int x, int y, bool status) {
  uint16_t color = status ? COLOR_SUCCESS : COLOR_DANGER;
  tft.fillCircle(x, y, 4, color);
}

String UIManager::getResourceName(int type) {
  if (type >= 0 && type <= 9) {
    return String(RESOURCE_NAMES[type]);
  }
  return "Unknown";
}

String UIManager::formatTimestamp(uint32_t timestamp) {
  // Simple timestamp formatting (seconds since boot)
  uint32_t hours = timestamp / 3600;
  uint32_t minutes = (timestamp % 3600) / 60;
  uint32_t seconds = timestamp % 60;
  
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}
