# ESP32 Nursery Monitor

An IoT-based nursery monitoring system built with **ESP32**, designed to track **soil moisture** and **light levels**, display readings on an **OLED screen**, send **WhatsApp alerts** for critical conditions (via Arduino + CallMeBot), and **log data remotely to ThingSpeak**.

## Features
- Real-time monitoring of water and light levels
- OLED display for live data output
- WhatsApp alerts for CRITICAL conditions
- Built-in cooldown (default: 3 minutes) to prevent message spam
- Remote logging to ThingSpeak (HTTP GET uploads)

---

## Hardware Requirements
- ESP32 board  
- DHT11 (temperature & humidity)  
- Light sensor (e.g., LDR + resistor)  
- Water/soil moisture sensor (analog)  
- OLED display (SSD1306, I²C)  
- Jumper wires & breadboard  
