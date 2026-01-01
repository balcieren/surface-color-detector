# Features & Working Logic

This document outlines the features, working logic, and code structure of the Surface Color Detector MCU project.

## Overview
The project is an ESP32-based portable color detector that uses a TCS3200 sensor to read surface colors. It displays the RGB values and detected color name on an OLED screen and transmits the data via Bluetooth Low Energy (BLE) to a connected client (e.g., mobile app).

## Hardware Components
- **Microcontroller:** ESP32
- **Color Sensor:** TCS3200 (RGB Light-to-Frequency Converter)
- **Display:** SSD1306 OLED (128x32, I2C)
- **Input:** Push Button (GPIO 13)
- **Connectivity:** Bluetooth Low Energy (BLE)

## Software Features

### 1. Color Detection
The system reads raw frequency data from the TCS3200 sensor for Red, Green, and Blue channels.
- **Mapping:** Frequencies are mapped to standard 0-255 RGB values using pre-calibrated minimum and maximum frequency thresholds.
- **Color Classification:** The system classifies the detected color into one of the following categories based on RGB thresholds:
  - RED
  - GREEN
  - BLUE
  - YELLOW
  - BLACK
  - WHITE
  - UNDEFINED

### 2. Sampling Mechanism (Averaging)
To ensure accuracy, the device uses a sampling method controlled by a physical button.
- **Active Sampling:** While the button is pressed, the device continuously reads color values and calculates a running average.
- **Real-time Feedback:** The OLED display updates in real-time with the current average RGB values and detected color name.
- **Noise Reduction:** This averaging process helps filter out sensor noise and slight variations in positioning.

### 3. BLE Communication
The device acts as a BLE Peripheral with the name **"Surface Color Detector"**.
- **Data Transmission:** Upon releasing the button (completing sampling), the final averaged color data is sent via BLE notification.
- **Data Format:** String format: `R,G,B,ColorName` (e.g., `255,0,0,RED`).
- **Connection Management:** Handles client connection and disconnection events.

### 4. OLED User Interface
- **Startup:** Displays a welcome message ("RGB Color Sensor TCS3200").
- **Operation:** Shows:
  - Red, Green, Blue integer values.
  - Detected Color Name (e.g., "RED").

## Working Logic

### Main Loop Workflow
1. **Idle/Read:** The sensor continuously reads color data in the background.
2. **Button Press (Start Sampling):**
   - When the button is held down, the system enters **Sampling Mode**.
   - Current color reading is added to the `ColorSampler`.
   - Display is updated with the new average.
3. **Button Release (Complete Sampling):**
   - The system calculates the final average of all samples collected during the press.
   - The final result is displayed on the OLED.
   - The result (`R,G,B,Name`) is transmitted via BLE.
   - The system waits for 3 seconds to allow the user to read the result.
   - The sampler is reset, ready for the next measurement.

### Code Structure (~/mcu/src)
- **`main.cpp`**: Orchestrates the main loop, handling button logic and coordinating between sensor, display, and BLE.
- **`color_sensor.cpp`**: Low-level driver for TCS3200. Handles frequency reading, mapping to RGB, and color name logic.
- **`display.cpp`**: Manages the SSD1306 OLED, dealing with initialization and rendering text.
- **`ble_service.cpp`**: Manages BLE server setup, advertising, and data notification.
- **`color_sampler.cpp`**: Logic for accumulating color readings and calculating averages.
- **`button.cpp`**: Simple abstraction for handling button input state.
