# Disaster Resource Management Mesh Network

A self-healing mesh network system for disaster resource management using ESP32 devices.

## Overview

This system creates a mesh network of ESP32 nodes that can request and broadcast resource needs during disasters. Each node can independently create alerts and receive alerts from other nodes in the network.

## Hardware Requirements

Per Node:
- ESP32 Development Board
- 16-Key I2C Keypad (Address: 0x3D)
- TFT Display (SPI interface)
- Potentiometer (connected to GPIO 36)
- Buzzer (connected to GPIO 25)
- NeoPixel LED (connected to GPIO 17)
- DS1307 RTC Module (I2C)

## Required Libraries

Install these libraries via Arduino Library Manager:

1. **painlessMesh** - For mesh networking
2. **ArduinoJson** - For JSON message parsing
3. **TFT_eSPI** - For TFT display
4. **I2CKeyPad** - For keypad input
5. **Adafruit_NeoPixel** - For status LED
6. **RTClib** - For real-time clock

## Key Mapping

### Keypad Layout
```
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

### Resource Keys (1-9)
- **1**: Food
- **2**: Water
- **3**: Medical Supplies
- **4**: Shelter
- **5**: Clothing
- **6**: Power/Electricity
- **7**: Transport
- **8**: Communication Equipment
- **9**: Rescue/Emergency Services

### Control Keys
- **A**: View all active alerts
- **B**: Clear/resolve an alert
- **C**: Refresh/sync with network
- **D**: System status and info
- **\***: Navigate previous / Cancel
- **0**: Select item
- **#**: Navigate next / Confirm

## Alert Creation Flow

1. Press resource key (1-9)
2. Adjust potentiometer to set severity (0-100%)
3. Press **#** to confirm or **\*** to cancel
4. Alert is broadcast to all nodes in mesh network

## Severity Levels

- **0-33%**: Low Priority (Green)
- **34-66%**: Medium Priority (Yellow)
- **67-100%**: High Priority (Red)

## Status LED (NeoPixel)

- **Blue**: Root node (gateway)
- **Green**: Connected to mesh network
- **Orange**: Searching for network

## Screens

### Home Screen
- Shows mesh status (Node ID, role, connected nodes)
- Displays active alert count
- Lists available resource types

### Alert Creation Screen
- Shows selected resource type
- Live severity adjustment
- Confirm/cancel options

### Alert List Screen
- Browse through all alerts
- Shows resource type, severity, timestamp, node ID
- Navigate with */# keys

### Alert Detail Screen
- Detailed information about selected alert
- Alert ID, node ID, timestamp, status

### Status Screen
- Current time and date
- Node ID and role
- Network statistics
- Alert counts

## Mesh Network

- Uses **painlessMesh** library
- Self-organizing and self-healing
- Automatic root node election
- Broadcasts alerts to all nodes
- Default SSID: `DisasterMesh`
- Default Password: `DisasterNet2024`

## File Structure

```
disaster_mesh/
├── disaster_mesh.ino      - Main program file
├── config.h               - Configuration and constants
├── alert_manager.h/cpp    - Alert handling and storage
├── hardware_manager.h/cpp - Hardware peripheral control
├── ui_manager.h/cpp       - Display and UI rendering
├── mesh_network.h/cpp     - Mesh network operations
└── README.md              - This file
```

## Compilation and Upload

1. Open `disaster_mesh.ino` in Arduino IDE
2. Select board: **ESP32 Dev Module**
3. Configure TFT_eSPI library for your display (User_Setup.h)
4. Click **Verify/Compile**
5. Click **Upload**

All related files will be automatically linked during compilation.

## Configuration

Edit `config.h` to customize:
- Mesh network credentials
- Pin assignments
- Alert limits
- UI colors
- Buzzer settings

## Troubleshooting

### Display not working
- Check TFT_eSPI User_Setup.h configuration
- Verify SPI connections

### Keypad not responding
- Check I2C address (should be 0x3D)
- Verify Wire library initialization

### Mesh not connecting
- Ensure all nodes use same MESH_PREFIX and MESH_PASSWORD
- Check WiFi region compatibility

### RTC time incorrect
- RTC will be set to compilation time on first upload
- Manually adjust in hardware_manager.cpp if needed

## Future Enhancements

- [ ] MQTT integration for dashboard
- [ ] GPS location tracking
- [ ] Battery level monitoring
- [ ] Alert priority queue
- [ ] Mesh topology visualization
- [ ] Alert acknowledgment system

## License

Open source project for disaster management and humanitarian purposes.

## Version

v1.0 - Initial Release
