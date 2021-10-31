# switchcraft_nano

## Disclaimer
These circuits are meant to be done with 12v and 5v voltages.
In ordinary circumstances, 12v and 5v circuits are not dangerous to humans.
Working with unplugged boards and circuits is still highly recommended for your health and the health of your equipment.

in NO circumstances you should work with higher voltages if you are not a certified professional. 
Main voltage (230v Europe, 120v US) can and will kill you if you don't know what you're doing.

I give information related to building SwitchCraft elements based on my own experience and successful builds.
However, building SwitchCraft is my hobby, I am not a professional electronics engineer and I cannot be held responsible if you damange your equipment or hurt yourself in the process.

## Build

This code is to be uploaded on a Arduino (or Elegoo) Nano to enables a button to work with a SwitchCraft unit (based on Raspberry Pi at https://github.com/Aurian-Nogues/switchcraft_pi)

Note that I am using BCM standard to describe pins so Pi Pin 21 means GPIO 21 / BOARD physical 40

## Hardware required
* a Nano board (Arduino, Elegoo or other brand) or a more powerful board (waste of money, Nano is good enough for what we do)
* some cables (Dupont if you want to avoid soldering)
* a 2 channels relay module
* A Raspberry Pi with SwitchCraft uploaded on it
* A physical button

## Upload code to your Nano
Note that if you use an Elegoo nano, they use an ATmetga328p chip instead of the normal FT232 in Arduino, this means that you need to install additional drivers.
Go to https://www.elegoo.com/pages/arduino-kits-support-files and pick the right board (V3.0 or V3.0+) to download the drivers.

Once your OS sees the chip, upload the content of main.cpp to your board.
The simplest way to do it is to use Arduino IDE. Follow this tutorial https://www.arduino.cc/en/Guide/ArduinoNano and upload the contents of main.cpp instead of the LED blink program

## wiring and explainations
### Relay basics
Ignore this part if you already know what a relay is.
A relay is a switch triggered by electric current.
When connecting a circuit to a relay, we create a loop between the following terminals:
* COM is the commin terminal, we need to plug something there in all cases
* NC is "normally closed", a circuit plugged here will be closed (ON) when the relay receives no signal. It will open (Off) when receiving a signal
* NO is "normally open", a circuit plugged here will be normally open (Off) when the relay receives no signal. It will close (ON) when receiving a signal

The relay also has a signal (IN) that will close / open circuits. It also needs power (5v) and ground.

In our case, we will create a 5v circuit for the Pi and a 12v circuit for the LCD panel. These circuits will be connected to NO terminals because we want them to be off by default and ON when the Nano is sending the signal to "keep them on"

### Power
* Create a 5 volts and ground rail
* 5v plugged to Nano 5v pin, and Relay DC+
* ground to Nano ground, Pi ground and relay DC-
This will make sure that Nano, Pi and relay are grounded together so we have the same voltage across all devices.
Not grounding devices together can cause voltages to shift and signals not to be read properly

### signals
Note that if you don't care why a terminal is connected to another you don't have to read anything but the connection information
The type of INPUT / OUTPUT or what they do is already set in the code and if you don't change anything you don't need to know what they do.

#### Nano to relay
* Nano D2 to Relay IN1
  * This will be an OUTPUT pin triggering the relay
  * This PIN outputs HIGH to leave the relay in its default state (NO open) and LOW to close the circuit plugged to NO
  * This channel of the relay will control the 12v circuit for the LCD panel

* Nano D3 to relay IN2
  * This will be an OUTPUT pin triggering the relay
  * This PIN outputs HIGH to leave the relay in its default state (NO open) and LOW to close the circuit plugged to NO
  * This channel of the relay will control the 5v circuit for the Raspberry Pi
 
#### Nano to Pi

* Button to Nano D4 and Ground
  *  This will be and INPUT_PULLUP pin
  *  Signal will be HIGH when button is not pressed, LOW (grounded) when pressed
  *  Depending on the state of the system, pressing the button will trigger poweroff or poweron sequences

* Nano D5 to Pi GPIO 20
  * Pi GPIO 20 is OUTPUT
  * Nano D5 is INPUT_PULLUP 
  * This connection enables the Nano to listen for PI signals. Signal is HIGH by default. If Pi overrides the signal with LOW (ground it), the Nano will take it as a request to start shutdown sequence

* Nano D6 to Pi GPIO 16
  * Nano D6 is OUTPUT
  * Pi GPIO 16 is INPUT_PULLDOWN
  * Default signal is LOW. Nano will constantly override it with HIGH. If it stops, this is the signal for Pi to initiate shutdown sequence

*  Nano D7 connected to PI GPIO 21
  * Pi GPIO 21 is OUTPUT LOW
  * Nano D7 is INPUT_PULLUP
  * The Nano will get HIGH by default. Pi will override this signal with LOW (ground it) as long as it is on
  * This is to make the Nano knows when the Pi is on or off. When the Pi is off, Nano will ignore all Pi signals (caused by static)

#### Relay to Pi
* Create a circuit between 5v rail, Pi 5v pin, relay channel 2 COM (linked to S2), relay channel 2 NO termnial and ground rail

#### Relay to LCD panel
* create a circuit between 12v power source, LCD driver, relay channel 1 COM (linked to S1), relay channel 1 NO terminal, 12v ground

## Expected result
The default state of all circuits is closed
### Initial Boot sequence
* Nano sends signal to relay to close Pi 5v circuit (ON)
* Nano waits for a few seconds, sends signals to relay to close LCD 12v circuit (ON)
* Pi will eventually boot and send signal to Nano that it is on so its signals should no longer be ignored

### poweroff from button
* Nano sends signal to Pi to trigger Linux shutdown sequence
* Nano sends signal to Relay to open (off) 12v LCD circuit
* Nano waits for a few seconds for Pi to poweroff
* Nano sends signal to Relay to open (off) Pi 5v circuit

### poweroff from bluetooth
* Pi receives signal to poweroff from bluetooth
* Pi sends signal to Nano to trigger poweroff sequence
* Pi initiates Linux shutdown sequence
* Nano sends signal to Relay to open (off) 12v LCD circuit
* Nano waits for a couple of seconds for Pi to finish shutdown sequence
* Nano sends signal to relay to open (off) the pi 5v circuit

### power on from button
* Nano sends signal to Relay to close (on) pi 5v circuit which will automatically start the Pi
* Nano waits for a couple of seconds
* Nano sends signal to relay to close (on) 12v LCD circuit









