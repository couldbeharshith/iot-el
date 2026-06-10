# System Architecture

## Overview

This document describes the software architecture of the Disaster Resource Management Mesh Network system.

## System Components

```
┌─────────────────────────────────────────────────┐
│           disaster_mesh.ino (Main)              │
│  - System initialization                        │
│  - Task scheduling                              │
│  - Input routing                                │
└────────────┬────────────────────────────────────┘
             │
    ┌────────┴─────────┬──────────┬──────────┐
    │                  │          │          │
┌───▼───┐      ┌──────▼──┐   ┌───▼───┐  ┌──▼───┐
│ Mesh  │      │   UI    │   │ Alert │  │ HW   │
│Network│      │ Manager │   │Manager│  │Manager│
└───────┘      └─────────┘   └───────┘  └──────┘
```

## Module Breakdown

### 1. Main Controller (`disaster_mesh.ino`)

**Responsibilities:**
- System initialization sequence
- Task scheduling and coordination
- User input routing to appropriate handlers
- Integration point for all modules

**Key Functions:**
- `setup()` - Initialize all subsystems
- `loop()` - Run mesh update and task scheduler
- `handleKeyInput()` - Route keypresses to screen handlers
- Task callbacks for UI, keypad, status updates

**Dependencies:**
- All other modules

---

### 2. Configuration (`config.h`)

**Responsibilities:**
- Central configuration and constants
- Pin definitions
- Network settings
- UI color schemes
- Resource type definitions

**Key Constants:**
- `MESH_PREFIX`, `MESH_PASSWORD` - Network credentials
- `PIN_*` - Hardware pin assignments
- `RESOURCE_NAMES[]` - Resource type strings
- `COLOR_*` - UI color palette
- Enum definitions for screens and statuses

**Dependencies:**
- None (pure definitions)

---

### 3. Alert Manager (`alert_manager.h/cpp`)

**Responsibilities:**
- Alert data structure and storage
- Alert lifecycle management (create, resolve, clear)
- JSON serialization/deserialization
- Severity level calculation
- Alert color coding

**Key Components:**

**Alert Structure:**
```cpp
struct Alert {
  uint32_t nodeId;        // Origin node
  uint8_t resourceType;   // 1-9 resource type
  uint8_t severity;       // 0-100 percentage
  uint32_t timestamp;     // Creation time
  AlertStatus status;     // Active/Resolved
  uint32_t alertId;       // Unique identifier
};
```

**Key Methods:**
- `addAlert()` - Add new alert to storage
- `resolveAlert()` - Mark alert as resolved
- `getAlert()` - Retrieve alert by index
- `alertToJson()` - Serialize for mesh transmission
- `jsonToAlert()` - Deserialize received alerts
- `getSeverityColor()` - Get color for severity level

**Storage:**
- Fixed-size array: `MAX_ALERTS` (20 by default)
- Automatic cleanup of resolved alerts when full

**Dependencies:**
- ArduinoJson for serialization
- config.h for constants

---

### 4. Hardware Manager (`hardware_manager.h/cpp`)

**Responsibilities:**
- Hardware peripheral abstraction
- Keypad input handling
- Buzzer control
- NeoPixel LED control
- Potentiometer reading
- RTC time management

**Managed Peripherals:**

**Keypad (I2C):**
- 4x4 matrix keypad
- Returns character for each key
- 'N' = No key, 'F' = Fail

**Buzzer:**
- Tone generation using LEDC
- Predefined beep patterns (short, alert)
- Non-blocking playback

**NeoPixel:**
- Single RGB LED status indicator
- Configurable brightness
- Solid color and blink patterns

**Potentiometer:**
- Analog input (0-4095)
- Mapped to severity (0-100)

**RTC (DS1307):**
- Real-time clock
- Time/date formatting
- Auto-set on first boot

