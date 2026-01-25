# MCU Firmware

ESP32 firmware for the Surface Color Detector. Reads color from a TCS3200 sensor, shows results on an OLED display, and broadcasts via Bluetooth Low Energy.

---

## Controls

| Action | Result |
|--------|--------|
| Short press | Take one sample, update running average on display |
| Long press (2s) | Finalize reading, send via BLE, show result |
| Press after result | Reset and start fresh |

Minimum 3 samples required before finalizing. A progress bar appears while holding the button.

---

## Build & Flash

```bash
pio run --target upload
```

Monitor output (9600 baud):
```bash
pio device monitor
```

Expected output on boot:
```
Starting...
BLE started: Surface Color Detector
Waiting for connections...
Short press: Add sample | Long press (2s): Finalize
Minimum samples required: 3
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
│   S0  ──────── GPIO33           SDA ──────── GPIO21    │
│   S1  ──────── GPIO32           SCL ──────── GPIO22    │
│   S2  ──────── GPIO27           VCC ──────── VIN       │
│   S3  ──────── GPIO25           GND ──────── GND       │
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
- Sensor LED stays on continuously (can be toggled via GPIO26)

---

## Source Files

```
src/
├── main.cpp                 # Entry point
├── sampling_controller.cpp  # State machine: idle → sampling → finalizing
├── color_sensor.cpp         # TCS3200 driver + color name detection
├── color_sampler.cpp        # Accumulates samples, computes average
├── display.cpp              # OLED rendering
├── ble_service.cpp          # BLE server with notify
└── button.cpp               # Debounced input with duration tracking

include/
├── *.h                      # Headers for above
└── logo_pwr.h               # Splash screen bitmap
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
color.red   = map(redFreq,   26, 155, 255, 0);
color.green = map(greenFreq, 24, 166, 255, 0);
color.blue  = map(blueFreq,  30, 197, 255, 0);
```

These values were calibrated against white (low freq) and black (high freq) surfaces. Adjust for your lighting conditions.

**Debug mode:**
```cpp
#define DEBUG_SENSOR  // in color_sensor.cpp
```
Prints raw frequencies: `Freq R:45 G:52 B:61`

**Color naming:**
Threshold-based classification handles 15+ colors including:
- Primary: red, green, blue, yellow
- Secondary: orange, cyan, magenta, purple, pink
- Neutral: black, white, gray shades, brown

See `detectColorName()` for exact thresholds.

---

## Tuning Parameters

```cpp
// sampling_controller.cpp
longPressDuration(2000)    // Hold time to finalize (ms)
minSamplesRequired(3)      // Minimum samples before finalize allowed

// Runtime adjustment:
controller.setLongPressDuration(3000);
controller.setMinSamplesRequired(5);
```

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
| Colors wrong | Calibration values, lighting |
| BLE not visible | Device name, UUID match |
| Button unresponsive | GPIO13 connection |
| No serial output | Baud rate 9600 |
