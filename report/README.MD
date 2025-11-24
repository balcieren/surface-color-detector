# Circuit Documentation

## Circuit Schema
View the complete circuit design: [CIRCUIT SCHEMA](https://app.cirkitdesigner.com/project/a10a9a18-1931-44ec-96a6-c74b36ce6794)

![Circuit Diagram](./circuit_image.png)

## Component Wiring Guide

### 0.91" OLED Display
| OLED Pin | ESP32 Pin |
|----------|-----------|
| SDA      | D21       |
| SCK      | D22       |
| VCC      | VIN       |
| GND      | GND       |

### TCS3200 Color Sensor
| TCS3200 Pin | ESP32 Pin |
|-------------|-----------|
| S0          | D25       |
| S1          | D26       |
| S2          | D27       |
| S3          | D14       |
| OUT         | D33       |
| LED         | D32       |
| VCC         | VIN       |
| GND         | GND       |

### Push Button
| Button Pin | ESP32 Pin |
|------------|-----------|
| Signal     | D13       |
| Ground     | GND       |
