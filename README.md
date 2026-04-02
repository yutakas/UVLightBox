# UVLightBox

A DIY UV light exposure box for alternative photography processes (cyanotype, Van Dyke brown, polymer photogravure). This project includes hardware designs and ESP32-based timer firmware.

## Safety Warnings

**UV Light Hazard:** This device uses 365nm UV-A LED strips. Prolonged exposure to UV-A light can cause eye damage (photokeratitis) and skin burns. Always avoid looking directly at the UV LEDs when powered on. Keep the lid closed during exposure. Consider UV-blocking safety glasses when working near the device.

**Electrical Safety:** This project involves 12V DC power supplies and relay-switched circuits. Ensure all wiring is secure and insulated. Do not operate with exposed connections. Disconnect power before making any wiring changes.

## Project Versions

### Version 1 (`UVLightBox/`)

A simple wooden UV exposure box (20" x 30" x 12") built with 8 x 2ft LED UV light bars mounted on a photo frame. See [UVLightBox/UVLightBox.md](UVLightBox/UVLightBox.md) for construction notes.

### Version 2 (`UVLightBox2/`)

An improved design with an ESP32-based digital timer, LCD display, and relay-controlled UV LED strips. Includes full/half brightness modes and EEPROM-saved settings.

- [Parts list](UVLightBox2/UVLightBox2.md)
- [Timer schematic](UVLightBox2/Timer/UVBoxTimer2_Schematic.png)
- [Firmware source](UVLightBox2/Timer/ArduinoSrc/)
- [Build photos](UVLightBox2/Images/)

## Hardware Components (Version 2)

| Component | Purpose |
|-----------|---------|
| ESP32 dev board | Microcontroller |
| LCD 1602 (I2C) | Display timer and mode |
| 2-channel relay module | Switch UV LED strips |
| 365nm UV LED strips (2835) | UV light source |
| 12V power supply | Main power |
| 12V to 5V converter | Power ESP32 and LCD |
| Cooling fan | Heat management |
| 4 push buttons | User input |
| 10K ohm resistors | Pull-down for buttons |

See [UVLightBox2.md](UVLightBox2/UVLightBox2.md) for product links and sourcing.

## Timer Firmware

### Features

- Adjustable timer: 10-second, 1-minute, and 1-hour increments (up to 5 hours)
- Full and half brightness modes (controls one or both relay channels)
- Settings saved to EEPROM (persists across power cycles)
- 16x2 LCD display showing mode, brightness, and countdown
- Press both Up and Down buttons simultaneously to reset timer to zero

### Pin Assignments

| Pin | Function |
|-----|----------|
| 32 | Relay 1 |
| 33 | Relay 2 |
| 25 | Button: Time Up |
| 26 | Button: Time Down |
| 34 | Button: Mode Change |
| 35 | Button: Start/Stop |
| SDA/SCL | I2C LCD (address 0x27) |

### Flashing the Firmware

1. Install the [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP32 board support:
   - Go to **File > Preferences**
   - Add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` to "Additional Board Manager URLs"
   - Go to **Tools > Board > Board Manager**, search "esp32", and install
3. Install the **LiquidCrystal I2C** library:
   - Go to **Tools > Manage Libraries**
   - Search "LiquidCrystal I2C" and install
4. Open `UVLightBox2/Timer/ArduinoSrc/lcd_timer_2.ino` in Arduino IDE
5. Select your board: **Tools > Board > ESP32 Dev Module**
6. Select the correct serial port under **Tools > Port**
7. Click **Upload**

### Dependencies

| Library | Source | License |
|---------|--------|---------|
| Wire.h | Arduino core (built-in) | LGPL 2.1 |
| EEPROM.h | Arduino core (built-in) | LGPL 2.1 |
| LiquidCrystal_I2C.h | Arduino Library Manager | LGPL |

## References

- [UV Light Sources for Printing](https://sandykingphotography.com/resources/technical-writing/uv-light-sources-for-printing) by Sandy King
- [UV LED Parts](https://clayharmonblog.com/uv-led-parts.html) by Clay Harmon
- [Polymer Photogravure Procedures](https://intaglioeditions.com/procedures/polymer_photogravure.html) by Intaglio Editions

## License

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2025 Yutaka Suzue
