# Library Installation Guide

Follow these steps to install all required libraries for the Disaster Mesh Network project.

## Method 1: Arduino Library Manager (Recommended)

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Install each of the following libraries:

### Required Libraries

#### 1. painlessMesh
- **Search for**: `painlessMesh`
- **Author**: Edwin van Leeuwen (use this one, NOT Alteriom version)
- **Version**: Latest stable version
- **Dependencies**: Will auto-install TaskScheduler, ESPAsyncTCP, AsyncTCP
- **⚠️ Note**: If you see multiple options, choose "painlessMesh" by Edwin van Leeuwen

#### 2. ArduinoJson
- **Search for**: `ArduinoJson`
- **Author**: Benoit Blanchon
- **Version**: 6.x.x (NOT version 7)
- **Note**: Use version 6, as it's more stable with ESP32

#### 3. TFT_eSPI
- **Search for**: `TFT_eSPI`
- **Author**: Bodmer
- **Version**: Latest stable version
- **⚠️ IMPORTANT**: After installation, configure User_Setup.h (see below)

#### 4. I2CKeyPad
- **Search for**: `I2CKeyPad`
- **Author**: Rob Tillaart
- **Version**: Latest stable version

#### 5. Adafruit NeoPixel
- **Search for**: `Adafruit NeoPixel`
- **Author**: Adafruit
- **Version**: Latest stable version

#### 6. RTClib
- **Search for**: `RTClib`
- **Author**: Adafruit
- **Version**: Latest stable version

## Method 2: Manual Installation

If Library Manager doesn't work:

1. Download each library from GitHub:
   - painlessMesh: https://github.com/gmag11/painlessMesh
   - ArduinoJson: https://github.com/bblanchon/ArduinoJson
   - TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
   - I2CKeyPad: https://github.com/RobTillaart/I2CKeyPad
   - Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
   - RTClib: https://github.com/adafruit/RTClib

2. Extract to Arduino libraries folder:
   - Windows: `Documents\Arduino\libraries\`
   - Mac: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

3. Restart Arduino IDE

## TFT_eSPI Configuration

**⚠️ CRITICAL STEP**: TFT_eSPI must be configured for your specific display.

### Option 1: Quick Setup (if you have a common display)

1. Navigate to TFT_eSPI library folder:
   - Windows: `Documents\Arduino\libraries\TFT_eSPI\`
   
2. Open `User_Setup_Select.h`

3. Comment out the default line:
   ```cpp
   //#include <User_Setup.h>
   ```

4. Uncomment the line matching your display, for example:
   ```cpp
   #include <User_Setups/Setup25_TTGO_T_Display.h>  // For TTGO T-Display
   ```

### Option 2: Custom Setup

1. Navigate to TFT_eSPI library folder
2. Open `User_Setup.h`
3. Configure for your display. Example for ST7789 240x135:

```cpp
#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 135

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000
```

4. Save and close

## Verification

To verify all libraries are installed:

1. Open Arduino IDE
2. Go to **Sketch → Include Library**
3. Check that all libraries appear in the list
4. Open `disaster_mesh.ino`
5. Click **Verify** button
6. If compilation succeeds, all libraries are correctly installed!

## Common Issues

### "painlessMesh.h: No such file or directory"
- Install painlessMesh from Library Manager
- Check library folder exists

### "ArduinoJson.h: No such file or directory"
- Install ArduinoJson version 6.x.x

### TFT Display shows garbage/blank screen
- Reconfigure TFT_eSPI User_Setup.h
- Check your display driver (ST7789, ILI9341, etc.)
- Verify pin connections

### Compilation errors with AsyncTCP
- Install latest ESP32 board support
- Tools → Board → Boards Manager → Search "ESP32" → Update

### I2CKeyPad not found
- Install from Library Manager
- Try manual installation from GitHub

## ESP32 Board Support

Don't forget to install ESP32 board support:

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Boards Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32"
6. Install "ESP32 by Espressif Systems"

## Board Selection

After installation:
- **Tools → Board → ESP32 Arduino → ESP32 Dev Module**

## Port Selection

- **Tools → Port → Select your ESP32 COM port**

## Upload Speed

If upload fails, try reducing speed:
- **Tools → Upload Speed → 115200**

## Ready to Upload!

Once all libraries are installed and configured:
1. Open `disaster_mesh.ino`
2. Select board and port
3. Click **Upload**
4. Monitor Serial output at 115200 baud

## Need Help?

If you encounter issues:
1. Check Serial Monitor for error messages
2. Verify all hardware connections
3. Double-check TFT_eSPI configuration
4. Ensure ESP32 board support is up to date
