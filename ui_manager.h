/*
 * UI Manager
 * Handles all TFT display rendering and screen management
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "alert_manager.h"
#include "hardware_manager.h"

// Forward declarations
extern AlertManager alertManager;
extern HardwareManager hardwareManager;

class UIManager {
public:
  UIManager();
  
  void begin();
  void update();
  
  // Screen management
  void setScreen(UIScreen screen);
  UIScreen getCurrentScreen() const;
  
  // Screen rendering
  void showBootScreen();
  void showHomeScreen();
  void showAlertCreateScreen();
  void showAlertListScreen();
  void showAlertDetailScreen();
  void showStatusScreen();
  
  // Alert creation
  void startAlertCreation(int resourceType);
  Alert confirmAlertCreation(int severity, uint32_t nodeId);
  
  // Alert navigation
  void navigateAlerts(int direction);
  int getCurrentAlertIndex() const;
  
  // Status updates
  void updateMeshStatus(uint32_t nodeId, int nodeCount, bool isRoot);
  
  // Drawing utilities
  void drawRoundedBox(int x, int y, int w, int h, int r, uint16_t color, bool filled = false);
  void drawProgressBar(int x, int y, int w, int h, int value, uint16_t color);
  void drawSeverityBadge(int x, int y, uint8_t severity);
  void drawStatusIcon(int x, int y, bool status);
  
private:
  TFT_eSPI tft;
  UIScreen currentScreen;
  UIScreen previousScreen;
  
  // Mesh status
  uint32_t meshNodeId;
  int meshNodeCount;
  bool meshIsRoot;
  
  // Alert creation state
  int creatingResourceType;
  
  // Alert list state
  int currentAlertIndex;
  
  // Screen dirty flag
  bool screenDirty;
  unsigned long lastUpdate;
  
  // Helper methods
  void clearScreen();
  void drawHeader(const char* title);
  void drawFooter(const char* text);
  String getResourceName(int type);
  String formatTimestamp(uint32_t timestamp);
};

#endif // UI_MANAGER_H
