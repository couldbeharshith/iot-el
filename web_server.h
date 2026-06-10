/*
 * Web Server for Dashboard
 * Root node serves a web interface for viewing alerts
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "config.h"
#include "alert_manager.h"

// Forward declarations
extern AlertManager alertManager;
extern painlessMesh mesh;

class WebServerManager {
public:
  WebServerManager();
  
  void begin();
  void update();
  
  bool isRunning() const;
  String getIPAddress() const;
  
private:
  AsyncWebServer* server;
  bool serverRunning;
  
  // Request handlers
  void handleRoot(AsyncWebServerRequest *request);
  void handleAlerts(AsyncWebServerRequest *request);
  void handleStats(AsyncWebServerRequest *request);
  
  // HTML generation
  String generateHTML();
  String generateAlertsJSON();
  String generateStatsJSON();
};

#endif // WEB_SERVER_H
