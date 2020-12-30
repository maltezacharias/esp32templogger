# esp32templogger
ESP32 Based Wifi Temperature logger for [custom hardware based on ESP32-Cam Module](https://github.com/maltezacharias/esp32templogger-hardware)

![Hardware Image](https://github.com/maltezacharias/esp32templogger-hardware/raw/main/img/templogger.png)

## Status

**Software: Work in progress**

Hardware: First prototype built, v0.2 committed to GitHub but never actually manufactured and tested.

## Features

Application:

Logging temperatures in a green house, Switching ON/OFF Heater (max. 6 Amps), 

Hardware includes:
- DS18B20 Temperature Sensor
- Mains voltage rated relay switched by a transistor
- ESP32-Cam Module with OV2640 Camera


Software features (planned/wip):
- Started on Arduino IDE and then switched to Espressif IDF with VSCode
- Task based approach
  - MQTT SSL Client Task, publishes images and sensor values, subscribes for minimal management commands
  - Camera Task regularly updating a picture in RAM for MQTT Task to send
  - Switching Task to switch relay based on temperature reading and/or MQTT commands
- Inter Task Communication via Queues, Semaphore for Image Update and Transmission
