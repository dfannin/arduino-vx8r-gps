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
A Ublox Neo-6m GPS module (NEO-6M-0-001)  was used (around $15USD on Ebay), including an active antenna. It is a stand-alone GPS module.  The Ublox has four connections, TX, RX, VCC and GND.   The module has a voltage regulator, and can take up to 6+ volts input.   The serial port is set to  9600,N,1 as a default.  

I had to change the ublox firmware settings to produced the desired output using the Ublox Ucenter software (available from the Ublox website). You'll need to connect the tx/rx and gnd pins to a USB/TTL3.3v FTDI or equivlent cable and connect to a computer running the Ucenter (windows based) software. I used Ubuntu/Wine to run the Ucenter software - it would crash sometimes, but I was to able to reprogram the module.   I changed the following settings and saved them to the EEPROM:  Output: NMEA, Output Version 2.3, Compatiblity Mode (checked), GPZDA messages set output to true.    

The Ublox is cross-over connected to an Arduino software serial port using AltSoftSerial, which uses pins D8 and D9 (RX and TX), so connect the Ublox RX to the Arduino TX pin and vis versa.  The Ublox was connected to the power supply, which was 4 x AAA batteries (6v+).
![Ublox](https://raw.github.com/dfannin/arduino-vx8r-gps/master/ublox-neo6m.jpg)


## Arduino

## Yaesu HT Cable

# Use  

# Extending/Modifiing Output 


