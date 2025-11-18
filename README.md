# EchoBot Project: ESP8266 + HC-SR04 + Buzzer + 2 LEDs + Laser

## Project Overview

The EchoBot is a mini motion detection robot based on the ESP8266 microcontroller. It uses an HC-SR04 ultrasonic sensor to measure the distance to objects. When an obstacle approaches within 35 cm (here 35 cm, but easily modifiable), the EchoBot enters "attack mode" and activates 2 red LEDs, a buzzer that emits sound, and a laser, in order to "intimidate" anyone who approaches.

This project is ideal for learning about ultrasonic sensors, peripheral control (LEDs, buzzer, laser), and microcontroller programming with ESP8266.

## Project Images

### Logo
![Logo](logo-echobot.png)

### Test 1-2
![Test 1-2](teste1.jpg)

### Test 3
![Test 3](teste3.jpg)

### Test 4
![Test 4](teste4-1.jpg)

### Test 5
![Test 5](teste5-1.jpg)

## Features

*   **Proximity Detection:** Measures the distance to obstacles using the HC-SR04 sensor. When an obstacle is close, it activates the system.
*   **Visual Feedback:**
    *   Two red LEDs light up.
    *   A laser module is activated.
*   **Auditory Feedback:** The buzzer emits intermittent sounds.
*   **Stabilized Reading:** AI SUGGESTION - Implements smoothing of sensor readings for greater accuracy and stability.
*   **Serial Communication:** Sends distance data and component status to the serial monitor for monitoring and user feedback.

## Required Components

To replicate this project, the following components are needed:

*   **Microcontroller:** ESP8266 (e.g., NodeMCU ESP-12E)
*   **Distance Sensor:** HC-SR04 (Ultrasonic Sensor)
*   **Visual Indicators:**
    *   2x LEDs (any color, user preference)
    *   2x 220 Ohm Resistors (use: https://www.calculator.net/resistor-calculator.html)
    *   1x Laser
*   **Auditory Indicator:** 1x Active Buzzer
*   **Cables:** 15-20 Cables (male-to-male, male-to-female)
*   **Breadboard:** For assembling the circuit
*   **Software:** Arduino IDE with ESP8266 support

## Connection Diagram (Fritzing / Diagram)

See project report (Project Architecture Chapter).

## Setup and Upload the Code

1.  **Install Arduino IDE:** Available at: (https://www.arduino.cc/en/software)
2.  **Add ESP8266 Support:**
    *   In Arduino IDE, `File > Preferences`.
    *   In the "Additional Board Manager URLs" field, add:
        `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
    *   Go to `Tools > Board > Boards Manager...`
    *   Search for "esp8266" and install the "esp8266 by ESP8266 Community" package.
3.  **Select the Board:**
    *   Go to `Tools > Board > ESP8266 Boards` and select the board (e.g., "NodeMCU 1.0 (ESP-12E Module)").
    *   Select the correct COM port in `Tools > Port`.
4.  **If the previous steps do not work:** Available at: (https://randomnerdtutorials.com/install-esp32-esp8266-usb-drivers-cp210x-windows/)
    *   Install a specific driver for Windows to recognize the ESP8266 (https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers)
    *   Go to Device Manager
    *   COM Ports - CP210, if not active, "Update driver", choose the CP210 folder and it will work.
5.  **Copy the Code:** Copy the `EchoBot.ino` code to the Arduino IDE.
6.  **Upload the Code:** Click the "Upload" button (right arrow) in the Arduino IDE. Ensure the ESP8266 is connected to the computer via USB.

## Configurable Parameters in the Code

Adjust the following parameters to customize the EchoBot's behavior:

*   `threshold_cm`: The distance in centimeters that activates the LEDs, buzzer, and laser. (Default: `35.0`)
*   `samples`: The number of sensor readings to use for calculating the average (smoothing). (Default: `5`)
*   `settle_ms`: The waiting time in milliseconds between each sensor reading sample. (Default: `50`)
*   `buzzerFreq`: The buzzer sound frequency in Hz. (Default: `2000`)
*   `buzzerDuration`: The duration of each "beep" of the buzzer in milliseconds. (Default: `100`)
*   `buzzerPause`: The duration of the pause between "beeps" in milliseconds. (Default: `100`)

##  troubleshooting

*   **" Nada a fazer , estudasses ... " in the Serial Monitor:**
    *   Check the HC-SR04 connections (VCC, GND, Trig, Echo).
    *   See if there is an object within the sensor's range (2cm to 2m).
    *   Check if the sensor is pointing at a sound-reflecting surface.
    *   If the object is less than 2cm away, the message will be displayed.
*   **LEDs/Buzzer/Laser not working:**
    *   Check the connections of the components to the correct ESP8266 pins.
    *   Confirm that the LEDs have individual resistors.
    *   Check if you have an active buzzer.
    *   Verify that the object's distance is below `threshold_cm`.
*   **Problems uploading the code:**
    *   Check if the board is correctly selected.

**Developed with AI assistance - Project for the Embedded Systems course - Bachelor's Degree in Computer Engineering - 3rd year**
