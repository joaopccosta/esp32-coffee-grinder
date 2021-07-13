# esp32-coffee-grinder

## Description

Coffee grinder automation using an ESP32 microcontroller.
This particular code is written having in mind that I own a manual coffee grinder (on a Lelit PL042TEMD machine).

## Dependencies

- [Setup esp32 board on Arduino IDE](https://circuitdigest.com/microcontroller-projects/programming-esp32-with-arduino-ide)
- [BluetoothSerial](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial)
- [OneButton](https://github.com/mathertel/OneButton)

## Operation

My machine's coffee grinder is completely manual. It has a dispenser button which starts the operation for as long as it is kept being pressed.
I wanted to implement something that, by knowing how many grams of coffee are dispensed in a given amount of time, would allow my coffee grinder to output the right those that I am looking for without having to think about weighting the coffee beforehand. (I know, I know... purists will say the coffee beans will go stale :) )

Therefore, what is implemented is the following:

- The module will keep track of a specific dispense timer
- This timer can be adjusted through a command sent over serial on bluetooth.
- The state of the module is also easy to get through a bluetooth command.
- And of course, you can trigger the module to start dispensing, through another command.
- Long-pressing the assigned I/O pin (hooked up to the grinder's button) will bypass the implementation and trigger the manual grinding process.
