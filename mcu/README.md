# MCU Firmware

ESP32 firmware for the Surface Color Detector. Reads color from a TCS3200 sensor, shows results on an OLED display, and broadcasts via Bluetooth Low Energy.

---

## Controls

| Action | Result |
|--------|--------|
| **Short press (< 2s)** | Take one sample, update running average on display (on release) |
| **Medium press (2s - 5s)** | Finalize reading, send via BLE, show result (on release) |
| **Long press (> 5s)** | Toggle LED on/off (power saving mode) (triggers immediately) |
| **Triple tap** | Reset all samples and start fresh |
| **Press after result** | Dismiss and continue |

### Power Saving Features
- **Auto LED off:** LED turns off after 2 minutes of inactivity
- **Wake on press:** Any button press wakes up the device from sleep mode

Minimum 3 samples required before finalizing. A progress bar appears while holding the button.

---

## Build & Flash

```bash
pio run --target upload
```

Monitor output (115200 baud):
```bash
pio device monitor -b 115200
```

Expected output on boot:
```
Starting...
BLE started: Surface Color Detector
Waiting for connections...
Controls:
  Short press: Add sample
  2s hold: Finalize and send
  5s hold: Toggle LED on/off
  Triple tap: Reset samples
Min samples: 3
Setup complete!
```

---

## Pinout

```
┌────────────────────────────────────────────────────────┐
│                        ESP32                           │
├────────────────────────────────────────────────────────┤
│                                                        │
│   TCS3200 Color Sensor          OLED Display           │
│   ────────────────────          ────────────           │
│   S0  ──────── GPIO27           SDA ──────── GPIO21    │
│   S1  ──────── GPIO25           SCL ──────── GPIO22    │
│   S2  ──────── GPIO32           VCC ──────── VIN       │
│   S3  ──────── GPIO33           GND ──────── GND       │
│   OUT ──────── GPIO35 (input)                          │
│   LED ──────── GPIO26                                  │
│   VCC ──────── VIN              Button                 │
│   GND ──────── GND              ──────                 │
│                                 SIG ──────── GPIO13    │
│                                 GND ──────── GND       │
└────────────────────────────────────────────────────────┘
```

**Notes:**
- GPIO35 is input-only on ESP32, ideal for frequency input
- Button uses internal pull-up resistor
- Sensor LED can be toggled via 5s button hold

---

## Source Files

```
src/
├── main.cpp                 # Entry point
├── sampling_controller.cpp  # State machine with button handling
├── color_sensor.cpp         # TCS3200 driver + color detection
├── color_sampler.cpp        # Accumulates samples, computes average
├── display.cpp              # OLED rendering
├── ble_service.cpp          # BLE server with notify
└── button.cpp               # Debounced input with tap/hold detection

include/
├── *.h                      # Headers for above
└── logo_pwr.h               # Splash screen bitmap
```

---

## Configuration Constants

All timing and calibration values are defined as public static constants:

```cpp
// Button timing (button.h)
Button::DEBOUNCE_DELAY       // 50ms
Button::TAP_TIMEOUT          // 400ms between taps
Button::SHORT_PRESS_MAX      // 500ms max for tap

// Sensor calibration (color_sensor.h)
ColorSensor::WHITE_RED_FREQ   // 26
ColorSensor::WHITE_GREEN_FREQ // 24
ColorSensor::WHITE_BLUE_FREQ  // 30
ColorSensor::BLACK_RED_FREQ   // 155
ColorSensor::BLACK_GREEN_FREQ // 166
ColorSensor::BLACK_BLUE_FREQ  // 197

// Controller timing (sampling_controller.h)
SamplingController::LONG_PRESS_DURATION   // 2000ms
SamplingController::LED_TOGGLE_DURATION   // 5000ms
SamplingController::AUTO_LED_OFF_TIMEOUT  // 120000ms (2 min)
SamplingController::MIN_SAMPLES_REQUIRED  // 3
```

Runtime adjustment:
```cpp
controller.setLongPressDuration(3000);
controller.setMinSamplesRequired(5);
```

---

## BLE Service

```
Device Name:      Surface Color Detector
Service UUID:     4fafc201-1fb5-459e-8fcc-c5c9c331914b
Characteristic:   beb5483e-36e1-4688-b7f5-ea07361b26a8

Properties:       READ | WRITE | NOTIFY | INDICATE
Data Format:      "R,G,B,ColorName"
Example:          "255,128,64,ORANGE"
```

iOS compatibility:
- TX power set to maximum (+9 dBm)
- Scan response enabled
- Min/max preferred intervals configured

---

## Color Detection Algorithm

The TCS3200 outputs a square wave whose frequency corresponds to light intensity for each color filter (red, green, blue).

**Frequency to RGB conversion:**
```cpp
// Lower frequency = more light = higher RGB value
color.red   = map(redFreq, WHITE_RED_FREQ, BLACK_RED_FREQ, 255, 0);
color.green = map(greenFreq, WHITE_GREEN_FREQ, BLACK_GREEN_FREQ, 255, 0);
color.blue  = map(blueFreq, WHITE_BLUE_FREQ, BLACK_BLUE_FREQ, 255, 0);
```

Calibration values defined as constants in `color_sensor.h`. Adjust for your lighting conditions.

**Debug mode:**
```cpp
#define DEBUG_SENSOR  // in color_sensor.cpp
```
Prints raw frequencies: `Freq R:45 G:52 B:61`

**Supported colors:**
- Primary: RED, GREEN, BLUE, YELLOW
- Secondary: ORANGE, CYAN, MAGENTA, PURPLE, PINK
- Neutral: BLACK, WHITE, DARK GRAY, GRAY, LIGHT GRAY, BROWN

See `detectColorName()` for exact thresholds.

---

## Dependencies

Installed automatically via PlatformIO:

| Library | Version | Purpose |
|---------|---------|---------|
| Adafruit SSD1306 | ^2.5.15 | OLED driver |
| Adafruit GFX | ^1.11.3 | Graphics primitives |
| Adafruit BusIO | ^1.14.1 | I2C abstraction |

---

## Troubleshooting

| Problem | Check |
|---------|-------|
| OLED blank | I2C wiring, address 0x3C |
| Colors wrong | Calibration constants in `color_sensor.h` |
| BLE not visible | Device name, UUID match |
| Button unresponsive | GPIO13 connection |
| No serial output | Baud rate **115200** |
| LED not toggling | Hold button for full 5 seconds |
| Samples not resetting | Tap 3 times quickly (<400ms between taps) |
