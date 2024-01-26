# Arduino Clock with Alarm

## Description

This Arduino program implements a clock with an alarm feature using an Arduino board and an LCD display. The clock displays the current time, date, and weekday on the LCD. It allows the user to set an alarm time, and when the set time is reached, a sound is played using a piezo buzzer.

## Hardware Setup

- Arduino Board
- Liquid Crystal Display (LCD)
- Buttons for cursor control, character change, and alarm setting
- Potentiometer connected to an analog pin for controlling sound pitch
- Piezo (Buzzer) for sound output

### Wiring

Connect the components to the Arduino board based on the provided pin configuration in the code.

## Usage

The program reads values from three buttons to control cursor movement, character changes, and alarm setting. The LCD displays hours, minutes, seconds, date, month, and year. The clock is based on a standard month having 30 days.

### Button Mapping

- Cursor Button (Digital Pin 7): Controls cursor movement
- Change Time Button (Digital Pin 6): Changes character values
- Alarm Button (Digital Pin 5): Sets and controls the alarm

## Version

Version: 6
Date: March 30, 2021
Author: Alexis Fernandez

## How It Works

The clock calculates time based on the Arduino's running seconds. The program includes functionalities to increase seconds, minutes, hours, and update date and weekday accordingly.

### Alarm Functionality

- The alarm can be set by pressing the alarm button and configuring the desired time.
- When the set alarm time matches the running time, a sound is played using the piezo buzzer.
- The alarm can be turned off by pressing the alarm button again.

## Limitations and Considerations

- The clock assumes a standard month with 30 days.
- Sound pitch is controlled using a potentiometer.

## Getting Started

1. Set up the hardware according to the provided wiring configuration.
2. Upload the provided Arduino code to the board.
3. Use the cursor and change time buttons to configure the clock and set the alarm.

## Dependencies

- [Liquid Crystal Library](https://www.arduino.cc/en/Reference/LiquidCrystal)

Feel free to modify and enhance the code based on your specific requirements.

