# Quick Start Guide

Get your Disaster Mesh Network up and running in 10 minutes!

## 📋 Prerequisites Checklist

- [ ] ESP32 Development Board
- [ ] 16-Key I2C Keypad
- [ ] TFT Display
- [ ] Potentiometer
- [ ] Buzzer
- [ ] NeoPixel LED
- [ ] DS1307 RTC Module
- [ ] Arduino IDE installed
- [ ] USB cable for ESP32

## 🔌 Hardware Connections

### I2C Devices (SDA/SCL)
```
ESP32        Device
GPIO21 (SDA) → Keypad SDA
GPIO22 (SCL) → Keypad SCL
GPIO21 (SDA) → RTC SDA
GPIO22 (SCL) → RTC SCL
```

### SPI Display (depends on your display)
```
ESP32        TFT Display
GPIO23       → MOSI/SDA
GPIO18       → SCK/SCL
GPIO5        → CS
GPIO16       → DC
GPIO23       → RST
```
*Note: Pin numbers may vary based on your display. Check TFT_eSPI configuration.*

### Other Peripherals
```
ESP32        Device
GPIO36       → Potentiometer (analog)
GPIO25       → Buzzer (signal)
GPIO17       → NeoPixel (data)
GND          → All GND connections
3.3V/5V      → Power (check device requirements)
```

## 📚 Step 1: Install Libraries

Follow the detailed guide in `LIBRARIES_INSTALL.md`.

Quick install via Arduino Library Manager:
1. painlessMesh
2. ArduinoJson (version 6.x)
3. TFT_eSPI
4. I2CKeyPad
5. Adafruit_NeoPixel
6. RTClib

**⚠️ IMPORTANT**: Configure TFT_eSPI's `User_Setup.h` for your display!

## 🔧 Step 2: Configure TFT_eSPI

1. Find TFT_eSPI library folder
2. Open `User_Setup.h`
3. Set your display driver (ST7789, ILI9341, etc.)
4. Set correct pin numbers
5. Save and close

## ⚙️ Step 3: Customize Settings (Optional)

Edit `config.h` to change:
```cpp
// Mesh network credentials
#define MESH_PREFIX     "DisasterMesh"
#define MESH_PASSWORD   "DisasterNet2024"

// Pin assignments (if different)
#define PIN_BUZZER      25
#define PIN_NEOPIXEL    17
#define PIN_POTENTIOMETER 36
```

## 📤 Step 4: Upload Code

1. Open `disaster_mesh.ino` in Arduino IDE
2. Select board: **Tools → Board → ESP32 Dev Module**
3. Select port: **Tools → Port → [Your ESP32 Port]**
4. Click **Upload** button (→)
5. Wait for upload to complete

## 🖥️ Step 5: Monitor Serial Output

1. Open Serial Monitor: **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. You should see:
```
=== Disaster Management Mesh Network ===
[✓] Hardware initialized
[✓] UI initialized
[✓] Alert manager initialized
[✓] Mesh network initialized
[✓] System ready!
Node ID: XXXXXXXX
```

## ✅ Step 6: Test Your Node

### Test 1: Keypad
- Press any key on keypad
- Should beep and display action on screen

### Test 2: Create Alert
1. Press key **1** (Food)
2. Adjust potentiometer to set severity
3. Watch severity bar change in real-time
4. Press **#** to confirm
5. Alert should be created and added to list

### Test 3: View Alerts
1. Press key **A**
2. Should show alert list
3. Press **#** to navigate next
4. Press **\*** to navigate previous

### Test 4: Status Screen
1. Press key **D**
2. Should show node ID, network status, time

### Test 5: NeoPixel
- Should glow **Orange** when searching for network
- Should glow **Green** when connected to mesh
- Should glow **Blue** if this is the root node

## 🌐 Step 7: Add More Nodes

1. Upload same code to additional ESP32 boards
2. Power them on
3. They will automatically join the mesh network
4. Watch node count increase on status screen
5. Create alert on one node → all nodes receive it!

## 📱 Testing Mesh Broadcast

### With 2+ Nodes:

**Node 1:**
1. Press **2** (Water)
2. Adjust severity to 80%
3. Press **#** to confirm

**Node 2:**
- Should beep twice (alert received)
- LED blinks red
- Press **A** to see the new alert in list
- Shows alert from Node 1

**Success!** Your mesh network is working!

## 🎯 Basic Usage

### Creating an Alert
```
Home Screen → Press 1-9 (resource) → Adjust pot → Press # (confirm)
```

### Viewing Alerts
```
Home Screen → Press A → Use */# to navigate → Press 0 to view details
```

### Resolving Alerts
```
Home Screen → Press A → Navigate to alert → Press B (resolve)
```

### Checking Status
```
Home Screen → Press D → View system info → Press D (back)
```

## 🐛 Troubleshooting

### Display is blank
- Check TFT_eSPI configuration
- Verify display power and connections
- Try increasing SPI frequency in User_Setup.h

### Keypad not responding
- Check I2C address (should be 0x3D)
- Verify SDA/SCL connections
- Test with I2C scanner sketch

### No mesh connection
- Check all nodes use same MESH_PREFIX and MESH_PASSWORD
- Give nodes time to connect (30-60 seconds)
- Power cycle all nodes

### Buzzer not working
- Check GPIO25 connection
- Verify buzzer polarity (if applicable)
- Test with simple tone sketch

### RTC time wrong
- RTC is set to compilation time on first upload
- To set manually, edit hardware_manager.cpp
- Add battery to RTC for time persistence

### Compilation errors
- Install all required libraries
- Check ESP32 board support is installed
- Verify Arduino IDE version (1.8.x or 2.x)

## 📊 Expected Behavior

### Normal Operation:
- Boot screen appears for 2 seconds
- Home screen shows mesh status
- NeoPixel indicates connection status
- Keypresses produce feedback beep
- Alerts propagate across network within seconds

### Root Node:
- Blue LED
- "Role: ROOT" on home screen
- Handles all external communication

### Child Nodes:
- Green LED (when connected)
- Orange LED (when searching)
- "Role: CHILD" on home screen

## 🎉 Success Criteria

You're ready to deploy when:
- [ ] Display shows clear UI
- [ ] Keypad responds to all keys
- [ ] Potentiometer changes severity bar
- [ ] Alerts can be created
- [ ] Multiple nodes see each other
- [ ] Alerts broadcast to all nodes
- [ ] Buzzer beeps on alerts
- [ ] NeoPixel shows correct status

## 🚀 Next Steps

1. Deploy nodes to different locations
2. Test range and self-healing (disconnect/reconnect nodes)
3. Add more nodes to expand coverage
4. Integrate with dashboard (MQTT - coming soon)
5. Mount in weatherproof enclosures for outdoor use

## 📞 Need Help?

Common issues and solutions:
- Check all connections match the pinout
- Verify library versions
- Review Serial Monitor output for error messages
- Test each peripheral individually

Happy meshing! 🎊
