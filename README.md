# Bike Rental System Utilizing RFID Based on Arduino Uno with Tracking System

## Introduction

This project aims to develop a Bike Rental System utilizing RFID technology, based on Arduino Uno microcontroller, along with a tracking system. The system allows users to rent bikes securely using RFID tags and tracks the bike's location using GPS and GSM modules.

## System Specifications

- Microcontroller: Arduino Uno
- Components:
  - RFID Reader
  - GPS Module
  - GSM Module
  - LCD
  - Buzzer
  - Dynamo
  - L298N Driver

## Working Principles

The system operates as follows:

1. **RFID Authentication**: 
   - Users authenticate themselves by tapping their RFID tags on the RFID reader.
   - If the RFID tag's ID matches the programmed one, the Dynamo rotates to unlock the bike, initiating the rental process.

2. **Rental Process**:
   - After unlocking the bike, a timer starts on the microcontroller.
   - When the RFID tag is removed, indicating the user has taken the bike, the buzzer alerts and the LCD displays the rental duration.
   - The user presents their identification card for payment, and the total rental time and cost are displayed on the LCD.

3. **Tracking System**:
   - The GPS module tracks the bike's location during the rental period.
   - The GSM module sends the bike's location to the bike owner's smartphone via a Google Maps link, enabling real-time tracking.

## Installation and Setup

1. **Hardware Setup**:
   - Connect the Arduino Uno to the RFID reader, GPS module, GSM module, LCD, buzzer, Dynamo, and L298N Driver as per the circuit diagram.
   
2. **Software Setup**:
   - Install the Arduino IDE.
   - Upload the provided Arduino code to the Arduino Uno.

3. **Configuration**:
   - Program the RFID tags with unique IDs for users.
   - Configure GSM module settings for sending SMS or data.

## Usage

1. Tap the RFID tag on the RFID reader to unlock the bike.
2. Remove the RFID tag and take the bike.
3. Upon return, tap the RFID tag again to lock the bike.
4. The GPS/GSM modules will track the bike's location during the rental period and send updates to the owner's smartphone.

## Future Enhancements

- Integration with a mobile app for user registration, rental booking, and payment.
- Enhanced tracking features such as geofencing and route history.
- Improvements in user interface and feedback mechanisms.