**Key Methods:**
- `readKeypad()` - Get current key press
- `readSeverity()` - Get potentiometer value
- `playShortBeep()` - Feedback sound
- `playAlertBeep()` - Alert notification sound
- `setStatusLED()` - Update LED color
- `getCurrentTime()` - Get RTC timestamp

**Dependencies:**
- I2CKeyPad library
- Adafruit_NeoPixel library
- RTClib library
- config.h

---

### 5. UI Manager (`ui_manager.h/cpp`)

**Responsibilities:**
- TFT display rendering
- Screen state management
- UI component drawing
- Visual feedback
- Screen navigation

**Screen Types:**
1. **SCREEN_HOME** - Main dashboard
2. **SCREEN_ALERT_CREATE** - Alert creation flow
3. **SCREEN_ALERT_LIST** - Browse alerts
4. **SCREEN_ALERT_DETAIL** - Alert details
5. **SCREEN_STATUS** - System information

**UI Components:**
- Rounded boxes (modern look)
- Progress bars (severity display)
- Color-coded badges
- Headers and footers
- Live-updating displays

**Key Methods:**
- `setScreen()` - Switch between screens
- `update()` - Refresh current screen
- `showXScreen()` - Render specific screen
- `drawRoundedBox()` - UI primitive
- `drawProgressBar()` - Severity visualization
- `updateMeshStatus()` - Update network info

**Update Strategy:**
- Dirty flag system (only redraw on changes)
- Throttled updates (100ms minimum)
- Live updates for alert creation screen

**Dependencies:**
- TFT_eSPI library
- alert_manager (for alert data)
- hardware_manager (for time, potentiometer)
- config.h (for colors, constants)

---

### 6. Mesh Network (`mesh_network.h/cpp`)

**Responsibilities:**
- painlessMesh initialization
- Network event handling
- Message transmission
- Message reception
- Alert broadcasting

**Network Architecture:**
```
┌──────────┐     ┌──────────┐     ┌──────────┐
│  Node A  │────▶│  Root    │◀────│  Node B  │
│ (Child)  │     │  Node    │     │ (Child)  │
└──────────┘     └────┬─────┘     └──────────┘
                      │
                      ▼
                 ┌─────────┐
                 │Dashboard│
                 │(Future) │
                 └─────────┘
```

**Mesh Events:**
- `receivedCallback()` - Handle incoming alerts
- `newConnectionCallback()` - New node joined
- `changedConnectionCallback()` - Topology changed
- `nodeTimeAdjustedCallback()` - Time synchronization

**Message Flow:**

**Sending Alert:**
1. User creates alert locally
2. Alert added to local storage
3. `sendAlertToMesh()` serializes to JSON
4. `mesh.sendBroadcast()` transmits to all nodes
5. All nodes receive via `receivedCallback()`

**Receiving Alert:**
1. `receivedCallback()` triggered
2. JSON deserialized to Alert struct
3. Duplicate check (by alertId)
4. Add to local storage if new
5. Play alert beep
6. Blink LED notification

**Self-Healing:**
- Automatic route reconfiguration
- Root node re-election
- No manual intervention required

**Dependencies:**
- painlessMesh library
- alert_manager (for JSON conversion)
- hardware_manager (for beep/LED)
- config.h (for network credentials)

---

## Data Flow

### Alert Creation Flow

```
User Input (Keypad)
       │
       ▼
hardwareManager.readKeypad()
       │
       ▼
handleHomeScreenInput() [Resource key]
       │
       ▼
uiManager.setScreen(ALERT_CREATE)
       │
       ▼
User adjusts potentiometer
       │
       ▼
hardwareManager.readSeverity()
       │
       ▼
uiManager.showAlertCreateScreen() [Live update]
       │
       ▼
User presses # (confirm)
       │
       ▼
uiManager.confirmAlertCreation()
       │
       ▼
alertManager.addAlert()
       │
       ▼
sendAlertToMesh()
       │
       ▼
mesh.sendBroadcast()
       │
       ▼
All nodes receive alert
```

