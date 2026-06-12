/*
 * Mesh Network Manager
 * Handles painlessMesh initialization and message handling
 */

#ifndef MESH_NETWORK_H
#define MESH_NETWORK_H

#include <Arduino.h>
#include <painlessMesh.h>
#include "config.h"
#include "alert_manager.h"

// Forward declarations
extern painlessMesh mesh;
extern Scheduler taskScheduler;
extern AlertManager alertManager;
extern class HardwareManager hardwareManager;

// Cache root node status (updated every 2 seconds in updateStatusTask)
extern uint32_t cachedRootNodeId;
extern bool cachedIsRoot;

// Mesh initialization
void initMesh();

// Mesh callbacks
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

// Mesh operations
void sendAlertToMesh(const Alert& alert);
void broadcastAlert(const Alert& alert);
void broadcastResolve(uint32_t alertId);

// Root node detection
bool findRoot(const painlessmesh::protocol::NodeTree& tree, uint32_t& rootId);
void refreshRootInfo();

// Sync operations
void requestSync();
void broadcastSyncRequest();
void sendSyncResponse(uint32_t targetNode);

#endif // MESH_NETWORK_H
