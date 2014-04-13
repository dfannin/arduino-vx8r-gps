arduino-vx8r-gps
================

Arduino code for buidling Yaesu VX-8R Handheld Transmitter-compatible GPS

# Overview

The Yaesu VX-8R HT for amateur radio VHF/UHF bands has a built-in APRS (Automatic Position Reporting System)  TNC (Terminal Node Controller) to operate as an APRS Beacon transmitter and receiver. To add position and velocity (GPS information) to the transmitted packet, an external Proprietary GPS module must be purchased from Yaesu and connected to HT.

The goal of this project was to create a compatbile GPS unit, made with open-source and generally available components, that could be used to proivde the position and velocity information for the Yaesu HT. The total cost of the project came in under $43USD, which included having to buy a proprietary cable for $22USB.

This GPS provides provides all the known GPS features used by the Yaesu, including position, altitude, speed, and direction. It also provides a GPS timestamp, which is used to automatically  set the time/date on the HT.  The GPS module and Arduino fit in a small candy tin (slightly larger than the Yaesu GPS unit),  and can be easily attached to the HT for portible operation.     

The code was designed to work with a specific GPS module (Ublox neo-6m), but should be easily adapted or extended to other NMEA-compatible GPS devices or required features. 

The unit was tested as a automobile  beacon transmitter, and provided acurate tracking information as reported by the http://aprs.fi website. 

