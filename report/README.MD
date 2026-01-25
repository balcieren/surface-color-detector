# Circuit Documentation

Hardware schematic and wiring reference for the Surface Color Detector.

---

## Interactive Schematic

**[Open in Cirkit Designer →](https://app.cirkitdesigner.com/project/a10a9a18-1931-44ec-96a6-c74b36ce6794)**

![Circuit Diagram](./circuit_image.png)

---

## Bill of Materials

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 1 | ESP32 DevKit V1 | 38-pin, USB-C or Micro | Main controller |
| 1 | TCS3200 | Color sensor module | Includes LED array |
| 1 | OLED Display | 0.91" 128x32, SSD1306, I2C | 4-pin module |
| 1 | Push Button | Momentary, normally open | Any size works |
| 15 | Jumper Wires | Male-to-male | For breadboard |
| 1 | Breadboard | Half-size or larger | Optional for prototyping |

---

## Wiring

### TCS3200 Color Sensor → ESP32

```
TCS3200          ESP32
───────          ─────
  S0  ──────────  GPIO33   (frequency scaling)
  S1  ──────────  GPIO32   (frequency scaling)
  S2  ──────────  GPIO27   (photodiode select)
  S3  ──────────  GPIO25   (photodiode select)
  OUT ──────────  GPIO35   (frequency output)
  LED ──────────  GPIO26   (LED control)
  VCC ──────────  VIN      (5V)
  GND ──────────  GND
```

**Frequency scaling:** Firmware sets S0=HIGH, S1=LOW for 20% output scaling.

**GPIO35:** This is an input-only pin on ESP32, making it ideal for reading the sensor's frequency output.

---

### OLED Display → ESP32

```
OLED             ESP32
────             ─────
  SDA ──────────  GPIO21   (I2C data)
  SCL ──────────  GPIO22   (I2C clock)
  VCC ──────────  VIN      (5V)
  GND ──────────  GND
```

I2C address: `0x3C` (most common for these modules)

The module has built-in pull-up resistors on SDA/SCL.

---

### Button → ESP32

```
Button           ESP32
──────           ─────
  Sig ──────────  GPIO13
  GND ──────────  GND
```

No external resistor needed. Firmware enables internal pull-up on GPIO13.

**Wiring options:**
- 2-pin button: Connect one leg to GPIO13, other to GND
- 4-pin button: Use diagonal corners (they're internally connected)

---

## Power

Everything runs on 5V USB power.

| Component | Typical Current |
|-----------|-----------------|
| ESP32 | 80 mA |
| TCS3200 | 20 mA |
| OLED | 20 mA |
| **Total** | **~120 mA** |

Standard USB port provides 500 mA, so there's plenty of headroom.

---

## Physical Layout Suggestions

```
┌─────────────────────────────────────────┐
│              Breadboard                 │
│                                         │
│   ┌─────────┐                           │
│   │  ESP32  │                           │
│   │         │     ┌──────────┐          │
│   │         │     │  TCS3200 │  ← Point │
│   │         │     │  Sensor  │    down  │
│   │         │     └──────────┘          │
│   │         │                           │
│   └─────────┘     ┌──────┐   ┌──────┐   │
│                   │ OLED │   │Button│   │
│                   └──────┘   └──────┘   │
└─────────────────────────────────────────┘
```

- Mount TCS3200 facing downward for surface scanning
- Keep OLED visible from above
- Place button within easy reach

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| OLED stays black | Wrong I2C pins or address | Check SDA→21, SCL→22 |
| Garbled display | I2C interference | Shorter wires, add pull-ups |
| All colors read as black | Sensor LED off | Check GPIO26, verify HIGH |
| Colors are wrong | Calibration mismatch | Adjust map() values in firmware |
| Button does nothing | Wiring or pin | Check GPIO13, verify GND connection |
| ESP32 won't boot | Power issue | Use quality USB cable, try different port |

---

## References

- [TCS3200 Datasheet](https://www.mouser.com/datasheet/2/588/tcs3200-e11-1107677.pdf)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