### Alert Reception Flow

```
mesh.sendBroadcast() [Remote node]
       │
       ▼
receivedCallback()
       │
       ▼
alertManager.jsonToAlert()
       │
       ▼
Duplicate check
       │
       ├─[Duplicate]──▶ Ignore
       │
       └─[New]──────┬─▶ alertManager.addAlert()
                    │
                    ├─▶ hardwareManager.playAlertBeep()
                    │
                    └─▶ hardwareManager.blinkStatusLED()
```

## Task Scheduling

Uses painlessMesh's built-in Scheduler:

**Tasks:**
1. `taskUpdateUI` - 100ms interval
   - Refresh current screen if dirty
   - Handle live updates (severity bar)

2. `taskCheckKeypad` - 50ms interval
   - Poll keypad for input
   - Debounce built into I2CKeyPad library

3. `taskUpdateStatus` - 2s interval
   - Query mesh network status
   - Update node count, role
   - Update status LED color

**Execution:**
```cpp
void loop() {
  mesh.update();           // Handle mesh communication
  taskScheduler.execute(); // Run scheduled tasks
}
```

## Memory Management

**Global Objects:**
- `mesh` - painlessMesh instance
- `taskScheduler` - Task scheduler
- `uiManager` - UI controller
- `alertManager` - Alert storage
- `hardwareManager` - Hardware interface

**Alert Storage:**
- Fixed array: 20 alerts maximum
- Each alert: ~20 bytes
- Total: ~400 bytes

**Screen Buffers:**
- Handled by TFT_eSPI library
- No double buffering (direct rendering)

## Error Handling

**Hardware Initialization:**
- Serial output for diagnostics
- Graceful degradation if peripherals missing
- Boot screen shows initialization progress

**Mesh Network:**
- Automatic reconnection
- Message retry (built into painlessMesh)
- No crashes on disconnection

**Alert Storage:**
- Bounds checking on array access
- Auto-clear resolved alerts when full
- Return empty alert on invalid index

## Future Enhancements

**Planned Features:**
1. MQTT gateway for root node
2. GPS location tagging
3. Alert acknowledgment system
4. Battery monitoring
5. Alert priority queue
6. Mesh topology visualization
7. OTA firmware updates

**Expansion Points:**
- Add `mqtt_manager.h/cpp` for dashboard
- Add `gps_manager.h/cpp` for location
- Extend Alert structure for more fields
- Add persistence (SPIFFS/SD card)

## Performance Characteristics

**Response Times:**
- Keypress feedback: <50ms
- Alert creation: <100ms
- Alert propagation: 1-3s (depends on mesh size)
- Screen updates: 100ms throttled

**Network Capacity:**
- Up to 100 nodes (painlessMesh limit)
- Recommended: 10-20 nodes per mesh
- Range: 50-100m indoor, 200-300m outdoor

**Power Consumption:**
- Active (with display): ~150-200mA
- Could add sleep mode for battery operation

## Code Organization Benefits

**Modularity:**
- Each manager is independent
- Clear separation of concerns
- Easy to test individual components

**Maintainability:**
- Logical file structure
- Clear naming conventions
- Comprehensive comments

**Extensibility:**
- Add new managers without changing existing code
- New alert types: edit config.h only
- New screens: add to ui_manager

**Debugging:**
- Serial output throughout
- Each module logs its operations
- Easy to isolate issues

## Compilation

Arduino IDE automatically compiles all `.cpp` files in the sketch folder:

```
disaster_mesh/
├── disaster_mesh.ino      ← Open this file
├── *.h                    ← Automatically included
└── *.cpp                  ← Automatically compiled
```

**Build Process:**
1. Preprocessor includes all `.h` files
2. Compiler compiles each `.cpp` file
3. Linker combines all object files
4. Binary uploaded to ESP32

No manual configuration needed!