This project builds from the work done by Taylor Meeks KG7BBG (http://www.lingnik.com/content/reverse-engineering-yaesu-vx-8dr-gps-interface).  


# Block Diagram
 ![BlockDiagram](https://raw.github.com/dfannin/arduino-vx8r-gps/master/arduino-vx8r-gps-fig01.png)

The GPS unit consists of three parts.  The GPS module provides NMEA messages via a two wire serial port (tx/rx). These messages are version 2.3 compliant.  The messages are sent to the Arduino, using a software serial port (AltSoftSerial), and are parsed, filtered and reformatted for the Yaesu.  The reformatted messages are sent to the Yaesu via the hardware serial port using a proprietary Yaesu cable (CT-M11 or equivilent), as the HT uses a 7 pin propreitary connector.  


# NMEA messages
The following NMEA messages are sent to the Yaesu (thanks to Taylor KG7BBG) :

* $GPGGA,hhmmss.sss,llll.llll,a,yyyyy.yyyy,a,x,xx,xx.x,xxxxx.x,M,xxxx.x,M,xxx.x,xxxx\*hh
* $GPGLL,llll.llll,a,llll.llll,a,nnnnnn.nnn,a,a\*hh
* $GPGSA,a,n,nn,nn,nn,nn,nn,,,,,,,,n.n,n.n,n.n\*hh
* $GPGSV,n,n,nn,nn,nn,nnn,nn,nn,,,nn,nn,nn,nnn,nn,nn,nn,nnn,\*hh
* $GPRMC,hhmmss.sss,a,llll.llll,a,yyyyy.yyyy,a,xxxx.xx,xxx.xx,xxxxxx,,\*hh
* $GPVTG,,a,,a,n.nn,a,n.n,a,a\*hh
* $GPZDA,hhmmss.sss,xx,xx,xxxx,,\*hh

Other points:
* Any other message types (like $GPTXT) are filtered from the output stream.
* Fields should be kept as fixed width (and zero padded) 
* No modification of the Yaesu -> GPS messages, they are passed through directly. This didn't appear to cause any issues.


# Hardware

## Schematic
 ![BlockDiagram](https://raw.github.com/dfannin/arduino-vx8r-gps/master/YaesuGPSSchematic.png)

## NMEA-compatible GPS Module
A Ublox Neo-6m GPS module (NEO-6M-0-001)  was used (around $15USD on Ebay), including an active antenna. It is a stand-alone GPS module. According to the datasheet, the GPS chip uses about 36mA during operation.  The Ublox has four connections, TX, RX, VCC and GND.   The module has a voltage regulator, and can take up to 6+ volts input.   The serial port is set to  9600,N,1 as a default.  

I had to change the ublox firmware settings to produced the desired output using the Ublox Ucenter software (available from the Ublox website). You'll need to connect the tx/rx and gnd pins to a USB/TTL 5/3.3v FTDI or equivlent cable and connect to a computer running the Ucenter (windows based) software. I used Ubuntu/Wine to run the Ucenter software - it would crash sometimes, but I was to able to reprogram the module.   I changed the following settings and saved them to the EEPROM:  Output: NMEA, Output Version 2.3, Compatiblity Mode (checked), GPZDA messages set output to true.    

The Ublox is cross-over connected to an Arduino software serial port using AltSoftSerial, which uses pins D8 and D9 (RX and TX), so connect the Ublox RX to the Arduino TX pin and vis versa.  The Ublox was connected to the power supply, which was 4 x AAA batteries (6v+).
![Ublox](https://raw.github.com/dfannin/arduino-vx8r-gps/master/ublox-neo6m.jpg)


## Arduino
An Arduino Nano v3 (atmel328, 5v, 16Mhz, 32k ram) was used, as its small size (0.73" x 1.70"), low power consumption (40mA), soldered connections and performance was a good fit for this application.   The Nano was powered by the same 6 volt power supply as the GPS module.  

The Nano is connected to the TX/RX pins of the GPS module, and is also connected to the serial port of the Yaesu HT.  Nano Pins D0/D1 (the hardware TX/RX pins) were used for Yaesu serial port.  Since the Yaesu is expecting TTL 3.3v levels for the serial port, a resistor voltage divider was used to bring the Nano 5v TTL level down to 3.3v.  A better approach would be to use a logic level shift chip, but the simple bridge appears to work well, and it was very easy to construct , by bridging the two resistors (270 and 470 ohms, 1/4 watt) across the Nano board.   The bridge is only used on the Arduino TX side, as the RX is driven at the 3.3v by the Yaesu, and doesn't require the voltage drop. 

The Nano can still be updated using the Usb interface; However, since the USB tx/rx pins are shared with D0/D1, you won't be able to connect to the Yaesu and the USB interface at the same time.  You should disconnect the Yaesu cable from the unit whenever doing a new software upgrade to the Nano.  As a benefit,  You can see the GPS output stream from the Nano when you connect to the USB port and use a term emulator, which makes it easier to debug.  I did this to avoid having two software serial ports.

![Ublox](https://raw.github.com/dfannin/arduino-vx8r-gps/master/ArduinoNanoFront.jpg)

### Arduino Software
The software provides the serial port forwarding and NMEA message translation between HT and GPS module.  A new class was created _GpsDevice_ that does the serial port stream reading, parsing, filtering and output.  See the Extension/Modificaiton section for hints on how to modify this class if you require other types of filtering or translation.   The checksums from the GPS module are calculated and compared before passing thru messages, and new checksums are generated for modified messages.  Other have observed, and I have seen that the Yaesu HT does not valid the checksums, it doesn't to add much overhead.

The serial port reads are done character by character, alternating each serial port stream; This was done to cut down on possible delay time between buffer reads and reducing the risk of buffer overruns.  So far, no issues with overruns or lost messages have been observed.  

The Arduino library _AltSoftSerial_ is required to be installed in your Arduino Development environment.  No other external libraries are required.

## Yaesu HT Cable

The Yaesu VX-8R uses a proprietary 7 pin connector, which I have been unable to find.  Therefore, I purchased a rather expensive USB cable (normally used for programing the Yaesu) and cut the USB end off, and wired the 3 pins (TX,RX and GND)  into the Nano as shown. A better option would be to use the CT-M11 cable from Yaesu, which provides all the pins outs, including the 3.3V power supply.  if you get a 3.3v Nano, you should be able to power the GPS and Nano from the HT power supply directly, rather than use an external battery pack.   

![Ublox](https://raw.github.com/dfannin/arduino-vx8r-gps/master/YaesuUSBCable.jpg)

## Case and Mounting 

I used a small mint tin (about half the size of an arduino tin) for the GPS and Nano.  I installed the GPS antenna on the outside of the case, and drilled holes for the antenna cable, Yaesu HT cable and an opening for the Nano usb connector.   After wiring the circuit and boards together, I used hot glue to mount all the components in the case. 

It's important to use a common ground for all modules and power supply, to minimize RF interference, including the gps/Nano,  case and Yaesu cable.

# Use  

# Extending/Modifying Output 


