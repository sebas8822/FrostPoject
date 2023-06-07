
# Frost Detection Project

This project focused on sensing various environmental values and determining when a frost event is likely to occur. The project utilizes an ESP32 microcontroller board along with several sensors, including a DHT11 temperature and humidity sensor and a soil moisture sensor. The collected data is displayed on an OLED screen and sent to a server for further processing. Additionally, the project includes functionalities for real-time clock synchronization and data storage on a MicroSD card.

## Features

- Real-time display of sensor readings on an OLED screen.
- Data transmission to a server for further processing.
- Automatic time synchronization using a DS1302 real-time clock module.
- Data storage on a MicroSD card for backup and offline analysis.
- LED indicators for Wi-Fi connection status and SD card data transfer.

## Setup

To set up the project, follow these steps:

1. Clone the repository to your local machine.
2. Connect the ESP32 microcontroller board to the required sensors as per the provided circuit diagram.
3. Install the necessary libraries listed in the `Dependencies` section below.
4. Update the code with your Wi-Fi network credentials, server URL or IP address, API key, and device identification.
5. Upload the code to the ESP32 microcontroller using the Arduino IDE or your preferred method.
6. Monitor the sensor readings on the OLED screen and observe the LED indicators for connectivity and data transfer status.

## Dependencies

The project relies on the following libraries:

- DHT: Library for the DHT11 temperature and humidity sensor.
- WiFi: Library for Wi-Fi connectivity.
- HTTPClient: Library for sending HTTP requests.
- ThreeWire and RtcDS1302: Libraries for controlling the DS1302 real-time clock module.
- Adafruit_GFX and Adafruit_SSD1306: Libraries for controlling the OLED display.
- FS and SD: Libraries for controlling the MicroSD card.
- SPI: Library for SPI communication with the MicroSD card.

Make sure to install these libraries in your Arduino IDE before compiling and uploading the code to the ESP32.

## Photos Project

![Alt Text](https://photos.app.goo.gl/8YB2nooVJkhc3cGC7)
